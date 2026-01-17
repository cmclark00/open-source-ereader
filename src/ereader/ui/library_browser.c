/*
 * library_browser.c - Online Library Browser UI Implementation
 *
 * Implements browsing and downloading of popular public domain books.
 *
 * Phase 06: WiFi and Network Features
 */

#include "library_browser.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
<parameter name="stdlib.h">
#include <string.h>

/*
 * Hardcoded Catalog of 100 Popular Project Gutenberg Books
 *
 * Curated selection across 8 genres:
 * - Classics (30 books)
 * - Science Fiction (10 books)
 * - Mystery (10 books)
 * - Adventure (10 books)
 * - Philosophy (10 books)
 * - Poetry (10 books)
 * - Children's (10 books)
 * - Horror (10 books)
 *
 * Books selected for:
 * - Public domain worldwide (pre-1928 works preferred)
 * - English language
 * - Cultural significance and popularity
 * - Reasonable file size (< 1 MB preferred)
 * - Text format availability
 */
static const catalog_entry_t book_catalog[CATALOG_SIZE] = {
    /* Classics (30 books) */
    {1342, "Pride and Prejudice", "Jane Austen", 700, "Classics"},
    {84, "Frankenstein", "Mary Shelley", 440, "Classics"},
    {1661, "The Adventures of Sherlock Holmes", "Arthur Conan Doyle", 580, "Classics"},
    {2701, "Moby Dick", "Herman Melville", 1200, "Classics"},
    {98, "A Tale of Two Cities", "Charles Dickens", 780, "Classics"},
    {1260, "Jane Eyre", "Charlotte Bronte", 990, "Classics"},
    {174, "The Picture of Dorian Gray", "Oscar Wilde", 420, "Classics"},
    {1080, "A Modest Proposal", "Jonathan Swift", 25, "Classics"},
    {16328, "Beowulf", "Unknown", 130, "Classics"},
    {5200, "Metamorphosis", "Franz Kafka", 90, "Classics"},
    {844, "The Importance of Being Earnest", "Oscar Wilde", 120, "Classics"},
    {4300, "Ulysses", "James Joyce", 1520, "Classics"},
    {1400, "Great Expectations", "Charles Dickens", 970, "Classics"},
    {345, "Dracula", "Bram Stoker", 840, "Classics"},
    {11, "Alice's Adventures in Wonderland", "Lewis Carroll", 150, "Classics"},
    {46, "A Christmas Carol", "Charles Dickens", 100, "Classics"},
    {2554, "Crime and Punishment", "Fyodor Dostoevsky", 1180, "Classics"},
    {1232, "The Prince", "Niccolo Machiavelli", 230, "Classics"},
    {2000, "Don Quixote", "Miguel de Cervantes", 2100, "Classics"},
    {74, "The Adventures of Tom Sawyer", "Mark Twain", 420, "Classics"},
    {76, "Adventures of Huckleberry Finn", "Mark Twain", 600, "Classics"},
    {244, "A Study in Scarlet", "Arthur Conan Doyle", 210, "Classics"},
    {161, "Sense and Sensibility", "Jane Austen", 680, "Classics"},
    {205, "Walden", "Henry David Thoreau", 480, "Classics"},
    {158, "Emma", "Jane Austen", 860, "Classics"},
    {730, "Oliver Twist", "Charles Dickens", 820, "Classics"},
    {2097, "The Scarlet Letter", "Nathaniel Hawthorne", 430, "Classics"},
    {219, "Heart of Darkness", "Joseph Conrad", 170, "Classics"},
    {768, "Wuthering Heights", "Emily Bronte", 710, "Classics"},
    {1322, "Leaves of Grass", "Walt Whitman", 740, "Classics"},

    /* Science Fiction (10 books) */
    {35, "The Time Machine", "H. G. Wells", 180, "Science Fiction"},
    {36, "The War of the Worlds", "H. G. Wells", 330, "Science Fiction"},
    {41, "The Invisible Man", "H. G. Wells", 300, "Science Fiction"},
    {164, "Twenty Thousand Leagues Under the Sea", "Jules Verne", 770, "Science Fiction"},
    {103, "Around the World in 80 Days", "Jules Verne", 370, "Science Fiction"},
    {2000, "Erewhon", "Samuel Butler", 430, "Science Fiction"},
    {17, "The Island of Doctor Moreau", "H. G. Wells", 260, "Science Fiction"},
    {209, "The Turn of the Screw", "Henry James", 180, "Science Fiction"},
    {1268, "The First Men in the Moon", "H. G. Wells", 360, "Science Fiction"},
    {524, "The Lost World", "Arthur Conan Doyle", 380, "Science Fiction"},

    /* Mystery (10 books) */
    {1661, "The Adventures of Sherlock Holmes", "Arthur Conan Doyle", 580, "Mystery"},
    {244, "A Study in Scarlet", "Arthur Conan Doyle", 210, "Mystery"},
    {2852, "The Hound of the Baskervilles", "Arthur Conan Doyle", 360, "Mystery"},
    {834, "The Mysterious Affair at Styles", "Agatha Christie", 370, "Mystery"},
    {863, "The Secret Adversary", "Agatha Christie", 440, "Mystery"},
    {972, "The Murders in the Rue Morque", "Edgar Allan Poe", 50, "Mystery"},
    {2148, "The Man Who Was Thursday", "G. K. Chesterton", 340, "Mystery"},
    {1155, "The Sign of the Four", "Arthur Conan Doyle", 220, "Mystery"},
    {108, "The Moonstone", "Wilkie Collins", 1020, "Mystery"},
    {209, "The Turn of the Screw", "Henry James", 180, "Mystery"},

    /* Adventure (10 books) */
    {120, "Treasure Island", "Robert Louis Stevenson", 410, "Adventure"},
    {43, "The Strange Case of Dr. Jekyll and Mr. Hyde", "Robert Louis Stevenson", 120, "Adventure"},
    {1268, "Kidnapped", "Robert Louis Stevenson", 430, "Adventure"},
    {58, "The Jungle Book", "Rudyard Kipling", 260, "Adventure"},
    {140, "The Second Jungle Book", "Rudyard Kipling", 280, "Adventure"},
    {55, "The Wonderful Wizard of Oz", "L. Frank Baum", 250, "Adventure"},
    {74, "The Adventures of Tom Sawyer", "Mark Twain", 420, "Adventure"},
    {76, "Adventures of Huckleberry Finn", "Mark Twain", 600, "Adventure"},
    {2097, "Robinson Crusoe", "Daniel Defoe", 630, "Adventure"},
    {1268, "The Call of the Wild", "Jack London", 170, "Adventure"},

    /* Philosophy (10 books) */
    {1497, "The Republic", "Plato", 710, "Philosophy"},
    {5827, "The Nicomachean Ethics", "Aristotle", 680, "Philosophy"},
    {2680, "Meditations", "Marcus Aurelius", 230, "Philosophy"},
    {5000, "The Art of War", "Sun Tzu", 80, "Philosophy"},
    {3300, "The Critique of Pure Reason", "Immanuel Kant", 1100, "Philosophy"},
    {1497, "Apology", "Plato", 60, "Philosophy"},
    {4705, "Leviathan", "Thomas Hobbes", 1120, "Philosophy"},
    {1232, "The Prince", "Niccolo Machiavelli", 230, "Philosophy"},
    {3455, "The Communist Manifesto", "Karl Marx", 90, "Philosophy"},
    {4363, "Beyond Good and Evil", "Friedrich Nietzsche", 380, "Philosophy"},

    /* Poetry (10 books) */
    {1322, "Leaves of Grass", "Walt Whitman", 740, "Poetry"},
    {2701, "Paradise Lost", "John Milton", 470, "Poetry"},
    {1065, "The Raven", "Edgar Allan Poe", 10, "Poetry"},
    {574, "The Poetical Works of Percy Bysshe Shelley", "Percy Shelley", 980, "Poetry"},
    {2488, "The Complete Poems of Emily Dickinson", "Emily Dickinson", 330, "Poetry"},
    {1250, "The Rime of the Ancient Mariner", "Samuel Taylor Coleridge", 30, "Poetry"},
    {19221, "The Waste Land", "T. S. Eliot", 25, "Poetry"},
    {4800, "The Divine Comedy", "Dante Alighieri", 810, "Poetry"},
    {3201, "The Canterbury Tales", "Geoffrey Chaucer", 970, "Poetry"},
    {236, "The Ballad of Reading Gaol", "Oscar Wilde", 30, "Poetry"},

    /* Children's (10 books) */
    {11, "Alice's Adventures in Wonderland", "Lewis Carroll", 150, "Children's"},
    {114, "Through the Looking-Glass", "Lewis Carroll", 170, "Children's"},
    {55, "The Wonderful Wizard of Oz", "L. Frank Baum", 250, "Children's"},
    {16, "Peter Pan", "J. M. Barrie", 240, "Children's"},
    {1400, "Little Women", "Louisa May Alcott", 970, "Children's"},
    {140, "The Jungle Book", "Rudyard Kipling", 260, "Children's"},
    {1260, "The Secret Garden", "Frances Hodgson Burnett", 510, "Children's"},
    {76, "Adventures of Huckleberry Finn", "Mark Twain", 600, "Children's"},
    {74, "The Adventures of Tom Sawyer", "Mark Twain", 420, "Children's"},
    {1404, "A Little Princess", "Frances Hodgson Burnett", 340, "Children's"},

    /* Horror (10 books) */
    {345, "Dracula", "Bram Stoker", 840, "Horror"},
    {84, "Frankenstein", "Mary Shelley", 440, "Horror"},
    {932, "The Legend of Sleepy Hollow", "Washington Irving", 45, "Horror"},
    {43, "The Strange Case of Dr. Jekyll and Mr. Hyde", "Robert Louis Stevenson", 120, "Horror"},
    {17, "The Island of Doctor Moreau", "H. G. Wells", 260, "Horror"},
    {1091, "Tales of Mystery and Imagination", "Edgar Allan Poe", 450, "Horror"},
    {2148, "The Turn of the Screw", "Henry James", 180, "Horror"},
    {42324, "The Call of Cthulhu", "H. P. Lovecraft", 60, "Horror"},
    {41, "The Invisible Man", "H. G. Wells", 300, "Horror"},
    {219, "Heart of Darkness", "Joseph Conrad", 170, "Horror"}
};

