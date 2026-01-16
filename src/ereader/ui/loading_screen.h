/*
 * loading_screen.h - Loading Screen for Slow Operations
 *
 * Provides visual feedback during slow operations such as:
 * - Opening large PDF/EPUB files
 * - Scanning book directory
 * - Processing text pagination
 *
 * Phase 05: Polish and Advanced Features
 */

#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"

/*
 * Loading Screen State
 */

typedef enum {
    LOADING_STATE_IDLE,         /* Not currently loading */
    LOADING_STATE_ACTIVE,       /* Loading in progress */
    LOADING_STATE_COMPLETE,     /* Loading complete */
    LOADING_STATE_ERROR         /* Loading failed */
} loading_state_t;

typedef struct {
    loading_state_t state;      /* Current loading state */
    char operation[64];         /* Operation description (e.g., "Opening book...") */
    char detail[128];           /* Optional detail (e.g., filename) */
    uint32_t frame;             /* Animation frame counter */
    int progress_current;       /* Current progress (for progress bar) */
    int progress_max;           /* Maximum progress (for progress bar) */
    bool show_progress;         /* Whether to show progress bar */
    bool show_spinner;          /* Whether to show spinner */
} loading_screen_t;

/*
 * Loading Screen Functions
 */

/**
 * Initialize a loading screen
 *
 * @param screen: Pointer to loading screen structure
 */
void loading_screen_init(loading_screen_t *screen);

/**
 * Start showing loading screen with a message
 *
 * @param screen: Pointer to loading screen
 * @param operation: Operation description (e.g., "Opening book")
 * @param detail: Optional detail text (e.g., book filename) or NULL
 * @param show_progress: Whether to show progress bar
 */
void loading_screen_start(loading_screen_t *screen,
                          const char *operation,
                          const char *detail,
                          bool show_progress);

/**
 * Update loading screen progress
 *
 * @param screen: Pointer to loading screen
 * @param current: Current progress value
 * @param maximum: Maximum progress value
 */
void loading_screen_update_progress(loading_screen_t *screen, int current, int maximum);

/**
 * Update loading screen animation frame
 *
 * @param screen: Pointer to loading screen
 */
void loading_screen_update_animation(loading_screen_t *screen);

/**
 * Mark loading as complete
 *
 * @param screen: Pointer to loading screen
 */
void loading_screen_complete(loading_screen_t *screen);

/**
 * Mark loading as failed with error message
 *
 * @param screen: Pointer to loading screen
 * @param error_message: Error message to display
 */
void loading_screen_error(loading_screen_t *screen, const char *error_message);

/**
 * Render loading screen to framebuffer
 *
 * @param screen: Pointer to loading screen
 * @param fb: Framebuffer to render to
 */
void loading_screen_render(loading_screen_t *screen, framebuffer_t *fb);

/**
 * Check if loading screen is active
 *
 * @param screen: Pointer to loading screen
 * @return: true if active, false otherwise
 */
bool loading_screen_is_active(loading_screen_t *screen);

/**
 * Reset loading screen to idle state
 *
 * @param screen: Pointer to loading screen
 */
void loading_screen_reset(loading_screen_t *screen);

/*
 * Convenience Functions for Common Operations
 */

/**
 * Show "Opening book..." loading screen
 *
 * @param screen: Pointer to loading screen
 * @param filename: Book filename
 */
void loading_screen_show_opening_book(loading_screen_t *screen, const char *filename);

/**
 * Show "Scanning library..." loading screen
 *
 * @param screen: Pointer to loading screen
 */
void loading_screen_show_scanning_library(loading_screen_t *screen);

/**
 * Show "Processing..." loading screen with progress
 *
 * @param screen: Pointer to loading screen
 * @param operation: Operation name
 */
void loading_screen_show_processing(loading_screen_t *screen, const char *operation);

#endif /* LOADING_SCREEN_H */
