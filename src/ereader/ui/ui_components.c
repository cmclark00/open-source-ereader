/*
 * ui_components.c - Reusable UI Components Implementation
 *
 * Implements polished UI components for loading indicators, dialogs,
 * borders, headers, footers, and other visual feedback elements.
 *
 * Phase 05: Polish and Advanced Features
 */

#include "ui_components.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constants for rendering calculations */
#define SPINNER_FRAMES 8
#define DOTS_FRAMES 4

/* Internal helper functions */
static void draw_single_border(framebuffer_t *fb, int x, int y, int width, int height);
static void draw_double_border(framebuffer_t *fb, int x, int y, int width, int height);
static void draw_decorative_corner(framebuffer_t *fb, int x, int y, bool top_left, bool top_right,
                                    bool bottom_left, bool bottom_right);

/*
 * Loading Indicator Implementation
 */

void loading_indicator_init(loading_indicator_t *indicator, int x, int y, const char *message) {
    if (!indicator) {
        return;
    }

    indicator->x = x;
    indicator->y = y;
    indicator->frame = 0;
    indicator->show_text = (message != NULL);

    if (message) {
        strncpy(indicator->message, message, sizeof(indicator->message) - 1);
        indicator->message[sizeof(indicator->message) - 1] = '\0';
    } else {
        indicator->message[0] = '\0';
    }
}

void loading_indicator_update(loading_indicator_t *indicator) {
    if (!indicator) {
        return;
    }
    indicator->frame++;
}

void loading_indicator_render(loading_indicator_t *indicator, framebuffer_t *fb) {
    if (!indicator || !fb) {
        return;
    }

    /* Render spinner */
    ui_render_spinner(fb, indicator->x, indicator->y, indicator->frame);

    /* Render message text below spinner if provided */
    if (indicator->show_text && indicator->message[0] != '\0') {
        int msg_len = strlen(indicator->message);
        int text_x = indicator->x - (msg_len * FONT_WIDTH / 2);
        int text_y = indicator->y + LOADING_SPINNER_SIZE + 8;

        text_render_string(fb, text_x, text_y, indicator->message, COLOR_BLACK);

        /* Add animated dots */
        int dots_x = text_x + (msg_len * FONT_WIDTH);
        ui_render_loading_dots(fb, dots_x, text_y, indicator->frame);
    }
}

/*
 * Progress Bar Implementation
 */

void progress_bar_init(progress_bar_t *bar, int x, int y, int width, bool show_percentage) {
    if (!bar) {
        return;
    }

    bar->x = x;
    bar->y = y;
    bar->width = width;
    bar->height = LOADING_BAR_HEIGHT;
    bar->current = 0;
    bar->maximum = 100;
    bar->show_percentage = show_percentage;
}

void progress_bar_set_value(progress_bar_t *bar, int current, int maximum) {
    if (!bar) {
        return;
    }

    bar->current = current;
    bar->maximum = maximum;

    /* Clamp to valid range */
    if (bar->current < 0) bar->current = 0;
    if (bar->current > bar->maximum) bar->current = bar->maximum;
}

void progress_bar_render(progress_bar_t *bar, framebuffer_t *fb) {
    if (!bar || !fb) {
        return;
    }

    /* Draw border */
    fb_draw_rect(fb, bar->x - 1, bar->y - 1, bar->width + 2, bar->height + 2, COLOR_BLACK);
    fb_draw_rect(fb, bar->x, bar->y, bar->width, bar->height, COLOR_WHITE);

    /* Calculate fill width */
    int fill_width = 0;
    if (bar->maximum > 0) {
        fill_width = (bar->current * bar->width) / bar->maximum;
    }

    /* Draw filled portion */
    if (fill_width > 0) {
        fb_draw_rect(fb, bar->x, bar->y, fill_width, bar->height, COLOR_BLACK);
    }

    /* Render percentage text if requested */
    if (bar->show_percentage) {
        char percent_text[16];
        int percentage = 0;
        if (bar->maximum > 0) {
            percentage = (bar->current * 100) / bar->maximum;
        }
        snprintf(percent_text, sizeof(percent_text), "%d%%", percentage);

        int text_len = strlen(percent_text);
        int text_x = bar->x + (bar->width / 2) - (text_len * FONT_WIDTH / 2);
        int text_y = bar->y + bar->height + 4;

        text_render_string(fb, text_x, text_y, percent_text, COLOR_BLACK);
    }
}

