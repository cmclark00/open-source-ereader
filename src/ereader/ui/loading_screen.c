/*
 * loading_screen.c - Loading Screen Implementation
 *
 * Provides visual feedback during slow operations with spinner,
 * progress bar, and status messages.
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

#include "loading_screen.h"
#include "ui_components.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Layout constants */
#define LOADING_CENTER_Y        (FB_HEIGHT / 2)
#define LOADING_CENTER_X        (FB_WIDTH / 2)
#define LOADING_TEXT_OFFSET_Y   30
#define LOADING_DETAIL_OFFSET_Y 50
#define LOADING_PROGRESS_Y      (LOADING_CENTER_Y + 40)
#define LOADING_PROGRESS_WIDTH  280

/*
 * Loading Screen Functions
 */

void loading_screen_init(loading_screen_t *screen) {
    if (!screen) {
        return;
    }

    screen->state = LOADING_STATE_IDLE;
    screen->operation[0] = '\0';
    screen->detail[0] = '\0';
    screen->frame = 0;
    screen->progress_current = 0;
    screen->progress_max = 100;
    screen->show_progress = false;
    screen->show_spinner = true;
}

void loading_screen_start(loading_screen_t *screen,
                          const char *operation,
                          const char *detail,
                          bool show_progress) {
    if (!screen) {
        return;
    }

    screen->state = LOADING_STATE_ACTIVE;
    screen->frame = 0;
    screen->show_progress = show_progress;
    screen->show_spinner = true;
    screen->progress_current = 0;
    screen->progress_max = 100;

    /* Copy operation text */
    if (operation) {
        strncpy(screen->operation, operation, sizeof(screen->operation) - 1);
        screen->operation[sizeof(screen->operation) - 1] = '\0';
    } else {
        strncpy(screen->operation, "Loading...", sizeof(screen->operation) - 1);
    }

    /* Copy detail text */
    if (detail) {
        strncpy(screen->detail, detail, sizeof(screen->detail) - 1);
        screen->detail[sizeof(screen->detail) - 1] = '\0';
    } else {
        screen->detail[0] = '\0';
    }
}

void loading_screen_update_progress(loading_screen_t *screen, int current, int maximum) {
    if (!screen) {
        return;
    }

    screen->progress_current = current;
    screen->progress_max = maximum;

    /* Clamp values */
    if (screen->progress_current < 0) {
        screen->progress_current = 0;
    }
    if (screen->progress_current > screen->progress_max) {
        screen->progress_current = screen->progress_max;
    }
}

void loading_screen_update_animation(loading_screen_t *screen) {
    if (!screen) {
        return;
    }

    if (screen->state == LOADING_STATE_ACTIVE) {
        screen->frame++;
    }
}

void loading_screen_complete(loading_screen_t *screen) {
    if (!screen) {
        return;
    }

    screen->state = LOADING_STATE_COMPLETE;
    screen->progress_current = screen->progress_max;
}

void loading_screen_error(loading_screen_t *screen, const char *error_message) {
    if (!screen) {
        return;
    }

    screen->state = LOADING_STATE_ERROR;
    screen->show_spinner = false;

    if (error_message) {
        strncpy(screen->detail, error_message, sizeof(screen->detail) - 1);
        screen->detail[sizeof(screen->detail) - 1] = '\0';
    }
}

