/*
 * menu.c - E-Reader Library Menu System Implementation
 *
 * Implements the menu UI for browsing and selecting books.
 *
 * Phase 03: Basic E-Reader Application
 */

#include "menu.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal helper function prototypes */
static void menu_draw_separator_line(framebuffer_t *fb, int line_number);
static void menu_format_page_indicator(menu_state_t *menu, char *buffer, size_t buffer_size);
static int menu_calculate_total_pages(int total_items, int items_per_page);

/*
 * Menu Initialization and Cleanup
 */

menu_state_t* menu_create(book_list_t *book_list, bookmark_list_t *bookmarks) {
    if (!book_list) {
        return NULL;
    }

    menu_state_t *menu = (menu_state_t*)calloc(1, sizeof(menu_state_t));
    if (!menu) {
        return NULL;
    }

    menu->book_list = book_list;
    menu->bookmarks = bookmarks;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->visible_items = MENU_VISIBLE_ITEMS;
    menu->needs_redraw = true;
    menu->refresh_counter = 0;

    return menu;
}

void menu_free(menu_state_t *menu) {
    if (menu) {
        /* Note: We don't free book_list or bookmarks as they're not owned by menu */
        free(menu);
    }
}

void menu_reset(menu_state_t *menu) {
    if (!menu) {
        return;
    }

    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->needs_redraw = true;
    menu->refresh_counter = 0;
}

/*
 * Menu Rendering
 */

int menu_render(menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return MENU_ERROR_NULL_POINTER;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Check if book list is empty */
    if (!menu->book_list || menu->book_list->count == 0) {
        return menu_render_empty(fb);
    }

    /* Render status bar */
    if (menu_render_status_bar(menu, fb) != MENU_SUCCESS) {
        return MENU_ERROR_RENDER_FAILED;
    }

    /* Render separator after status bar */
    menu_draw_separator_line(fb, MENU_SEPARATOR_1_LINE);

    /* Render menu items */
    if (menu_render_items(menu, fb) != MENU_SUCCESS) {
        return MENU_ERROR_RENDER_FAILED;
    }

    /* Render separator before hints */
    menu_draw_separator_line(fb, MENU_SEPARATOR_2_LINE);

    /* Render control hints */
    if (menu_render_hints(fb) != MENU_SUCCESS) {
        return MENU_ERROR_RENDER_FAILED;
    }

    /* Increment refresh counter (for forcing full refresh every N selections) */
    menu->refresh_counter++;

    return MENU_SUCCESS;
}

int menu_render_status_bar(menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return MENU_ERROR_NULL_POINTER;
    }

    char status_bar[MAX_LINE_LENGTH];
    char page_indicator[32];

    /* Format page indicator [current/total] */
    menu_format_page_indicator(menu, page_indicator, sizeof(page_indicator));

    /* Create status bar: "E-Reader Library         [1/3]" */
    int title_len = strlen(MENU_TITLE);
    int indicator_len = strlen(page_indicator);
    int padding_spaces = CHARS_PER_LINE - title_len - indicator_len;

    if (padding_spaces < 1) {
        padding_spaces = 1;
    }

    snprintf(status_bar, sizeof(status_bar), "%s%*s%s",
             MENU_TITLE, padding_spaces, "", page_indicator);

    /* Render to framebuffer at line 0 */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (MENU_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, status_bar, COLOR_BLACK);

    return MENU_SUCCESS;
}

int menu_render_items(menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb || !menu->book_list) {
        return MENU_ERROR_NULL_POINTER;
    }

    int total_books = menu->book_list->count;
    if (total_books == 0) {
        return MENU_ERROR_NO_BOOKS;
    }

    /* Ensure scroll offset is valid */
    if (menu->scroll_offset < 0) {
        menu->scroll_offset = 0;
    }
    if (menu->scroll_offset >= total_books) {
        menu->scroll_offset = (total_books > menu->visible_items) ?
                             (total_books - menu->visible_items) : 0;
    }

    /* Render visible items */
    for (int i = 0; i < menu->visible_items && (menu->scroll_offset + i) < total_books; i++) {
        int book_index = menu->scroll_offset + i;
        book_metadata_t *book = book_list_get(menu->book_list, book_index);

        if (!book) {
            continue;
        }

        /* Determine if this item is selected */
        bool is_selected = (book_index == menu->selected_index);

        /* Calculate line position */
        int line_number = MENU_FIRST_ITEM_LINE + i;
        int x = MARGIN_LEFT;
        int y = MARGIN_TOP + (line_number * LINE_HEIGHT);

        /* Build display string */
        char display_text[MAX_LINE_LENGTH];
        char truncated_title[MENU_MAX_TITLE_LENGTH + 1];

        /* Truncate title to fit (accounting for selection marker) */
        int max_title_chars = CHARS_PER_LINE - MENU_SELECTION_MARKER_LEN - 2;
        menu_truncate_title(book->filename, max_title_chars,
                          truncated_title, sizeof(truncated_title));

        /* Format with or without selection marker */
        if (is_selected) {
            snprintf(display_text, sizeof(display_text), "%s%s",
                    MENU_SELECTION_MARKER, truncated_title);

            /* Optional: Invert the region for highlighting */
            /* Calculate region to invert */
            int highlight_x = x;
            int highlight_y = y;
            int highlight_width = TEXT_AREA_WIDTH;
            int highlight_height = FONT_HEIGHT;

            /* Draw inverted rectangle for selection highlight */
            fb_invert_region(fb, highlight_x, highlight_y,
                           highlight_width, highlight_height);
        } else {
            snprintf(display_text, sizeof(display_text), "  %s", truncated_title);
        }

        /* Render the text */
        text_render_string(fb, x, y, display_text, COLOR_BLACK);
    }

    return MENU_SUCCESS;
}

