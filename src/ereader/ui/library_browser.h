/*
 * library_browser.h - Online Library Browser UI
 *
 * Provides a browsable catalog of popular public domain books from Project Gutenberg.
 * Allows users to browse, search (future), and download books directly to the device.
 *
 * Phase 06: WiFi and Network Features
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

#ifndef LIBRARY_BROWSER_H
#define LIBRARY_BROWSER_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../network/download_manager.h"
#include "../../button-test/button_input.h"

/*
 * Library Browser Configuration Constants
 */
#define LIBRARY_BROWSER_TITLE        "Online Library"
#define LIBRARY_BROWSER_HINTS_MAIN   "UP/DOWN:Navigate  SELECT:Details  BACK:Exit"
#define LIBRARY_BROWSER_HINTS_DETAIL "UP/DOWN:Scroll  SELECT:Download  BACK:Return"
#define LIBRARY_BROWSER_HINTS_DOWNLOAD "BACK:Cancel"

/* Layout Constants (based on 400x300 display, 8x16 font) */
#define LIBRARY_STATUS_BAR_LINE     0       /* Line 0: Status bar with title */
#define LIBRARY_SEPARATOR_1_LINE    1       /* Line 1: Separator */
#define LIBRARY_CONTENT_START_LINE  2       /* Line 2: Content start */
#define LIBRARY_CONTENT_END_LINE    15      /* Line 15: Content end */
#define LIBRARY_SEPARATOR_2_LINE    16      /* Line 16: Separator */
#define LIBRARY_HINTS_LINE          17      /* Line 17: Control hints */

#define LIBRARY_VISIBLE_ITEMS       14      /* Number of items visible in list */

/* Selection marker */
#define LIBRARY_SELECTION_MARKER    "> "
#define LIBRARY_SELECTION_MARKER_LEN 2

/* Catalog constants */
#define CATALOG_SIZE                100     /* Number of books in catalog */
#define MAX_BOOK_TITLE_LENGTH       64      /* Maximum title length */
#define MAX_BOOK_AUTHOR_LENGTH      48      /* Maximum author name length */
#define MAX_BOOK_GENRE_LENGTH       24      /* Maximum genre name length */

/* Library browser states */
typedef enum {
    LIBRARY_STATE_BROWSING = 0,     /* Browsing catalog list */
    LIBRARY_STATE_DETAILS,          /* Viewing book details */
    LIBRARY_STATE_DOWNLOADING,      /* Download in progress */
    LIBRARY_STATE_RESULT,           /* Download result (success/error) */
    LIBRARY_STATE_COUNT             /* Total number of library states */
} library_browser_state_t;

/* Book catalog entry structure */
typedef struct {
    int gutenberg_id;                           /* Project Gutenberg book ID */
    char title[MAX_BOOK_TITLE_LENGTH];          /* Display title */
    char author[MAX_BOOK_AUTHOR_LENGTH];        /* Author name */
    int size_kb;                                /* Approximate file size (KB) */
    char genre[MAX_BOOK_GENRE_LENGTH];          /* Genre for filtering (future) */
} catalog_entry_t;

/* Library browser state structure */
typedef struct {
    library_browser_state_t state;      /* Current state */

    /* Browsing state */
    int selected_index;                 /* Currently highlighted book (0-based) */
    int scroll_offset;                  /* First visible item index (0-based) */
    int visible_items;                  /* Number of items that fit on screen */

    /* Details state */
    int details_scroll;                 /* Scroll offset for details view */

    /* Download state */
    bool download_active;               /* Download in progress */
    download_progress_t download_progress; /* Current download progress */
    char download_url[MAX_URL_LENGTH];  /* URL being downloaded */
    char download_filename[MAX_FILENAME_LENGTH]; /* Filename being downloaded */

    /* Result state */
    bool download_success;              /* Last download result */
    char error_message[MAX_ERROR_MESSAGE_LENGTH]; /* Error message if failed */

    bool needs_redraw;                  /* Flag indicating full redraw needed */
} library_browser_menu_t;

