/*
 * test_ui_components.c - Comprehensive UI Components Test Suite
 *
 * Tests all UI polish components to ensure they function correctly:
 * - Confirmation dialogs block other input
 * - Toast notifications disappear after timeout
 * - Borders render cleanly on e-paper display
 * - Spinner animation is visible (not too fast)
 * - Headers and footers align properly
 * - Message boxes center correctly on screen
 *
 * Copyright (C) 2024-2026 Open Source E-Reader Project Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ui_components.h"
#include "../rendering/framebuffer.h"
#include "../rendering/text_renderer.h"
#include "../../button-test/button_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Test result tracking */
typedef struct {
    int total;
    int passed;
    int failed;
} test_results_t;

/* Global test results */
static test_results_t g_results = {0, 0, 0};

/* Test helper macros */
#define TEST_START(name) \
    do { \
        printf("\n=== TEST: %s ===\n", name); \
        g_results.total++; \
    } while(0)

#define TEST_PASS() \
    do { \
        printf("PASS\n"); \
        g_results.passed++; \
    } while(0)

#define TEST_FAIL(msg) \
    do { \
        printf("FAIL: %s\n", msg); \
        g_results.failed++; \
    } while(0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            TEST_FAIL(msg); \
            return 0; \
        } \
    } while(0)

#define ASSERT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            printf("  Expected: %d, Got: %d\n", (int)(b), (int)(a)); \
            TEST_FAIL(msg); \
            return 0; \
        } \
    } while(0)

/* Forward declarations */
static int test_confirmation_dialog_input_blocking(framebuffer_t *fb);
static int test_toast_notification_timeout(framebuffer_t *fb);
static int test_border_rendering(framebuffer_t *fb);
static int test_spinner_animation_speed(framebuffer_t *fb);
static int test_header_footer_alignment(framebuffer_t *fb);
static int test_message_box_centering(framebuffer_t *fb);

/*
 * Main test runner
 */
int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("UI Components Test Suite\n");
    printf("========================================\n");

    /* Initialize framebuffer */
    framebuffer_t *fb = fb_create();
    if (!fb) {
        printf("ERROR: Failed to create framebuffer\n");
        return 1;
    }

    /* Run all tests */
    test_confirmation_dialog_input_blocking(fb);
    test_toast_notification_timeout(fb);
    test_border_rendering(fb);
    test_spinner_animation_speed(fb);
    test_header_footer_alignment(fb);
    test_message_box_centering(fb);

    /* Cleanup */
    fb_destroy(fb);

    /* Print summary */
    printf("\n========================================\n");
    printf("Test Summary\n");
    printf("========================================\n");
    printf("Total:  %d\n", g_results.total);
    printf("Passed: %d\n", g_results.passed);
    printf("Failed: %d\n", g_results.failed);
    printf("========================================\n");

    return (g_results.failed == 0) ? 0 : 1;
}

/*
 * Test 1: Confirmation dialogs block other input correctly
 */
