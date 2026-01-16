/*
 * battery_monitor.c - E-Reader Battery Monitoring Implementation
 *
 * Implements battery voltage and capacity monitoring with automatic
 * hardware detection and multiple backend support.
 *
 * Author: E-Reader Project
 * License: Open Source
 */

#include "battery_monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

/* Default configuration values (for single-cell LiPo 3.7V) */
#define DEFAULT_VOLTAGE_FULL      4200.0f  /* 4.2V - fully charged */
#define DEFAULT_VOLTAGE_EMPTY     3200.0f  /* 3.2V - empty (don't discharge below this) */
#define DEFAULT_VOLTAGE_CRITICAL  3300.0f  /* 3.3V - critical low, shut down soon */
#define DEFAULT_UPDATE_INTERVAL   60000    /* 60 seconds between updates */
#define DEFAULT_ADC_CHANNEL       0
#define DEFAULT_VOLTAGE_DIVIDER   2.0f     /* 1:1 resistor divider (R1=R2) */

/* I2C device paths and addresses */
#define I2C_DEVICE_PATH           "/dev/i2c-1"
#define INA219_I2C_ADDR_DEFAULT   0x40
#define INA219_I2C_ADDR_ALT1      0x41
#define INA219_I2C_ADDR_ALT2      0x44
#define INA219_I2C_ADDR_ALT3      0x45

/* INA219 registers */
#define INA219_REG_CONFIG         0x00
#define INA219_REG_SHUNT_VOLTAGE  0x01
#define INA219_REG_BUS_VOLTAGE    0x02
#define INA219_REG_POWER          0x03
#define INA219_REG_CURRENT        0x04
#define INA219_REG_CALIBRATION    0x05

/* SPI device paths */
#define SPI_DEVICE_PATH           "/dev/spidev0.0"

/* MCP3008 configuration */
#define MCP3008_SPI_SPEED         1000000  /* 1MHz SPI clock */
#define MCP3008_ADC_MAX           1023     /* 10-bit ADC */
#define MCP3008_VREF              3300.0f  /* 3.3V reference */

/*
 * Backend-specific data structures
 */

/* INA219 backend data */
typedef struct {
    int i2c_fd;
    uint8_t i2c_address;
} ina219_data_t;

/* MCP3008 backend data */
typedef struct {
    int spi_fd;
    uint8_t channel;
} mcp3008_data_t;

/*
 * Helper: Get current time in milliseconds
 */
static uint64_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/*
 * Helper: Calculate battery percentage from voltage
 */
static uint8_t voltage_to_percentage(float voltage_mv, float full_mv, float empty_mv) {
    if (voltage_mv >= full_mv) {
        return 100;
    }
    if (voltage_mv <= empty_mv) {
        return 0;
    }

    /* Linear interpolation */
    float percentage = ((voltage_mv - empty_mv) / (full_mv - empty_mv)) * 100.0f;
    return (uint8_t)percentage;
}

/*
 * Backend detection: Try to detect INA219 on I2C bus
 */
static bool detect_ina219(uint8_t *address) {
    int fd;
    uint8_t addresses[] = {
        INA219_I2C_ADDR_DEFAULT,
        INA219_I2C_ADDR_ALT1,
        INA219_I2C_ADDR_ALT2,
        INA219_I2C_ADDR_ALT3
    };

    fd = open(I2C_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        return false;  /* I2C bus not available */
    }

    /* Try each possible address */
    for (size_t i = 0; i < sizeof(addresses); i++) {
        if (ioctl(fd, I2C_SLAVE, addresses[i]) >= 0) {
            /* Try to read config register */
            uint8_t reg = INA219_REG_CONFIG;
            if (write(fd, &reg, 1) == 1) {
                uint8_t data[2];
                if (read(fd, data, 2) == 2) {
                    /* Found INA219 */
                    *address = addresses[i];
                    close(fd);
                    return true;
                }
            }
        }
    }

    close(fd);
    return false;
}