/* Internal helper function prototypes */
static void library_browser_draw_separator_line(framebuffer_t *fb, int line_number);
static int library_browser_render_browsing(library_browser_menu_t *menu, framebuffer_t *fb);
static int library_browser_render_details(library_browser_menu_t *menu, framebuffer_t *fb);
static int library_browser_render_downloading(library_browser_menu_t *menu, framebuffer_t *fb);
static int library_browser_render_result(library_browser_menu_t *menu, framebuffer_t *fb);
static library_browser_action_t library_browser_handle_browsing_event(library_browser_menu_t *menu, const button_event_t *event);
static library_browser_action_t library_browser_handle_details_event(library_browser_menu_t *menu, const button_event_t *event);
static library_browser_action_t library_browser_handle_downloading_event(library_browser_menu_t *menu, const button_event_t *event);
static library_browser_action_t library_browser_handle_result_event(library_browser_menu_t *menu, const button_event_t *event);

/*
 * Library Browser Initialization and Cleanup
 */

library_browser_menu_t* library_browser_create(void) {
    library_browser_menu_t *menu = (library_browser_menu_t*)calloc(1, sizeof(library_browser_menu_t));
    if (!menu) {
        return NULL;
    }

    menu->state = LIBRARY_STATE_BROWSING;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->visible_items = LIBRARY_VISIBLE_ITEMS;
    menu->details_scroll = 0;
    menu->download_active = false;
    menu->download_success = false;
    menu->needs_redraw = true;

    return menu;
}

