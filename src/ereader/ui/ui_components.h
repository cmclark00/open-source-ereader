/*
 * ui_components.h - Reusable UI Components for E-Reader
 *
 * Provides polished, reusable UI components including:
 * - Loading indicators and progress bars
 * - Confirmation dialogs
 * - Decorative borders and boxes
 * - Consistent headers and footers
 * - Visual feedback elements
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

#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../../button-test/button_input.h"

/*
 * Component Configuration Constants
 */

/* Dialog dimensions and positions */
#define DIALOG_WIDTH_SMALL      200     /* Small dialog (confirmation) */
#define DIALOG_WIDTH_MEDIUM     280     /* Medium dialog (loading) */
#define DIALOG_WIDTH_LARGE      360     /* Large dialog (detailed message) */
#define DIALOG_MIN_HEIGHT       80      /* Minimum dialog height */
#define DIALOG_PADDING          10      /* Internal padding */
#define DIALOG_BORDER_WIDTH     2       /* Border thickness */

/* Loading indicator styles */
#define LOADING_SPINNER_SIZE    16      /* Spinner size in pixels */
#define LOADING_DOT_COUNT       3       /* Number of dots in "..." animation */
#define LOADING_BAR_HEIGHT      10      /* Progress bar height */

/* Border styles */
typedef enum {
    BORDER_NONE,            /* No border */
    BORDER_SINGLE,          /* Single line border */
    BORDER_DOUBLE,          /* Double line border */
    BORDER_DECORATIVE       /* Decorative corner border */
} border_style_t;

/* Dialog result */
typedef enum {
    DIALOG_RESULT_NONE,     /* No action yet */
    DIALOG_RESULT_YES,      /* User confirmed (SELECT button) */
    DIALOG_RESULT_NO,       /* User cancelled (BACK button) */
    DIALOG_RESULT_DISMISSED /* Dialog was dismissed */
} dialog_result_t;

/*
 * Loading Indicator Component
 */

typedef struct {
    int x;                  /* Center X position */
    int y;                  /* Center Y position */
    uint32_t frame;         /* Animation frame counter */
    bool show_text;         /* Whether to show text message */
    char message[64];       /* Loading message text */
} loading_indicator_t;

/**
 * Initialize a loading indicator
 *
 * @param indicator: Pointer to loading indicator structure
 * @param x: Center X position on screen
 * @param y: Center Y position on screen
 * @param message: Text message to display (or NULL for none)
 */
void loading_indicator_init(loading_indicator_t *indicator, int x, int y, const char *message);

/**
 * Update loading indicator animation (call each frame)
 *
 * @param indicator: Pointer to loading indicator
 */
void loading_indicator_update(loading_indicator_t *indicator);

/**
 * Render loading indicator to framebuffer
 *
 * @param indicator: Pointer to loading indicator
 * @param fb: Framebuffer to render to
 */
void loading_indicator_render(loading_indicator_t *indicator, framebuffer_t *fb);

/*
 * Progress Bar Component
 */

typedef struct {
    int x;                  /* Top-left X position */
    int y;                  /* Top-left Y position */
    int width;              /* Bar width in pixels */
    int height;             /* Bar height in pixels */
    int current;            /* Current progress value (0-100) */
    int maximum;            /* Maximum value (typically 100) */
    bool show_percentage;   /* Whether to show percentage text */
} progress_bar_t;

/**
 * Initialize a progress bar
 *
 * @param bar: Pointer to progress bar structure
 * @param x: Top-left X position
 * @param y: Top-left Y position
 * @param width: Bar width in pixels
 * @param show_percentage: Whether to show percentage text
 */
void progress_bar_init(progress_bar_t *bar, int x, int y, int width, bool show_percentage);

/**
 * Set progress bar value
 *
 * @param bar: Pointer to progress bar
 * @param current: Current value
 * @param maximum: Maximum value
 */
void progress_bar_set_value(progress_bar_t *bar, int current, int maximum);

/**
 * Render progress bar to framebuffer
 *
 * @param bar: Pointer to progress bar
 * @param fb: Framebuffer to render to
 */
void progress_bar_render(progress_bar_t *bar, framebuffer_t *fb);

/*
 * Confirmation Dialog Component
 */

typedef struct {
    char title[64];         /* Dialog title */
    char message[256];      /* Dialog message (multi-line supported) */
    char yes_text[16];      /* Text for "yes" option (default: "Yes") */
    char no_text[16];       /* Text for "no" option (default: "No") */
    dialog_result_t result; /* Current result state */
    bool is_active;         /* Whether dialog is active */
    int x;                  /* Calculated X position (centered) */
    int y;                  /* Calculated Y position (centered) */
    int width;              /* Calculated dialog width */
    int height;             /* Calculated dialog height */
} confirmation_dialog_t;

/**
 * Initialize a confirmation dialog
 *
 * @param dialog: Pointer to dialog structure
 * @param title: Dialog title
 * @param message: Dialog message
 * @param yes_text: Text for yes button (or NULL for default "Yes")
 * @param no_text: Text for no button (or NULL for default "No")
 */
