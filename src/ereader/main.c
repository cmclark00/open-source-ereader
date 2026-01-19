/*
 * main.c - E-Reader Application Main Entry Point
 *
 * This file implements the main application logic, including:
 * - Application initialization and cleanup
 * - Main event loop
 * - State machine management
 * - Signal handling for graceful shutdown
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
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ereader.h"
#include "rendering/framebuffer.h"
#include "rendering/text_renderer.h"
#include "books/book_manager.h"
#include "formats/format_interface.h"
#include "ui/menu.h"
#include "ui/reader.h"
#include "ui/settings_menu.h"
#include "ui/loading_screen.h"
#include "settings/settings_manager.h"

/* Include Phase 1 and Phase 2 components */
#include "../display-test/epd_driver.h"
#include "../button-test/button_input.h"

/* Global application context for signal handlers */
static app_context_t *g_app_ctx = NULL;

/* Forward declarations */
static void signal_handler(int signum);
static int ensure_directories_exist(void);
static int app_render_startup(app_context_t *ctx);
static int app_render_empty(app_context_t *ctx);
static int app_render_error(app_context_t *ctx);

/*
 * Signal handler for graceful shutdown
 */
static void signal_handler(int signum) {
    printf("Received signal %d, requesting shutdown...\n", signum);
    if (g_app_ctx != NULL) {
        app_request_shutdown(g_app_ctx);
    }
}

/*
 * Ensure required directories exist
 */
static int ensure_directories_exist(void) {
    struct stat st;

    /* Create /etc/ereader if it doesn't exist */
    if (stat(EREADER_CONFIG_DIR, &st) != 0) {
        if (mkdir(EREADER_CONFIG_DIR, 0755) != 0) {
            fprintf(stderr, "Failed to create %s: %s\n",
                    EREADER_CONFIG_DIR, strerror(errno));
            return -1;
        }
    }

    /* Create /books if it doesn't exist */
    if (stat(EREADER_BOOKS_DIR, &st) != 0) {
        if (mkdir(EREADER_BOOKS_DIR, 0755) != 0) {
            fprintf(stderr, "Failed to create %s: %s\n",
                    EREADER_BOOKS_DIR, strerror(errno));
            return -1;
        }
    }

    return 0;
}

/*
 * Initialize application context and all subsystems
 */