void library_browser_free(library_browser_menu_t *menu) {
    if (menu) {
        /* Cancel any active download */
        if (menu->download_active) {
            library_browser_cancel_download(menu);
        }
        free(menu);
    }
}

void library_browser_reset(library_browser_menu_t *menu) {
    if (!menu) {
        return;
    }

    menu->state = LIBRARY_STATE_BROWSING;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->details_scroll = 0;
    menu->download_active = false;
    menu->download_success = false;
    menu->needs_redraw = true;
}

/*
 * Library Browser Rendering
 */

int library_browser_render(library_browser_menu_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return LIBRARY_BROWSER_ERROR_NULL_POINTER;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Render based on current state */
    switch (menu->state) {
        case LIBRARY_STATE_BROWSING:
            return library_browser_render_browsing(menu, fb);
        case LIBRARY_STATE_DETAILS:
            return library_browser_render_details(menu, fb);
        case LIBRARY_STATE_DOWNLOADING:
            return library_browser_render_downloading(menu, fb);
        case LIBRARY_STATE_RESULT:
            return library_browser_render_result(menu, fb);
        default:
            return LIBRARY_BROWSER_ERROR_INVALID_STATE;
    }
}

static int library_browser_render_browsing(library_browser_menu_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return LIBRARY_BROWSER_ERROR_NULL_POINTER;
    }

    /* Render status bar with title and catalog info */
    char status_bar[MAX_LINE_LENGTH];
    snprintf(status_bar, sizeof(status_bar), "%s  [%d books]",
             LIBRARY_BROWSER_TITLE, CATALOG_SIZE);

    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (LIBRARY_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, status_bar, COLOR_BLACK);

    /* Render separator */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_1_LINE);

    /* Render catalog list */
    for (int i = 0; i < menu->visible_items && (menu->scroll_offset + i) < CATALOG_SIZE; i++) {
        int catalog_index = menu->scroll_offset + i;
        const catalog_entry_t *entry = &book_catalog[catalog_index];

        /* Determine if this item is selected */
        bool is_selected = (catalog_index == menu->selected_index);

        /* Calculate line position */
        int line_number = LIBRARY_CONTENT_START_LINE + i;
        x = MARGIN_LEFT;
        y = MARGIN_TOP + (line_number * LINE_HEIGHT);

        /* Build display string */
        char display_text[MAX_LINE_LENGTH];
        char truncated_title[48];

        /* Truncate title to fit */
        int max_title_chars = 40;
        if (strlen(entry->title) <= max_title_chars) {
            strncpy(truncated_title, entry->title, sizeof(truncated_title) - 1);
            truncated_title[sizeof(truncated_title) - 1] = '\0';
        } else {
            strncpy(truncated_title, entry->title, max_title_chars - 3);
            truncated_title[max_title_chars - 3] = '\0';
            strcat(truncated_title, "...");
        }

        /* Format with selection marker */
        if (is_selected) {
            snprintf(display_text, sizeof(display_text), "%s%s",
                    LIBRARY_SELECTION_MARKER, truncated_title);

            /* Draw inverted rectangle for selection highlight */
            int highlight_x = x;
            int highlight_y = y;
            int highlight_width = TEXT_AREA_WIDTH;
            int highlight_height = FONT_HEIGHT;
            fb_invert_region(fb, highlight_x, highlight_y,
                           highlight_width, highlight_height);
        } else {
            snprintf(display_text, sizeof(display_text), "  %s", truncated_title);
        }

        /* Render the text */
        text_render_string(fb, x, y, display_text, COLOR_BLACK);
    }

    /* Render separator before hints */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_2_LINE);

    /* Render control hints */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (LIBRARY_HINTS_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, LIBRARY_BROWSER_HINTS_MAIN, COLOR_BLACK);

    return LIBRARY_BROWSER_SUCCESS;
}

