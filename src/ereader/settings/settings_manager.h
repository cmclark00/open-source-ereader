/*
 * settings_manager.h - E-Reader Settings Management
 *
 * This module provides persistent storage and retrieval of user settings.
 * Settings are stored in /etc/ereader/settings.conf in key=value format.
 *
 * Supported settings:
 * - font_size: small, medium, large
 * - line_spacing: single, 1.5, double
 * - margins: narrow, normal, wide
 * - display_mode: normal, dark (for future inverted rendering)
 * - auto_sleep_minutes: 5, 10, 15, 30, never
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

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <stdbool.h>

/*
 * Settings file location
 */
#define SETTINGS_FILE "/etc/ereader/settings.conf"

/*
 * Font size options
 */
typedef enum {
    FONT_SIZE_SMALL = 0,
    FONT_SIZE_MEDIUM = 1,
    FONT_SIZE_LARGE = 2,
    FONT_SIZE_DEFAULT = FONT_SIZE_MEDIUM
} font_size_t;

/*
 * Line spacing options
 */
typedef enum {
    LINE_SPACING_SINGLE = 0,
    LINE_SPACING_1_5 = 1,
    LINE_SPACING_DOUBLE = 2,
    LINE_SPACING_DEFAULT = LINE_SPACING_SINGLE
} line_spacing_t;

/*
 * Margin options
 */
typedef enum {
    MARGINS_NARROW = 0,
    MARGINS_NORMAL = 1,
    MARGINS_WIDE = 2,
    MARGINS_DEFAULT = MARGINS_NORMAL
} margins_t;

/*
 * Display mode options
 */
typedef enum {
    DISPLAY_MODE_NORMAL = 0,
    DISPLAY_MODE_DARK = 1,
    DISPLAY_MODE_DEFAULT = DISPLAY_MODE_NORMAL
} display_mode_t;

/*
 * Auto-sleep timeout options
 */
typedef enum {
    AUTO_SLEEP_5_MIN = 5,
    AUTO_SLEEP_10_MIN = 10,
    AUTO_SLEEP_15_MIN = 15,
    AUTO_SLEEP_30_MIN = 30,
    AUTO_SLEEP_NEVER = 0,
    AUTO_SLEEP_DEFAULT = AUTO_SLEEP_15_MIN
} auto_sleep_t;

/*
 * Settings structure
 *
 * Contains all user-configurable settings
 */
typedef struct {
    font_size_t font_size;
    line_spacing_t line_spacing;
    margins_t margins;
    display_mode_t display_mode;
    auto_sleep_t auto_sleep_minutes;
} settings_t;

/*
 * Settings error codes
 */
typedef enum {
    SETTINGS_SUCCESS = 0,
    SETTINGS_ERROR_FILE_NOT_FOUND = 1,
    SETTINGS_ERROR_INVALID_FORMAT = 2,
    SETTINGS_ERROR_WRITE_FAILED = 3,
    SETTINGS_ERROR_INVALID_VALUE = 4
} settings_error_t;

/*
 * Initialize settings with defaults
 *
 * Parameters:
 *   settings - Settings structure to initialize
 */
void settings_init_defaults(settings_t *settings);

/*
 * Load settings from file
 *
 * If the file doesn't exist or is invalid, defaults are used.
 *
 * Parameters:
 *   settings - Settings structure to populate
 *   filepath - Path to settings file (use SETTINGS_FILE for default)
 *
 * Returns:
 *   SETTINGS_SUCCESS on success
 *   SETTINGS_ERROR_FILE_NOT_FOUND if file doesn't exist (defaults loaded)
 *   SETTINGS_ERROR_INVALID_FORMAT if file format is invalid (defaults loaded)
 */
settings_error_t settings_load(settings_t *settings, const char *filepath);

/*
 * Save settings to file
 *
 * Parameters:
 *   settings - Settings structure to save
 *   filepath - Path to settings file (use SETTINGS_FILE for default)
 *
 * Returns:
 *   SETTINGS_SUCCESS on success
 *   SETTINGS_ERROR_WRITE_FAILED on failure
 */
settings_error_t settings_save(const settings_t *settings, const char *filepath);

/*
 * Getters for individual settings
 */
font_size_t settings_get_font_size(const settings_t *settings);
line_spacing_t settings_get_line_spacing(const settings_t *settings);
margins_t settings_get_margins(const settings_t *settings);
display_mode_t settings_get_display_mode(const settings_t *settings);
auto_sleep_t settings_get_auto_sleep(const settings_t *settings);

/*
 * Setters for individual settings
 */
void settings_set_font_size(settings_t *settings, font_size_t size);
void settings_set_line_spacing(settings_t *settings, line_spacing_t spacing);
void settings_set_margins(settings_t *settings, margins_t margins);
void settings_set_display_mode(settings_t *settings, display_mode_t mode);
void settings_set_auto_sleep(settings_t *settings, auto_sleep_t minutes);

/*
 * Convert settings values to human-readable strings
 */
const char* settings_font_size_to_string(font_size_t size);
const char* settings_line_spacing_to_string(line_spacing_t spacing);
const char* settings_margins_to_string(margins_t margins);
const char* settings_display_mode_to_string(display_mode_t mode);
const char* settings_auto_sleep_to_string(auto_sleep_t minutes);

/*
 * Convert strings to settings values
 *
 * Returns:
 *   true if conversion successful
 *   false if string is invalid (value unchanged)
 */
bool settings_font_size_from_string(const char *str, font_size_t *size);
bool settings_line_spacing_from_string(const char *str, line_spacing_t *spacing);
bool settings_margins_from_string(const char *str, margins_t *margins);
bool settings_display_mode_from_string(const char *str, display_mode_t *mode);
bool settings_auto_sleep_from_string(const char *str, auto_sleep_t *minutes);

/*
 * Convert error code to string
 *
 * Parameters:
 *   error - Error code
 *
 * Returns:
 *   String representation of error
 */
const char* settings_error_to_string(settings_error_t error);

#endif /* SETTINGS_MANAGER_H */