/* Error codes */
typedef enum {
    LIBRARY_BROWSER_SUCCESS = 0,
    LIBRARY_BROWSER_ERROR_NULL_POINTER = -1,
    LIBRARY_BROWSER_ERROR_INVALID_STATE = -2,
    LIBRARY_BROWSER_ERROR_RENDER_FAILED = -3,
    LIBRARY_BROWSER_ERROR_DOWNLOAD_FAILED = -4,
    LIBRARY_BROWSER_ERROR_NO_WIFI = -5
} library_browser_error_t;

/* Library browser action result */
typedef enum {
    LIBRARY_BROWSER_ACTION_NONE,        /* No action taken */
    LIBRARY_BROWSER_ACTION_REDRAW,      /* Menu needs to be redrawn */
    LIBRARY_BROWSER_ACTION_DOWNLOAD_COMPLETE, /* Download completed successfully */
    LIBRARY_BROWSER_ACTION_EXIT         /* User wants to exit library browser */
} library_browser_action_t;

/*
 * Function Prototypes
 */

/**
 * Create and initialize library browser menu
 *
 * @return: Pointer to library browser menu state, or NULL on error
 */
library_browser_menu_t* library_browser_create(void);

/**
 * Free library browser menu state
 *
 * @param menu: Library browser menu state to free (may be NULL)
 */
void library_browser_free(library_browser_menu_t *menu);

/**
 * Reset library browser to initial state
 *
 * @param menu: Library browser menu state to reset
 */
void library_browser_reset(library_browser_menu_t *menu);

/**
 * Render the library browser to framebuffer
 *
 * Renders the appropriate view based on current state.
 *
 * @param menu: Library browser menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int library_browser_render(library_browser_menu_t *menu, framebuffer_t *fb);

/**
 * Handle button event and update library browser state
 *
 * @param menu: Library browser menu state
 * @param event: Button event to handle
 * @return: Library browser action indicating what happened
 */
library_browser_action_t library_browser_handle_event(library_browser_menu_t *menu, const button_event_t *event);

/**
 * Get catalog entry by index
 *
 * @param index: Catalog index (0-based)
 * @return: Pointer to catalog entry, or NULL if index out of range
 */
const catalog_entry_t* library_browser_get_catalog_entry(int index);

/**
 * Get total catalog size
 *
 * @return: Number of books in catalog
 */
int library_browser_get_catalog_size(void);

/**
 * Build Project Gutenberg download URL for a book
 *
 * @param gutenberg_id: Project Gutenberg book ID
 * @param url_buffer: Output buffer for URL
 * @param buffer_size: Size of URL buffer
 * @return: 0 on success, -1 on error
 */
int library_browser_build_download_url(int gutenberg_id, char *url_buffer, int buffer_size);

/**
 * Start downloading a book from the catalog
 *
 * @param menu: Library browser menu state
 * @param catalog_index: Index of book in catalog
 * @return: 0 on success, negative error code on failure
 */
int library_browser_start_download(library_browser_menu_t *menu, int catalog_index);

/**
 * Update download progress
 *
 * Should be called periodically during download to update progress display.
 *
 * @param menu: Library browser menu state
 * @return: 0 on success, negative error code on failure
 */
int library_browser_update_download_progress(library_browser_menu_t *menu);

/**
 * Cancel active download
 *
 * @param menu: Library browser menu state
 * @return: 0 on success, negative error code on failure
 */
int library_browser_cancel_download(library_browser_menu_t *menu);

/**
 * Check if menu needs to be redrawn
 *
 * @param menu: Library browser menu state
 * @return: true if redraw needed, false otherwise
 */
bool library_browser_needs_redraw(library_browser_menu_t *menu);

/**
 * Clear the needs_redraw flag
 *
 * @param menu: Library browser menu state
 */
void library_browser_clear_redraw_flag(library_browser_menu_t *menu);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* library_browser_error_string(library_browser_error_t error);

#endif /* LIBRARY_BROWSER_H */
