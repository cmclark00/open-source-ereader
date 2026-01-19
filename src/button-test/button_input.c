/*
 * button_input.c - GPIO Button Input Library Implementation
 *
 * This library reads button events from Linux input event devices created
 * by the gpio-keys driver.
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

#include "button_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/input.h>

/* Software debounce interval in milliseconds */
#define DEBOUNCE_MS 50

/* Maximum number of input devices to check */
#define MAX_INPUT_DEVICES 32

/* Button input context structure */
struct button_input_ctx {
    int fd;                          /* File descriptor for input device */
    button_callback_t callback;      /* User callback function */
    void *user_data;                 /* User data for callback */
    struct timeval last_event_time[256];  /* Last event time per key code (for debouncing) */
};

/*
 * Calculate time difference in milliseconds
 */
static long timeval_diff_ms(const struct timeval *t1, const struct timeval *t2)
{
    long sec_diff = t1->tv_sec - t2->tv_sec;
    long usec_diff = t1->tv_usec - t2->tv_usec;
    return (sec_diff * 1000) + (usec_diff / 1000);
}

/*
 * Check if an input device is the gpio-keys device
 */
static bool is_gpio_keys_device(int fd)
{
    char name[256] = {0};

    /* Get device name */
    if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), name) < 0) {
        return false;
    }

    /* Check if it's gpio-keys or contains "button" in the name */
    if (strstr(name, "gpio-keys") != NULL ||
        strstr(name, "gpio_keys") != NULL ||
        strstr(name, "button") != NULL) {
        return true;
    }

    return false;
}

/*
 * Find the gpio-keys input device
 */
static int find_gpio_keys_device(void)
{
    char device_path[256];
    int fd;

    /* Try common device names first */
    for (int i = 0; i < MAX_INPUT_DEVICES; i++) {
        snprintf(device_path, sizeof(device_path), "/dev/input/event%d", i);

        /* Try to open device */
        fd = open(device_path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;  /* Device doesn't exist or can't be opened */
        }

        /* Check if it's the gpio-keys device */
        if (is_gpio_keys_device(fd)) {
            printf("Found gpio-keys device: %s\n", device_path);
            return fd;
        }

        /* Not the right device, close and continue */
        close(fd);
    }

    fprintf(stderr, "Error: Could not find gpio-keys input device\n");
    fprintf(stderr, "Make sure the ereader-buttons device tree overlay is loaded\n");
    return -1;
}

/*
 * Initialize button input system
 */
button_input_ctx_t* button_input_init(void)
{
    button_input_ctx_t *ctx;
    int fd;

    /* Find and open the gpio-keys device */
    fd = find_gpio_keys_device();
    if (fd < 0) {
        return NULL;
    }

    /* Allocate context structure */
    ctx = calloc(1, sizeof(button_input_ctx_t));
    if (!ctx) {
        close(fd);
        return NULL;
    }

    ctx->fd = fd;
    ctx->callback = NULL;
    ctx->user_data = NULL;

    /* Initialize last event times to zero */
    memset(ctx->last_event_time, 0, sizeof(ctx->last_event_time));

    return ctx;
}

/*
 * Set button event callback
 */
void button_input_set_callback(button_input_ctx_t *ctx,
                                button_callback_t callback,
                                void *user_data)
{
    if (!ctx) {
        return;
    }

    ctx->callback = callback;
    ctx->user_data = user_data;
}

/*
 * Convert Linux key code to button_t
 */
static button_t keycode_to_button(uint16_t code)
{
    switch (code) {
        case KEY_UP:    return BUTTON_UP;
        case KEY_DOWN:  return BUTTON_DOWN;
        case KEY_ENTER: return BUTTON_SELECT;
        case KEY_ESC:   return BUTTON_BACK;
        case KEY_MENU:  return BUTTON_MENU;
        default:        return BUTTON_UNKNOWN;
    }
}

/*
 * Check if event should be debounced (filtered out)
 */