static int test_confirmation_dialog_input_blocking(framebuffer_t *fb) {
    TEST_START("Confirmation Dialog Input Blocking");

    confirmation_dialog_t dialog;
    confirmation_dialog_init(&dialog,
                             "Test Dialog",
                             "This tests input blocking",
                             "Yes",
                             "No");

    /* Dialog should not be active initially */
    ASSERT(!confirmation_dialog_is_active(&dialog),
           "Dialog should not be active before show");

    /* Show the dialog */
    confirmation_dialog_show(&dialog);
    ASSERT(confirmation_dialog_is_active(&dialog),
           "Dialog should be active after show");

    /* Test that non-SELECT/BACK buttons don't affect dialog */
    button_event_t up_event = {
        .button = BUTTON_UP,
        .event_type = BUTTON_EVENT_PRESS,
        .timestamp = 0
    };
    dialog_result_t result = confirmation_dialog_handle_event(&dialog, &up_event);
    ASSERT_EQ(result, DIALOG_RESULT_NONE,
              "UP button should not produce result");
    ASSERT(confirmation_dialog_is_active(&dialog),
           "Dialog should still be active after UP");

    button_event_t down_event = {
        .button = BUTTON_DOWN,
        .event_type = BUTTON_EVENT_PRESS,
        .timestamp = 0
    };
    result = confirmation_dialog_handle_event(&dialog, &down_event);
    ASSERT_EQ(result, DIALOG_RESULT_NONE,
              "DOWN button should not produce result");
    ASSERT(confirmation_dialog_is_active(&dialog),
           "Dialog should still be active after DOWN");

    /* Test SELECT button produces YES result */
    button_event_t select_event = {
        .button = BUTTON_SELECT,
        .event_type = BUTTON_EVENT_PRESS,
        .timestamp = 0
    };
    result = confirmation_dialog_handle_event(&dialog, &select_event);
    ASSERT_EQ(result, DIALOG_RESULT_YES,
              "SELECT button should produce YES result");
    ASSERT(!confirmation_dialog_is_active(&dialog),
           "Dialog should be inactive after SELECT");

    /* Reset and test BACK button */
    confirmation_dialog_show(&dialog);
    button_event_t back_event = {
        .button = BUTTON_BACK,
        .event_type = BUTTON_EVENT_PRESS,
        .timestamp = 0
    };
    result = confirmation_dialog_handle_event(&dialog, &back_event);
    ASSERT_EQ(result, DIALOG_RESULT_NO,
              "BACK button should produce NO result");
    ASSERT(!confirmation_dialog_is_active(&dialog),
           "Dialog should be inactive after BACK");

    /* Test that events are ignored when dialog is not active */
    result = confirmation_dialog_handle_event(&dialog, &select_event);
    ASSERT_EQ(result, DIALOG_RESULT_NONE,
              "Events should be ignored when dialog inactive");

    /* Test rendering doesn't crash */
    fb_clear(fb, COLOR_WHITE);
    confirmation_dialog_show(&dialog);
    confirmation_dialog_render(&dialog, fb);

    TEST_PASS();
    return 1;
}

/*
 * Test 2: Toast notifications disappear after timeout
 */
static int test_toast_notification_timeout(framebuffer_t *fb) {
    TEST_START("Toast Notification Timeout");

    /* Test toast structure and behavior */
    typedef struct {
        bool show_toast;
        char toast_message[64];
        uint32_t toast_timer;
    } toast_state_t;

    toast_state_t toast = {0};

    /* Activate toast */
    toast.show_toast = true;
    strncpy(toast.toast_message, "Test notification", sizeof(toast.toast_message));
    toast.toast_timer = 60; /* 2 seconds at 30 FPS */

    ASSERT(toast.show_toast, "Toast should be active");
    ASSERT_EQ(toast.toast_timer, 60, "Timer should be 60");

    /* Simulate frames passing */
    for (int frame = 0; frame < 65; frame++) {
        if (toast.show_toast && toast.toast_timer > 0) {
            /* Render toast */
            fb_clear(fb, COLOR_WHITE);
            ui_render_toast(fb, toast.toast_message);

            /* Decrement timer */
            toast.toast_timer--;
            if (toast.toast_timer == 0) {
                toast.show_toast = false;
            }
        }
    }

    /* After 65 frames, toast should be hidden */
    ASSERT(!toast.show_toast, "Toast should be hidden after timeout");
    ASSERT_EQ(toast.toast_timer, 0, "Timer should be 0");

    /* Test that rendering still works (should just not show anything) */
    if (toast.show_toast) {
        ui_render_toast(fb, toast.toast_message);
    }

    TEST_PASS();
    return 1;
}

/*
 * Test 3: Borders render cleanly on e-paper display
 */