/*
 * Backend detection: Try to detect MCP3008 on SPI bus
 */
static bool detect_mcp3008(void) {
    int fd = open(SPI_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        return false;  /* SPI not available */
    }

    /* SPI exists, assume MCP3008 is present */
    /* (There's no way to definitively detect without reading, which
     *  might give garbage data if chip isn't present) */
    close(fd);
    return true;
}

/*
 * Backend: Read voltage from INA219
 */
static int ina219_read_voltage(ina219_data_t *data, battery_status_t *status) {
    uint8_t reg = INA219_REG_BUS_VOLTAGE;

    /* Write register address */
    if (write(data->i2c_fd, &reg, 1) != 1) {
        return -1;
    }

    /* Read 2 bytes */
    uint8_t buf[2];
    if (read(data->i2c_fd, buf, 2) != 2) {
        return -1;
    }

    /* Convert to voltage (LSB = 4mV) */
    uint16_t raw = (buf[0] << 8) | buf[1];
    raw >>= 3;  /* Shift out lower 3 bits (flags) */
    status->voltage_mv = raw * 4.0f;  /* 4mV per LSB */

    /* Read shunt voltage for current calculation */
    reg = INA219_REG_SHUNT_VOLTAGE;
    if (write(data->i2c_fd, &reg, 1) == 1) {
        if (read(data->i2c_fd, buf, 2) == 2) {
            int16_t shunt_raw = (int16_t)((buf[0] << 8) | buf[1]);
            /* Shunt voltage in mV (LSB = 10uV = 0.01mV) */
            float shunt_mv = shunt_raw * 0.01f;
            /* Assuming 0.1 ohm shunt resistor: I = V / R */
            status->current_ma = shunt_mv / 0.1f;
            status->power_mw = status->voltage_mv * status->current_ma / 1000.0f;
        }
    }

    /* Detect charging (current is negative when charging) */
    status->is_charging = (status->current_ma < -10.0f);

    return 0;
}

/*
 * Backend: Read voltage from MCP3008 ADC
 */
static int mcp3008_read_voltage(mcp3008_data_t *data, battery_status_t *status,
                                float divider_ratio) {
    uint8_t tx_buf[3];
    uint8_t rx_buf[3];

    /* MCP3008 command: start bit, single-ended, channel select */
    tx_buf[0] = 0x01;  /* Start bit */
    tx_buf[1] = (0x08 | data->channel) << 4;  /* Single-ended + channel */
    tx_buf[2] = 0x00;

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 3,
        .speed_hz = MCP3008_SPI_SPEED,
        .bits_per_word = 8,
    };

    if (ioctl(data->spi_fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        return -1;
    }

    /* Extract 10-bit ADC value */
    uint16_t adc_value = ((rx_buf[1] & 0x03) << 8) | rx_buf[2];

    /* Convert to voltage */
    float adc_voltage = (adc_value / (float)MCP3008_ADC_MAX) * MCP3008_VREF;

    /* Account for voltage divider */
    status->voltage_mv = adc_voltage * divider_ratio;

    /* ADC can't detect current or charging status */
    status->current_ma = 0.0f;
    status->power_mw = 0.0f;
    status->is_charging = false;

    return 0;
}

/*
 * Initialize battery monitor
 */
