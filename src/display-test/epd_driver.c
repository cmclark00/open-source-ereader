/*
 * epd_driver.c - Waveshare 4.2" E-Paper Display Driver Implementation
 *
 * Driver for Waveshare 4.2" e-paper display (400x300, B/W)
 * Controller: UC8176/IL0398
 * Interface: SPI
 *
 * Copyright (C) 2024-2026 Open Source E-Reader Project Contributors
 *
 * This file is part of the Open Source E-Reader project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: E-Reader Project
 */

#include "epd_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include <time.h>
#include <errno.h>

/* GPIO access via sysfs (simple, legacy method) */
#define GPIO_PATH "/sys/class/gpio"

/* Private variables */
static int spi_fd = -1;
static uint8_t framebuffer[EPD_BUFFER_SIZE];

/* GPIO file descriptors */
static int gpio_rst_fd = -1;
static int gpio_dc_fd = -1;
static int gpio_busy_fd = -1;

/* Helper function to sleep in milliseconds */
static void delay_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/* GPIO functions using sysfs */
static int gpio_export(int pin) {
    char buf[64];
    int fd = open(GPIO_PATH "/export", O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf));
    close(fd);
    delay_ms(10);  /* Allow time for sysfs entry to appear */
    return 0;
}

static int gpio_unexport(int pin) {
    char buf[64];
    int fd = open(GPIO_PATH "/unexport", O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}

static int gpio_set_direction(int pin, const char *direction) {
    char path[128];
    snprintf(path, sizeof(path), GPIO_PATH "/gpio%d/direction", pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    write(fd, direction, strlen(direction));
    close(fd);
    return 0;
}

static int gpio_open_value(int pin, int flags) {
    char path[128];
    snprintf(path, sizeof(path), GPIO_PATH "/gpio%d/value", pin);
    return open(path, flags);
}

static int gpio_write(int fd, int value) {
    char buf[2];
    buf[0] = value ? '1' : '0';
    buf[1] = '\0';
    lseek(fd, 0, SEEK_SET);
    return write(fd, buf, 1) == 1 ? 0 : -1;
}

static int gpio_read(int fd) {
    char buf[2];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, 1) != 1) {
        return -1;
    }
    return buf[0] == '1' ? 1 : 0;
}

/* SPI communication functions */
static int spi_transfer(uint8_t *data, int len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)data,
        .rx_buf = 0,
        .len = len,
        .speed_hz = SPI_SPEED_HZ,
        .bits_per_word = 8,
    };

    return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
}

static int epd_send_command(uint8_t cmd) {
    gpio_write(gpio_dc_fd, 0);  /* DC low = command */
    return spi_transfer(&cmd, 1);
}

static int epd_send_data(uint8_t data) {
    gpio_write(gpio_dc_fd, 1);  /* DC high = data */
    return spi_transfer(&data, 1);
}

static int epd_send_data_buffer(uint8_t *data, int len) {
    gpio_write(gpio_dc_fd, 1);  /* DC high = data */
    return spi_transfer(data, len);
}

/* Wait for BUSY pin to go low (display ready) */
int epd_wait_ready(int timeout_ms) {
    int elapsed = 0;
    while (elapsed < timeout_ms) {
        int busy = gpio_read(gpio_busy_fd);
        if (busy == 0) {  /* BUSY low = ready */
            return 0;
        }
        delay_ms(10);
        elapsed += 10;
    }
    fprintf(stderr, "EPD: Timeout waiting for ready\n");
    return -1;
}

/* Hardware reset */
static void epd_reset(void) {
    gpio_write(gpio_rst_fd, 1);
    delay_ms(20);
    gpio_write(gpio_rst_fd, 0);
    delay_ms(2);
    gpio_write(gpio_rst_fd, 1);
    delay_ms(20);
}

