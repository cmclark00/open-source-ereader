/*
 * settings_manager.c - E-Reader Settings Management Implementation
 *
 * Provides persistent storage and retrieval of user settings.
 * Settings are stored in /etc/ereader/settings.conf in key=value format.
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

#include "settings_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Maximum line length in settings file */
#define MAX_LINE_LENGTH 256

/* Trim whitespace from both ends of a string */
static void trim_whitespace(char *str) {
    if (str == NULL) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }

    /* Trim trailing whitespace */
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    *(end + 1) = '\0';

    /* Move trimmed string to beginning if needed */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/*
 * Initialize settings with defaults
 */
void settings_init_defaults(settings_t *settings) {
    if (settings == NULL) return;

    settings->font_size = FONT_SIZE_DEFAULT;
    settings->line_spacing = LINE_SPACING_DEFAULT;
    settings->margins = MARGINS_DEFAULT;
    settings->display_mode = DISPLAY_MODE_DEFAULT;
    settings->auto_sleep_minutes = AUTO_SLEEP_DEFAULT;
}

/*
 * Load settings from file
 */
settings_error_t settings_load(settings_t *settings, const char *filepath) {
    if (settings == NULL || filepath == NULL) {
        return SETTINGS_ERROR_INVALID_VALUE;
    }

    /* Initialize with defaults first */
    settings_init_defaults(settings);

    /* Open settings file */
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        /* File doesn't exist, use defaults */
        return SETTINGS_ERROR_FILE_NOT_FOUND;
    }

    /* Read settings line by line */
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    bool had_errors = false;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;

        /* Skip empty lines and comments */
        trim_whitespace(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        /* Parse key=value */
        char *equals = strchr(line, '=');
        if (equals == NULL) {
            fprintf(stderr, "Settings: Invalid format at line %d: %s\n", line_num, line);
            had_errors = true;
            continue;
        }

        /* Split into key and value */
        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        trim_whitespace(key);
        trim_whitespace(value);

        /* Parse setting based on key */
        if (strcmp(key, "font_size") == 0) {
            font_size_t size;
            if (settings_font_size_from_string(value, &size)) {
                settings->font_size = size;
            } else {
                fprintf(stderr, "Settings: Invalid font_size value: %s\n", value);
                had_errors = true;
            }
        } else if (strcmp(key, "line_spacing") == 0) {
            line_spacing_t spacing;
            if (settings_line_spacing_from_string(value, &spacing)) {
                settings->line_spacing = spacing;
            } else {
                fprintf(stderr, "Settings: Invalid line_spacing value: %s\n", value);
                had_errors = true;
            }
        } else if (strcmp(key, "margins") == 0) {
            margins_t margins;
            if (settings_margins_from_string(value, &margins)) {
                settings->margins = margins;
            } else {
                fprintf(stderr, "Settings: Invalid margins value: %s\n", value);
                had_errors = true;
            }
        } else if (strcmp(key, "display_mode") == 0) {
            display_mode_t mode;
            if (settings_display_mode_from_string(value, &mode)) {
                settings->display_mode = mode;
            } else {
                fprintf(stderr, "Settings: Invalid display_mode value: %s\n", value);
                had_errors = true;
            }
        } else if (strcmp(key, "auto_sleep_minutes") == 0) {
            auto_sleep_t minutes;
            if (settings_auto_sleep_from_string(value, &minutes)) {
                settings->auto_sleep_minutes = minutes;
            } else {
                fprintf(stderr, "Settings: Invalid auto_sleep_minutes value: %s\n", value);
                had_errors = true;
            }
        } else {
            fprintf(stderr, "Settings: Unknown setting: %s\n", key);
            had_errors = true;
        }
    }

    fclose(fp);

    return had_errors ? SETTINGS_ERROR_INVALID_FORMAT : SETTINGS_SUCCESS;
}

/*
 * Save settings to file
 */
settings_error_t settings_save(const settings_t *settings, const char *filepath) {
    if (settings == NULL || filepath == NULL) {
        return SETTINGS_ERROR_INVALID_VALUE;
    }

    /* Open settings file for writing */
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open settings file for writing: %s\n", strerror(errno));
        return SETTINGS_ERROR_WRITE_FAILED;
    }

    /* Write header comment */
    fprintf(fp, "# E-Reader Settings\n");
    fprintf(fp, "# This file is automatically generated\n");
    fprintf(fp, "# Format: key=value\n\n");

    /* Write settings */
    fprintf(fp, "font_size=%s\n", settings_font_size_to_string(settings->font_size));
    fprintf(fp, "line_spacing=%s\n", settings_line_spacing_to_string(settings->line_spacing));
    fprintf(fp, "margins=%s\n", settings_margins_to_string(settings->margins));
    fprintf(fp, "display_mode=%s\n", settings_display_mode_to_string(settings->display_mode));
    fprintf(fp, "auto_sleep_minutes=%s\n", settings_auto_sleep_to_string(settings->auto_sleep_minutes));

    fclose(fp);
    return SETTINGS_SUCCESS;
}

/*
 * Getters
 */
font_size_t settings_get_font_size(const settings_t *settings) {
    return settings ? settings->font_size : FONT_SIZE_DEFAULT;
}

line_spacing_t settings_get_line_spacing(const settings_t *settings) {
    return settings ? settings->line_spacing : LINE_SPACING_DEFAULT;
}

margins_t settings_get_margins(const settings_t *settings) {
    return settings ? settings->margins : MARGINS_DEFAULT;
}