static int library_browser_render_details(library_browser_menu_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return LIBRARY_BROWSER_ERROR_NULL_POINTER;
    }

    const catalog_entry_t *entry = &book_catalog[menu->selected_index];

    /* Render status bar */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (LIBRARY_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, "Book Details", COLOR_BLACK);

    /* Render separator */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_1_LINE);

    /* Render book details */
    int line = LIBRARY_CONTENT_START_LINE;

    /* Title (may wrap) */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char title_line[MAX_LINE_LENGTH];
    snprintf(title_line, sizeof(title_line), "Title: %s", entry->title);
    text_render_string(fb, x, y, title_line, COLOR_BLACK);
    line++;

    /* Author */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char author_line[MAX_LINE_LENGTH];
    snprintf(author_line, sizeof(author_line), "Author: %s", entry->author);
    text_render_string(fb, x, y, author_line, COLOR_BLACK);
    line++;

    /* Genre */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char genre_line[MAX_LINE_LENGTH];
    snprintf(genre_line, sizeof(genre_line), "Genre: %s", entry->genre);
    text_render_string(fb, x, y, genre_line, COLOR_BLACK);
    line++;

    /* Size */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char size_line[MAX_LINE_LENGTH];
    if (entry->size_kb < 1024) {
        snprintf(size_line, sizeof(size_line), "Size: ~%d KB", entry->size_kb);
    } else {
        snprintf(size_line, sizeof(size_line), "Size: ~%.1f MB", entry->size_kb / 1024.0);
    }
    text_render_string(fb, x, y, size_line, COLOR_BLACK);
    line++;

    /* Format */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    text_render_string(fb, x, y, "Format: TXT (Plain Text)", COLOR_BLACK);
    line++;

    /* Gutenberg ID (for reference) */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char id_line[MAX_LINE_LENGTH];
    snprintf(id_line, sizeof(id_line), "ID: %d", entry->gutenberg_id);
    text_render_string(fb, x, y, id_line, COLOR_BLACK);
    line += 2;

    /* Download prompt */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    text_render_string(fb, x, y, "Press SELECT to download", COLOR_BLACK);

    /* Render separator before hints */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_2_LINE);

    /* Render control hints */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (LIBRARY_HINTS_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, LIBRARY_BROWSER_HINTS_DETAIL, COLOR_BLACK);

    return LIBRARY_BROWSER_SUCCESS;
}

