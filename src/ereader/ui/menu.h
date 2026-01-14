/*
 * menu.h - E-Reader Library Menu System
 *
 * Provides menu UI for browsing and selecting books from the library.
 * Handles scrolling, selection highlighting, and rendering to framebuffer.
 *
 * Phase 03: Basic E-Reader Application
 */

#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../books/book_manager.h"
#include "../../button-test/button_input.h"

/*
 * Menu Configuration Constants
 */
#define MENU_TITLE              "E-Reader Library"
#define MENU_CONTROL_HINTS      "UP/DOWN:Select  ENTER:Open"
#define MENU_EMPTY_MESSAGE      "No books found."
#define MENU_EMPTY_HINT         "Copy .txt files to /books/"

/* Layout Constants (based on 400x300 display, 8x16 font) */
#define MENU_STATUS_BAR_LINE    0       /* Line 0: Status bar with title and page indicator */
#define MENU_SEPARATOR_1_LINE   1       /* Line 1: Separator */
#define MENU_FIRST_ITEM_LINE    2       /* Line 2: First menu item */
#define MENU_LAST_ITEM_LINE     15      /* Line 15: Last menu item */
#define MENU_SEPARATOR_2_LINE   16      /* Line 16: Separator */
#define MENU_HINTS_LINE         17      /* Line 17: Control hints */

#define MENU_VISIBLE_ITEMS      14      /* Number of items visible at once (lines 2-15) */
#define MENU_MAX_TITLE_LENGTH   50      /* Maximum characters for book title display */

/* Selection marker */
#define MENU_SELECTION_MARKER   "> "
#define MENU_SELECTION_MARKER_LEN 2

/* Error codes */
typedef enum {
    MENU_SUCCESS = 0,
    MENU_ERROR_NULL_POINTER = -1,
    MENU_ERROR_INVALID_STATE = -2,
    MENU_ERROR_RENDER_FAILED = -3,
    MENU_ERROR_NO_BOOKS = -4
} menu_error_t;

/* Menu action result - what happened after handling an event */
typedef enum {
    MENU_ACTION_NONE,           /* No action taken */
    MENU_ACTION_REDRAW,         /* Menu needs to be redrawn */
    MENU_ACTION_SELECT_BOOK,    /* User selected a book to open */
    MENU_ACTION_EXIT            /* User wants to exit menu */
} menu_action_t;

/*
 * Menu State Structure
 *
 * Maintains current menu state including book list, selection, and scroll position
 */
typedef struct {
    book_list_t *book_list;         /* Pointer to book list (not owned by menu) */
    bookmark_list_t *bookmarks;     /* Pointer to bookmarks (not owned by menu) */

    int selected_index;             /* Currently highlighted book (0-based, global index) */
    int scroll_offset;              /* First visible item index (0-based) */
    int visible_items;              /* Number of items that fit on screen (14) */

    bool needs_redraw;              /* Flag indicating full redraw is needed */
    int refresh_counter;            /* Counter for forcing full refresh (prevent ghosting) */
} menu_state_t;

/*
 * Menu Initialization and Cleanup
 */

/**
 * Create and initialize a new menu state
 *
 * @param book_list: Pointer to book list (must remain valid during menu lifetime)
 * @param bookmarks: Pointer to bookmarks (must remain valid during menu lifetime)
 * @return: Pointer to menu state, or NULL on error
 */
menu_state_t* menu_create(book_list_t *book_list, bookmark_list_t *bookmarks);

/**
 * Free menu state and associated resources
 *
 * @param menu: Menu state to free (may be NULL)
 */
void menu_free(menu_state_t *menu);

/**
 * Reset menu state to initial position
 *
 * @param menu: Menu state to reset
 */
void menu_reset(menu_state_t *menu);

/*
 * Menu Rendering
 */

/**
 * Render the complete menu to framebuffer
 *
 * Draws status bar, book list, selection highlight, and control hints.
 *
 * @param menu: Menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int menu_render(menu_state_t *menu, framebuffer_t *fb);

/**
 * Render just the status bar (title and page indicator)
 *
 * @param menu: Menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int menu_render_status_bar(menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the book list items (with selection highlight)
 *
 * @param menu: Menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int menu_render_items(menu_state_t *menu, framebuffer_t *fb);

/**
 * Render control hints at bottom of screen
 *
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int menu_render_hints(framebuffer_t *fb);

/**
 * Render empty state message (when no books found)
 *
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int menu_render_empty(framebuffer_t *fb);

/*
 * Menu Navigation and Input Handling
 */

/**
 * Handle button event and update menu state
 *
 * @param menu: Menu state
 * @param event: Button event to handle
 * @return: Menu action indicating what happened
 */
menu_action_t menu_handle_event(menu_state_t *menu, const button_event_t *event);

/**
 * Move selection up one item
 *
 * @param menu: Menu state
 * @return: true if selection changed, false otherwise
 */
bool menu_move_up(menu_state_t *menu);

/**
 * Move selection down one item
 *
 * @param menu: Menu state
 * @return: true if selection changed, false otherwise
 */
bool menu_move_down(menu_state_t *menu);

/**
 * Move selection up one page (visible_items)
 *
 * @param menu: Menu state
 * @return: true if selection changed, false otherwise
 */
bool menu_page_up(menu_state_t *menu);

/**
 * Move selection down one page (visible_items)
 *
 * @param menu: Menu state
 * @return: true if selection changed, false otherwise
 */
bool menu_page_down(menu_state_t *menu);

/**
 * Get currently selected book
 *
 * @param menu: Menu state
 * @return: Pointer to selected book metadata, or NULL if none selected
 */
book_metadata_t* menu_get_selected_book(menu_state_t *menu);

/**
 * Get index of currently selected book
 *
 * @param menu: Menu state
 * @return: Selected index (0-based), or -1 if no selection
 */
int menu_get_selected_index(menu_state_t *menu);

/*
 * Menu State Queries
 */

/**
 * Check if menu needs to be redrawn
 *
 * @param menu: Menu state
 * @return: true if redraw needed, false otherwise
 */
bool menu_needs_redraw(menu_state_t *menu);

/**
 * Clear the needs_redraw flag
 *
 * @param menu: Menu state
 */
void menu_clear_redraw_flag(menu_state_t *menu);

/**
 * Get the current page number (for status bar display)
 *
 * @param menu: Menu state
 * @return: Current page number (1-based)
 */
int menu_get_current_page(menu_state_t *menu);

/**
 * Get the total number of pages
 *
 * @param menu: Menu state
 * @return: Total page count
 */
int menu_get_total_pages(menu_state_t *menu);

/*
 * Utility Functions
 */

/**
 * Truncate a book title to fit in display width
 *
 * @param title: Original title
 * @param max_length: Maximum characters
 * @param buffer: Output buffer
 * @param buffer_size: Size of output buffer
 */
void menu_truncate_title(const char *title, int max_length, char *buffer, size_t buffer_size);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* menu_error_string(menu_error_t error);

#endif /* MENU_H */
