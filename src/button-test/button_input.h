/*
 * button_input.h - GPIO Button Input Library for E-Reader
 *
 * This library provides a simple interface for reading button events from
 * Linux input event devices (/dev/input/eventX). It's designed for use with
 * the gpio-keys device tree binding.
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

#ifndef BUTTON_INPUT_H
#define BUTTON_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>

/*
 * Button identifiers matching our gpio-keys device tree configuration
 * These correspond to the Linux input event codes
 */
typedef enum {
    BUTTON_UP     = KEY_UP,      /* 103 - Navigate up */
    BUTTON_DOWN   = KEY_DOWN,    /* 108 - Navigate down */
    BUTTON_SELECT = KEY_ENTER,   /* 28  - Confirm/Enter */
    BUTTON_BACK   = KEY_ESC,     /* 1   - Go back/Cancel */
    BUTTON_MENU   = KEY_MENU,    /* 139 - Open menu */
    BUTTON_UNKNOWN = 0
} button_t;

/*
 * Button event types
 */
typedef enum {
    BUTTON_EVENT_PRESS,      /* Button was pressed down */
    BUTTON_EVENT_RELEASE,    /* Button was released */
    BUTTON_EVENT_REPEAT,     /* Button is being held (auto-repeat) */
    BUTTON_EVENT_UNKNOWN
} button_event_type_t;

/*
 * Button event structure
 */
typedef struct {
    button_t button;                    /* Which button triggered the event */
    button_event_type_t event_type;     /* Press, release, or repeat */
    struct timeval timestamp;           /* When the event occurred */
    uint32_t value;                     /* Raw value from input event (0=release, 1=press, 2=repeat) */
} button_event_t;

/*
 * Button callback function type
 * Called when a button event occurs
 *
 * Parameters:
 *   event - Pointer to button event structure
 *   user_data - User-provided data pointer (passed to button_input_set_callback)
 */
typedef void (*button_callback_t)(const button_event_t *event, void *user_data);

/*
 * Button input context (opaque handle)
 */
typedef struct button_input_ctx button_input_ctx_t;

/*
 * Initialize button input system
 *
 * Searches for and opens the gpio-keys input device.
 * Typically this is /dev/input/event0 or similar.
 *
 * Returns:
 *   Pointer to button input context on success
 *   NULL on failure (check errno for details)
 */
button_input_ctx_t* button_input_init(void);

/*
 * Set button event callback
 *
 * Register a callback function to be called when button events occur.
 * Only one callback can be active at a time.
 *
 * Parameters:
 *   ctx - Button input context
 *   callback - Callback function pointer (or NULL to disable callbacks)
 *   user_data - User-provided data pointer (passed to callback)
 */
void button_input_set_callback(button_input_ctx_t *ctx,
                                button_callback_t callback,
                                void *user_data);

/*
 * Read next button event (blocking)
 *
 * Blocks until a button event is available, then reads it.
 * This function applies software debouncing.
 *
 * Parameters:
 *   ctx - Button input context
 *   event - Pointer to event structure to fill
 *
 * Returns:
 *   0 on success
 *   -1 on error (check errno)
 */
int button_input_read_event(button_input_ctx_t *ctx, button_event_t *event);

/*
 * Read next button event with timeout (blocking with timeout)
 *
 * Blocks until a button event is available or timeout expires.
 *
 * Parameters:
 *   ctx - Button input context
 *   event - Pointer to event structure to fill
 *   timeout_ms - Timeout in milliseconds (0 = non-blocking, -1 = infinite)
 *
 * Returns:
 *   1 if event was read
 *   0 if timeout expired
 *   -1 on error (check errno)
 */
int button_input_read_event_timeout(button_input_ctx_t *ctx,
                                     button_event_t *event,
                                     int timeout_ms);

/*
 * Get file descriptor for button input device
 *
 * Useful for integrating with select()/poll()/epoll() event loops.
 *
 * Parameters:
 *   ctx - Button input context
 *
 * Returns:
 *   File descriptor (>= 0) on success
 *   -1 if context is invalid
 */
int button_input_get_fd(button_input_ctx_t *ctx);

/*
 * Clean up button input system
 *
 * Closes the input device and frees resources.
 *
 * Parameters:
 *   ctx - Button input context (may be NULL)
 */
void button_input_cleanup(button_input_ctx_t *ctx);

/*
 * Convert button code to string name
 *
 * Parameters:
 *   button - Button identifier
 *
 * Returns:
 *   String name of button (e.g., "UP", "DOWN")
 *   "UNKNOWN" if button code is not recognized
 */
const char* button_to_string(button_t button);

/*
 * Convert button event type to string
 *
 * Parameters:
 *   event_type - Button event type
 *
 * Returns:
 *   String name of event type (e.g., "PRESS", "RELEASE")
 *   "UNKNOWN" if event type is not recognized
 */
const char* button_event_type_to_string(button_event_type_t event_type);

#endif /* BUTTON_INPUT_H */