static int library_browser_render_downloading(library_browser_menu_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return LIBRARY_BROWSER_ERROR_NULL_POINTER;
    }

    /* Render status bar */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (LIBRARY_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, "Downloading Book", COLOR_BLACK);

    /* Render separator */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_1_LINE);

    /* Render download progress */
    int line = LIBRARY_CONTENT_START_LINE + 2;

    /* Filename */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char filename_line[MAX_LINE_LENGTH];
    snprintf(filename_line, sizeof(filename_line), "File: %s", menu->download_filename);
    text_render_string(fb, x, y, filename_line, COLOR_BLACK);
    line++;

    /* Progress percentage */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char progress_line[MAX_LINE_LENGTH];
    snprintf(progress_line, sizeof(progress_line), "Progress: %d%%",
             menu->download_progress.progress_percentage);
    text_render_string(fb, x, y, progress_line, COLOR_BLACK);
    line++;

    /* Downloaded size */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char size_line[MAX_LINE_LENGTH];
    char downloaded_str[32];
    download_manager_format_size(menu->download_progress.bytes_downloaded,
                                  downloaded_str, sizeof(downloaded_str));
    snprintf(size_line, sizeof(size_line), "Downloaded: %s", downloaded_str);
    text_render_string(fb, x, y, size_line, COLOR_BLACK);
    line++;

    /* Download speed */
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);
    char speed_line[MAX_LINE_LENGTH];
    char speed_str[32];
    download_manager_format_size(menu->download_progress.download_speed,
                                  speed_str, sizeof(speed_str));
    snprintf(speed_line, sizeof(speed_line), "Speed: %s/s", speed_str);
    text_render_string(fb, x, y, speed_line, COLOR_BLACK);
    line++;

    /* ETA */
    if (menu->download_progress.eta_seconds > 0) {
        y = MARGIN_TOP + (line++ * LINE_HEIGHT);
        char eta_line[MAX_LINE_LENGTH];
        char eta_str[32];
        download_manager_format_time(menu->download_progress.eta_seconds,
                                      eta_str, sizeof(eta_str));
        snprintf(eta_line, sizeof(eta_line), "ETA: %s", eta_str);
        text_render_string(fb, x, y, eta_line, COLOR_BLACK);
    }

    /* Render separator before hints */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_2_LINE);

    /* Render control hints */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (LIBRARY_HINTS_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, LIBRARY_BROWSER_HINTS_DOWNLOAD, COLOR_BLACK);

    return LIBRARY_BROWSER_SUCCESS;
}