/* Initialize GPIO pins */
static int epd_gpio_init(void) {
    /* Export GPIO pins */
    if (gpio_export(PIN_RST) < 0 ||
        gpio_export(PIN_DC) < 0 ||
        gpio_export(PIN_BUSY) < 0) {
        fprintf(stderr, "EPD: Failed to export GPIO pins (may already be exported)\n");
        /* Continue anyway - pins might already be exported */
    }

    /* Set directions */
    if (gpio_set_direction(PIN_RST, "out") < 0 ||
        gpio_set_direction(PIN_DC, "out") < 0 ||
        gpio_set_direction(PIN_BUSY, "in") < 0) {
        fprintf(stderr, "EPD: Failed to set GPIO directions\n");
        return -1;
    }

    /* Open value files */
    gpio_rst_fd = gpio_open_value(PIN_RST, O_WRONLY);
    gpio_dc_fd = gpio_open_value(PIN_DC, O_WRONLY);
    gpio_busy_fd = gpio_open_value(PIN_BUSY, O_RDONLY);

    if (gpio_rst_fd < 0 || gpio_dc_fd < 0 || gpio_busy_fd < 0) {
        fprintf(stderr, "EPD: Failed to open GPIO value files\n");
        return -1;
    }

    /* Set initial states */
    gpio_write(gpio_rst_fd, 1);
    gpio_write(gpio_dc_fd, 0);

    return 0;
}

/* Initialize SPI */
static int epd_spi_init(void) {
    uint8_t mode = SPI_MODE;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED_HZ;

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        fprintf(stderr, "EPD: Failed to open SPI device %s: %s\n",
                SPI_DEVICE, strerror(errno));
        return -1;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        fprintf(stderr, "EPD: Failed to configure SPI\n");
        close(spi_fd);
        spi_fd = -1;
        return -1;
    }

    return 0;
}

/* Initialize the driver */
int epd_init(void) {
    /* Initialize GPIO */
    if (epd_gpio_init() < 0) {
        return -1;
    }

    /* Initialize SPI */
    if (epd_spi_init() < 0) {
        return -1;
    }

    /* Clear framebuffer */
    memset(framebuffer, 0xFF, EPD_BUFFER_SIZE);  /* White */

    printf("EPD: Driver initialized successfully\n");
    return 0;
}