/*
 * Confirmation Dialog Implementation
 */

void confirmation_dialog_init(confirmation_dialog_t *dialog,
                               const char *title,
                               const char *message,
                               const char *yes_text,
                               const char *no_text) {
    if (!dialog) {
        return;
    }

    /* Copy title */
    if (title) {
        strncpy(dialog->title, title, sizeof(dialog->title) - 1);
        dialog->title[sizeof(dialog->title) - 1] = '\0';
    } else {
        dialog->title[0] = '\0';
    }

    /* Copy message */
    if (message) {
        strncpy(dialog->message, message, sizeof(dialog->message) - 1);
        dialog->message[sizeof(dialog->message) - 1] = '\0';
    } else {
        dialog->message[0] = '\0';
    }

    /* Set button text */
    if (yes_text) {
        strncpy(dialog->yes_text, yes_text, sizeof(dialog->yes_text) - 1);
    } else {
        strncpy(dialog->yes_text, "Yes", sizeof(dialog->yes_text) - 1);
    }
    dialog->yes_text[sizeof(dialog->yes_text) - 1] = '\0';

    if (no_text) {
        strncpy(dialog->no_text, no_text, sizeof(dialog->no_text) - 1);
    } else {
        strncpy(dialog->no_text, "No", sizeof(dialog->no_text) - 1);
    }
    dialog->no_text[sizeof(dialog->no_text) - 1] = '\0';

    dialog->result = DIALOG_RESULT_NONE;
    dialog->is_active = false;

    /* Calculate centered position (will be finalized when shown) */
    dialog->width = DIALOG_WIDTH_SMALL;
    dialog->height = DIALOG_MIN_HEIGHT;
    dialog->x = (FB_WIDTH - dialog->width) / 2;
    dialog->y = (FB_HEIGHT - dialog->height) / 2;
}

void confirmation_dialog_show(confirmation_dialog_t *dialog) {
    if (!dialog) {
        return;
    }

    dialog->is_active = true;
    dialog->result = DIALOG_RESULT_NONE;
}

void confirmation_dialog_hide(confirmation_dialog_t *dialog) {
    if (!dialog) {
        return;
    }

    dialog->is_active = false;
}

dialog_result_t confirmation_dialog_handle_event(confirmation_dialog_t *dialog,
                                                   const button_event_t *event) {
    if (!dialog || !event || !dialog->is_active) {
        return DIALOG_RESULT_NONE;
    }

    /* Only handle button press events */
    if (event->event_type != BUTTON_EVENT_PRESS) {
        return DIALOG_RESULT_NONE;
    }

    switch (event->button) {
        case BUTTON_SELECT:
            /* User confirmed */
            dialog->result = DIALOG_RESULT_YES;
            dialog->is_active = false;
            return DIALOG_RESULT_YES;

        case BUTTON_BACK:
            /* User cancelled */
            dialog->result = DIALOG_RESULT_NO;
            dialog->is_active = false;
            return DIALOG_RESULT_NO;

        default:
            break;
    }

    return DIALOG_RESULT_NONE;
}

void confirmation_dialog_render(confirmation_dialog_t *dialog, framebuffer_t *fb) {
    if (!dialog || !fb || !dialog->is_active) {
        return;
    }

    /* Draw background box (white filled with black border) */
    fb_draw_rect(fb, dialog->x, dialog->y, dialog->width, dialog->height, COLOR_WHITE);
    ui_draw_border(fb, dialog->x, dialog->y, dialog->width, dialog->height, BORDER_DOUBLE);

    int y_offset = dialog->y + DIALOG_PADDING;

    /* Render title if present */
    if (dialog->title[0] != '\0') {
        int title_len = strlen(dialog->title);
        int title_x = dialog->x + (dialog->width - title_len * FONT_WIDTH) / 2;
        text_render_string(fb, title_x, y_offset, dialog->title, COLOR_BLACK);
        y_offset += FONT_HEIGHT + 4;

        /* Draw separator under title */
        int sep_x1 = dialog->x + DIALOG_PADDING;
        int sep_x2 = dialog->x + dialog->width - DIALOG_PADDING;
        fb_draw_hline(fb, sep_x1, y_offset, sep_x2 - sep_x1, COLOR_BLACK);
        y_offset += 8;
    }

    /* Render message (centered, multi-line support with \n) */
    char message_copy[256];
    strncpy(message_copy, dialog->message, sizeof(message_copy) - 1);
    message_copy[sizeof(message_copy) - 1] = '\0';

    char *line = strtok(message_copy, "\n");
    while (line != NULL) {
        int line_len = strlen(line);
        int line_x = dialog->x + (dialog->width - line_len * FONT_WIDTH) / 2;
        text_render_string(fb, line_x, y_offset, line, COLOR_BLACK);
        y_offset += FONT_HEIGHT;
        line = strtok(NULL, "\n");
    }

    /* Render button hints at bottom */
    y_offset = dialog->y + dialog->height - DIALOG_PADDING - FONT_HEIGHT;
    char button_hints[64];
    snprintf(button_hints, sizeof(button_hints), "SELECT:%s  BACK:%s",
             dialog->yes_text, dialog->no_text);

    int hints_len = strlen(button_hints);
    int hints_x = dialog->x + (dialog->width - hints_len * FONT_WIDTH) / 2;
    text_render_string(fb, hints_x, y_offset, button_hints, COLOR_BLACK);
}