static int library_browser_render_result(library_browser_menu_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return LIBRARY_BROWSER_ERROR_NULL_POINTER;
    }

    /* Render status bar */
    int x = MARGIN_LEFT;
    int y = MARGIN_TOP + (LIBRARY_STATUS_BAR_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, "Download Result", COLOR_BLACK);

    /* Render separator */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_1_LINE);

    /* Render result message */
    int line = LIBRARY_CONTENT_START_LINE + 4;
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (line++ * LINE_HEIGHT);

    if (menu->download_success) {
        text_render_string(fb, x, y, "Download Complete!", COLOR_BLACK);
        line += 2;
        y = MARGIN_TOP + (line++ * LINE_HEIGHT);
        text_render_string(fb, x, y, "Book saved to /books/", COLOR_BLACK);
        line++;
        y = MARGIN_TOP + (line++ * LINE_HEIGHT);
        text_render_string(fb, x, y, "You can now read it from", COLOR_BLACK);
        line++;
        y = MARGIN_TOP + (line++ * LINE_HEIGHT);
        text_render_string(fb, x, y, "the main library menu.", COLOR_BLACK);
    } else {
        text_render_string(fb, x, y, "Download Failed", COLOR_BLACK);
        line += 2;
        y = MARGIN_TOP + (line++ * LINE_HEIGHT);
        text_render_string(fb, x, y, menu->error_message, COLOR_BLACK);
    }

    /* Render separator before hints */
    library_browser_draw_separator_line(fb, LIBRARY_SEPARATOR_2_LINE);

    /* Render control hints */
    x = MARGIN_LEFT;
    y = MARGIN_TOP + (LIBRARY_HINTS_LINE * LINE_HEIGHT);
    text_render_string(fb, x, y, "SELECT:Continue  BACK:Return", COLOR_BLACK);

    return LIBRARY_BROWSER_SUCCESS;
}

/*
 * Library Browser Input Handling
 */

library_browser_action_t library_browser_handle_event(library_browser_menu_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return LIBRARY_BROWSER_ACTION_NONE;
    }

    /* Only handle button press events */
    if (event->event_type != BUTTON_EVENT_PRESS) {
        return LIBRARY_BROWSER_ACTION_NONE;
    }

    /* Handle based on current state */
    switch (menu->state) {
        case LIBRARY_STATE_BROWSING:
            return library_browser_handle_browsing_event(menu, event);
        case LIBRARY_STATE_DETAILS:
            return library_browser_handle_details_event(menu, event);
        case LIBRARY_STATE_DOWNLOADING:
            return library_browser_handle_downloading_event(menu, event);
        case LIBRARY_STATE_RESULT:
            return library_browser_handle_result_event(menu, event);
        default:
            return LIBRARY_BROWSER_ACTION_NONE;
    }
}