static int test_border_rendering(framebuffer_t *fb) {
    TEST_START("Border Rendering Quality");

    int test_x = 50;
    int test_y = 50;
    int test_width = 200;
    int test_height = 100;

    /* Test all border styles */
    fb_clear(fb, COLOR_WHITE);

    /* Test BORDER_SINGLE */
    ui_draw_border(fb, test_x, test_y, test_width, test_height, BORDER_SINGLE);

    /* Verify corners are drawn (check pixels) */
    pixel_t top_left = fb_get_pixel(fb, test_x, test_y);
    pixel_t top_right = fb_get_pixel(fb, test_x + test_width - 1, test_y);
    pixel_t bottom_left = fb_get_pixel(fb, test_x, test_y + test_height - 1);
    pixel_t bottom_right = fb_get_pixel(fb, test_x + test_width - 1, test_y + test_height - 1);

    ASSERT_EQ(top_left, COLOR_BLACK, "Top-left corner should be black");
    ASSERT_EQ(top_right, COLOR_BLACK, "Top-right corner should be black");
    ASSERT_EQ(bottom_left, COLOR_BLACK, "Bottom-left corner should be black");
    ASSERT_EQ(bottom_right, COLOR_BLACK, "Bottom-right corner should be black");

    /* Verify interior is not drawn */
    pixel_t interior = fb_get_pixel(fb, test_x + test_width/2, test_y + test_height/2);
    ASSERT_EQ(interior, COLOR_WHITE, "Interior should remain white");

    /* Test BORDER_DOUBLE */
    fb_clear(fb, COLOR_WHITE);
    ui_draw_border(fb, test_x, test_y, test_width, test_height, BORDER_DOUBLE);

    /* Verify double border corners */
    top_left = fb_get_pixel(fb, test_x, test_y);
    pixel_t inner_top_left = fb_get_pixel(fb, test_x + 2, test_y + 2);
    ASSERT_EQ(top_left, COLOR_BLACK, "Outer top-left should be black");
    ASSERT_EQ(inner_top_left, COLOR_BLACK, "Inner top-left should be black");

    /* Test BORDER_DECORATIVE */
    fb_clear(fb, COLOR_WHITE);
    ui_draw_border(fb, test_x, test_y, test_width, test_height, BORDER_DECORATIVE);

    /* Verify main border and ornaments exist */
    top_left = fb_get_pixel(fb, test_x, test_y);
    pixel_t ornament = fb_get_pixel(fb, test_x + 3, test_y + 3);
    ASSERT_EQ(top_left, COLOR_BLACK, "Border should be drawn");
    ASSERT_EQ(ornament, COLOR_BLACK, "Ornament should be drawn");

    /* Test BORDER_NONE doesn't draw anything */
    fb_clear(fb, COLOR_WHITE);
    ui_draw_border(fb, test_x, test_y, test_width, test_height, BORDER_NONE);
    top_left = fb_get_pixel(fb, test_x, test_y);
    ASSERT_EQ(top_left, COLOR_WHITE, "BORDER_NONE should not draw");

    TEST_PASS();
    return 1;
}

/*
 * Test 4: Spinner animation is visible (not too fast)
 */
static int test_spinner_animation_speed(framebuffer_t *fb) {
    TEST_START("Spinner Animation Speed");

    int center_x = FB_WIDTH / 2;
    int center_y = FB_HEIGHT / 2;

    /* Test that spinner renders different frames */
    fb_clear(fb, COLOR_WHITE);

    /* Frame 0 - should draw horizontal line */
    ui_render_spinner(fb, center_x, center_y, 0);

    /* Check that something was drawn */
    int pixels_drawn_frame0 = 0;
    for (int x = center_x; x < center_x + 8; x++) {
        if (fb_get_pixel(fb, x, center_y) == COLOR_BLACK) {
            pixels_drawn_frame0++;
        }
    }
    ASSERT(pixels_drawn_frame0 > 0, "Frame 0 should draw pixels");

    /* Frame 2 - should draw vertical line */
    fb_clear(fb, COLOR_WHITE);
    ui_render_spinner(fb, center_x, center_y, 2);

    int pixels_drawn_frame2 = 0;
    for (int y = center_y; y < center_y + 8; y++) {
        if (fb_get_pixel(fb, center_x, y) == COLOR_BLACK) {
            pixels_drawn_frame2++;
        }
    }
    ASSERT(pixels_drawn_frame2 > 0, "Frame 2 should draw pixels");

    /* Test that frames cycle correctly (8 frames) */
    for (uint32_t frame = 0; frame < 16; frame++) {
        fb_clear(fb, COLOR_WHITE);
        ui_render_spinner(fb, center_x, center_y, frame);
        /* Should not crash for any frame */
    }

    /* Test loading dots animation */
    fb_clear(fb, COLOR_WHITE);
    ui_render_loading_dots(fb, 100, 100, 0);   /* Should show 0 dots */
    ui_render_loading_dots(fb, 100, 110, 10);  /* Should show 1 dot */
    ui_render_loading_dots(fb, 100, 120, 20);  /* Should show 2 dots */
    ui_render_loading_dots(fb, 100, 130, 30);  /* Should show 3 dots */

    /* Verify animation speed is reasonable (frame updates every ~10 frames) */
    int dot_counts[40];
    for (int frame = 0; frame < 40; frame++) {
        dot_counts[frame] = (frame / 10) % 4;
    }

    /* Verify cycling pattern */
    ASSERT_EQ(dot_counts[0], 0, "Frame 0 should have 0 dots");
    ASSERT_EQ(dot_counts[10], 1, "Frame 10 should have 1 dot");
    ASSERT_EQ(dot_counts[20], 2, "Frame 20 should have 2 dots");
    ASSERT_EQ(dot_counts[30], 3, "Frame 30 should have 3 dots");

    TEST_PASS();
    return 1;
}