app_context_t* app_init(void) {
    app_context_t *ctx = NULL;

    printf("Initializing e-reader application v%s...\n", EREADER_VERSION_STRING);

    /* Allocate application context */
    ctx = calloc(1, sizeof(app_context_t));
    if (ctx == NULL) {
        fprintf(stderr, "Failed to allocate application context\n");
        return NULL;
    }

    /* Initialize state */
    ctx->state = STATE_STARTUP;
    ctx->previous_state = STATE_STARTUP;
    ctx->running = true;
    ctx->needs_redraw = true;

    /* Ensure required directories exist */
    if (ensure_directories_exist() != 0) {
        fprintf(stderr, "Failed to create required directories\n");
        free(ctx);
        return NULL;
    }

    /* Initialize format system */
    printf("Initializing format system...\n");
    if (format_init() != FORMAT_SUCCESS) {
        fprintf(stderr, "Failed to initialize format system\n");
        free(ctx);
        return NULL;
    }

    /* Initialize display driver */
    printf("Initializing display driver...\n");
    if (epd_init() != 0) {
        fprintf(stderr, "Failed to initialize display driver\n");
        app_set_error(ctx, ERROR_INIT_DISPLAY, "Failed to initialize display driver");
        free(ctx);
        return NULL;
    }

    if (epd_display_init() != 0) {
        fprintf(stderr, "Failed to initialize display hardware\n");
        app_set_error(ctx, ERROR_INIT_DISPLAY, "Failed to initialize display hardware");
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Initialize framebuffer */
    printf("Initializing framebuffer...\n");
    ctx->framebuffer = fb_init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (ctx->framebuffer == NULL) {
        fprintf(stderr, "Failed to initialize framebuffer\n");
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Initialize button input */
    printf("Initializing button input...\n");
    ctx->button_ctx = button_input_init();
    if (ctx->button_ctx == NULL) {
        fprintf(stderr, "Failed to initialize button input\n");
        app_set_error(ctx, ERROR_INIT_BUTTONS, "Failed to initialize button input");
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Initialize book manager */
    printf("Scanning book directory...\n");
    ctx->book_list = book_list_create();
    if (ctx->book_list == NULL) {
        fprintf(stderr, "Failed to create book list\n");
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Scan for books */
    book_error_t book_err = book_list_scan(ctx->book_list, EREADER_BOOKS_DIR);
    if (book_err != BOOK_ERROR_NONE && book_err != BOOK_ERROR_NO_BOOKS) {
        fprintf(stderr, "Failed to scan books: %s\n", book_error_string(book_err));
        book_list_free(ctx->book_list);
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    printf("Found %d book(s)\n", book_list_get_count(ctx->book_list));

    /* Load bookmarks */
    printf("Loading bookmarks...\n");
    ctx->bookmarks = bookmark_list_create();
    if (ctx->bookmarks == NULL) {
        fprintf(stderr, "Failed to create bookmark list\n");
        book_list_free(ctx->book_list);
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    bookmark_list_load(ctx->bookmarks, EREADER_BOOKMARKS_FILE);

    /* Load settings */
    printf("Loading settings...\n");
    ctx->settings = malloc(sizeof(settings_t));
    if (ctx->settings == NULL) {
        fprintf(stderr, "Failed to allocate settings\n");
        bookmark_list_free(ctx->bookmarks);
        book_list_free(ctx->book_list);
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Initialize with defaults and load from file */
    settings_init_defaults((settings_t*)ctx->settings);
    settings_error_t settings_err = settings_load((settings_t*)ctx->settings, SETTINGS_FILE);
    if (settings_err != SETTINGS_SUCCESS && settings_err != SETTINGS_ERROR_FILE_NOT_FOUND) {
        fprintf(stderr, "Warning: Failed to load settings: %s\n",
                settings_error_to_string(settings_err));
    }

    /* Initialize menu state */
    printf("Initializing menu...\n");
    ctx->menu_state = menu_create(ctx->book_list, ctx->bookmarks);
    if (ctx->menu_state == NULL) {
        fprintf(stderr, "Failed to create menu state\n");
        bookmark_list_free(ctx->bookmarks);
        book_list_free(ctx->book_list);
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }

    /* Initialize loading screen */
    printf("Initializing loading screen...\n");
    ctx->loading_screen = malloc(sizeof(loading_screen_t));
    if (ctx->loading_screen == NULL) {
        fprintf(stderr, "Failed to allocate loading screen\n");
        menu_free(ctx->menu_state);
        bookmark_list_free(ctx->bookmarks);
        book_list_free(ctx->book_list);
        button_input_cleanup(ctx->button_ctx);
        fb_free(ctx->framebuffer);
        epd_cleanup();
        free(ctx);
        return NULL;
    }
    loading_screen_init((loading_screen_t*)ctx->loading_screen);

    printf("Initialization complete\n");
    return ctx;
}

/*
 * Clean up application context and free all resources
 */
void app_cleanup(app_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }

    printf("Cleaning up application...\n");

    /* Save bookmarks before cleanup */
    if (ctx->bookmarks != NULL) {
        bookmark_list_save(ctx->bookmarks, EREADER_BOOKMARKS_FILE);
    }

    /* Cleanup reader state */
    if (ctx->reader_state != NULL) {
        reader_free(ctx->reader_state);
        ctx->reader_state = NULL;
    }

    /* Cleanup menu state */
    if (ctx->menu_state != NULL) {
        menu_free(ctx->menu_state);
        ctx->menu_state = NULL;
    }

    /* Cleanup settings menu state */
    if (ctx->settings_menu_state != NULL) {
        settings_menu_free(ctx->settings_menu_state);
        ctx->settings_menu_state = NULL;
    }
    /* Cleanup loading screen */
    if (ctx->loading_screen != NULL) {
        free(ctx->loading_screen);
        ctx->loading_screen = NULL;
    }


    /* Save and cleanup settings */
    if (ctx->settings != NULL) {
        settings_save((settings_t*)ctx->settings, SETTINGS_FILE);
        free(ctx->settings);
        ctx->settings = NULL;
    }

    /* Cleanup current book */
    if (ctx->current_book != NULL) {
        book_free(ctx->current_book);
        ctx->current_book = NULL;
    }

    /* Cleanup bookmarks */
    if (ctx->bookmarks != NULL) {
        bookmark_list_free(ctx->bookmarks);
        ctx->bookmarks = NULL;
    }

    /* Cleanup book list */
    if (ctx->book_list != NULL) {
        book_list_free(ctx->book_list);
        ctx->book_list = NULL;
    }

    /* Cleanup button input */
    if (ctx->button_ctx != NULL) {
        button_input_cleanup(ctx->button_ctx);
        ctx->button_ctx = NULL;
    }

    /* Cleanup framebuffer */
    if (ctx->framebuffer != NULL) {
        fb_free(ctx->framebuffer);
        ctx->framebuffer = NULL;
    }

    /* Cleanup display driver */
    epd_sleep();
    epd_cleanup();

    free(ctx);
    printf("Cleanup complete\n");
}

/*
 * Transition to a new application state
 */
int app_change_state(app_context_t *ctx, app_state_t new_state) {
    if (ctx == NULL) {
        return -1;
    }

    printf("State transition: %s -> %s\n",
           app_state_to_string(ctx->state),
           app_state_to_string(new_state));

    /* Store previous state */
    ctx->previous_state = ctx->state;
    ctx->state = new_state;
    ctx->needs_redraw = true;

    /* Handle state-specific initialization */
    switch (new_state) {
        case STATE_MENU_LIBRARY:
            /* Initialize or reset menu */
            if (ctx->menu_state == NULL) {
                ctx->menu_state = menu_create(ctx->book_list, ctx->bookmarks);
                if (ctx->menu_state == NULL) {
                    app_set_error(ctx, ERROR_INVALID_STATE, "Failed to create menu");
                    return -1;
                }
            } else {
                menu_reset(ctx->menu_state);
            }
            break;

        case STATE_READING:
            /* Reader state should be initialized by menu selection handler */
            if (ctx->reader_state == NULL) {
                app_set_error(ctx, ERROR_INVALID_STATE, "Reader state not initialized");
                return -1;
            }
            break;

        case STATE_SETTINGS:
            /* Initialize or reset settings menu */
            if (ctx->settings_menu_state == NULL) {
                ctx->settings_menu_state = settings_menu_create((settings_t*)ctx->settings);
                if (ctx->settings_menu_state == NULL) {
                    app_set_error(ctx, ERROR_INVALID_STATE, "Failed to create settings menu");
                    return -1;
                }
            } else {
                settings_menu_reset(ctx->settings_menu_state);
            }
            break;

        case STATE_EMPTY:
            /* No initialization needed for empty state */
            break;

        case STATE_ERROR:
            /* Error state, display error message */
            break;

        case STATE_SHUTDOWN:
            /* Request shutdown */
            ctx->running = false;
            break;

        case STATE_STARTUP:
        default:
            /* No special handling */
            break;
    }

    return 0;
}

/*
 * Process button event in current state
 */
int app_handle_button_event(app_context_t *ctx, void *button_event_ptr) {
    if (ctx == NULL || button_event_ptr == NULL) {
        return -1;
    }

    button_event_t *event = (button_event_t *)button_event_ptr;

    /* Handle global shortcuts (available in all states) */
    /* None currently defined */

    /* Dispatch to state-specific handler */
    switch (ctx->state) {
        case STATE_STARTUP:
            /* No button handling during startup */
            break;

        case STATE_MENU_LIBRARY: {
            /* Check for MENU button to open settings */
            if (event->button == BUTTON_MENU && event->event_type == BUTTON_EVENT_PRESS) {
                app_change_state(ctx, STATE_SETTINGS);
                break;
            }

            menu_action_t action = menu_handle_event(ctx->menu_state, event);

            switch (action) {
                case MENU_ACTION_SELECT_BOOK: {
                    /* Get selected book */
                    book_metadata_t *metadata = menu_get_selected_book(ctx->menu_state);
                    if (metadata == NULL) {
                        break;
                    }

                    /* Load book */
                    /* Show loading screen */
                    loading_screen_t *loading = (loading_screen_t*)ctx->loading_screen;
                    loading_screen_show_opening_book(loading, metadata->filename);
                    loading_screen_render(loading, (framebuffer_t*)ctx->framebuffer);
                    app_refresh_display(ctx);

                    printf("Loading book: %s\n", metadata->filename);
                    ctx->current_book = book_create();
                    if (ctx->current_book == NULL) {
                        loading_screen_reset(loading);
                        app_set_error(ctx, ERROR_OUT_OF_MEMORY, "Failed to create book");
                        break;
                    }

                    book_error_t err = book_load(ctx->current_book, metadata->filepath);
                    if (err != BOOK_ERROR_NONE) {
                        fprintf(stderr, "Failed to load book: %s\n", book_error_string(err));
                        book_free(ctx->current_book);
                        ctx->current_book = NULL;
                        app_set_error(ctx, ERROR_LOAD_BOOK, "Failed to load book");
                        loading_screen_reset(loading);
                        break;
                    }

                    /* Get bookmark for this book */
                    int initial_page = -1;  /* -1 = use bookmark or start at beginning */
                    bookmark_t *bookmark = bookmark_get(ctx->bookmarks, metadata->filename);
                    if (bookmark != NULL) {
                        initial_page = bookmark->page;
                        printf("Found bookmark at page %d\n", initial_page);
                    }

                    /* Create reader state */
                    ctx->reader_state = reader_create(ctx->current_book, metadata, ctx->bookmarks, initial_page);
                    if (ctx->reader_state == NULL) {
                        book_free(ctx->current_book);
                        ctx->current_book = NULL;
                        loading_screen_reset(loading);
                        app_set_error(ctx, ERROR_INVALID_STATE, "Failed to create reader");
                        break;
                    }

                    /* Complete loading screen */
                    loading_screen_complete(loading);
                    loading_screen_reset(loading);

                    /* Transition to reading state */
                    app_change_state(ctx, STATE_READING);
                    break;
                }

                case MENU_ACTION_EXIT:
                    app_request_shutdown(ctx);
                    break;

                case MENU_ACTION_REDRAW:
                    ctx->needs_redraw = true;
                    break;

                case MENU_ACTION_NONE:
                default:
                    /* No action needed */
                    break;
            }
            break;
        }

        case STATE_READING: {
            reader_action_t action = reader_handle_event(ctx->reader_state, event);

            switch (action) {
                case READER_ACTION_EXIT:
                    /* Return to menu */
                    reader_free(ctx->reader_state);
                    ctx->reader_state = NULL;

                    book_free(ctx->current_book);
                    ctx->current_book = NULL;

                    app_change_state(ctx, STATE_MENU_LIBRARY);
                    break;

                case READER_ACTION_SAVE_BOOKMARK:
                    /* Bookmark saved automatically by reader */
                    printf("Bookmark saved\n");
                    break;

                case READER_ACTION_REDRAW:
                case READER_ACTION_PAGE_CHANGED:
                    ctx->needs_redraw = true;
                    break;

                case READER_ACTION_NONE:
                default:
                    /* No action needed */
                    break;
            }
            break;
        }

        case STATE_SETTINGS: {
            settings_menu_action_t action = settings_menu_handle_event(ctx->settings_menu_state, event);

            switch (action) {
                case SETTINGS_MENU_ACTION_SAVE_EXIT:
                    /* Save settings and return to library menu */
                    if (settings_menu_settings_changed(ctx->settings_menu_state)) {
                        int save_result = settings_menu_save_settings(ctx->settings_menu_state);
                        if (save_result != SETTINGS_MENU_SUCCESS) {
                            fprintf(stderr, "Warning: Failed to save settings\n");
                        }
                        printf("Settings saved\n");
                    }
                    app_change_state(ctx, STATE_MENU_LIBRARY);
                    break;

                case SETTINGS_MENU_ACTION_VALUE_CHANGED:
                case SETTINGS_MENU_ACTION_REDRAW:
                    ctx->needs_redraw = true;
                    break;

                case SETTINGS_MENU_ACTION_NONE:
                default:
                    /* No action needed */
                    break;
            }
            break;
        }

        case STATE_EMPTY:
            /* Back button returns to menu (to rescan) */
            if (event->button == BUTTON_BACK &&
                event->event_type == BUTTON_EVENT_PRESS) {
                /* Rescan books */
                book_list_scan(ctx->book_list, EREADER_BOOKS_DIR);
                if (book_list_get_count(ctx->book_list) > 0) {
                    app_change_state(ctx, STATE_MENU_LIBRARY);
                }
            }
            break;

        case STATE_ERROR:
            /* Any button clears error and returns to menu */
            if (event->event_type == BUTTON_EVENT_PRESS) {
                if (book_list_get_count(ctx->book_list) > 0) {
                    app_change_state(ctx, STATE_MENU_LIBRARY);
                } else {
                    app_change_state(ctx, STATE_EMPTY);
                }
            }
            break;

        case STATE_SHUTDOWN:
            /* No button handling during shutdown */
            break;

        default:
            break;
    }

    return 0;
}

/*
 * Render startup splash screen
 */
static int app_render_startup(app_context_t *ctx) {
    framebuffer_t *fb = (framebuffer_t *)ctx->framebuffer;
    if (fb == NULL) {
        return -1;
    }

    fb_clear(fb, COLOR_WHITE);

    /* Render centered title */
    const char *title = "E-Reader";
    const char *version = "Version " EREADER_VERSION_STRING;
    const char *loading = "Loading...";

    int title_y = 140;
    int version_y = 160;
    int loading_y = 180;

    text_render_string(fb, 150, title_y, title);
    text_render_string(fb, 140, version_y, version);
    text_render_string(fb, 150, loading_y, loading);

    return 0;
}

/*
 * Render empty library message
 */
static int app_render_empty(app_context_t *ctx) {
    framebuffer_t *fb = (framebuffer_t *)ctx->framebuffer;
    if (fb == NULL) {
        return -1;
    }

    fb_clear(fb, COLOR_WHITE);

    const char *msg1 = "No books found.";
    const char *msg2 = "Copy books to /books/";
    const char *msg3 = "(.txt/.epub/.pdf)";
    const char *msg4 = "Press BACK to rescan";

    int y = 120;
    text_render_string(fb, 100, y, msg1);
    text_render_string(fb, 80, y + 20, msg2);
    text_render_string(fb, 90, y + 40, msg3);
    text_render_string(fb, 80, y + 60, msg4);

    return 0;
}

/*
 * Render error message
 */
static int app_render_error(app_context_t *ctx) {
    framebuffer_t *fb = (framebuffer_t *)ctx->framebuffer;
    if (fb == NULL) {
        return -1;
    }

    fb_clear(fb, COLOR_WHITE);

    const char *title = "Error";
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "%s: %s",
             app_error_to_string(ctx->last_error),
             ctx->error_message);

    int y = 130;
    text_render_string(fb, 170, y, title);
    text_render_string(fb, 20, y + 30, error_msg);
    text_render_string(fb, 100, y + 60, "Press any button");

    return 0;
}

/*
 * Render current application state to framebuffer
 */
int app_render(app_context_t *ctx) {
    if (ctx == NULL || ctx->framebuffer == NULL) {
        return -1;
    }

    framebuffer_t *fb = (framebuffer_t *)ctx->framebuffer;

    switch (ctx->state) {
        case STATE_STARTUP:
            return app_render_startup(ctx);

        case STATE_MENU_LIBRARY:
            if (ctx->menu_state != NULL) {
                return menu_render(ctx->menu_state, fb);
            }
            break;

        case STATE_READING:
            if (ctx->reader_state != NULL) {
                return reader_render(ctx->reader_state, fb);
            }
            break;

        case STATE_SETTINGS:
            if (ctx->settings_menu_state != NULL) {
                return settings_menu_render(ctx->settings_menu_state, fb);
            }
            break;

        case STATE_EMPTY:
            return app_render_empty(ctx);

        case STATE_ERROR:
            return app_render_error(ctx);

        case STATE_SHUTDOWN:
            fb_clear(fb, COLOR_WHITE);
            text_render_string(fb, 140, 140, "Shutting down...");
            return 0;

        default:
            break;
    }

    return 0;
}

/*
 * Refresh display with current framebuffer contents
 */
int app_refresh_display(app_context_t *ctx) {
    if (ctx == NULL || ctx->framebuffer == NULL) {
        return -1;
    }

    framebuffer_t *fb = (framebuffer_t *)ctx->framebuffer;

    /* Copy framebuffer to e-paper display buffer */
    uint8_t *epd_buffer = epd_get_framebuffer();
    if (epd_buffer == NULL) {
        return -1;
    }

    fb_copy_to_buffer(fb, epd_buffer);

    /* Refresh display */
    return epd_refresh();
}

/*
 * Set application error state
 */
int app_set_error(app_context_t *ctx, app_error_t error, const char *message) {
    if (ctx == NULL) {
        return -1;
    }

    ctx->last_error = error;
    if (message != NULL) {
        strncpy(ctx->error_message, message, sizeof(ctx->error_message) - 1);
        ctx->error_message[sizeof(ctx->error_message) - 1] = '\0';
    } else {
        ctx->error_message[0] = '\0';
    }

    return 0;
}

/*
 * Request graceful shutdown
 */
void app_request_shutdown(app_context_t *ctx) {
    if (ctx != NULL) {
        ctx->shutdown_requested = true;
        printf("Shutdown requested\n");
    }
}

/*
 * Run the main application loop
 */
int app_run(app_context_t *ctx) {
    if (ctx == NULL) {
        return -1;
    }

    printf("Starting main event loop...\n");

    /* Render and display startup screen */
    app_render(ctx);
    app_refresh_display(ctx);

    /* Brief startup delay to show splash screen */
    usleep(500000);  /* 500ms */

    /* Determine initial state based on book count */
    int book_count = book_list_get_count(ctx->book_list);
    if (book_count > 0) {
        app_change_state(ctx, STATE_MENU_LIBRARY);
    } else {
        app_change_state(ctx, STATE_EMPTY);
    }

    /* Main event loop */
    while (ctx->running && !ctx->shutdown_requested) {
        /* Render if needed */
        if (ctx->needs_redraw) {
            app_render(ctx);
            app_refresh_display(ctx);
            ctx->needs_redraw = false;
        }

        /* Wait for button event with timeout */
        button_event_t event;
        int ret = button_input_read_event_timeout(ctx->button_ctx, &event, 1000);

        if (ret > 0) {
            /* Event received */
            printf("Button event: %s %s\n",
                   button_to_string(event.button),
                   button_event_type_to_string(event.event_type));

            app_handle_button_event(ctx, &event);
        } else if (ret < 0) {
            /* Error reading button event */
            fprintf(stderr, "Error reading button event: %s\n", strerror(errno));
            /* Continue anyway, don't exit on button read error */
        }
        /* ret == 0 means timeout, which is normal */
    }

    /* Shutdown state */
    app_change_state(ctx, STATE_SHUTDOWN);
    app_render(ctx);
    app_refresh_display(ctx);

    printf("Exiting main loop\n");
    return 0;
}

/*
 * Convert application state to string
 */
const char* app_state_to_string(app_state_t state) {
    switch (state) {
        case STATE_STARTUP:      return "STARTUP";
        case STATE_MENU_LIBRARY: return "MENU_LIBRARY";
        case STATE_READING:      return "READING";
        case STATE_SETTINGS:     return "SETTINGS";
        case STATE_EMPTY:        return "EMPTY";
        case STATE_ERROR:        return "ERROR";
        case STATE_SHUTDOWN:     return "SHUTDOWN";
        default:                 return "UNKNOWN";
    }
}

/*
 * Convert error code to string
 */
const char* app_error_to_string(app_error_t error) {
    switch (error) {
        case ERROR_NONE:            return "No error";
        case ERROR_INIT_DISPLAY:    return "Display initialization failed";
        case ERROR_INIT_BUTTONS:    return "Button input failed";
        case ERROR_SCAN_BOOKS:      return "Book scan failed";
        case ERROR_LOAD_BOOK:       return "Book load failed";
        case ERROR_SAVE_BOOKMARK:   return "Bookmark save failed";
        case ERROR_OUT_OF_MEMORY:   return "Out of memory";
        case ERROR_INVALID_STATE:   return "Invalid state";
        case ERROR_SYSTEM:          return "System error";
        default:                    return "Unknown error";
    }
}

/*
 * Main entry point
 */
int main(int argc, char *argv[]) {
    int ret = 0;

    printf("===========================================\n");
    printf("E-Reader Application v%s\n", EREADER_VERSION_STRING);
    printf("===========================================\n\n");

    /* Set up signal handlers for graceful shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize application */
    g_app_ctx = app_init();
    if (g_app_ctx == NULL) {
        fprintf(stderr, "Failed to initialize application\n");
        return 1;
    }

    /* Run main loop */
    ret = app_run(g_app_ctx);

    /* Cleanup */
    app_cleanup(g_app_ctx);
    g_app_ctx = NULL;

    printf("\n===========================================\n");
    printf("E-Reader Application exited\n");
    printf("===========================================\n");

    return (ret == 0) ? 0 : 1;
}