/* Initialize the display hardware */
int epd_display_init(void) {
    printf("EPD: Initializing display...\n");

    /* Hardware reset */
    epd_reset();
    epd_wait_ready(5000);

    /* Initialization sequence for UC8176/IL0398 */
    /* Based on Waveshare example code */

    /* Panel setting: LUT from OTP, B/W mode, shift right, scan down */
    epd_send_command(CMD_PANEL_SETTING);
    epd_send_data(0x1F);  /* KW-3f, KWR-2F, BWROTP 0f, BWOTP 1f */

    /* Power setting */
    epd_send_command(CMD_POWER_SETTING);
    epd_send_data(0x03);  /* VDS_EN, VDG_EN */
    epd_send_data(0x00);  /* VCOM_HV, VGHL_LV[1], VGHL_LV[0] */
    epd_send_data(0x2B);  /* VDH */
    epd_send_data(0x2B);  /* VDL */
    epd_send_data(0x09);  /* VDHR */

    /* Booster soft start */
    epd_send_command(CMD_BOOSTER_SOFT_START);
    epd_send_data(0x07);
    epd_send_data(0x07);
    epd_send_data(0x17);

    /* Power on */
    epd_send_command(CMD_POWER_ON);
    epd_wait_ready(5000);

    /* Panel setting */
    epd_send_command(CMD_PANEL_SETTING);
    epd_send_data(0x1F);

    /* PLL control - 100Hz */
    epd_send_command(CMD_PLL_CONTROL);
    epd_send_data(0x3C);

    /* Resolution setting */
    epd_send_command(CMD_RESOLUTION_SETTING);
    epd_send_data((EPD_WIDTH >> 8) & 0xFF);  /* Width high byte */
    epd_send_data(EPD_WIDTH & 0xFF);         /* Width low byte */
    epd_send_data((EPD_HEIGHT >> 8) & 0xFF); /* Height high byte */
    epd_send_data(EPD_HEIGHT & 0xFF);        /* Height low byte */

    /* VCOM and data interval setting */
    epd_send_command(CMD_VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_data(0x97);  /* WBmode:VBDF 17|D7 VBDW 97, VBDB 57, WBRmode:VBDF F7, VBDW 77, VBDB 37, VBDR B7 */

    printf("EPD: Display initialization complete\n");
    return 0;
}

/* Clear display to specified color */
int epd_clear(uint8_t color) {
    printf("EPD: Clearing display to %s...\n",
           color == COLOR_BLACK ? "black" : "white");

    /* Fill framebuffer */
    memset(framebuffer, color, EPD_BUFFER_SIZE);

    /* Send framebuffer to display */
    epd_send_command(CMD_DATA_START_TRANSMISSION_1);
    for (int i = 0; i < EPD_BUFFER_SIZE; i++) {
        epd_send_data(0xFF);  /* Old data (white) */
    }

    epd_send_command(CMD_DATA_START_TRANSMISSION_2);
    epd_send_data_buffer(framebuffer, EPD_BUFFER_SIZE);

    /* Refresh display */
    epd_send_command(CMD_DISPLAY_REFRESH);
    delay_ms(100);
    epd_wait_ready(10000);

    printf("EPD: Clear complete\n");
    return 0;
}

/* Refresh display with current framebuffer */
int epd_refresh(void) {
    printf("EPD: Refreshing display...\n");

    /* Send old data (for better refresh) */
    epd_send_command(CMD_DATA_START_TRANSMISSION_1);
    for (int i = 0; i < EPD_BUFFER_SIZE; i++) {
        epd_send_data(0xFF);  /* White */
    }

    /* Send new data */
    epd_send_command(CMD_DATA_START_TRANSMISSION_2);
    epd_send_data_buffer(framebuffer, EPD_BUFFER_SIZE);

    /* Trigger refresh */
    epd_send_command(CMD_DISPLAY_REFRESH);
    delay_ms(100);
    epd_wait_ready(10000);

    printf("EPD: Refresh complete\n");
    return 0;
}

/* Put display to sleep */
int epd_sleep(void) {
    printf("EPD: Entering deep sleep mode\n");

    epd_send_command(CMD_VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_data(0xF7);

    epd_send_command(CMD_POWER_OFF);
    epd_wait_ready(5000);

    epd_send_command(CMD_DEEP_SLEEP);
    epd_send_data(0xA5);  /* Check code */

    return 0;
}

/* Set a pixel in the framebuffer */
void epd_set_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= EPD_WIDTH || y < 0 || y >= EPD_HEIGHT) {
        return;
    }

    /* Calculate byte and bit position */
    /* Display is organized as horizontal bytes */
    int byte_index = (y * EPD_WIDTH + x) / 8;
    int bit_index = 7 - (x % 8);

    if (color == COLOR_BLACK) {
        framebuffer[byte_index] &= ~(1 << bit_index);  /* Clear bit = black */
    } else {
        framebuffer[byte_index] |= (1 << bit_index);   /* Set bit = white */
    }
}

/* Get framebuffer pointer */
uint8_t* epd_get_framebuffer(void) {
    return framebuffer;
}

/* Draw text - implementation in font rendering */
int epd_draw_text(int x, int y, const char *text, uint8_t color) {
    /* Forward declaration - implemented with font support */
    extern int font_draw_text(int x, int y, const char *text, uint8_t color);
    return font_draw_text(x, y, text, color);
}

/* Cleanup */
void epd_cleanup(void) {
    printf("EPD: Cleaning up...\n");

    /* Close GPIO files */
    if (gpio_rst_fd >= 0) close(gpio_rst_fd);
    if (gpio_dc_fd >= 0) close(gpio_dc_fd);
    if (gpio_busy_fd >= 0) close(gpio_busy_fd);

    /* Unexport GPIO pins */
    gpio_unexport(PIN_RST);
    gpio_unexport(PIN_DC);
    gpio_unexport(PIN_BUSY);

    /* Close SPI */
    if (spi_fd >= 0) {
        close(spi_fd);
    }

    printf("EPD: Cleanup complete\n");
}
