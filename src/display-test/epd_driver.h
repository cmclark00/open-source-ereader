/*
 * epd_driver.h - Waveshare 4.2" E-Paper Display Driver Header
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

#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* Display Specifications */
#define EPD_WIDTH       400
#define EPD_HEIGHT      300
#define EPD_BUFFER_SIZE ((EPD_WIDTH * EPD_HEIGHT) / 8)  /* 1 bit per pixel */

/* GPIO Pin Definitions (BCM numbering) */
#define PIN_RST         17      /* Reset pin (GPIO 17, physical pin 11) */
#define PIN_DC          25      /* Data/Command pin (GPIO 25, physical pin 22) */
#define PIN_BUSY        24      /* Busy status pin (GPIO 24, physical pin 18) */
#define PIN_CS          8       /* Chip Select (GPIO 8, physical pin 24 - CE0) */

/* SPI Configuration */
#define SPI_DEVICE      "/dev/spidev0.0"
#define SPI_SPEED_HZ    4000000  /* 4 MHz */
#define SPI_MODE        0        /* SPI Mode 0 (CPOL=0, CPHA=0) */

/* UC8176/IL0398 Commands */
#define CMD_PANEL_SETTING                   0x00
#define CMD_POWER_SETTING                   0x01
#define CMD_POWER_OFF                       0x02
#define CMD_POWER_OFF_SEQUENCE              0x03
#define CMD_POWER_ON                        0x04
#define CMD_POWER_ON_MEASURE                0x05
#define CMD_BOOSTER_SOFT_START              0x06
#define CMD_DEEP_SLEEP                      0x07
#define CMD_DATA_START_TRANSMISSION_1       0x10
#define CMD_DATA_STOP                       0x11
#define CMD_DISPLAY_REFRESH                 0x12
#define CMD_DATA_START_TRANSMISSION_2       0x13
#define CMD_PLL_CONTROL                     0x30
#define CMD_TEMPERATURE_SENSOR_CALIBRATION  0x40
#define CMD_TEMPERATURE_SENSOR_SELECTION    0x41
#define CMD_TEMPERATURE_SENSOR_WRITE        0x42
#define CMD_TEMPERATURE_SENSOR_READ         0x43
#define CMD_VCOM_AND_DATA_INTERVAL_SETTING  0x50
#define CMD_LOW_POWER_DETECTION             0x51
#define CMD_TCON_SETTING                    0x60
#define CMD_RESOLUTION_SETTING              0x61
#define CMD_GET_STATUS                      0x71
#define CMD_AUTO_MEASURE_VCOM               0x80
#define CMD_VCOM_VALUE                      0x81
#define CMD_VCM_DC_SETTING                  0x82
#define CMD_PARTIAL_WINDOW                  0x90
#define CMD_PARTIAL_IN                      0x91
#define CMD_PARTIAL_OUT                     0x92
#define CMD_PROGRAM_MODE                    0xA0
#define CMD_ACTIVE_PROGRAM                  0xA1
#define CMD_READ_OTP_DATA                   0xA2
#define CMD_POWER_SAVING                    0xE3

/* Color Definitions */
#define COLOR_WHITE     0xFF
#define COLOR_BLACK     0x00

/* Function Prototypes */

/**
 * Initialize the e-paper display driver
 * Returns: 0 on success, -1 on error
 */
int epd_init(void);

/**
 * Clean up and release resources
 */
void epd_cleanup(void);

/**
 * Initialize the display hardware and controller
 * Returns: 0 on success, -1 on error
 */
int epd_display_init(void);

/**
 * Clear the display to a specific color
 * @param color: COLOR_WHITE or COLOR_BLACK
 * Returns: 0 on success, -1 on error
 */
int epd_clear(uint8_t color);

/**
 * Update the display with current framebuffer contents
 * Returns: 0 on success, -1 on error
 */
int epd_refresh(void);

/**
 * Put the display into deep sleep mode (low power)
 * Returns: 0 on success, -1 on error
 */
int epd_sleep(void);

/**
 * Draw text at specified position using built-in font
 * @param x: X coordinate (0-399)
 * @param y: Y coordinate (0-299)
 * @param text: Null-terminated string to display
 * @param color: COLOR_WHITE or COLOR_BLACK
 * Returns: 0 on success, -1 on error
 */
int epd_draw_text(int x, int y, const char *text, uint8_t color);

/**
 * Set a pixel in the framebuffer
 * @param x: X coordinate (0-399)
 * @param y: Y coordinate (0-299)
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void epd_set_pixel(int x, int y, uint8_t color);

/**
 * Get the framebuffer pointer for direct manipulation
 * Returns: Pointer to framebuffer (EPD_BUFFER_SIZE bytes)
 */
uint8_t* epd_get_framebuffer(void);

/**
 * Wait for the display to become ready (BUSY pin low)
 * @param timeout_ms: Maximum time to wait in milliseconds
 * Returns: 0 if ready, -1 on timeout
 */
int epd_wait_ready(int timeout_ms);

#endif /* EPD_DRIVER_H */