void loading_screen_render(loading_screen_t *screen, framebuffer_t *fb) {
    if (!screen || !fb) {
        return;
    }

    if (screen->state == LOADING_STATE_IDLE) {
        return;
    }

    /* Clear screen */
    fb_clear(fb, COLOR_WHITE);

    /* Render spinner if active */
    if (screen->show_spinner && screen->state == LOADING_STATE_ACTIVE) {
        ui_render_spinner(fb, LOADING_CENTER_X, LOADING_CENTER_Y, screen->frame);
    }

    /* Render operation text (centered) */
    if (screen->operation[0] != '\0') {
        int op_len = strlen(screen->operation);
        int op_x = LOADING_CENTER_X - (op_len * FONT_WIDTH / 2);
        int op_y = LOADING_CENTER_Y - LOADING_TEXT_OFFSET_Y;

        text_render_string(fb, op_x, op_y, screen->operation, COLOR_BLACK);

        /* Add animated dots if active */
        if (screen->state == LOADING_STATE_ACTIVE) {
            int dots_x = op_x + (op_len * FONT_WIDTH);
            ui_render_loading_dots(fb, dots_x, op_y, screen->frame);
        }
    }

    /* Render detail text (centered, below operation) */
    if (screen->detail[0] != '\0') {
        int detail_len = strlen(screen->detail);
        int detail_x = LOADING_CENTER_X - (detail_len * FONT_WIDTH / 2);
        int detail_y = LOADING_CENTER_Y - LOADING_TEXT_OFFSET_Y + FONT_HEIGHT + 4;

        /* Truncate if too long */
        char truncated_detail[128];
        int max_chars = 40;
        if (detail_len > max_chars) {
            strncpy(truncated_detail, screen->detail, max_chars - 3);
            truncated_detail[max_chars - 3] = '\0';
            strcat(truncated_detail, "...");
        } else {
            strncpy(truncated_detail, screen->detail, sizeof(truncated_detail) - 1);
            truncated_detail[sizeof(truncated_detail) - 1] = '\0';
        }

        int final_len = strlen(truncated_detail);
        int final_x = LOADING_CENTER_X - (final_len * FONT_WIDTH / 2);
        text_render_string(fb, final_x, detail_y, truncated_detail, COLOR_BLACK);
    }

    /* Render progress bar if enabled */
    if (screen->show_progress) {
        progress_bar_t progress;
        int progress_x = LOADING_CENTER_X - (LOADING_PROGRESS_WIDTH / 2);

        progress_bar_init(&progress, progress_x, LOADING_PROGRESS_Y,
                         LOADING_PROGRESS_WIDTH, true);
        progress_bar_set_value(&progress, screen->progress_current, screen->progress_max);
        progress_bar_render(&progress, fb);
    }

    /* Render status message based on state */
    if (screen->state == LOADING_STATE_COMPLETE) {
        const char *complete_msg = "Complete!";
        int msg_len = strlen(complete_msg);
        int msg_x = LOADING_CENTER_X - (msg_len * FONT_WIDTH / 2);
        int msg_y = LOADING_CENTER_Y + LOADING_DETAIL_OFFSET_Y;
        text_render_string(fb, msg_x, msg_y, complete_msg, COLOR_BLACK);
    } else if (screen->state == LOADING_STATE_ERROR) {
        const char *error_msg = "Error!";
        int msg_len = strlen(error_msg);
        int msg_x = LOADING_CENTER_X - (msg_len * FONT_WIDTH / 2);
        int msg_y = LOADING_CENTER_Y + LOADING_DETAIL_OFFSET_Y;
        text_render_string(fb, msg_x, msg_y, error_msg, COLOR_BLACK);
    }
}

bool loading_screen_is_active(loading_screen_t *screen) {
    if (!screen) {
        return false;
    }

    return (screen->state == LOADING_STATE_ACTIVE);
}

void loading_screen_reset(loading_screen_t *screen) {
    if (!screen) {
        return;
    }

    loading_screen_init(screen);
}

/*
 * Convenience Functions for Common Operations
 */

void loading_screen_show_opening_book(loading_screen_t *screen, const char *filename) {
    if (!screen) {
        return;
    }

    loading_screen_start(screen, "Opening book", filename, false);
}

void loading_screen_show_scanning_library(loading_screen_t *screen) {
    if (!screen) {
        return;
    }

    loading_screen_start(screen, "Scanning library", NULL, false);
}

void loading_screen_show_processing(loading_screen_t *screen, const char *operation) {
    if (!screen) {
        return;
    }

    loading_screen_start(screen, operation, NULL, true);
}
