/*
 * text_renderer.h - Text Rendering Engine
 *
 * Provides text rendering capabilities with word wrapping and pagination.
 * Supports multiple font sizes: small (6x12), medium (8x16), large (10x20).
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

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <stdint.h>
#include <stdbool.h>
#include "framebuffer.h"

/* Font size options - matches settings_manager.h */
typedef enum {
    TEXT_FONT_SIZE_SMALL = 0,   /* 6x12 font - ~63 chars/line, 21 lines/page */
    TEXT_FONT_SIZE_MEDIUM = 1,  /* 8x16 font - ~47 chars/line, 14 lines/page */
    TEXT_FONT_SIZE_LARGE = 2    /* 10x20 font - ~38 chars/line, 11 lines/page */
} text_font_size_t;

/* Font Specifications - these are dynamic based on current font size */
#define FONT_FIRST_CHAR     32   /* First printable ASCII character (space) */
#define FONT_LAST_CHAR      126  /* Last printable ASCII character (~) */

/* Legacy macros for backward compatibility - now dynamic */
#define FONT_WIDTH          text_renderer_get_font_width()
#define FONT_HEIGHT         text_renderer_get_font_height()

/* Rendering Configuration */
#define MARGIN_TOP          20   /* Top margin in pixels */
#define MARGIN_BOTTOM       20   /* Bottom margin in pixels */
#define MARGIN_LEFT         10   /* Left margin in pixels */
#define MARGIN_RIGHT        10   /* Right margin in pixels */
#define LINE_SPACING        2    /* Extra pixels between lines */

/* Calculated Layout Constants */
#define TEXT_AREA_WIDTH     (FB_WIDTH - MARGIN_LEFT - MARGIN_RIGHT)   /* 380 pixels */
#define TEXT_AREA_HEIGHT    (FB_HEIGHT - MARGIN_TOP - MARGIN_BOTTOM)  /* 260 pixels */
#define CHARS_PER_LINE      (TEXT_AREA_WIDTH / FONT_WIDTH)            /* 47 chars */
#define LINE_HEIGHT         (FONT_HEIGHT + LINE_SPACING)              /* 18 pixels */
#define LINES_PER_PAGE      (TEXT_AREA_HEIGHT / LINE_HEIGHT)          /* 14 lines */

/* Maximum Text Sizes */
#define MAX_LINE_LENGTH     256   /* Maximum characters per line buffer */
#define MAX_LINES_IN_PAGE   32    /* Maximum lines that can be stored in a page */

/* Text Alignment Options */
typedef enum {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} text_align_t;

/* Page Structure - represents one screen of text */
typedef struct {
    char *lines[MAX_LINES_IN_PAGE];  /* Array of line pointers */
    int line_count;                   /* Number of lines in this page */
    int start_offset;                 /* Character offset in source text where page starts */
    int end_offset;                   /* Character offset in source text where page ends */
} text_page_t;

/* Pagination Context - manages splitting text into pages */
typedef struct {
    text_page_t *pages;      /* Array of pages */
    int page_count;          /* Total number of pages */
    int current_page;        /* Currently displayed page (0-indexed) */
    char *source_text;       /* Original text being paginated */
    size_t source_length;    /* Length of source text */
} pagination_t;

/**
 * Initialize the text renderer (loads font data)
 * @return: 0 on success, -1 on error
 */
int text_renderer_init(void);

/**
 * Render a single character to the framebuffer
 * @param fb: Pointer to framebuffer
 * @param x: X coordinate (top-left of character)
 * @param y: Y coordinate (top-left of character)
 * @param c: Character to render
 * @param color: COLOR_BLACK or COLOR_WHITE
 * @return: Width of rendered character (typically FONT_WIDTH)
 */
int text_render_char(framebuffer_t *fb, int x, int y, char c, uint8_t color);