int menu_render_hints(framebuffer_t *fb) {
    if (!fb) {
        return MENU_ERROR_NULL_POINTER;
    }

    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (MENU_HINTS_LINE * LINE_HEIGHT);

    text_render_string(fb, x, y, MENU_CONTROL_HINTS, COLOR_BLACK);

    return MENU_SUCCESS;
}

int menu_render_empty(framebuffer_t *fb) {
    if (!fb) {
        return MENU_ERROR_NULL_POINTER;
    }

    /* Render title */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP;
    text_render_string(fb, x, y, MENU_TITLE, COLOR_BLACK);

    /* Render separator */
    menu_draw_separator_line(fb, MENU_SEPARATOR_1_LINE);

    /* Render centered empty message */
    int empty_y = MARGIN_TOP + (8 * LINE_HEIGHT);  /* Middle of screen */

    /* Center "No books found." */
    int msg_len = strlen(MENU_EMPTY_MESSAGE);
    int msg_x = MARGIN_LEFT + ((CHARS_PER_LINE - msg_len) / 2) * FONT_WIDTH;
    text_render_string(fb, msg_x, empty_y, MENU_EMPTY_MESSAGE, COLOR_BLACK);

    /* Center hint message */
    int hint_y = empty_y + (2 * LINE_HEIGHT);
    int hint_len = strlen(MENU_EMPTY_HINT);
    int hint_x = MARGIN_LEFT + ((CHARS_PER_LINE - hint_len) / 2) * FONT_WIDTH;
    text_render_string(fb, hint_x, hint_y, MENU_EMPTY_HINT, COLOR_BLACK);

    return MENU_SUCCESS;
}

/*
 * Menu Navigation and Input Handling
 */

menu_action_t menu_handle_event(menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return MENU_ACTION_NONE;
    }

    /* Only handle button press events (not release or repeat) */
    if (event->event_type != BUTTON_EVENT_PRESS) {
        return MENU_ACTION_NONE;
    }

    /* Check if book list is empty */
    if (!menu->book_list || menu->book_list->count == 0) {
        /* Only allow exit on empty list */
        if (event->button == BUTTON_BACK) {
            return MENU_ACTION_EXIT;
        }
        return MENU_ACTION_NONE;
    }

    menu_action_t action = MENU_ACTION_NONE;

    switch (event->button) {
        case BUTTON_UP:
            if (menu_move_up(menu)) {
                menu->needs_redraw = true;
                action = MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_DOWN:
            if (menu_move_down(menu)) {
                menu->needs_redraw = true;
                action = MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_SELECT:
            /* User wants to open the selected book */
            action = MENU_ACTION_SELECT_BOOK;
            break;

        case BUTTON_BACK:
            /* Return to previous menu or exit */
            action = MENU_ACTION_EXIT;
            break;

        case BUTTON_MENU:
            /* Reserved for future use (settings menu) */
            break;

        default:
            break;
    }

    return action;
}

bool menu_move_up(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return false;
    }

    int total_books = menu->book_list->count;
    int old_index = menu->selected_index;

    /* Move selection up */
    menu->selected_index--;

    /* Handle wrapping to bottom */
    if (menu->selected_index < 0) {
        menu->selected_index = total_books - 1;
        /* Scroll to show last page */
        menu->scroll_offset = (total_books > menu->visible_items) ?
                             (total_books - menu->visible_items) : 0;
        return true;
    }

    /* Adjust scroll offset if selection moved above visible area */
    if (menu->selected_index < menu->scroll_offset) {
        menu->scroll_offset = menu->selected_index;
    }

    return (menu->selected_index != old_index);
}