void confirmation_dialog_init(confirmation_dialog_t *dialog,
                               const char *title,
                               const char *message,
                               const char *yes_text,
                               const char *no_text);

/**
 * Show the confirmation dialog
 *
 * @param dialog: Pointer to dialog
 */
void confirmation_dialog_show(confirmation_dialog_t *dialog);

/**
 * Hide the confirmation dialog
 *
 * @param dialog: Pointer to dialog
 */
void confirmation_dialog_hide(confirmation_dialog_t *dialog);

/**
 * Handle button event for dialog
 *
 * @param dialog: Pointer to dialog
 * @param event: Button event
 * @return: Dialog result (YES, NO, or NONE if still active)
 */
dialog_result_t confirmation_dialog_handle_event(confirmation_dialog_t *dialog,
                                                   const button_event_t *event);

/**
 * Render confirmation dialog to framebuffer
 *
 * @param dialog: Pointer to dialog
 * @param fb: Framebuffer to render to
 */
void confirmation_dialog_render(confirmation_dialog_t *dialog, framebuffer_t *fb);

/**
 * Check if dialog is active
 *
 * @param dialog: Pointer to dialog
 * @return: true if active, false otherwise
 */
bool confirmation_dialog_is_active(confirmation_dialog_t *dialog);

/*
 * Message Box Component (Non-Interactive)
 */

/**
 * Render a centered message box to framebuffer
 *
 * @param fb: Framebuffer to render to
 * @param title: Box title (or NULL for no title)
 * @param message: Message text (multi-line supported with \n)
 * @param border: Border style
 */
void message_box_render(framebuffer_t *fb,
                        const char *title,
                        const char *message,
                        border_style_t border);

/*
 * Decorative Border Drawing
 */

/**
 * Draw a decorative box border around a region
 *
 * @param fb: Framebuffer to render to
 * @param x: Top-left X coordinate
 * @param y: Top-left Y coordinate
 * @param width: Box width
 * @param height: Box height
 * @param style: Border style
 */
void ui_draw_border(framebuffer_t *fb, int x, int y, int width, int height, border_style_t style);

/**
 * Draw decorative corner ornaments
 *
 * @param fb: Framebuffer to render to
 * @param x: Top-left X coordinate
 * @param y: Top-left Y coordinate
 * @param width: Box width
 * @param height: Box height
 */
void ui_draw_corner_ornaments(framebuffer_t *fb, int x, int y, int width, int height);

/*
 * Header and Footer Helpers
 */

/**
 * Render a standardized header with title and optional icon
 *
 * @param fb: Framebuffer to render to
 * @param title: Header title text
 * @param page_info: Optional page info (e.g., "[1/5]") or NULL
 * @param line_number: Line number to render at (typically 0)
 */
void ui_render_header(framebuffer_t *fb, const char *title, const char *page_info, int line_number);

/**
 * Render a standardized footer with control hints
 *
 * @param fb: Framebuffer to render to
 * @param hints: Control hints text
 * @param line_number: Line number to render at (typically last line)
 */
void ui_render_footer(framebuffer_t *fb, const char *hints, int line_number);

/**
 * Render a horizontal separator line with optional decorative ends
 *
 * @param fb: Framebuffer to render to
 * @param y: Y position for line
 * @param decorative: Whether to add decorative end caps
 */
void ui_render_separator(framebuffer_t *fb, int y, bool decorative);

/*
 * Visual Feedback Elements
 */

/**
 * Render a "toast" notification at the bottom of the screen
 *
 * @param fb: Framebuffer to render to
 * @param message: Notification message
 */
void ui_render_toast(framebuffer_t *fb, const char *message);

/**
 * Render a spinning indicator (for background operations)
 *
 * @param fb: Framebuffer to render to
 * @param x: Center X position
 * @param y: Center Y position
 * @param frame: Animation frame (0-7 for 8-frame animation)
 */
void ui_render_spinner(framebuffer_t *fb, int x, int y, uint32_t frame);

/**
 * Render animated "..." dots for loading states
 *
 * @param fb: Framebuffer to render to
 * @param x: Starting X position
 * @param y: Y position
 * @param frame: Animation frame (cycles through dots)
 */
void ui_render_loading_dots(framebuffer_t *fb, int x, int y, uint32_t frame);

/*
 * Text Wrapping Helper (for dialogs and messages)
 */

/**
 * Wrap text to fit within a specific width
 *
 * @param text: Input text
 * @param max_width_chars: Maximum characters per line
 * @param lines: Output array of line pointers (allocated strings, must be freed)
 * @param max_lines: Maximum number of lines
 * @return: Number of lines created
 */
int ui_wrap_text(const char *text, int max_width_chars, char **lines, int max_lines);

/**
 * Free wrapped text lines created by ui_wrap_text
 *
 * @param lines: Array of line pointers
 * @param line_count: Number of lines to free
 */
void ui_free_wrapped_text(char **lines, int line_count);

#endif /* UI_COMPONENTS_H */