/**
 * Render a string to the framebuffer (no word wrapping)
 * @param fb: Pointer to framebuffer
 * @param x: Starting X coordinate
 * @param y: Starting Y coordinate
 * @param text: Null-terminated string to render
 * @param color: COLOR_BLACK or COLOR_WHITE
 * @return: Number of characters rendered
 */
int text_render_string(framebuffer_t *fb, int x, int y, const char *text, uint8_t color);

/**
 * Render a string with word wrapping
 * @param fb: Pointer to framebuffer
 * @param x: Starting X coordinate
 * @param y: Starting Y coordinate
 * @param text: Null-terminated string to render
 * @param max_width: Maximum width in pixels (for wrapping)
 * @param color: COLOR_BLACK or COLOR_WHITE
 * @return: Number of lines rendered
 */
int text_render_wrapped(framebuffer_t *fb, int x, int y, const char *text,
                       int max_width, uint8_t color);

/**
 * Calculate text layout (split text into lines with word wrapping)
 * @param text: Source text to layout
 * @param max_width: Maximum line width in pixels
 * @param lines: Output array of line strings (caller must allocate)
 * @param max_lines: Maximum number of lines to generate
 * @return: Number of lines generated (may be less than max_lines)
 */
int text_calculate_layout(const char *text, int max_width,
                         char **lines, int max_lines);

/**
 * Create pagination context for a text string
 * @param text: Source text to paginate (must remain valid during pagination lifetime)
 * @param text_length: Length of source text
 * @return: Pointer to pagination context, or NULL on error
 */
pagination_t* text_create_pagination(const char *text, size_t text_length);

/**
 * Free pagination context and associated memory
 * @param pg: Pagination context to free
 */
void text_free_pagination(pagination_t *pg);

/**
 * Get a specific page from pagination context
 * @param pg: Pagination context
 * @param page_index: Page number (0-indexed)
 * @return: Pointer to page structure, or NULL if invalid
 */
text_page_t* text_get_page(pagination_t *pg, int page_index);

/**
 * Render a page to the framebuffer
 * @param fb: Pointer to framebuffer
 * @param page: Page to render
 * @param color: COLOR_BLACK or COLOR_WHITE
 * @return: 0 on success, -1 on error
 */
int text_render_page(framebuffer_t *fb, text_page_t *page, uint8_t color);

/**
 * Measure the width of a string in pixels
 * @param text: String to measure
 * @param length: Number of characters to measure (or -1 for entire string)
 * @return: Width in pixels
 */
int text_measure_width(const char *text, int length);

/**
 * Get the number of characters that fit in a given width
 * @param text: String to measure
 * @param max_width: Maximum width in pixels
 * @return: Number of characters that fit
 */
int text_chars_in_width(const char *text, int max_width);

/**
 * Set the current font size
 * This affects all subsequent rendering operations and pagination calculations.
 * Call this before creating pagination contexts to use the new font size.
 * @param size: Font size to use (SMALL, MEDIUM, or LARGE)
 */
void text_renderer_set_font_size(text_font_size_t size);

/**
 * Get the current font size
 * @return: Current font size setting
 */
text_font_size_t text_renderer_get_font_size(void);

/**
 * Get the width of the current font
 * @return: Font width in pixels
 */
int text_renderer_get_font_width(void);

/**
 * Get the height of the current font
 * @return: Font height in pixels
 */
int text_renderer_get_font_height(void);

/**
 * Get the number of characters per line for the current font
 * @return: Characters that fit on one line with current font and margins
 */
int text_renderer_get_chars_per_line(void);

/**
 * Get the number of lines per page for the current font
 * @return: Lines that fit on one page with current font and margins
 */
int text_renderer_get_lines_per_page(void);

/**
 * Re-paginate existing pagination context with new font size
 * This is useful when the user changes font size while reading.
 * The function attempts to maintain approximate reading position.
 * @param pg: Existing pagination context
 * @param current_page: Current page number (0-indexed)
 * @return: New approximate page number in re-paginated content, or -1 on error
 */
int text_renderer_repaginate(pagination_t *pg, int current_page);

#endif /* TEXT_RENDERER_H */