bool menu_move_down(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return false;
    }

    int total_books = menu->book_list->count;
    int old_index = menu->selected_index;

    /* Move selection down */
    menu->selected_index++;

    /* Handle wrapping to top */
    if (menu->selected_index >= total_books) {
        menu->selected_index = 0;
        menu->scroll_offset = 0;
        return true;
    }

    /* Adjust scroll offset if selection moved below visible area */
    int last_visible_index = menu->scroll_offset + menu->visible_items - 1;
    if (menu->selected_index > last_visible_index) {
        menu->scroll_offset++;
    }

    return (menu->selected_index != old_index);
}

bool menu_page_up(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return false;
    }

    int old_index = menu->selected_index;

    /* Move selection up by one page */
    menu->selected_index -= menu->visible_items;
    if (menu->selected_index < 0) {
        menu->selected_index = 0;
    }

    /* Adjust scroll offset */
    menu->scroll_offset = menu->selected_index;
    if (menu->scroll_offset < 0) {
        menu->scroll_offset = 0;
    }

    return (menu->selected_index != old_index);
}

bool menu_page_down(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return false;
    }

    int total_books = menu->book_list->count;
    int old_index = menu->selected_index;

    /* Move selection down by one page */
    menu->selected_index += menu->visible_items;
    if (menu->selected_index >= total_books) {
        menu->selected_index = total_books - 1;
    }

    /* Adjust scroll offset */
    menu->scroll_offset = menu->selected_index - menu->visible_items + 1;
    if (menu->scroll_offset < 0) {
        menu->scroll_offset = 0;
    }
    if (menu->scroll_offset > (total_books - menu->visible_items)) {
        menu->scroll_offset = (total_books > menu->visible_items) ?
                             (total_books - menu->visible_items) : 0;
    }

    return (menu->selected_index != old_index);
}

book_metadata_t* menu_get_selected_book(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return NULL;
    }

    if (menu->selected_index < 0 || menu->selected_index >= menu->book_list->count) {
        return NULL;
    }

    return book_list_get(menu->book_list, menu->selected_index);
}

int menu_get_selected_index(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return -1;
    }

    return menu->selected_index;
}

/*
 * Menu State Queries
 */

bool menu_needs_redraw(menu_state_t *menu) {
    if (!menu) {
        return false;
    }
    return menu->needs_redraw;
}

void menu_clear_redraw_flag(menu_state_t *menu) {
    if (menu) {
        menu->needs_redraw = false;
    }
}

int menu_get_current_page(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return 1;
    }

    /* Calculate which page the selected item is on */
    return (menu->selected_index / menu->visible_items) + 1;
}

int menu_get_total_pages(menu_state_t *menu) {
    if (!menu || !menu->book_list || menu->book_list->count == 0) {
        return 1;
    }

    return menu_calculate_total_pages(menu->book_list->count, menu->visible_items);
}

/*
 * Utility Functions
 */

void menu_truncate_title(const char *title, int max_length, char *buffer, size_t buffer_size) {
    if (!title || !buffer || buffer_size == 0) {
        return;
    }

    int title_len = strlen(title);

    if (title_len <= max_length) {
        /* Title fits, copy as-is */
        strncpy(buffer, title, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        /* Truncate and add ellipsis */
        int copy_len = max_length - 3;  /* Reserve space for "..." */
        if (copy_len < 0) {
            copy_len = 0;
        }

        strncpy(buffer, title, copy_len);
        buffer[copy_len] = '\0';
        strncat(buffer, "...", buffer_size - strlen(buffer) - 1);
    }
}

const char* menu_error_string(menu_error_t error) {
    switch (error) {
        case MENU_SUCCESS:
            return "Success";
        case MENU_ERROR_NULL_POINTER:
            return "Null pointer error";
        case MENU_ERROR_INVALID_STATE:
            return "Invalid menu state";
        case MENU_ERROR_RENDER_FAILED:
            return "Rendering failed";
        case MENU_ERROR_NO_BOOKS:
            return "No books available";
        default:
            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

static void menu_draw_separator_line(framebuffer_t *fb, int line_number) {
    if (!fb) {
        return;
    }

    int y = MARGIN_TOP + (line_number * LINE_HEIGHT) + (FONT_HEIGHT / 2);
    int x1 = MARGIN_LEFT;
    int x2 = FB_WIDTH - MARGIN_RIGHT - 1;

    fb_draw_hline(fb, x1, x2, y, COLOR_BLACK);
}

static void menu_format_page_indicator(menu_state_t *menu, char *buffer, size_t buffer_size) {
    if (!menu || !buffer || buffer_size == 0) {
        return;
    }

    int current_page = menu_get_current_page(menu);
    int total_pages = menu_get_total_pages(menu);

    snprintf(buffer, buffer_size, "[%d/%d]", current_page, total_pages);
}

static int menu_calculate_total_pages(int total_items, int items_per_page) {
    if (total_items <= 0 || items_per_page <= 0) {
        return 1;
    }

    /* Calculate pages: ceiling division */
    return (total_items + items_per_page - 1) / items_per_page;
}