display_mode_t settings_get_display_mode(const settings_t *settings) {
    return settings ? settings->display_mode : DISPLAY_MODE_DEFAULT;
}

auto_sleep_t settings_get_auto_sleep(const settings_t *settings) {
    return settings ? settings->auto_sleep_minutes : AUTO_SLEEP_DEFAULT;
}

/*
 * Setters
 */
void settings_set_font_size(settings_t *settings, font_size_t size) {
    if (settings) settings->font_size = size;
}

void settings_set_line_spacing(settings_t *settings, line_spacing_t spacing) {
    if (settings) settings->line_spacing = spacing;
}

void settings_set_margins(settings_t *settings, margins_t margins) {
    if (settings) settings->margins = margins;
}

void settings_set_display_mode(settings_t *settings, display_mode_t mode) {
    if (settings) settings->display_mode = mode;
}

void settings_set_auto_sleep(settings_t *settings, auto_sleep_t minutes) {
    if (settings) settings->auto_sleep_minutes = minutes;
}

/*
 * Convert settings values to strings
 */
const char* settings_font_size_to_string(font_size_t size) {
    switch (size) {
        case FONT_SIZE_SMALL:  return "small";
        case FONT_SIZE_MEDIUM: return "medium";
        case FONT_SIZE_LARGE:  return "large";
        default:               return "medium";
    }
}

const char* settings_line_spacing_to_string(line_spacing_t spacing) {
    switch (spacing) {
        case LINE_SPACING_SINGLE: return "single";
        case LINE_SPACING_1_5:    return "1.5";
        case LINE_SPACING_DOUBLE: return "double";
        default:                  return "single";
    }
}

const char* settings_margins_to_string(margins_t margins) {
    switch (margins) {
        case MARGINS_NARROW: return "narrow";
        case MARGINS_NORMAL: return "normal";
        case MARGINS_WIDE:   return "wide";
        default:             return "normal";
    }
}

const char* settings_display_mode_to_string(display_mode_t mode) {
    switch (mode) {
        case DISPLAY_MODE_NORMAL: return "normal";
        case DISPLAY_MODE_DARK:   return "dark";
        default:                  return "normal";
    }
}

const char* settings_auto_sleep_to_string(auto_sleep_t minutes) {
    static char buffer[16];
    if (minutes == AUTO_SLEEP_NEVER) {
        return "never";
    }
    snprintf(buffer, sizeof(buffer), "%d", (int)minutes);
    return buffer;
}

/*
 * Convert strings to settings values
 */
bool settings_font_size_from_string(const char *str, font_size_t *size) {
    if (str == NULL || size == NULL) return false;

    if (strcmp(str, "small") == 0) {
        *size = FONT_SIZE_SMALL;
        return true;
    } else if (strcmp(str, "medium") == 0) {
        *size = FONT_SIZE_MEDIUM;
        return true;
    } else if (strcmp(str, "large") == 0) {
        *size = FONT_SIZE_LARGE;
        return true;
    }
    return false;
}

bool settings_line_spacing_from_string(const char *str, line_spacing_t *spacing) {
    if (str == NULL || spacing == NULL) return false;

    if (strcmp(str, "single") == 0) {
        *spacing = LINE_SPACING_SINGLE;
        return true;
    } else if (strcmp(str, "1.5") == 0) {
        *spacing = LINE_SPACING_1_5;
        return true;
    } else if (strcmp(str, "double") == 0) {
        *spacing = LINE_SPACING_DOUBLE;
        return true;
    }
    return false;
}

bool settings_margins_from_string(const char *str, margins_t *margins) {
    if (str == NULL || margins == NULL) return false;

    if (strcmp(str, "narrow") == 0) {
        *margins = MARGINS_NARROW;
        return true;
    } else if (strcmp(str, "normal") == 0) {
        *margins = MARGINS_NORMAL;
        return true;
    } else if (strcmp(str, "wide") == 0) {
        *margins = MARGINS_WIDE;
        return true;
    }
    return false;
}

bool settings_display_mode_from_string(const char *str, display_mode_t *mode) {
    if (str == NULL || mode == NULL) return false;

    if (strcmp(str, "normal") == 0) {
        *mode = DISPLAY_MODE_NORMAL;
        return true;
    } else if (strcmp(str, "dark") == 0) {
        *mode = DISPLAY_MODE_DARK;
        return true;
    }
    return false;
}

bool settings_auto_sleep_from_string(const char *str, auto_sleep_t *minutes) {
    if (str == NULL || minutes == NULL) return false;

    if (strcmp(str, "never") == 0) {
        *minutes = AUTO_SLEEP_NEVER;
        return true;
    }

    int value = atoi(str);
    if (value == 5 || value == 10 || value == 15 || value == 30) {
        *minutes = (auto_sleep_t)value;
        return true;
    }

    return false;
}

/*
 * Convert error code to string
 */
const char* settings_error_to_string(settings_error_t error) {
    switch (error) {
        case SETTINGS_SUCCESS:              return "Success";
        case SETTINGS_ERROR_FILE_NOT_FOUND: return "Settings file not found";
        case SETTINGS_ERROR_INVALID_FORMAT: return "Invalid settings file format";
        case SETTINGS_ERROR_WRITE_FAILED:   return "Failed to write settings file";
        case SETTINGS_ERROR_INVALID_VALUE:  return "Invalid parameter value";
        default:                            return "Unknown error";
    }
}