bool confirmation_dialog_is_active(confirmation_dialog_t *dialog) {
    if (!dialog) {
        return false;
    }
    return dialog->is_active;
}

/*
 * Message Box Implementation
 */

void message_box_render(framebuffer_t *fb,
                        const char *title,
                        const char *message,
                        border_style_t border) {
    if (!fb || !message) {
        return;
    }

    /* Calculate box dimensions based on content */
    int max_line_len = 0;
    int line_count = 0;

    /* Count lines and find longest line */
    char message_copy[256];
    strncpy(message_copy, message, sizeof(message_copy) - 1);
    message_copy[sizeof(message_copy) - 1] = '\0';

    char *line = strtok(message_copy, "\n");
    while (line != NULL && line_count < 10) {
        int len = strlen(line);
        if (len > max_line_len) max_line_len = len;
        line_count++;
        line = strtok(NULL, "\n");
    }

    /* Add title to line count if present */
    int title_lines = 0;
    if (title && title[0] != '\0') {
        title_lines = 1;
        int title_len = strlen(title);
        if (title_len > max_line_len) max_line_len = title_len;
    }

    /* Calculate box size */
    int box_width = (max_line_len * FONT_WIDTH) + (DIALOG_PADDING * 2);
    int box_height = ((line_count + title_lines + 1) * FONT_HEIGHT) + (DIALOG_PADDING * 2);

    /* Ensure minimum size */
    if (box_width < DIALOG_WIDTH_SMALL) box_width = DIALOG_WIDTH_SMALL;
    if (box_height < DIALOG_MIN_HEIGHT) box_height = DIALOG_MIN_HEIGHT;

    /* Center on screen */
    int box_x = (FB_WIDTH - box_width) / 2;
    int box_y = (FB_HEIGHT - box_height) / 2;

    /* Draw box background */
    fb_draw_rect(fb, box_x, box_y, box_width, box_height, COLOR_WHITE);

    /* Draw border */
    ui_draw_border(fb, box_x, box_y, box_width, box_height, border);

    /* Render title if present */
    int y_offset = box_y + DIALOG_PADDING;
    if (title && title[0] != '\0') {
        int title_len = strlen(title);
        int title_x = box_x + (box_width - title_len * FONT_WIDTH) / 2;
        text_render_string(fb, title_x, y_offset, title, COLOR_BLACK);
        y_offset += FONT_HEIGHT + 4;

        /* Separator under title */
        fb_draw_hline(fb, box_x + DIALOG_PADDING, y_offset,
                      box_width - (DIALOG_PADDING * 2), COLOR_BLACK);
        y_offset += 8;
    }

    /* Render message lines (re-tokenize) */
    strncpy(message_copy, message, sizeof(message_copy) - 1);
    line = strtok(message_copy, "\n");
    while (line != NULL) {
        int line_len = strlen(line);
        int line_x = box_x + (box_width - line_len * FONT_WIDTH) / 2;
        text_render_string(fb, line_x, y_offset, line, COLOR_BLACK);
        y_offset += FONT_HEIGHT;
        line = strtok(NULL, "\n");
    }
}

/*
 * Border Drawing Implementation
 */

