/*
 * reader.c - E-Reader Text Reading View Implementation
 *
 * Implements the reading view UI for displaying book pages with navigation.
 *
 * Phase 03: Basic E-Reader Application
 * Phase 04: Multi-format support (TXT, EPUB, PDF)
 */

#include "reader.h"
#include "../rendering/text_renderer.h"
#include "../formats/format_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal helper function prototypes */
static void reader_draw_separator_line(framebuffer_t *fb, int line_number);

/*
 * Reader Initialization and Cleanup
 */

reader_state_t* reader_create(book_t *book, book_metadata_t *metadata, bookmark_list_t *bookmarks, int initial_page) {
    if (!book || !book->text) {
        return NULL;
    }

    reader_state_t *reader = (reader_state_t*)calloc(1, sizeof(reader_state_t));
    if (!reader) {
        return NULL;
    }

    reader->book = book;
    reader->metadata = metadata;
    reader->bookmarks = bookmarks;
    reader->needs_redraw = true;
    reader->refresh_counter = 0;

    /* Create pagination for the book */
    reader->pagination = text_create_pagination(book->text, book->text_length);
    if (!reader->pagination) {
        free(reader);
        return NULL;
    }

    reader->total_pages = reader->pagination->page_count;

    /* Determine initial page */
    if (initial_page == -1) {
        /* Use bookmark if available */
        if (bookmarks) {
            int bookmark_page = bookmark_get(bookmarks, book->filename);
            if (bookmark_page >= 0 && bookmark_page < reader->total_pages) {
                reader->current_page = bookmark_page;
            } else {
                reader->current_page = 0;
            }
        } else {
            reader->current_page = 0;
        }
    } else if (initial_page >= 0 && initial_page < reader->total_pages) {
        reader->current_page = initial_page;
    } else {
        reader->current_page = 0;
    }

    return reader;
}

void reader_free(reader_state_t *reader) {
    if (reader) {
        /* Free pagination context (we own this) */
        if (reader->pagination) {
            text_free_pagination(reader->pagination);
        }
        /* Note: We don't free book or bookmarks as they're not owned by reader */
        free(reader);
    }
}

void reader_reset(reader_state_t *reader) {
    if (!reader) {
        return;
    }

    reader->current_page = 0;
    reader->needs_redraw = true;
    reader->refresh_counter = 0;
}

/*
 * Reader Rendering
 */

