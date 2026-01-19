/*
 * battery_monitor.h - E-Reader Battery Monitoring
 *
 * This module provides battery voltage and capacity monitoring for
 * e-readers powered by LiPo/Li-ion batteries. Supports multiple
 * hardware backends:
 *
 * 1. I2C-based battery HATs (INA219, LC709203F, MAX17048, etc.)
 * 2. SPI-based ADC chips (MCP3008, MCP3208, MCP3002)
 * 3. No hardware (dummy mode for USB-powered operation)
 *
 * The module automatically detects available hardware at initialization
 * and provides a unified API for battery status regardless of backend.
 *
 * Hardware Detection Priority:
 * 1. Check for I2C battery monitor chips (INA219 at 0x40, 0x41, 0x44, 0x45)
 * 2. Check for SPI ADC chips (MCP3008/MCP3002 on SPI0)
 * 3. Fall back to dummy mode (reports 100% always)
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

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Battery monitoring backend types
 */
typedef enum {
    BATTERY_BACKEND_NONE = 0,      /* No hardware detected (dummy mode) */
    BATTERY_BACKEND_INA219,         /* INA219 I2C power monitor */
    BATTERY_BACKEND_MCP3008,        /* MCP3008 SPI ADC with voltage divider */
    BATTERY_BACKEND_LC709203F,      /* LC709203F I2C battery fuel gauge */
    BATTERY_BACKEND_MAX17048        /* MAX17048 I2C battery fuel gauge */
} battery_backend_type_t;

/*
 * Battery status information
 */
typedef struct {
    float voltage_mv;          /* Battery voltage in millivolts */
    float current_ma;          /* Current draw in milliamps (if supported) */
    float power_mw;            /* Power consumption in milliwatts (if supported) */
    uint8_t percentage;        /* Battery charge percentage (0-100) */
    bool is_charging;          /* True if battery is charging */
    bool is_low;               /* True if battery is critically low */
    bool is_valid;             /* True if reading is valid */
} battery_status_t;

/*
 * Battery monitor configuration
 */
typedef struct {
    /* Voltage thresholds (in millivolts) */
    float voltage_full;        /* Voltage when battery is full (default: 4200mV for LiPo) */
    float voltage_empty;       /* Voltage when battery is empty (default: 3200mV for LiPo) */
    float voltage_critical;    /* Critical low voltage threshold (default: 3300mV) */

    /* ADC configuration (for SPI backends) */
    uint8_t adc_channel;       /* ADC channel to read (default: 0) */
    float voltage_divider_ratio; /* Voltage divider ratio (default: 2.0 for 1:1 divider) */

    /* I2C configuration */
    uint8_t i2c_address;       /* I2C device address (default: auto-detect) */

    /* Update rate */
    uint32_t update_interval_ms; /* Milliseconds between updates (default: 60000 = 1 minute) */
} battery_config_t;

/*
 * Battery monitor state
 */
typedef struct {
    battery_backend_type_t backend;  /* Detected hardware backend */
    battery_config_t config;          /* Configuration settings */
    battery_status_t status;          /* Current battery status */
    uint64_t last_update_ms;          /* Timestamp of last update */
    void *backend_data;               /* Backend-specific private data */
} battery_monitor_state_t;

/*
 * Initialize battery monitor
 *
 * Automatically detects available battery monitoring hardware and
 * initializes the appropriate backend. Falls back to dummy mode if
 * no hardware is detected.
 *
 * Parameters:
 *   config - Configuration settings (may be NULL for defaults)
 *
 * Returns:
 *   Pointer to battery monitor state on success
 *   NULL on critical failure
 */
battery_monitor_state_t* battery_monitor_init(const battery_config_t *config);

/*
 * Clean up battery monitor and free resources
 *
 * Parameters:
 *   state - Battery monitor state (may be NULL)
 */
void battery_monitor_cleanup(battery_monitor_state_t *state);

/*
 * Update battery status (call periodically)
 *
 * Reads current battery voltage, current, and percentage.
 * Should be called from main loop, but respects update_interval_ms
 * to avoid excessive reads.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   0 on success
 *   -1 on error (status.is_valid will be false)
 */
int battery_monitor_update(battery_monitor_state_t *state);

/*
 * Get current battery status
 *
 * Returns the most recent battery status reading.
 * Call battery_monitor_update() first to refresh.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   Battery status structure (status.is_valid indicates if data is current)
 */
battery_status_t battery_monitor_get_status(const battery_monitor_state_t *state);

/*
 * Get battery percentage (0-100)
 *
 * Convenience function to get just the percentage.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   Battery percentage (0-100)
 *   100 if no hardware detected (dummy mode)
 */
uint8_t battery_monitor_get_percentage(const battery_monitor_state_t *state);

/*
 * Get battery voltage in millivolts
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   Battery voltage in millivolts
 *   0.0 if invalid or no hardware
 */
float battery_monitor_get_voltage(const battery_monitor_state_t *state);

/*
 * Check if battery is critically low
 *
 * Returns true if battery voltage is below critical threshold.
 * System should save state and shut down when this returns true.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   true if battery is critically low
 *   false otherwise
 */
bool battery_monitor_is_critical(const battery_monitor_state_t *state);

/*
 * Check if battery is charging
 *
 * Only supported by some backends (INA219, fuel gauge chips).
 * Always returns false for ADC-based monitoring.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   true if battery is charging
 *   false otherwise or if not supported
 */
bool battery_monitor_is_charging(const battery_monitor_state_t *state);

/*
 * Get backend type name (for debugging)
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   Human-readable backend name
 */
const char* battery_monitor_get_backend_name(const battery_monitor_state_t *state);

/*
 * Force a battery reading update
 *
 * Bypasses the update_interval_ms throttling and forces an
 * immediate hardware read.
 *
 * Parameters:
 *   state - Battery monitor state
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int battery_monitor_force_update(battery_monitor_state_t *state);

/*
 * Get default battery configuration
 *
 * Returns a configuration structure with sensible defaults for
 * single-cell LiPo batteries (3.7V nominal).
 *
 * Returns:
 *   Default configuration structure
 */
battery_config_t battery_monitor_get_default_config(void);

#endif /* BATTERY_MONITOR_H */