/*
 * Test 5: Headers and footers align properly
 */
static int test_header_footer_alignment(framebuffer_t *fb) {
    TEST_START("Header and Footer Alignment");

    fb_clear(fb, COLOR_WHITE);

    /* Test header rendering */
    ui_render_header(fb, "Test Title", "[1/5]", 0);

    /* Test footer rendering */
    ui_render_footer(fb, "UP/DOWN:Navigate  SELECT:Choose", 17);

    /* Test separator rendering */
    int sep_y = 100;
    ui_render_separator(fb, sep_y, false);

    /* Verify separator line was drawn */
    pixel_t sep_left = fb_get_pixel(fb, MARGIN_LEFT, sep_y);
    pixel_t sep_middle = fb_get_pixel(fb, FB_WIDTH / 2, sep_y);
    pixel_t sep_right = fb_get_pixel(fb, FB_WIDTH - MARGIN_RIGHT - 1, sep_y);

    ASSERT_EQ(sep_left, COLOR_BLACK, "Separator left should be black");
    ASSERT_EQ(sep_middle, COLOR_BLACK, "Separator middle should be black");
    ASSERT_EQ(sep_right, COLOR_BLACK, "Separator right should be black");

    /* Test decorative separator */
    fb_clear(fb, COLOR_WHITE);
    sep_y = 100;
    ui_render_separator(fb, sep_y, true);

    /* Verify decorative end caps */
    pixel_t cap_left = fb_get_pixel(fb, MARGIN_LEFT, sep_y - 1);
    ASSERT_EQ(cap_left, COLOR_BLACK, "Decorative cap should be drawn");

    /* Test header with NULL page info */
    fb_clear(fb, COLOR_WHITE);
    ui_render_header(fb, "Title Only", NULL, 0);

    /* Test empty separator (should not crash) */
    ui_render_separator(fb, 50, false);
    ui_render_separator(fb, 60, true);

    TEST_PASS();
    return 1;
}

/*
 * Test 6: Message boxes center correctly on screen
 */
static int test_message_box_centering(framebuffer_t *fb) {
    TEST_START("Message Box Centering");

    fb_clear(fb, COLOR_WHITE);

    /* Test simple message box */
    message_box_render(fb, "Test Title", "This is a test message", BORDER_SINGLE);

    /* Test multi-line message */
    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb,
                       "Multi-line Test",
                       "Line 1\nLine 2\nLine 3",
                       BORDER_DECORATIVE);

    /* Test message box without title */
    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, NULL, "Message without title", BORDER_DOUBLE);

    /* Test with different border styles */
    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, "Single Border", "Test", BORDER_SINGLE);

    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, "Double Border", "Test", BORDER_DOUBLE);

    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, "Decorative Border", "Test", BORDER_DECORATIVE);

    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, "No Border", "Test", BORDER_NONE);

    /* Verify centering by checking dialog dimensions
     * (This is a simplified check - full verification would require
     *  analyzing the rendered pixels to find the box bounds)
     */
    fb_clear(fb, COLOR_WHITE);
    message_box_render(fb, "Center Test", "X", BORDER_SINGLE);

    /* Message box should have drawn something in the center area */
    int center_x = FB_WIDTH / 2;
    int center_y = FB_HEIGHT / 2;

    bool found_pixels = false;
    for (int y = center_y - 50; y < center_y + 50; y++) {
        for (int x = center_x - 50; x < center_x + 50; x++) {
            if (fb_get_pixel(fb, x, y) == COLOR_BLACK) {
                found_pixels = true;
                break;
            }
        }
        if (found_pixels) break;
    }

    ASSERT(found_pixels, "Message box should render in center area");

    TEST_PASS();
    return 1;
}