void ui_draw_border(framebuffer_t *fb, int x, int y, int width, int height, border_style_t style) {
    if (!fb) {
        return;
    }

    switch (style) {
        case BORDER_SINGLE:
            draw_single_border(fb, x, y, width, height);
            break;

        case BORDER_DOUBLE:
            draw_double_border(fb, x, y, width, height);
            break;

        case BORDER_DECORATIVE:
            draw_single_border(fb, x, y, width, height);
            ui_draw_corner_ornaments(fb, x, y, width, height);
            break;

        case BORDER_NONE:
        default:
            /* No border */
            break;
    }
}

static void draw_single_border(framebuffer_t *fb, int x, int y, int width, int height) {
    /* Top */
    fb_draw_hline(fb, x, y, width, COLOR_BLACK);
    /* Bottom */
    fb_draw_hline(fb, x, y + height - 1, width, COLOR_BLACK);
    /* Left */
    fb_draw_vline(fb, x, y, height, COLOR_BLACK);
    /* Right */
    fb_draw_vline(fb, x + width - 1, y, height, COLOR_BLACK);
}

static void draw_double_border(framebuffer_t *fb, int x, int y, int width, int height) {
    /* Outer border */
    draw_single_border(fb, x, y, width, height);
    /* Inner border (2 pixels inset) */
    draw_single_border(fb, x + 2, y + 2, width - 4, height - 4);
}

void ui_draw_corner_ornaments(framebuffer_t *fb, int x, int y, int width, int height) {
    if (!fb) {
        return;
    }

    /* Draw decorative corners (small diagonal lines) */
    int ornament_size = 6;

    /* Top-left */
    for (int i = 0; i < ornament_size; i++) {
        fb_set_pixel(fb, x + 2 + i, y + 2 + i, COLOR_BLACK);
    }

    /* Top-right */
    for (int i = 0; i < ornament_size; i++) {
        fb_set_pixel(fb, x + width - 3 - i, y + 2 + i, COLOR_BLACK);
    }

    /* Bottom-left */
    for (int i = 0; i < ornament_size; i++) {
        fb_set_pixel(fb, x + 2 + i, y + height - 3 - i, COLOR_BLACK);
    }

    /* Bottom-right */
    for (int i = 0; i < ornament_size; i++) {
        fb_set_pixel(fb, x + width - 3 - i, y + height - 3 - i, COLOR_BLACK);
    }
}

/*
 * Header and Footer Helpers
 */

void ui_render_header(framebuffer_t *fb, const char *title, const char *page_info, int line_number) {
    if (!fb || !title) {
        return;
    }

    int y = MARGIN_TOP + (line_number * LINE_HEIGHT);
    char header_text[MAX_LINE_LENGTH];

    if (page_info) {
        /* Title on left, page info on right */
        int title_len = strlen(title);
        int info_len = strlen(page_info);
        int padding = CHARS_PER_LINE - title_len - info_len;
        if (padding < 1) padding = 1;

        snprintf(header_text, sizeof(header_text), "%s%*s%s",
                 title, padding, "", page_info);
    } else {
        /* Just title */
        strncpy(header_text, title, sizeof(header_text) - 1);
        header_text[sizeof(header_text) - 1] = '\0';
    }

    text_render_string(fb, MARGIN_LEFT, y, header_text, COLOR_BLACK);
}

void ui_render_footer(framebuffer_t *fb, const char *hints, int line_number) {
    if (!fb || !hints) {
        return;
    }

    int y = MARGIN_TOP + (line_number * LINE_HEIGHT);
    text_render_string(fb, MARGIN_LEFT, y, hints, COLOR_BLACK);
}

void ui_render_separator(framebuffer_t *fb, int y, bool decorative) {
    if (!fb) {
        return;
    }

    int x1 = MARGIN_LEFT;
    int x2 = FB_WIDTH - MARGIN_RIGHT - 1;

    /* Draw main horizontal line */
    fb_draw_hline(fb, x1, y, x2 - x1, COLOR_BLACK);

    if (decorative) {
        /* Add decorative end caps (small vertical lines) */
        fb_draw_vline(fb, x1, y - 2, 5, COLOR_BLACK);
        fb_draw_vline(fb, x2, y - 2, 5, COLOR_BLACK);
    }
}

/*
 * Visual Feedback Elements
 */

