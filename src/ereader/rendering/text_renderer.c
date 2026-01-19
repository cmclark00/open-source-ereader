/*
 * text_renderer.c - Text Rendering Engine Implementation
 *
 * Implements text rendering with word wrapping and pagination support.
 * Supports multiple font sizes with dynamic switching.
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
#include <string.h>
#include <ctype.h>
#include "text_renderer.h"
#include "font_data.h"

/* Global state for current font size */
static text_font_size_t current_font_size = TEXT_FONT_SIZE_MEDIUM;

/* Embedded 8x16 bitmap font - Basic ASCII characters (32-90) */
/* Each character is 16 bytes (8 pixels wide x 16 pixels tall) */
/* Bit 7 is leftmost pixel, bit 0 is rightmost pixel */
static const uint8_t font_8x16[][16] = {
    /* Space (32) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* ! (33) */
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x18,
     0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    /* " (34) */
    {0x00, 0x66, 0x66, 0x66, 0x24, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* # (35) */
    {0x00, 0x00, 0x00, 0x6C, 0x6C, 0xFE, 0x6C, 0x6C,
     0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00},
    /* $ (36) */
    {0x18, 0x18, 0x7C, 0xC6, 0xC2, 0xC0, 0x7C, 0x06,
     0x06, 0x86, 0xC6, 0x7C, 0x18, 0x18, 0x00, 0x00},
    /* % (37) */
    {0x00, 0x00, 0x00, 0x00, 0xC2, 0xC6, 0x0C, 0x18,
     0x30, 0x60, 0xC6, 0x86, 0x00, 0x00, 0x00, 0x00},
    /* & (38) */
    {0x00, 0x00, 0x38, 0x6C, 0x6C, 0x38, 0x76, 0xDC,
     0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00},
    /* ' (39) */
    {0x00, 0x30, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* ( (40) */
    {0x00, 0x00, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x30,
     0x30, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00},
    /* ) (41) */
    {0x00, 0x00, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x0C,
     0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00},
    /* * (42) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3C, 0xFF,
     0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* + (43) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E,
     0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* , (44) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00},
    /* - (45) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* . (46) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    /* / (47) */
    {0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0C, 0x18,
     0x30, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00},
    /* 0-9 (48-57) */
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xCE, 0xDE, 0xF6,
     0xE6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18,
     0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0x06, 0x0C, 0x18, 0x30,
     0x60, 0xC0, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0x06, 0x06, 0x3C, 0x06,
     0x06, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x0C, 0x1C, 0x3C, 0x6C, 0xCC, 0xFE,
     0x0C, 0x0C, 0x0C, 0x1E, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFE, 0xC0, 0xC0, 0xC0, 0xFC, 0x06,
     0x06, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x38, 0x60, 0xC0, 0xC0, 0xFC, 0xC6,
     0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFE, 0xC6, 0x06, 0x06, 0x0C, 0x18,
     0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0xC6,
     0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7E, 0x06,
     0x06, 0x06, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00},
    /* : ; < = > ? @ (58-64) */
    {0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
     0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
     0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60,
     0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00,
     0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06,
     0x0C, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0x0C, 0x18, 0x18,
     0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xDE, 0xDE, 0xDE,
     0xDC, 0xC0, 0xC1, 0x7E, 0x00, 0x00, 0x00, 0x00},
    /* A-Z (65-90) */
    {0x00, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE,
     0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x66,
     0x66, 0x66, 0x66, 0xFC, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x3C, 0x66, 0xC2, 0xC0, 0xC0, 0xC0,
     0xC0, 0xC2, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xF8, 0x6C, 0x66, 0x66, 0x66, 0x66,
     0x66, 0x66, 0x6C, 0xF8, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFE, 0x66, 0x62, 0x68, 0x78, 0x68,
     0x60, 0x62, 0x66, 0xFE, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFE, 0x66, 0x62, 0x68, 0x78, 0x68,
     0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x3C, 0x66, 0xC2, 0xC0, 0xC0, 0xDE,
     0xC6, 0xC6, 0x66, 0x3A, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xFE, 0xC6,
     0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18,
     0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
     0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xE6, 0x66, 0x6C, 0x6C, 0x78, 0x78,
     0x6C, 0x66, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xF0, 0x60, 0x60, 0x60, 0x60, 0x60,
     0x60, 0x62, 0x66, 0xFE, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6,
     0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xE6, 0xF6, 0xFE, 0xDE, 0xCE,
     0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6,
     0xC6, 0xC6, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x60,
     0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6,
     0xC6, 0xD6, 0xDE, 0x7C, 0x0C, 0x0E, 0x00, 0x00},
    {0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x6C,
     0x66, 0x66, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0x60, 0x38, 0x0C,
     0x06, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7E, 0x7E, 0x5A, 0x18, 0x18, 0x18,
     0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6,
     0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6,
     0xC6, 0x6C, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xD6, 0xD6,
     0xD6, 0xFE, 0xEE, 0x6C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xC6, 0xC6, 0x6C, 0x6C, 0x38, 0x38,
     0x6C, 0x6C, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18,
     0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFE, 0xC6, 0x86, 0x0C, 0x18, 0x30,
     0x60, 0xC2, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00},
    /* Characters 91-126: Use space for now (can be extended later) */
};

/**
 * Initialize the text renderer
 */
int text_renderer_init(void) {
    /* Font is embedded, no initialization needed */
    return 0;
}

/**
 * Render a single character to the framebuffer
 */
int text_render_char(framebuffer_t *fb, int x, int y, char c, uint8_t color) {
    if (!fb) return -1;

    /* Map character to font array index */
    int char_index;
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        char_index = 0;  /* Space for unsupported characters */
    } else {
        char_index = c - FONT_FIRST_CHAR;
    }

    /* Get current font dimensions */
    int font_width = text_renderer_get_font_width();
    int font_height = text_renderer_get_font_height();

    /* Render character based on current font size */
    switch (current_font_size) {
        case TEXT_FONT_SIZE_SMALL:
            /* Small font: 6x12 */
            for (int row = 0; row < font_height && row < 12; row++) {
                uint8_t byte = (char_index < 95) ? font_6x12_data[char_index][row] : 0;
                for (int col = 0; col < font_width && col < 8; col++) {
                    if (byte & (0x80 >> col)) {
                        fb_set_pixel(fb, x + col, y + row, color);
                    }
                }
            }
            break;

        case TEXT_FONT_SIZE_LARGE:
            /* Large font: 10x20 - uses 2 bytes per row */
            for (int row = 0; row < font_height && row < 20; row++) {
                if (char_index < 95) {
                    /* Large font uses 2 bytes per row (10 pixels wide) */
                    uint16_t word = (font_10x20_data[char_index][row * 2] << 8) |
                                     font_10x20_data[char_index][row * 2 + 1];
                    for (int col = 0; col < font_width && col < 10; col++) {
                        if (word & (0x8000 >> col)) {
                            fb_set_pixel(fb, x + col, y + row, color);
                        }
                    }
                }
            }
            break;

        case TEXT_FONT_SIZE_MEDIUM:
        default:
            /* Medium font: 8x16 (existing font) */
            for (int row = 0; row < font_height && row < 16; row++) {
                uint8_t byte = (char_index <= 58) ? font_8x16[char_index][row] : 0;
                for (int col = 0; col < font_width && col < 8; col++) {
                    if (byte & (0x80 >> col)) {
                        fb_set_pixel(fb, x + col, y + row, color);
                    }
                }
            }
            break;
    }

    return font_width;
}

/**
 * Render a string to the framebuffer (no word wrapping)
 */
int text_render_string(framebuffer_t *fb, int x, int y, const char *text, uint8_t color) {
    if (!fb || !text) return 0;

    int font_width = text_renderer_get_font_width();
    int font_height = text_renderer_get_font_height();
    int line_height = font_height + LINE_SPACING;

    int cur_x = x;
    int count = 0;

    while (*text) {
        if (*text == '\n') {
            /* Newline: move to next line */
            cur_x = x;
            y += line_height;
        } else if (*text == '\t') {
            /* Tab: advance by 4 character widths */
            cur_x += font_width * 4;
        } else {
            /* Regular character */
            text_render_char(fb, cur_x, y, *text, color);
            cur_x += font_width;
            count++;
        }
        text++;
    }

    return count;
}

/**
 * Measure the width of a string in pixels
 */
int text_measure_width(const char *text, int length) {
    if (!text) return 0;

    int font_width = text_renderer_get_font_width();
    int width = 0;
    int i = 0;

    while (text[i] && (length < 0 || i < length)) {
        if (text[i] == '\t') {
            width += font_width * 4;
        } else if (text[i] != '\n') {
            width += font_width;
        }
        i++;
    }

    return width;
}

/**
 * Get the number of characters that fit in a given width
 */
int text_chars_in_width(const char *text, int max_width) {
    if (!text) return 0;

    int font_width = text_renderer_get_font_width();
    int width = 0;
    int count = 0;

    while (text[count]) {
        int char_width = (text[count] == '\t') ? (font_width * 4) : font_width;
        if (width + char_width > max_width) {
            break;
        }
        width += char_width;
        count++;
    }

    return count;
}

/**
 * Render a string with word wrapping
 */
int text_render_wrapped(framebuffer_t *fb, int x, int y, const char *text,
                       int max_width, uint8_t color) {
    if (!fb || !text) return 0;

    int font_width = text_renderer_get_font_width();
    int font_height = text_renderer_get_font_height();
    int line_height = font_height + LINE_SPACING;

    int cur_x = x;
    int cur_y = y;
    int line_count = 0;
    const char *p = text;

    while (*p) {
        /* Skip leading spaces on new lines */
        if (cur_x == x) {
            while (*p == ' ') p++;
        }

        if (*p == '\0') break;

        /* Handle explicit newlines */
        if (*p == '\n') {
            cur_x = x;
            cur_y += line_height;
            line_count++;
            p++;
            continue;
        }

        /* Find end of current word */
        const char *word_start = p;
        while (*p && *p != ' ' && *p != '\n') p++;
        int word_len = p - word_start;

        /* Measure word width */
        int word_width = text_measure_width(word_start, word_len);

        /* Check if word fits on current line */
        if (cur_x + word_width > x + max_width) {
            /* Word doesn't fit, wrap to next line */
            if (cur_x > x) {
                /* Move to next line */
                cur_x = x;
                cur_y += line_height;
                line_count++;
            } else {
                /* Word is too long for one line, break it */
                int chars_fit = text_chars_in_width(word_start, max_width);
                for (int i = 0; i < chars_fit; i++) {
                    text_render_char(fb, cur_x, cur_y, word_start[i], color);
                    cur_x += font_width;
                }
                p = word_start + chars_fit;
                cur_x = x;
                cur_y += line_height;
                line_count++;
                continue;
            }
        }

        /* Render the word */
        for (int i = 0; i < word_len; i++) {
            text_render_char(fb, cur_x, cur_y, word_start[i], color);
            cur_x += font_width;
        }

        /* Skip trailing space */
        if (*p == ' ') {
            cur_x += font_width;
            p++;
        }
    }

    return line_count + 1;
}

/**
 * Calculate text layout (split text into lines with word wrapping)
 */
int text_calculate_layout(const char *text, int max_width,
                         char **lines, int max_lines) {
    if (!text || !lines) return 0;

    int line_count = 0;
    const char *p = text;

    while (*p && line_count < max_lines) {
        /* Skip leading spaces */
        while (*p == ' ') p++;
        if (*p == '\0') break;

        /* Start new line */
        char line_buffer[MAX_LINE_LENGTH];
        int line_pos = 0;
        int line_width = 0;

        while (*p && *p != '\n' && line_count < max_lines) {
            /* Find end of word */
            const char *word_start = p;
            while (*p && *p != ' ' && *p != '\n') p++;
            int word_len = p - word_start;

            /* Measure word */
            int word_width = text_measure_width(word_start, word_len);

            /* Check if word fits */
            if (line_width + word_width > max_width) {
                if (line_width == 0) {
                    /* Word too long, break it */
                    int chars_fit = text_chars_in_width(word_start, max_width);
                    memcpy(line_buffer + line_pos, word_start, chars_fit);
                    line_pos += chars_fit;
                    p = word_start + chars_fit;
                }
                break;
            }

            /* Add word to line */
            if (line_pos > 0 && line_buffer[line_pos - 1] != ' ') {
                line_buffer[line_pos++] = ' ';
                line_width += FONT_WIDTH;
            }
            memcpy(line_buffer + line_pos, word_start, word_len);
            line_pos += word_len;
            line_width += word_width;

            /* Skip space */
            if (*p == ' ') p++;
        }

        /* Store line */
        if (line_pos > 0) {
            line_buffer[line_pos] = '\0';
            lines[line_count] = malloc(line_pos + 1);
            if (lines[line_count]) {
                strcpy(lines[line_count], line_buffer);
                line_count++;
            }
        }

        /* Skip newline */
        if (*p == '\n') p++;
    }

    return line_count;
}

/**
 * Create pagination context for a text string
 */
pagination_t* text_create_pagination(const char *text, size_t text_length) {
    if (!text) return NULL;

    pagination_t *pg = calloc(1, sizeof(pagination_t));
    if (!pg) return NULL;

    /* Store reference to source text */
    pg->source_text = (char *)text;
    pg->source_length = text_length;

    /* Calculate maximum lines we can store */
    char **all_lines = malloc(sizeof(char *) * 1000);  /* Temporary storage */
    if (!all_lines) {
        free(pg);
        return NULL;
    }

    /* Split text into lines */
    int total_lines = text_calculate_layout(text, TEXT_AREA_WIDTH, all_lines, 1000);

    /* Calculate number of pages */
    pg->page_count = (total_lines + LINES_PER_PAGE - 1) / LINES_PER_PAGE;
    if (pg->page_count == 0) pg->page_count = 1;

    /* Allocate page structures */
    pg->pages = calloc(pg->page_count, sizeof(text_page_t));
    if (!pg->pages) {
        for (int i = 0; i < total_lines; i++) {
            free(all_lines[i]);
        }
        free(all_lines);
        free(pg);
        return NULL;
    }

    /* Distribute lines into pages */
    int line_idx = 0;
    for (int page = 0; page < pg->page_count; page++) {
        text_page_t *p = &pg->pages[page];
        p->line_count = 0;

        for (int i = 0; i < LINES_PER_PAGE && line_idx < total_lines; i++) {
            p->lines[p->line_count++] = all_lines[line_idx++];
        }
    }

    free(all_lines);
    pg->current_page = 0;

    return pg;
}

/**
 * Free pagination context and associated memory
 */
void text_free_pagination(pagination_t *pg) {
    if (!pg) return;

    if (pg->pages) {
        for (int page = 0; page < pg->page_count; page++) {
            text_page_t *p = &pg->pages[page];
            for (int i = 0; i < p->line_count; i++) {
                free(p->lines[i]);
            }
        }
        free(pg->pages);
    }

    free(pg);
}

/**
 * Get a specific page from pagination context
 */
text_page_t* text_get_page(pagination_t *pg, int page_index) {
    if (!pg || !pg->pages) return NULL;
    if (page_index < 0 || page_index >= pg->page_count) return NULL;

    return &pg->pages[page_index];
}

/**
 * Render a page to the framebuffer
 */
int text_render_page(framebuffer_t *fb, text_page_t *page, uint8_t color) {
    if (!fb || !page) return -1;

    int y = MARGIN_TOP;

    for (int i = 0; i < page->line_count; i++) {
        if (page->lines[i]) {
            text_render_string(fb, MARGIN_LEFT, y, page->lines[i], color);
            y += LINE_HEIGHT;
        }
    }

    return 0;
}

/* =============================================================================
 * Dynamic Font Size Support
 * ============================================================================= */

/**
 * Set the current font size
 */
void text_renderer_set_font_size(text_font_size_t size) {
    if (size >= TEXT_FONT_SIZE_SMALL && size <= TEXT_FONT_SIZE_LARGE) {
        current_font_size = size;
    }
}

/**
 * Get the current font size
 */
text_font_size_t text_renderer_get_font_size(void) {
    return current_font_size;
}

/**
 * Get the width of the current font
 */
int text_renderer_get_font_width(void) {
    switch (current_font_size) {
        case TEXT_FONT_SIZE_SMALL:  return FONT_SMALL_WIDTH;   /* 6 pixels */
        case TEXT_FONT_SIZE_MEDIUM: return FONT_MEDIUM_WIDTH;  /* 8 pixels */
        case TEXT_FONT_SIZE_LARGE:  return FONT_LARGE_WIDTH;   /* 10 pixels */
        default:                    return FONT_MEDIUM_WIDTH;
    }
}

/**
 * Get the height of the current font
 */
int text_renderer_get_font_height(void) {
    switch (current_font_size) {
        case TEXT_FONT_SIZE_SMALL:  return FONT_SMALL_HEIGHT;   /* 12 pixels */
        case TEXT_FONT_SIZE_MEDIUM: return FONT_MEDIUM_HEIGHT;  /* 16 pixels */
        case TEXT_FONT_SIZE_LARGE:  return FONT_LARGE_HEIGHT;   /* 20 pixels */
        default:                    return FONT_MEDIUM_HEIGHT;
    }
}

/**
 * Get the number of characters per line for the current font
 */
int text_renderer_get_chars_per_line(void) {
    int text_area_width = FB_WIDTH - MARGIN_LEFT - MARGIN_RIGHT;  /* 380 pixels */
    int font_width = text_renderer_get_font_width();
    return text_area_width / font_width;
}

/**
 * Get the number of lines per page for the current font
 */
int text_renderer_get_lines_per_page(void) {
    int text_area_height = FB_HEIGHT - MARGIN_TOP - MARGIN_BOTTOM;  /* 260 pixels */
    int font_height = text_renderer_get_font_height();
    int line_height = font_height + LINE_SPACING;
    return text_area_height / line_height;
}

/**
 * Re-paginate existing pagination context with new font size
 */
int text_renderer_repaginate(pagination_t *pg, int current_page) {
    if (!pg || !pg->source_text) return -1;

    /* Calculate current position as percentage through the document */
    int old_page_count = pg->page_count;
    if (old_page_count == 0) return 0;

    /* Approximate character offset based on current page */
    float position_percent = (float)current_page / (float)old_page_count;

    /* Free existing pagination data but keep source text pointer */
    char *source_backup = pg->source_text;
    size_t source_len_backup = pg->source_length;

    if (pg->pages) {
        for (int page = 0; page < pg->page_count; page++) {
            text_page_t *p = &pg->pages[page];
            for (int i = 0; i < p->line_count; i++) {
                free(p->lines[i]);
            }
        }
        free(pg->pages);
        pg->pages = NULL;
    }

    /* Recalculate layout with new font */
    int text_area_width = FB_WIDTH - MARGIN_LEFT - MARGIN_RIGHT;
    char **all_lines = malloc(sizeof(char *) * 1000);
    if (!all_lines) return -1;

    int total_lines = text_calculate_layout(source_backup, text_area_width, all_lines, 1000);
    int lines_per_page = text_renderer_get_lines_per_page();

    /* Calculate new page count */
    pg->page_count = (total_lines + lines_per_page - 1) / lines_per_page;
    if (pg->page_count == 0) pg->page_count = 1;

    /* Allocate new page structures */
    pg->pages = calloc(pg->page_count, sizeof(text_page_t));
    if (!pg->pages) {
        for (int i = 0; i < total_lines; i++) {
            free(all_lines[i]);
        }
        free(all_lines);
        return -1;
    }

    /* Distribute lines into pages */
    int line_idx = 0;
    for (int page = 0; page < pg->page_count; page++) {
        text_page_t *p = &pg->pages[page];
        p->line_count = 0;

        for (int i = 0; i < lines_per_page && line_idx < total_lines; i++) {
            p->lines[p->line_count++] = all_lines[line_idx++];
        }
    }

    free(all_lines);

    /* Restore source text references */
    pg->source_text = source_backup;
    pg->source_length = source_len_backup;

    /* Calculate new page position */
    int new_page = (int)(position_percent * pg->page_count);
    if (new_page >= pg->page_count) new_page = pg->page_count - 1;
    if (new_page < 0) new_page = 0;

    return new_page;
}