int reader_render(reader_state_t *reader, framebuffer_t *fb) {
    if (!reader || !fb) {
        return READER_ERROR_NULL_POINTER;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Check if book is empty */
    if (reader->total_pages == 0) {
        return reader_render_empty(fb);
    }

    /* Render status bar */
    if (reader_render_status_bar(reader, fb) != READER_SUCCESS) {
        return READER_ERROR_RENDER_FAILED;
    }

    /* Render separator after status bar */
    reader_draw_separator_line(fb, READER_SEPARATOR_LINE);

    /* Render current page text */
    if (reader_render_page(reader, fb) != READER_SUCCESS) {
        return READER_ERROR_RENDER_FAILED;
    }

    /* Render separator before hints */
    reader_draw_separator_line(fb, READER_SEPARATOR_2_LINE);

    /* Render control hints */
    if (reader_render_hints(fb) != READER_SUCCESS) {
        return READER_ERROR_RENDER_FAILED;
    }

    /* Increment refresh counter */
    reader->refresh_counter++;

    return READER_SUCCESS;
}

int reader_render_status_bar(reader_state_t *reader, framebuffer_t *fb) {
    if (!reader || !fb || !reader->book || !reader->metadata) {
        return READER_ERROR_NULL_POINTER;
    }

    char status_bar[MAX_LINE_LENGTH];
    char page_indicator[32];
    char title_with_format[MAX_LINE_LENGTH];
    char title_buffer[MAX_LINE_LENGTH];
    char format_indicator;

    /* Get format indicator character */
    format_indicator = format_get_type_indicator(reader->metadata->format);

    /* Format page indicator [current/total] - 1-based for user display */
    reader_format_page_indicator(reader->current_page + 1, reader->total_pages,
                                  page_indicator, sizeof(page_indicator));

    /* Use book title from metadata (or filename if title is empty) */
    const char *display_title = (reader->metadata->title[0] != '\0') 
                                 ? reader->metadata->title 
                                 : reader->book->filename;

    /* Create title with format indicator: [F] Title */
    snprintf(title_with_format, sizeof(title_with_format), "[%c] %s",
             format_indicator, display_title);

    /* Truncate if needed (account for format indicator: "[F] ") */
    reader_truncate_title(title_with_format, CHARS_PER_LINE - 10,
                          title_buffer, sizeof(title_buffer));

    /* Create status bar with title on left, page on right */
    int title_len = strlen(title_buffer);
    int indicator_len = strlen(page_indicator);
    int padding_spaces = CHARS_PER_LINE - title_len - indicator_len;

    if (padding_spaces < 1) {
        padding_spaces = 1;
    }

    snprintf(status_bar, sizeof(status_bar), "%s%*s%s",
             title_buffer, padding_spaces, "", page_indicator);

    /* Render to framebuffer at line 0 */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (READER_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, status_bar, COLOR_BLACK);

    return READER_SUCCESS;
}

int reader_render_page(reader_state_t *reader, framebuffer_t *fb) {
    if (!reader || !fb || !reader->pagination) {
        return READER_ERROR_NULL_POINTER;
    }

    if (reader->current_page < 0 || reader->current_page >= reader->total_pages) {
        return READER_ERROR_INVALID_STATE;
    }

    /* Get the current page from pagination */
    text_page_t *page = text_get_page(reader->pagination, reader->current_page);
    if (!page) {
        return READER_ERROR_PAGINATION_FAILED;
    }

    /* Render page text starting at line 2 */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (READER_FIRST_TEXT_LINE * LINE_HEIGHT);

    /* Render each line of the page */
    for (int i = 0; i < page->line_count && i < READER_TEXT_LINES; i++) {
        if (page->lines[i]) {
            text_render_string(fb, x, y + (i * LINE_HEIGHT), page->lines[i], COLOR_BLACK);
        }
    }

    return READER_SUCCESS;
}

int reader_render_hints(framebuffer_t *fb) {
    if (!fb) {
        return READER_ERROR_NULL_POINTER;
    }

    /* Render control hints at bottom */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (READER_HINTS_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, READER_CONTROL_HINTS, COLOR_BLACK);

    return READER_SUCCESS;
}

int reader_render_empty(framebuffer_t *fb) {
    if (!fb) {
        return READER_ERROR_NULL_POINTER;
    }

    /* Render "Book is empty" message in center of screen */
    const char *message = "Book is empty.";
    int x = MARGIN_LEFT + ((CHARS_PER_LINE - strlen(message)) / 2) * FONT_WIDTH;
    int y = MARGIN_TOP + (LINES_PER_PAGE / 2) * LINE_HEIGHT;

    text_render_string(fb, x, y, message, COLOR_BLACK);

    return READER_SUCCESS;
}

/*
 * Reader Navigation and Input Handling
 */

reader_action_t reader_handle_event(reader_state_t *reader, const button_event_t *event) {
    if (!reader || !event) {
        return READER_ACTION_NONE;
    }

    /* Only respond to button press events (ignore release and repeat) */
    if (event->event_type != BUTTON_EVENT_PRESS) {
        return READER_ACTION_NONE;
    }

    switch (event->button) {
        case BUTTON_UP:
            /* Previous page */
            if (reader_prev_page(reader)) {
                return READER_ACTION_PAGE_CHANGED;
            }
            return READER_ACTION_NONE;

        case BUTTON_DOWN:
            /* Next page */
            if (reader_next_page(reader)) {
                return READER_ACTION_PAGE_CHANGED;
            }
            return READER_ACTION_NONE;

        case BUTTON_BACK:
            /* Exit reader (save bookmark first) */
            return READER_ACTION_EXIT;

        case BUTTON_SELECT:
            /* Save bookmark explicitly */
            return READER_ACTION_SAVE_BOOKMARK;

        case BUTTON_MENU:
            /* Future: Open reading menu */
            return READER_ACTION_NONE;

        default:
            return READER_ACTION_NONE;
    }
}

bool reader_next_page(reader_state_t *reader) {
    if (!reader) {
        return false;
    }

    /* Check if already at last page */
    if (reader->current_page >= reader->total_pages - 1) {
        return false;
    }

    reader->current_page++;
    reader->needs_redraw = true;
    return true;
}

bool reader_prev_page(reader_state_t *reader) {
    if (!reader) {
        return false;
    }

    /* Check if already at first page */
    if (reader->current_page <= 0) {
        return false;
    }

    reader->current_page--;
    reader->needs_redraw = true;
    return true;
}

bool reader_goto_page(reader_state_t *reader, int page) {
    if (!reader) {
        return false;
    }

    /* Validate page number */
    if (page < 0 || page >= reader->total_pages) {
        return false;
    }

    /* Check if page actually changed */
    if (page == reader->current_page) {
        return false;
    }

    reader->current_page = page;
    reader->needs_redraw = true;
    return true;
}

int reader_get_current_page(reader_state_t *reader) {
    if (!reader) {
        return -1;
    }
    return reader->current_page;
}

int reader_get_total_pages(reader_state_t *reader) {
    if (!reader) {
        return 0;
    }
    return reader->total_pages;
}

int reader_save_bookmark(reader_state_t *reader) {
    if (!reader || !reader->book || !reader->bookmarks) {
        return READER_ERROR_NULL_POINTER;
    }

    /* Save current page as bookmark */
    int result = bookmark_update(reader->bookmarks, reader->book->filename, reader->current_page);
    if (result != BOOK_SUCCESS) {
        return READER_ERROR_INVALID_STATE;
    }

    /* Save bookmarks to file */
    result = bookmark_list_save(reader->bookmarks, BOOKMARKS_FILE);
    if (result != BOOK_SUCCESS) {
        return READER_ERROR_INVALID_STATE;
    }

    return READER_SUCCESS;
}

/*
 * Reader State Queries
 */

bool reader_needs_redraw(reader_state_t *reader) {
    if (!reader) {
        return false;
    }
    return reader->needs_redraw;
}

void reader_clear_redraw_flag(reader_state_t *reader) {
    if (reader) {
        reader->needs_redraw = false;
    }
}

bool reader_is_first_page(reader_state_t *reader) {
    if (!reader) {
        return false;
    }
    return reader->current_page == 0;
}

bool reader_is_last_page(reader_state_t *reader) {
    if (!reader) {
        return false;
    }
    return reader->current_page >= reader->total_pages - 1;
}

bool reader_is_empty(reader_state_t *reader) {
    if (!reader) {
        return true;
    }
    return reader->total_pages == 0;
}

/*
 * Utility Functions
 */

void reader_truncate_title(const char *title, int max_length, char *buffer, size_t buffer_size) {
    if (!title || !buffer || buffer_size == 0) {
        return;
    }

    int title_len = strlen(title);

    if (title_len <= max_length) {
        /* No truncation needed */
        strncpy(buffer, title, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        /* Truncate with ellipsis */
        int copy_len = max_length - 3;  /* Reserve 3 chars for "..." */
        if (copy_len < 0) copy_len = 0;

        strncpy(buffer, title, copy_len);
        buffer[copy_len] = '\0';
        strncat(buffer, "...", buffer_size - copy_len - 1);
    }
}

void reader_format_page_indicator(int current_page, int total_pages, char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return;
    }

    /* Format as [current/total] */
    snprintf(buffer, buffer_size, "[%d/%d]", current_page, total_pages);
}

const char* reader_error_string(reader_error_t error) {
    switch (error) {
        case READER_SUCCESS:
            return "Success";
        case READER_ERROR_NULL_POINTER:
            return "Null pointer error";
        case READER_ERROR_INVALID_STATE:
            return "Invalid state";
        case READER_ERROR_RENDER_FAILED:
            return "Rendering failed";
        case READER_ERROR_NO_PAGES:
            return "No pages available";
        case READER_ERROR_PAGINATION_FAILED:
            return "Pagination failed";
        default:
            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

static void reader_draw_separator_line(framebuffer_t *fb, int line_number) {
    if (!fb) {
        return;
    }

    /* Draw a horizontal line across the screen */
    int y = MARGIN_TOP + (line_number * LINE_HEIGHT) + (LINE_HEIGHT / 2);
    fb_draw_hline(fb, MARGIN_LEFT, y, FB_WIDTH - MARGIN_LEFT - MARGIN_RIGHT, COLOR_BLACK);
}
