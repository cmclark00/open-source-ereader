/*
 * reader.h - E-Reader Text Reading View
 *
 * Provides reading view UI for displaying book pages with navigation.
 * Handles page turning, status bar display, and reading state management.
 *
 * Phase 03: Basic E-Reader Application
 */

#ifndef READER_H
#define READER_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../rendering/text_renderer.h"
#include "../books/book_manager.h"
#include "../../button-test/button_input.h"

/*
 * Reader Configuration Constants
 */
#define READER_CONTROL_HINTS    "UP:Prev  DOWN:Next  BACK:Exit"

/* Layout Constants (based on 400x300 display, 8x16 font) */
#define READER_STATUS_BAR_LINE   0       /* Line 0: Status bar with title and page */
#define READER_SEPARATOR_LINE    1       /* Line 1: Separator */
#define READER_FIRST_TEXT_LINE   2       /* Line 2: First line of text */
#define READER_LAST_TEXT_LINE    15      /* Line 15: Last line of text */
#define READER_SEPARATOR_2_LINE  16      /* Line 16: Separator */
#define READER_HINTS_LINE        17      /* Line 17: Control hints */

#define READER_TEXT_LINES        14      /* Number of text lines (lines 2-15) */

/* Error codes */
typedef enum {
    READER_SUCCESS = 0,
    READER_ERROR_NULL_POINTER = -1,
    READER_ERROR_INVALID_STATE = -2,
    READER_ERROR_RENDER_FAILED = -3,
    READER_ERROR_NO_PAGES = -4,
    READER_ERROR_PAGINATION_FAILED = -5
} reader_error_t;

/* Reader action result - what happened after handling an event */
typedef enum {
    READER_ACTION_NONE,           /* No action taken */
    READER_ACTION_REDRAW,         /* Page needs to be redrawn */
    READER_ACTION_PAGE_CHANGED,   /* Page changed (implies redraw) */
    READER_ACTION_EXIT,           /* User wants to exit reader */
    READER_ACTION_SAVE_BOOKMARK   /* Save current position */
} reader_action_t;

/*
 * Reader State Structure
 *
 * Maintains current reading state including book, pagination, and current page
 */
typedef struct {
    book_t *book;                   /* Currently loaded book (not owned by reader) */
    pagination_t *pagination;       /* Pagination context for current book (owned) */
    bookmark_list_t *bookmarks;     /* Pointer to bookmarks (not owned by reader) */

    int current_page;               /* Current page number (0-based) */
    int total_pages;                /* Total number of pages in book */

    bool needs_redraw;              /* Flag indicating full redraw is needed */
    int refresh_counter;            /* Counter for forcing full refresh (prevent ghosting) */
} reader_state_t;

/*
 * Reader Initialization and Cleanup
 */

/**
 * Create and initialize a new reader state
 *
 * @param book: Pointer to loaded book (must remain valid during reader lifetime)
 * @param bookmarks: Pointer to bookmarks (must remain valid during reader lifetime)
 * @param initial_page: Page to start reading at (0-based, -1 = use bookmark)
 * @return: Pointer to reader state, or NULL on error
 */
reader_state_t* reader_create(book_t *book, bookmark_list_t *bookmarks, int initial_page);

/**
 * Free reader state and associated resources
 *
 * @param reader: Reader state to free (may be NULL)
 */
void reader_free(reader_state_t *reader);

/**
 * Reset reader to first page
 *
 * @param reader: Reader state to reset
 */
void reader_reset(reader_state_t *reader);

/*
 * Reader Rendering
 */

/**
 * Render the complete reading view to framebuffer
 *
 * Draws status bar, current page text, and control hints.
 *
 * @param reader: Reader state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int reader_render(reader_state_t *reader, framebuffer_t *fb);

/**
 * Render just the status bar (page indicator and book title)
 *
 * @param reader: Reader state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int reader_render_status_bar(reader_state_t *reader, framebuffer_t *fb);

/**
 * Render the current page of text
 *
 * @param reader: Reader state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int reader_render_page(reader_state_t *reader, framebuffer_t *fb);

/**
 * Render control hints at bottom of screen
 *
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int reader_render_hints(framebuffer_t *fb);

/**
 * Render empty book message (when book has no content)
 *
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int reader_render_empty(framebuffer_t *fb);

/*
 * Reader Navigation and Input Handling
 */

/**
 * Handle button event and update reader state
 *
 * @param reader: Reader state
 * @param event: Button event to handle
 * @return: Reader action indicating what happened
 */
reader_action_t reader_handle_event(reader_state_t *reader, const button_event_t *event);

/**
 * Go to next page
 *
 * @param reader: Reader state
 * @return: true if page changed, false if already at last page
 */
bool reader_next_page(reader_state_t *reader);

/**
 * Go to previous page
 *
 * @param reader: Reader state
 * @return: true if page changed, false if already at first page
 */
bool reader_prev_page(reader_state_t *reader);

/**
 * Go to specific page
 *
 * @param reader: Reader state
 * @param page: Page number (0-based)
 * @return: true if page changed, false if invalid page number
 */
bool reader_goto_page(reader_state_t *reader, int page);

/**
 * Get current page number
 *
 * @param reader: Reader state
 * @return: Current page number (0-based)
 */
int reader_get_current_page(reader_state_t *reader);

/**
 * Get total page count
 *
 * @param reader: Reader state
 * @return: Total number of pages
 */
int reader_get_total_pages(reader_state_t *reader);

/**
 * Save current reading position as bookmark
 *
 * @param reader: Reader state
 * @return: 0 on success, negative error code on failure
 */
int reader_save_bookmark(reader_state_t *reader);

/*
 * Reader State Queries
 */

/**
 * Check if reader needs to be redrawn
 *
 * @param reader: Reader state
 * @return: true if redraw needed, false otherwise
 */
bool reader_needs_redraw(reader_state_t *reader);

/**
 * Clear the needs_redraw flag
 *
 * @param reader: Reader state
 */
void reader_clear_redraw_flag(reader_state_t *reader);

/**
 * Check if on first page
 *
 * @param reader: Reader state
 * @return: true if on first page, false otherwise
 */
bool reader_is_first_page(reader_state_t *reader);

/**
 * Check if on last page
 *
 * @param reader: Reader state
 * @return: true if on last page, false otherwise
 */
bool reader_is_last_page(reader_state_t *reader);

/**
 * Check if book has no pages (empty)
 *
 * @param reader: Reader state
 * @return: true if book is empty, false otherwise
 */
bool reader_is_empty(reader_state_t *reader);

/*
 * Utility Functions
 */

/**
 * Truncate a book title to fit in status bar
 *
 * @param title: Original title
 * @param max_length: Maximum characters
 * @param buffer: Output buffer
 * @param buffer_size: Size of output buffer
 */
void reader_truncate_title(const char *title, int max_length, char *buffer, size_t buffer_size);

/**
 * Format page indicator string (e.g., "[5/42]")
 *
 * @param current_page: Current page (1-based for display)
 * @param total_pages: Total pages
 * @param buffer: Output buffer
 * @param buffer_size: Size of output buffer
 */
void reader_format_page_indicator(int current_page, int total_pages, char *buffer, size_t buffer_size);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* reader_error_string(reader_error_t error);

#endif /* READER_H */