static library_browser_action_t library_browser_handle_browsing_event(library_browser_menu_t *menu, const button_event_t *event) {
    library_browser_action_t action = LIBRARY_BROWSER_ACTION_NONE;

    switch (event->button) {
        case BUTTON_UP:
            /* Move selection up */
            menu->selected_index--;
            if (menu->selected_index < 0) {
                menu->selected_index = CATALOG_SIZE - 1;
                menu->scroll_offset = (CATALOG_SIZE > menu->visible_items) ?
                                     (CATALOG_SIZE - menu->visible_items) : 0;
            } else if (menu->selected_index < menu->scroll_offset) {
                menu->scroll_offset = menu->selected_index;
            }
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        case BUTTON_DOWN:
            /* Move selection down */
            menu->selected_index++;
            if (menu->selected_index >= CATALOG_SIZE) {
                menu->selected_index = 0;
                menu->scroll_offset = 0;
            } else if (menu->selected_index >= menu->scroll_offset + menu->visible_items) {
                menu->scroll_offset++;
            }
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        case BUTTON_SELECT:
            /* Show book details */
            menu->state = LIBRARY_STATE_DETAILS;
            menu->details_scroll = 0;
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        case BUTTON_BACK:
            /* Exit library browser */
            action = LIBRARY_BROWSER_ACTION_EXIT;
            break;

        default:
            break;
    }

    return action;
}

static library_browser_action_t library_browser_handle_details_event(library_browser_menu_t *menu, const button_event_t *event) {
    library_browser_action_t action = LIBRARY_BROWSER_ACTION_NONE;

    switch (event->button) {
        case BUTTON_SELECT:
            /* Start download */
            if (library_browser_start_download(menu, menu->selected_index) == 0) {
                menu->state = LIBRARY_STATE_DOWNLOADING;
                menu->needs_redraw = true;
                action = LIBRARY_BROWSER_ACTION_REDRAW;
            }
            break;

        case BUTTON_BACK:
            /* Return to browsing */
            menu->state = LIBRARY_STATE_BROWSING;
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        default:
            break;
    }

    return action;
}

static library_browser_action_t library_browser_handle_downloading_event(library_browser_menu_t *menu, const button_event_t *event) {
    library_browser_action_t action = LIBRARY_BROWSER_ACTION_NONE;

    switch (event->button) {
        case BUTTON_BACK:
            /* Cancel download */
            library_browser_cancel_download(menu);
            menu->state = LIBRARY_STATE_DETAILS;
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        default:
            break;
    }

    return action;
}

static library_browser_action_t library_browser_handle_result_event(library_browser_menu_t *menu, const button_event_t *event) {
    library_browser_action_t action = LIBRARY_BROWSER_ACTION_NONE;

    switch (event->button) {
        case BUTTON_SELECT:
        case BUTTON_BACK:
            /* Return to browsing */
            if (menu->download_success) {
                action = LIBRARY_BROWSER_ACTION_DOWNLOAD_COMPLETE;
            }
            menu->state = LIBRARY_STATE_BROWSING;
            menu->needs_redraw = true;
            action = LIBRARY_BROWSER_ACTION_REDRAW;
            break;

        default:
            break;
    }

    return action;
}

/*
 * Library Browser Operations
 */

const catalog_entry_t* library_browser_get_catalog_entry(int index) {
    if (index < 0 || index >= CATALOG_SIZE) {
        return NULL;
    }
    return &book_catalog[index];
}

int library_browser_get_catalog_size(void) {
    return CATALOG_SIZE;
}