static bool should_debounce(button_input_ctx_t *ctx,
                             uint16_t code,
                             const struct timeval *timestamp)
{
    if (code >= 256) {
        return false;  /* Invalid code, don't debounce */
    }

    struct timeval *last_time = &ctx->last_event_time[code];

    /* If this is the first event for this key, don't debounce */
    if (last_time->tv_sec == 0 && last_time->tv_usec == 0) {
        *last_time = *timestamp;
        return false;
    }

    /* Calculate time since last event */
    long diff_ms = timeval_diff_ms(timestamp, last_time);

    /* Debounce if event occurred too soon after last event */
    if (diff_ms < DEBOUNCE_MS) {
        return true;  /* Filter out this event */
    }

    /* Update last event time */
    *last_time = *timestamp;
    return false;
}

/*
 * Read next button event
 */
int button_input_read_event(button_input_ctx_t *ctx, button_event_t *event)
{
    struct input_event ev;
    ssize_t bytes;

    if (!ctx || !event) {
        errno = EINVAL;
        return -1;
    }

    /* Read events until we get a key event */
    while (1) {
        bytes = read(ctx->fd, &ev, sizeof(ev));

        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* No data available in non-blocking mode */
                continue;
            }
            return -1;  /* Error */
        }

        if (bytes != sizeof(ev)) {
            /* Incomplete read, should not happen */
            errno = EIO;
            return -1;
        }

        /* We only care about key events (EV_KEY) */
        if (ev.type != EV_KEY) {
            continue;
        }

        /* Apply software debouncing */
        if (should_debounce(ctx, ev.code, &ev.time)) {
            continue;  /* Skip this event */
        }

        /* Convert to button event */
        event->button = keycode_to_button(ev.code);
        event->timestamp = ev.time;
        event->value = ev.value;

        /* Determine event type */
        switch (ev.value) {
            case 0:
                event->event_type = BUTTON_EVENT_RELEASE;
                break;
            case 1:
                event->event_type = BUTTON_EVENT_PRESS;
                break;
            case 2:
                event->event_type = BUTTON_EVENT_REPEAT;
                break;
            default:
                event->event_type = BUTTON_EVENT_UNKNOWN;
                break;
        }

        /* Call callback if registered */
        if (ctx->callback) {
            ctx->callback(event, ctx->user_data);
        }

        return 0;  /* Success */
    }
}

/*
 * Read next button event with timeout
 */
int button_input_read_event_timeout(button_input_ctx_t *ctx,
                                     button_event_t *event,
                                     int timeout_ms)
{
    fd_set readfds;
    struct timeval timeout;
    int ret;

    if (!ctx || !event) {
        errno = EINVAL;
        return -1;
    }

    /* Set up file descriptor set */
    FD_ZERO(&readfds);
    FD_SET(ctx->fd, &readfds);

    /* Set up timeout */
    if (timeout_ms >= 0) {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
    }

    /* Wait for data with timeout */
    ret = select(ctx->fd + 1, &readfds, NULL, NULL,
                 timeout_ms >= 0 ? &timeout : NULL);

    if (ret < 0) {
        return -1;  /* Error */
    }

    if (ret == 0) {
        return 0;  /* Timeout */
    }

    /* Data available, read event */
    return button_input_read_event(ctx, event) == 0 ? 1 : -1;
}

/*
 * Get file descriptor for button input device
 */
int button_input_get_fd(button_input_ctx_t *ctx)
{
    if (!ctx) {
        return -1;
    }
    return ctx->fd;
}

/*
 * Clean up button input system
 */
void button_input_cleanup(button_input_ctx_t *ctx)
{
    if (!ctx) {
        return;
    }

    if (ctx->fd >= 0) {
        close(ctx->fd);
    }

    free(ctx);
}

/*
 * Convert button code to string name
 */
const char* button_to_string(button_t button)
{
    switch (button) {
        case BUTTON_UP:     return "UP";
        case BUTTON_DOWN:   return "DOWN";
        case BUTTON_SELECT: return "SELECT";
        case BUTTON_BACK:   return "BACK";
        case BUTTON_MENU:   return "MENU";
        default:            return "UNKNOWN";
    }
}

/*
 * Convert button event type to string
 */
const char* button_event_type_to_string(button_event_type_t event_type)
{
    switch (event_type) {
        case BUTTON_EVENT_PRESS:   return "PRESS";
        case BUTTON_EVENT_RELEASE: return "RELEASE";
        case BUTTON_EVENT_REPEAT:  return "REPEAT";
        default:                   return "UNKNOWN";
    }
}