battery_monitor_state_t* battery_monitor_init(const battery_config_t *config) {
    battery_monitor_state_t *state = NULL;
    uint8_t i2c_addr = 0;

    /* Allocate state */
    state = calloc(1, sizeof(battery_monitor_state_t));
    if (state == NULL) {
        fprintf(stderr, "battery_monitor: Failed to allocate state\n");
        return NULL;
    }

    /* Set configuration (use defaults if not provided) */
    if (config != NULL) {
        state->config = *config;
    } else {
        state->config = battery_monitor_get_default_config();
    }

    /* Auto-detect hardware backend */
    printf("Battery monitor: Detecting hardware...\n");

    /* Priority 1: Try INA219 I2C power monitor */
    if (detect_ina219(&i2c_addr)) {
        printf("Battery monitor: Found INA219 at I2C address 0x%02X\n", i2c_addr);
        state->backend = BATTERY_BACKEND_INA219;

        /* Initialize INA219 backend */
        ina219_data_t *ina_data = calloc(1, sizeof(ina219_data_t));
        if (ina_data == NULL) {
            free(state);
            return NULL;
        }

        ina_data->i2c_fd = open(I2C_DEVICE_PATH, O_RDWR);
        if (ina_data->i2c_fd < 0) {
            fprintf(stderr, "battery_monitor: Failed to open I2C device\n");
            free(ina_data);
            free(state);
            return NULL;
        }

        if (ioctl(ina_data->i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
            fprintf(stderr, "battery_monitor: Failed to set I2C address\n");
            close(ina_data->i2c_fd);
            free(ina_data);
            free(state);
            return NULL;
        }

        ina_data->i2c_address = i2c_addr;
        state->backend_data = ina_data;
    }
    /* Priority 2: Try MCP3008 SPI ADC */
    else if (detect_mcp3008()) {
        printf("Battery monitor: Found SPI device, assuming MCP3008 ADC\n");
        state->backend = BATTERY_BACKEND_MCP3008;

        /* Initialize MCP3008 backend */
        mcp3008_data_t *adc_data = calloc(1, sizeof(mcp3008_data_t));
        if (adc_data == NULL) {
            free(state);
            return NULL;
        }

        adc_data->spi_fd = open(SPI_DEVICE_PATH, O_RDWR);
        if (adc_data->spi_fd < 0) {
            fprintf(stderr, "battery_monitor: Failed to open SPI device\n");
            free(adc_data);
            free(state);
            return NULL;
        }

        adc_data->channel = state->config.adc_channel;
        state->backend_data = adc_data;
    }
    /* No hardware detected - use dummy mode */
    else {
        printf("Battery monitor: No hardware detected, using dummy mode\n");
        state->backend = BATTERY_BACKEND_NONE;
        state->backend_data = NULL;

        /* Set dummy status (always 100%) */
        state->status.voltage_mv = DEFAULT_VOLTAGE_FULL;
        state->status.percentage = 100;
        state->status.is_valid = true;
        state->status.is_charging = false;
        state->status.is_low = false;
    }

    /* Perform initial update */
    battery_monitor_update(state);

    return state;
}

/*
 * Clean up battery monitor
 */
void battery_monitor_cleanup(battery_monitor_state_t *state) {
    if (state == NULL) {
        return;
    }

    /* Close backend resources */
    switch (state->backend) {
        case BATTERY_BACKEND_INA219: {
            ina219_data_t *data = (ina219_data_t *)state->backend_data;
            if (data != NULL) {
                close(data->i2c_fd);
                free(data);
            }
            break;
        }
        case BATTERY_BACKEND_MCP3008: {
            mcp3008_data_t *data = (mcp3008_data_t *)state->backend_data;
            if (data != NULL) {
                close(data->spi_fd);
                free(data);
            }
            break;
        }
        default:
            break;
    }

    free(state);
    printf("Battery monitor cleaned up\n");
}

/*
 * Update battery status
 */
int battery_monitor_update(battery_monitor_state_t *state) {
    if (state == NULL) {
        return -1;
    }

    /* Check if enough time has passed since last update */
    uint64_t now = get_time_ms();
    if (state->last_update_ms != 0 &&
        (now - state->last_update_ms) < state->config.update_interval_ms) {
        return 0;  /* Skip update, too soon */
    }

    state->last_update_ms = now;

    /* Reset status */
    state->status.is_valid = false;

    /* Read from backend */
    int result = -1;
    switch (state->backend) {
        case BATTERY_BACKEND_INA219:
            result = ina219_read_voltage((ina219_data_t *)state->backend_data,
                                        &state->status);
            break;

        case BATTERY_BACKEND_MCP3008:
            result = mcp3008_read_voltage((mcp3008_data_t *)state->backend_data,
                                         &state->status,
                                         state->config.voltage_divider_ratio);
            break;

        case BATTERY_BACKEND_NONE:
            /* Dummy mode - always report 100% */
            state->status.voltage_mv = DEFAULT_VOLTAGE_FULL;
            state->status.percentage = 100;
            state->status.is_charging = false;
            state->status.is_low = false;
            state->status.is_valid = true;
            result = 0;
            break;

        default:
            return -1;
    }

    if (result != 0) {
        fprintf(stderr, "battery_monitor: Failed to read battery status\n");
        return -1;
    }

    /* Calculate percentage */
    state->status.percentage = voltage_to_percentage(
        state->status.voltage_mv,
        state->config.voltage_full,
        state->config.voltage_empty
    );

    /* Check critical low threshold */
    state->status.is_low = (state->status.voltage_mv < state->config.voltage_critical);

    state->status.is_valid = true;

    return 0;
}

/*
 * Force update (bypass throttling)
 */
int battery_monitor_force_update(battery_monitor_state_t *state) {
    if (state == NULL) {
        return -1;
    }

    /* Reset last update time to force immediate read */
    state->last_update_ms = 0;

    return battery_monitor_update(state);
}

/*
 * Get current battery status
 */
battery_status_t battery_monitor_get_status(const battery_monitor_state_t *state) {
    battery_status_t invalid_status = {0};

    if (state == NULL) {
        return invalid_status;
    }

    return state->status;
}

/*
 * Get battery percentage
 */
uint8_t battery_monitor_get_percentage(const battery_monitor_state_t *state) {
    if (state == NULL || !state->status.is_valid) {
        return 100;  /* Default to 100% if unknown */
    }

    return state->status.percentage;
}

/*
 * Get battery voltage
 */
float battery_monitor_get_voltage(const battery_monitor_state_t *state) {
    if (state == NULL || !state->status.is_valid) {
        return 0.0f;
    }

    return state->status.voltage_mv;
}

/*
 * Check if battery is critically low
 */
bool battery_monitor_is_critical(const battery_monitor_state_t *state) {
    if (state == NULL || !state->status.is_valid) {
        return false;
    }

    return state->status.is_low;
}

/*
 * Check if battery is charging
 */
bool battery_monitor_is_charging(const battery_monitor_state_t *state) {
    if (state == NULL || !state->status.is_valid) {
        return false;
    }

    return state->status.is_charging;
}

/*
 * Get backend type name
 */
const char* battery_monitor_get_backend_name(const battery_monitor_state_t *state) {
    if (state == NULL) {
        return "Unknown";
    }

    switch (state->backend) {
        case BATTERY_BACKEND_INA219:
            return "INA219 (I2C)";
        case BATTERY_BACKEND_MCP3008:
            return "MCP3008 (SPI ADC)";
        case BATTERY_BACKEND_LC709203F:
            return "LC709203F (I2C Fuel Gauge)";
        case BATTERY_BACKEND_MAX17048:
            return "MAX17048 (I2C Fuel Gauge)";
        case BATTERY_BACKEND_NONE:
            return "None (Dummy Mode)";
        default:
            return "Unknown";
    }
}

/*
 * Get default configuration
 */
battery_config_t battery_monitor_get_default_config(void) {
    battery_config_t config = {
        .voltage_full = DEFAULT_VOLTAGE_FULL,
        .voltage_empty = DEFAULT_VOLTAGE_EMPTY,
        .voltage_critical = DEFAULT_VOLTAGE_CRITICAL,
        .adc_channel = DEFAULT_ADC_CHANNEL,
        .voltage_divider_ratio = DEFAULT_VOLTAGE_DIVIDER,
        .i2c_address = 0,  /* Auto-detect */
        .update_interval_ms = DEFAULT_UPDATE_INTERVAL
    };

    return config;
}
