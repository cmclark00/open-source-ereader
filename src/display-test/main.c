/*
 * main.c - E-Paper Display Test Application
 *
 * Simple test application to verify Waveshare 4.2" e-paper display functionality
 * Displays "Hello E-Reader" on the screen
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "epd_driver.h"
#include "font.h"

/* Global flag for clean shutdown */
static volatile int keep_running = 1;

/* Signal handler for clean shutdown */
void signal_handler(int signum) {
    printf("\nReceived signal %d, shutting down...\n", signum);
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    int ret;

    printf("==============================================\n");
    printf("  Waveshare 4.2\" E-Paper Display Test\n");
    printf("  Open Source E-Reader Project - Phase 1\n");
    printf("==============================================\n\n");

    /* Set up signal handlers for clean shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize the driver */
    printf("Step 1: Initializing driver...\n");
    ret = epd_init();
    if (ret < 0) {
        fprintf(stderr, "ERROR: Failed to initialize driver\n");
        fprintf(stderr, "Make sure:\n");
        fprintf(stderr, "  - SPI is enabled in device tree\n");
        fprintf(stderr, "  - GPIO pins are accessible\n");
        fprintf(stderr, "  - You have proper permissions (run as root or with GPIO access)\n");
        return 1;
    }
    printf("Driver initialized successfully!\n\n");

    /* Initialize the display hardware */
    printf("Step 2: Initializing display hardware...\n");
    ret = epd_display_init();
    if (ret < 0) {
        fprintf(stderr, "ERROR: Failed to initialize display\n");
        epd_cleanup();
        return 1;
    }
    printf("Display hardware initialized!\n\n");

    /* Clear display to white */
    printf("Step 3: Clearing display to white...\n");
    ret = epd_clear(COLOR_WHITE);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Failed to clear display\n");
        epd_cleanup();
        return 1;
    }
    printf("Display cleared!\n\n");

    /* Draw "Hello E-Reader" text */
    printf("Step 4: Drawing text...\n");

    /* Center the text on the display */
    /* "Hello E-Reader" is 15 characters * 8 pixels = 120 pixels wide */
    /* Center horizontally: (400 - 120) / 2 = 140 */
    /* Center vertically: (300 - 16) / 2 = 142 */
    int text_x = 140;
    int text_y = 130;

    const char *message = "Hello E-Reader";

    ret = epd_draw_text(text_x, text_y, message, COLOR_BLACK);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Failed to draw text\n");
        epd_cleanup();
        return 1;
    }
    printf("Text drawn to framebuffer!\n\n");

    /* Add a subtitle */
    const char *subtitle = "Phase 1 Complete";
    int subtitle_x = 120;
    int subtitle_y = 150;

    epd_draw_text(subtitle_x, subtitle_y, subtitle, COLOR_BLACK);

    /* Refresh display to show the text */
    printf("Step 5: Refreshing display (this takes ~5 seconds)...\n");
    ret = epd_refresh();
    if (ret < 0) {
        fprintf(stderr, "ERROR: Failed to refresh display\n");
        epd_cleanup();
        return 1;
    }
    printf("Display updated successfully!\n\n");

    /* Display success message */
    printf("==============================================\n");
    printf("  SUCCESS!\n");
    printf("==============================================\n");
    printf("\n");
    printf("The e-paper display should now show:\n");
    printf("  \"Hello E-Reader\"\n");
    printf("  \"Phase 1 Complete\"\n");
    printf("\n");
    printf("Display will remain active for 10 seconds,\n");
    printf("then enter sleep mode.\n");
    printf("\n");
    printf("Press Ctrl+C to exit immediately.\n");
    printf("\n");

    /* Wait for 10 seconds or until interrupted */
    for (int i = 10; i > 0 && keep_running; i--) {
        printf("\rEntering sleep mode in %d seconds... ", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n\n");

    /* Put display to sleep */
    if (keep_running) {
        printf("Step 6: Entering sleep mode...\n");
        epd_sleep();
        printf("Display in sleep mode (low power).\n");
        printf("Image will persist on screen even after power off.\n\n");
    }

    /* Cleanup */
    printf("Step 7: Cleaning up...\n");
    epd_cleanup();

    printf("\n==============================================\n");
    printf("  Test Complete!\n");
    printf("==============================================\n");
    printf("\nPhase 1 display test completed successfully.\n");
    printf("The e-paper display is working correctly.\n\n");

    return 0;
}
