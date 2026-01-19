/*
 * settings_menu.c - E-Reader Settings Menu UI Implementation
 *
 * Implements the settings menu UI for configuring e-reader preferences.
 *
 * Phase 05: Polish and Advanced Features
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

#include "settings_menu.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal helper function prototypes */
static void settings_menu_draw_separator_line(framebuffer_t *fb, int line_number);

/*
 * Settings Menu Initialization and Cleanup
 */

settings_menu_state_t* settings_menu_create(settings_t *settings) {
    if (!settings) {
        return NULL;
    }

    settings_menu_state_t *menu = (settings_menu_state_t*)calloc(1, sizeof(settings_menu_state_t));
    if (!menu) {
        return NULL;
    }

    menu->settings = settings;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->visible_items = SETTINGS_VISIBLE_ITEMS;
    menu->total_items = SETTING_ITEM_COUNT;
    menu->needs_redraw = true;
    menu->settings_changed = false;
    menu->refresh_counter = 0;

    return menu;
}

void settings_menu_free(settings_menu_state_t *menu) {
    if (menu) {
        /* Note: We don't free settings as they're not owned by menu */
        free(menu);
    }
}

void settings_menu_reset(settings_menu_state_t *menu) {
    if (!menu) {
        return;
    }

    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->needs_redraw = true;
    menu->settings_changed = false;
    menu->refresh_counter = 0;
}

/*
 * Settings Menu Rendering
 */

int settings_menu_render(settings_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return SETTINGS_MENU_ERROR_NULL_POINTER;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Render status bar */
    if (settings_menu_render_status_bar(menu, fb) != SETTINGS_MENU_SUCCESS) {
        return SETTINGS_MENU_ERROR_RENDER_FAILED;
    }

    /* Render separator after status bar */
    settings_menu_draw_separator_line(fb, SETTINGS_SEPARATOR_1_LINE);

    /* Render settings items */
    if (settings_menu_render_items(menu, fb) != SETTINGS_MENU_SUCCESS) {
        return SETTINGS_MENU_ERROR_RENDER_FAILED;
    }

    /* Render separator before hints */
    settings_menu_draw_separator_line(fb, SETTINGS_SEPARATOR_2_LINE);

    /* Render control hints */
    if (settings_menu_render_hints(fb) != SETTINGS_MENU_SUCCESS) {
        return SETTINGS_MENU_ERROR_RENDER_FAILED;
    }

    menu->needs_redraw = false;

    /* Increment refresh counter for periodic full refreshes (prevent ghosting) */
    menu->refresh_counter++;

    return SETTINGS_MENU_SUCCESS;
}

int settings_menu_render_status_bar(settings_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return SETTINGS_MENU_ERROR_NULL_POINTER;
    }

    /* Render title centered on status bar */
    int title_len = strlen(SETTINGS_MENU_TITLE);
    int x_offset = (fb->width / FONT_WIDTH - title_len) / 2;

    text_render_string(fb, SETTINGS_MENU_TITLE, x_offset, SETTINGS_STATUS_BAR_LINE, COLOR_BLACK);

    return SETTINGS_MENU_SUCCESS;
}

int settings_menu_render_items(settings_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return SETTINGS_MENU_ERROR_NULL_POINTER;
    }

    /* Render each visible setting item */
    for (int i = 0; i < menu->visible_items && (menu->scroll_offset + i) < menu->total_items; i++) {
        int item_index = menu->scroll_offset + i;
        int line_number = SETTINGS_FIRST_ITEM_LINE + i;

        char line_buffer[64];
        char value_buffer[32];

        /* Get setting name */
        const char *setting_name = settings_menu_get_setting_name((setting_item_t)item_index);

        /* Get current value */
        settings_menu_get_value_string(menu, (setting_item_t)item_index, value_buffer, sizeof(value_buffer));

        /* Format: "> Setting Name: Value" or "  Setting Name: Value" */
        if (item_index == menu->selected_index) {
            snprintf(line_buffer, sizeof(line_buffer), "%s%-18s: %s",
                     SETTINGS_SELECTION_MARKER, setting_name, value_buffer);
        } else {
            snprintf(line_buffer, sizeof(line_buffer), "  %-18s: %s",
                     setting_name, value_buffer);
        }

        /* Render the line */
        text_render_string(fb, line_buffer, 0, line_number, COLOR_BLACK);
    }

    return SETTINGS_MENU_SUCCESS;
}

int settings_menu_render_hints(framebuffer_t *fb) {
    if (!fb) {
        return SETTINGS_MENU_ERROR_NULL_POINTER;
    }

    /* Center the hints on the bottom line */
    int hints_len = strlen(SETTINGS_MENU_HINTS);
    int x_offset = (fb->width / FONT_WIDTH - hints_len) / 2;

    text_render_string(fb, SETTINGS_MENU_HINTS, x_offset, SETTINGS_HINTS_LINE, COLOR_BLACK);

    return SETTINGS_MENU_SUCCESS;
}