void ui_render_toast(framebuffer_t *fb, const char *message) {
    if (!fb || !message) {
        return;
    }

    int msg_len = strlen(message);
    int toast_width = (msg_len * FONT_WIDTH) + 20;
    int toast_height = FONT_HEIGHT + 10;
    int toast_x = (FB_WIDTH - toast_width) / 2;
    int toast_y = FB_HEIGHT - toast_height - 30;

    /* Draw toast background */
    fb_draw_rect(fb, toast_x, toast_y, toast_width, toast_height, COLOR_WHITE);
    ui_draw_border(fb, toast_x, toast_y, toast_width, toast_height, BORDER_SINGLE);

    /* Render message */
    int text_x = toast_x + 10;
    int text_y = toast_y + 5;
    text_render_string(fb, text_x, text_y, message, COLOR_BLACK);
}

void ui_render_spinner(framebuffer_t *fb, int x, int y, uint32_t frame) {
    if (!fb) {
        return;
    }

    /* 8-frame spinner animation (rotating line segments) */
    int radius = LOADING_SPINNER_SIZE / 2;
    int current_frame = frame % SPINNER_FRAMES;

    /* Draw rotating segments based on frame */
    switch (current_frame) {
        case 0: /* Horizontal right */
            fb_draw_hline(fb, x, y, radius, COLOR_BLACK);
            break;
        case 1: /* Diagonal down-right */
            for (int i = 0; i < radius; i++) {
                fb_set_pixel(fb, x + i, y + i, COLOR_BLACK);
            }
            break;
        case 2: /* Vertical down */
            fb_draw_vline(fb, x, y, radius, COLOR_BLACK);
            break;
        case 3: /* Diagonal down-left */
            for (int i = 0; i < radius; i++) {
                fb_set_pixel(fb, x - i, y + i, COLOR_BLACK);
            }
            break;
        case 4: /* Horizontal left */
            fb_draw_hline(fb, x - radius, y, radius, COLOR_BLACK);
            break;
        case 5: /* Diagonal up-left */
            for (int i = 0; i < radius; i++) {
                fb_set_pixel(fb, x - i, y - i, COLOR_BLACK);
            }
            break;
        case 6: /* Vertical up */
            fb_draw_vline(fb, x, y - radius, radius, COLOR_BLACK);
            break;
        case 7: /* Diagonal up-right */
            for (int i = 0; i < radius; i++) {
                fb_set_pixel(fb, x + i, y - i, COLOR_BLACK);
            }
            break;
    }
}

void ui_render_loading_dots(framebuffer_t *fb, int x, int y, uint32_t frame) {
    if (!fb) {
        return;
    }

    /* Render animated "..." */
    int dot_count = (frame / 10) % (LOADING_DOT_COUNT + 1);  /* Cycle 0-3 dots */

    for (int i = 0; i < LOADING_DOT_COUNT; i++) {
        char dot = (i < dot_count) ? '.' : ' ';
        char dot_str[2] = {dot, '\0'};
        text_render_string(fb, x + (i * FONT_WIDTH), y, dot_str, COLOR_BLACK);
    }
}

/*
 * Text Wrapping Helpers
 */

int ui_wrap_text(const char *text, int max_width_chars, char **lines, int max_lines) {
    if (!text || !lines || max_lines <= 0 || max_width_chars <= 0) {
        return 0;
    }

    int line_count = 0;
    int text_len = strlen(text);
    int pos = 0;

    while (pos < text_len && line_count < max_lines) {
        int line_len = 0;
        int last_space = -1;

        /* Find break point */
        while (pos + line_len < text_len && line_len < max_width_chars) {
            if (text[pos + line_len] == '\n') {
                /* Explicit line break */
                break;
            }
            if (text[pos + line_len] == ' ') {
                last_space = line_len;
            }
            line_len++;
        }

        /* Wrap at last space if we didn't hit explicit break or end */
        if (pos + line_len < text_len && text[pos + line_len] != '\n') {
            if (last_space > 0) {
                line_len = last_space;
            }
        }

        /* Allocate and copy line */
        lines[line_count] = (char*)malloc(line_len + 1);
        if (!lines[line_count]) {
            /* Allocation failed, return what we have */
            return line_count;
        }

        strncpy(lines[line_count], text + pos, line_len);
        lines[line_count][line_len] = '\0';

        line_count++;
        pos += line_len;

        /* Skip space or newline after wrap */
        if (pos < text_len && (text[pos] == ' ' || text[pos] == '\n')) {
            pos++;
        }
    }

    return line_count;
}

void ui_free_wrapped_text(char **lines, int line_count) {
    if (!lines) {
        return;
    }

    for (int i = 0; i < line_count; i++) {
        if (lines[i]) {
            free(lines[i]);
            lines[i] = NULL;
        }
    }
}
