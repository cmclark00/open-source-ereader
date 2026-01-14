/*
 * ereader.h - E-Reader Application Header
 *
 * Application-wide definitions, constants, and structures for the e-reader.
 * This header defines the application state machine, configuration constants,
 * and common types used across all modules.
 *
 * Author: E-Reader Project
 * License: Open Source
 */

#ifndef EREADER_H
#define EREADER_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Application version information
 */
#define EREADER_VERSION_MAJOR   0
#define EREADER_VERSION_MINOR   3
#define EREADER_VERSION_PATCH   0
#define EREADER_VERSION_STRING  "0.3.0"

/*
 * Application configuration constants
 */
#define EREADER_LOG_FILE        "/var/log/ereader.log"
#define EREADER_CONFIG_DIR      "/etc/ereader"
#define EREADER_BOOKS_DIR       "/books"
#define EREADER_BOOKMARKS_FILE  "/etc/ereader/bookmarks.txt"

/*
 * Display configuration (from architecture design)
 */
#define DISPLAY_WIDTH           400
#define DISPLAY_HEIGHT          300
#define DISPLAY_BPP             1       /* 1 bit per pixel (monochrome) */

/*
 * Performance targets (from architecture design)
 */
#define STARTUP_TIMEOUT_MS      10000   /* 10 seconds max boot time */
#define BUTTON_RESPONSE_MS      100     /* 100ms max button response time */
#define PAGE_TURN_TIMEOUT_MS    6000    /* 6 seconds max page turn time */

/*
 * Application state machine
 *
 * Based on the state machine defined in EREADER_DESIGN.md:
 * - STARTUP: Initial boot state, display splash screen
 * - MENU_LIBRARY: Display book list, handle navigation
 * - READING: Display book content, handle page navigation
 * - EMPTY: Display empty library message
 * - ERROR: Display error message
 * - SHUTDOWN: Graceful shutdown in progress
 */
typedef enum {
    STATE_STARTUP,          /* Initial boot, loading resources */
    STATE_MENU_LIBRARY,     /* Book library menu */
    STATE_READING,          /* Reading a book */
    STATE_EMPTY,            /* Empty library (no books found) */
    STATE_ERROR,            /* Error state */
    STATE_SHUTDOWN          /* Shutting down */
} app_state_t;

/*
 * Application-wide error codes
 */
typedef enum {
    ERROR_NONE = 0,         /* No error */
    ERROR_INIT_DISPLAY,     /* Failed to initialize display driver */
    ERROR_INIT_BUTTONS,     /* Failed to initialize button input */
    ERROR_SCAN_BOOKS,       /* Failed to scan book directory */
    ERROR_LOAD_BOOK,        /* Failed to load book file */
    ERROR_SAVE_BOOKMARK,    /* Failed to save bookmark */
    ERROR_OUT_OF_MEMORY,    /* Out of memory */
    ERROR_INVALID_STATE,    /* Invalid state transition */
    ERROR_SYSTEM            /* System error (check errno) */
} app_error_t;

/*
 * Application context structure
 *
 * Contains all application-wide state and resources.
 * This is passed to most application functions.
 */
typedef struct {
    /* Current application state */
    app_state_t state;
    app_state_t previous_state;

    /* Error information */
    app_error_t last_error;
    char error_message[256];

    /* Hardware interfaces */
    void *button_ctx;       /* button_input_ctx_t* from button_input.h */

    /* Book management */
    void *book_list;        /* book_list_t* from book_manager.h */
    void *current_book;     /* book_t* from book_manager.h */
    void *bookmarks;        /* bookmark_list_t* from book_manager.h */

    /* UI state */
    void *menu_state;       /* menu_state_t* from menu.h */
    void *reader_state;     /* reader_state_t* from reader.h */

    /* Framebuffer */
    void *framebuffer;      /* framebuffer_t* from framebuffer.h */

    /* Flags */
    bool needs_redraw;      /* Screen needs to be redrawn */
    bool running;           /* Application is running */
    bool shutdown_requested; /* Shutdown has been requested */

} app_context_t;

/*
 * Function prototypes
 */

/**
 * Initialize application context and all subsystems
 *
 * Initializes:
 * - Display driver
 * - Button input
 * - Framebuffer
 * - Book manager
 * - UI components
 *
 * Returns:
 *   Pointer to application context on success
 *   NULL on failure
 */
app_context_t* app_init(void);

/**
 * Clean up application context and free all resources
 *
 * Parameters:
 *   ctx - Application context (may be NULL)
 */
void app_cleanup(app_context_t *ctx);

/**
 * Run the main application loop
 *
 * This is the main event loop that:
 * - Processes button events
 * - Updates application state
 * - Renders UI
 * - Handles state transitions
 *
 * Parameters:
 *   ctx - Application context
 *
 * Returns:
 *   0 on normal exit
 *   -1 on error
 */
int app_run(app_context_t *ctx);

/**
 * Transition to a new application state
 *
 * Handles state transition logic, cleanup of old state,
 * and initialization of new state.
 *
 * Parameters:
 *   ctx - Application context
 *   new_state - Target state
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int app_change_state(app_context_t *ctx, app_state_t new_state);

/**
 * Process button event in current state
 *
 * Dispatches button events to appropriate handlers based on
 * current application state.
 *
 * Parameters:
 *   ctx - Application context
 *   button_event - Button event from input system
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int app_handle_button_event(app_context_t *ctx, void *button_event);

/**
 * Render current application state to framebuffer
 *
 * Calls appropriate rendering function based on current state:
 * - STARTUP: splash screen
 * - MENU_LIBRARY: menu UI
 * - READING: reader UI
 * - EMPTY: empty library message
 * - ERROR: error message
 *
 * Parameters:
 *   ctx - Application context
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int app_render(app_context_t *ctx);

/**
 * Refresh display with current framebuffer contents
 *
 * Calls the e-paper display driver to update the screen.
 *
 * Parameters:
 *   ctx - Application context
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int app_refresh_display(app_context_t *ctx);

/**
 * Set application error state
 *
 * Sets the error code and message, and optionally transitions
 * to error state.
 *
 * Parameters:
 *   ctx - Application context
 *   error - Error code
 *   message - Error message (optional, can be NULL)
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int app_set_error(app_context_t *ctx, app_error_t error, const char *message);

/**
 * Request graceful shutdown
 *
 * Sets the shutdown_requested flag, which will cause the main loop
 * to exit cleanly after completing the current iteration.
 *
 * Parameters:
 *   ctx - Application context
 */
void app_request_shutdown(app_context_t *ctx);

/**
 * Convert application state to string
 *
 * Parameters:
 *   state - Application state
 *
 * Returns:
 *   String representation of state
 */
const char* app_state_to_string(app_state_t state);

/**
 * Convert error code to string
 *
 * Parameters:
 *   error - Error code
 *
 * Returns:
 *   String representation of error
 */
const char* app_error_to_string(app_error_t error);

#endif /* EREADER_H */