/*
 * Settings Menu Navigation and Input Handling
 */

settings_menu_action_t settings_menu_handle_event(settings_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return SETTINGS_MENU_ACTION_NONE;
    }

    /* Only handle PRESS events */
    if (event->type != BUTTON_EVENT_PRESS) {
        return SETTINGS_MENU_ACTION_NONE;
    }

    switch (event->button) {
        case BUTTON_UP:
            if (settings_menu_move_up(menu)) {
                menu->needs_redraw = true;
                return SETTINGS_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_DOWN:
            if (settings_menu_move_down(menu)) {
                menu->needs_redraw = true;
                return SETTINGS_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_SELECT:
            if (settings_menu_cycle_value(menu)) {
                menu->settings_changed = true;
                menu->needs_redraw = true;
                return SETTINGS_MENU_ACTION_VALUE_CHANGED;
            }
            break;

        case BUTTON_BACK:
            /* Save and exit */
            return SETTINGS_MENU_ACTION_SAVE_EXIT;

        case BUTTON_MENU:
            /* Also exit on MENU button */
            return SETTINGS_MENU_ACTION_SAVE_EXIT;

        default:
            break;
    }

    return SETTINGS_MENU_ACTION_NONE;
}

bool settings_menu_move_up(settings_menu_state_t *menu) {
    if (!menu) {
        return false;
    }

    if (menu->selected_index > 0) {
        menu->selected_index--;

        /* Adjust scroll offset if needed */
        if (menu->selected_index < menu->scroll_offset) {
            menu->scroll_offset = menu->selected_index;
        }

        return true;
    }

    return false;
}

bool settings_menu_move_down(settings_menu_state_t *menu) {
    if (!menu) {
        return false;
    }

    if (menu->selected_index < menu->total_items - 1) {
        menu->selected_index++;

        /* Adjust scroll offset if needed */
        if (menu->selected_index >= menu->scroll_offset + menu->visible_items) {
            menu->scroll_offset = menu->selected_index - menu->visible_items + 1;
        }

        return true;
    }

    return false;
}

bool settings_menu_cycle_value(settings_menu_state_t *menu) {
    if (!menu || !menu->settings) {
        return false;
    }

    setting_item_t item = (setting_item_t)menu->selected_index;

    switch (item) {
        case SETTING_ITEM_FONT_SIZE: {
            font_size_t current = settings_get_font_size(menu->settings);
            font_size_t next;

            if (current == FONT_SIZE_SMALL) {
                next = FONT_SIZE_MEDIUM;
            } else if (current == FONT_SIZE_MEDIUM) {
                next = FONT_SIZE_LARGE;
            } else {
                next = FONT_SIZE_SMALL;
            }

            settings_set_font_size(menu->settings, next);
            return true;
        }

        case SETTING_ITEM_LINE_SPACING: {
            line_spacing_t current = settings_get_line_spacing(menu->settings);
            line_spacing_t next;

            if (current == LINE_SPACING_SINGLE) {
                next = LINE_SPACING_1_5;
            } else if (current == LINE_SPACING_1_5) {
                next = LINE_SPACING_DOUBLE;
            } else {
                next = LINE_SPACING_SINGLE;
            }

            settings_set_line_spacing(menu->settings, next);
            return true;
        }

        case SETTING_ITEM_MARGINS: {
            margins_t current = settings_get_margins(menu->settings);
            margins_t next;

            if (current == MARGINS_NARROW) {
                next = MARGINS_NORMAL;
            } else if (current == MARGINS_NORMAL) {
                next = MARGINS_WIDE;
            } else {
                next = MARGINS_NARROW;
            }

            settings_set_margins(menu->settings, next);
            return true;
        }

        case SETTING_ITEM_DISPLAY_MODE: {
            display_mode_t current = settings_get_display_mode(menu->settings);
            display_mode_t next;

            if (current == DISPLAY_MODE_NORMAL) {
                next = DISPLAY_MODE_DARK;
            } else {
                next = DISPLAY_MODE_NORMAL;
            }

            settings_set_display_mode(menu->settings, next);
            return true;
        }

        case SETTING_ITEM_AUTO_SLEEP: {
            auto_sleep_t current = settings_get_auto_sleep(menu->settings);
            auto_sleep_t next;

            if (current == AUTO_SLEEP_NEVER) {
                next = AUTO_SLEEP_5_MIN;
            } else if (current == AUTO_SLEEP_5_MIN) {
                next = AUTO_SLEEP_10_MIN;
            } else if (current == AUTO_SLEEP_10_MIN) {
                next = AUTO_SLEEP_15_MIN;
            } else if (current == AUTO_SLEEP_15_MIN) {
                next = AUTO_SLEEP_30_MIN;
            } else {
                next = AUTO_SLEEP_NEVER;
            }

            settings_set_auto_sleep(menu->settings, next);
            return true;
        }

        case SETTING_ITEM_WIFI:
            /* WiFi is handled specially - doesn't cycle, opens WiFi menu */
            /* This will be caught by the caller to transition to WiFi state */
            return true;

        case SETTING_ITEM_ONLINE_LIBRARY:
            /* Online Library is handled specially - doesn't cycle, opens library browser */
            /* This will be caught by the caller to transition to library browser state */
            return true;

        default:
            break;
    }

    return false;
}

setting_item_t settings_menu_get_selected_item(settings_menu_state_t *menu) {
    if (!menu) {
        return SETTING_ITEM_FONT_SIZE;
    }

    return (setting_item_t)menu->selected_index;
}

/*
 * Settings Menu State Queries
 */

bool settings_menu_needs_redraw(settings_menu_state_t *menu) {
    if (!menu) {
        return false;
    }

    return menu->needs_redraw;
}

void settings_menu_clear_redraw_flag(settings_menu_state_t *menu) {
    if (menu) {
        menu->needs_redraw = false;
    }
}

bool settings_menu_settings_changed(settings_menu_state_t *menu) {
    if (!menu) {
        return false;
    }

    return menu->settings_changed;
}

int settings_menu_save_settings(settings_menu_state_t *menu) {
    if (!menu || !menu->settings) {
        return SETTINGS_MENU_ERROR_NULL_POINTER;
    }

    settings_error_t result = settings_save(menu->settings, SETTINGS_FILE);
    if (result != SETTINGS_SUCCESS) {
        return SETTINGS_MENU_ERROR_SAVE_FAILED;
    }

    menu->settings_changed = false;
    return SETTINGS_MENU_SUCCESS;
}

/*
 * Utility Functions
 */

const char* settings_menu_get_setting_name(setting_item_t item) {
    switch (item) {
        case SETTING_ITEM_FONT_SIZE:
            return "Font Size";
        case SETTING_ITEM_LINE_SPACING:
            return "Line Spacing";
        case SETTING_ITEM_MARGINS:
            return "Margins";
        case SETTING_ITEM_DISPLAY_MODE:
            return "Display Mode";
        case SETTING_ITEM_AUTO_SLEEP:
            return "Auto Sleep";
        case SETTING_ITEM_WIFI:
            return "WiFi Settings";
        case SETTING_ITEM_ONLINE_LIBRARY:
            return "Online Library";
        default:
            return "Unknown";
    }
}

void settings_menu_get_value_string(settings_menu_state_t *menu, setting_item_t item,
                                    char *buffer, size_t buffer_size) {
    if (!menu || !menu->settings || !buffer || buffer_size == 0) {
        if (buffer && buffer_size > 0) {
            buffer[0] = '\0';
        }
        return;
    }

    const char *value_str = NULL;

    switch (item) {
        case SETTING_ITEM_FONT_SIZE:
            value_str = settings_font_size_to_string(settings_get_font_size(menu->settings));
            break;
        case SETTING_ITEM_LINE_SPACING:
            value_str = settings_line_spacing_to_string(settings_get_line_spacing(menu->settings));
            break;
        case SETTING_ITEM_MARGINS:
            value_str = settings_margins_to_string(settings_get_margins(menu->settings));
            break;
        case SETTING_ITEM_DISPLAY_MODE:
            value_str = settings_display_mode_to_string(settings_get_display_mode(menu->settings));
            break;
        case SETTING_ITEM_AUTO_SLEEP:
            value_str = settings_auto_sleep_to_string(settings_get_auto_sleep(menu->settings));
            break;
        case SETTING_ITEM_WIFI:
            value_str = "Configure >";
            break;
        case SETTING_ITEM_ONLINE_LIBRARY:
            value_str = "Browse Books >";
            break;
        default:
            value_str = "Unknown";
            break;
    }

    snprintf(buffer, buffer_size, "%s", value_str);
}

const char* settings_menu_error_string(settings_menu_error_t error) {
    switch (error) {
        case SETTINGS_MENU_SUCCESS:
            return "Success";
        case SETTINGS_MENU_ERROR_NULL_POINTER:
            return "Null pointer error";
        case SETTINGS_MENU_ERROR_INVALID_STATE:
            return "Invalid state";
        case SETTINGS_MENU_ERROR_RENDER_FAILED:
            return "Render failed";
        case SETTINGS_MENU_ERROR_SAVE_FAILED:
            return "Failed to save settings";
        default:
            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

static void settings_menu_draw_separator_line(framebuffer_t *fb, int line_number) {
    if (!fb) {
        return;
    }

    /* Draw a line of dashes across the screen */
    int chars_per_line = fb->width / FONT_WIDTH;
    char separator[64];

    for (int i = 0; i < chars_per_line && i < (int)sizeof(separator) - 1; i++) {
        separator[i] = '-';
    }
    separator[chars_per_line < (int)sizeof(separator) ? chars_per_line : (int)sizeof(separator) - 1] = '\0';

    text_render_string(fb, separator, 0, line_number, COLOR_BLACK);
}
