/*
 * main.c - Button Test Application for E-Reader
 *
 * This application tests the GPIO button input system by displaying
 * button press/release events to the console.
 *
 * Usage: button-test
 *
 * Press CTRL+C to exit.
 *
 * Author: E-Reader Project
 * License: Open Source
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "button_input.h"

/* Global flag for clean shutdown */
static volatile bool running = true;

/*
 * Signal handler for SIGINT (Ctrl+C)
 */
static void signal_handler(int sig)
{
    (void)sig;  /* Unused parameter */
    running = false;
}

/*
 * Format timestamp as HH:MM:SS.mmm
 */
static void format_timestamp(const struct timeval *tv, char *buf, size_t bufsize)
{
    struct tm *tm_info;
    time_t sec = tv->tv_sec;
    int msec = tv->tv_usec / 1000;

    tm_info = localtime(&sec);
    strftime(buf, bufsize, "%H:%M:%S", tm_info);
    snprintf(buf + 8, bufsize - 8, ".%03d", msec);
}

/*
 * Print button event to console
 */
static void print_button_event(const button_event_t *event)
{
    char timestamp_str[32];
    const char *button_name = button_to_string(event->button);
    const char *event_type_name = button_event_type_to_string(event->event_type);

    format_timestamp(&event->timestamp, timestamp_str, sizeof(timestamp_str));

    /* Color codes for terminal output */
    const char *color_press = "\033[1;32m";    /* Green bold */
    const char *color_release = "\033[1;31m";  /* Red bold */
    const char *color_repeat = "\033[1;33m";   /* Yellow bold */
    const char *color_reset = "\033[0m";       /* Reset */

    const char *color;
    switch (event->event_type) {
        case BUTTON_EVENT_PRESS:
            color = color_press;
            break;
        case BUTTON_EVENT_RELEASE:
            color = color_release;
            break;
        case BUTTON_EVENT_REPEAT:
            color = color_repeat;
            break;
        default:
            color = color_reset;
            break;
    }

    printf("[%s] %s%-8s%s | %-10s | value=%u\n",
           timestamp_str,
           color,
           button_name,
           color_reset,
           event_type_name,
           event->value);
    fflush(stdout);
}

/*
 * Button event callback
 */
static void button_callback(const button_event_t *event, void *user_data)
{
    (void)user_data;  /* Unused parameter */
    print_button_event(event);
}

/*
 * Print usage instructions
 */
static void print_usage(void)
{
    printf("\n");
    printf("===========================================\n");
    printf("  E-Reader Button Test Application\n");
    printf("===========================================\n");
    printf("\n");
    printf("This application tests the GPIO button input system.\n");
    printf("\n");
    printf("Button Layout:\n");
    printf("  UP     - GPIO 5  (Pin 29) - Navigate up\n");
    printf("  DOWN   - GPIO 6  (Pin 31) - Navigate down\n");
    printf("  SELECT - GPIO 13 (Pin 33) - Confirm/Enter\n");
    printf("  BACK   - GPIO 19 (Pin 35) - Go back/Cancel\n");
    printf("  MENU   - GPIO 26 (Pin 37) - Open menu\n");
    printf("\n");
    printf("Press any button to see events.\n");
    printf("Press CTRL+C to exit.\n");
    printf("\n");
    printf("Event Legend:\n");
    printf("  \033[1;32mGreen\033[0m  - Button pressed\n");
    printf("  \033[1;31mRed\033[0m    - Button released\n");
    printf("  \033[1;33mYellow\033[0m - Button held (auto-repeat)\n");
    printf("\n");
    printf("-------------------------------------------\n");
    printf("\n");
}

/*
 * Main function
 */
int main(void)
{
    button_input_ctx_t *ctx;
    button_event_t event;
    int ret;

    /* Print banner */
    print_usage();

    /* Set up signal handler for clean shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize button input system */
    printf("Initializing button input system...\n");
    ctx = button_input_init();
    if (!ctx) {
        fprintf(stderr, "ERROR: Failed to initialize button input system\n");
        fprintf(stderr, "Make sure:\n");
        fprintf(stderr, "  1. The ereader-buttons device tree overlay is loaded\n");
        fprintf(stderr, "  2. Buttons are wired correctly (see WIRING_GUIDE.md)\n");
        fprintf(stderr, "  3. You have permission to access /dev/input/event* devices\n");
        return 1;
    }

    printf("Button input system initialized successfully.\n");
    printf("Waiting for button events...\n\n");

    /* Set button callback (optional - we also read events manually) */
    /* button_input_set_callback(ctx, button_callback, NULL); */

    /* Main event loop */
    while (running) {
        /* Read button event with timeout */
        ret = button_input_read_event_timeout(ctx, &event, 100);

        if (ret < 0) {
            /* Error reading event */
            perror("Error reading button event");
            break;
        }

        if (ret == 0) {
            /* Timeout - no event, check if we should exit */
            continue;
        }

        /* Event received, print it */
        print_button_event(&event);

        /* Optional: Provide helpful feedback for first-time users */
        if (event.event_type == BUTTON_EVENT_PRESS) {
            if (event.button == BUTTON_SELECT) {
                /* Could add special handling here */
            }
        }
    }

    printf("\n");
    printf("Shutting down...\n");

    /* Clean up */
    button_input_cleanup(ctx);

    printf("Button test application terminated.\n");
    return 0;
}