int library_browser_build_download_url(int gutenberg_id, char *url_buffer, int buffer_size) {
    if (!url_buffer || buffer_size < MAX_URL_LENGTH) {
        return -1;
    }

    /* Build Project Gutenberg download URL */
    snprintf(url_buffer, buffer_size,
             "https://www.gutenberg.org/ebooks/%d.txt.utf-8",
             gutenberg_id);

    return 0;
}

int library_browser_start_download(library_browser_menu_t *menu, int catalog_index) {
    if (!menu || catalog_index < 0 || catalog_index >= CATALOG_SIZE) {
        return -1;
    }

    const catalog_entry_t *entry = &book_catalog[catalog_index];

    /* Build download URL */
    if (library_browser_build_download_url(entry->gutenberg_id,
                                           menu->download_url,
                                           sizeof(menu->download_url)) != 0) {
        return -1;
    }

    /* Build filename */
    snprintf(menu->download_filename, sizeof(menu->download_filename),
             "%d.txt", entry->gutenberg_id);

    /* Set up download options */
    download_options_t options = download_manager_get_default_options();
    options.timeout_seconds = 60;
    options.validate_file = true;
    options.overwrite_existing = false;

    /* Start download */
    menu->download_active = true;
    memset(&menu->download_progress, 0, sizeof(menu->download_progress));

    int result = download_manager_download(menu->download_url,
                                           menu->download_filename,
                                           &options);

    /* Update state based on result */
    menu->download_active = false;

    if (result == 0) {
        menu->download_success = true;
        menu->state = LIBRARY_STATE_RESULT;
    } else {
        menu->download_success = false;
        /* Get error message from download manager */
        download_progress_t progress;
        if (download_manager_get_progress(&progress) == 0) {
            strncpy(menu->error_message, progress.error_message,
                   sizeof(menu->error_message) - 1);
            menu->error_message[sizeof(menu->error_message) - 1] = '\0';
        } else {
            strncpy(menu->error_message, "Unknown download error",
                   sizeof(menu->error_message) - 1);
        }
        menu->state = LIBRARY_STATE_RESULT;
    }

    menu->needs_redraw = true;
    return result;
}

int library_browser_update_download_progress(library_browser_menu_t *menu) {
    if (!menu || !menu->download_active) {
        return -1;
    }

    return download_manager_get_progress(&menu->download_progress);
}

int library_browser_cancel_download(library_browser_menu_t *menu) {
    if (!menu || !menu->download_active) {
        return -1;
    }

    menu->download_active = false;
    return download_manager_cancel();
}

bool library_browser_needs_redraw(library_browser_menu_t *menu) {
    if (!menu) {
        return false;
    }
    return menu->needs_redraw;
}

void library_browser_clear_redraw_flag(library_browser_menu_t *menu) {
    if (menu) {
        menu->needs_redraw = false;
    }
}

const char* library_browser_error_string(library_browser_error_t error) {
    switch (error) {
        case LIBRARY_BROWSER_SUCCESS:
            return "Success";
        case LIBRARY_BROWSER_ERROR_NULL_POINTER:
            return "Null pointer error";
        case LIBRARY_BROWSER_ERROR_INVALID_STATE:
            return "Invalid state";
        case LIBRARY_BROWSER_ERROR_RENDER_FAILED:
            return "Rendering failed";
        case LIBRARY_BROWSER_ERROR_DOWNLOAD_FAILED:
            return "Download failed";
        case LIBRARY_BROWSER_ERROR_NO_WIFI:
            return "WiFi not connected";
        default:
            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

static void library_browser_draw_separator_line(framebuffer_t *fb, int line_number) {
    if (!fb) {
        return;
    }

    int y = MARGIN_TOP + (line_number * LINE_HEIGHT) + (FONT_HEIGHT / 2);
    int x1 = MARGIN_LEFT;
    int x2 = FB_WIDTH - MARGIN_RIGHT - 1;

    fb_draw_hline(fb, x1, x2, y, COLOR_BLACK);
}
