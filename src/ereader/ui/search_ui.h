/*
 * search_ui.h - E-Reader Search User Interface
 *
 * Provides UI for text search within books.
 * Currently implements a read-only search interface with predefined search terms.
 * Full text input via buttons is deferred to Phase 6 (WiFi keyboard support).
 *
 * Phase 05: Polish and Advanced Features
 */

#ifndef SEARCH_UI_H
#define SEARCH_UI_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../search/search_engine.h"
#include "../../button-test/button_input.h"

/*
 * Search UI Configuration
 */
#define SEARCH_UI_MAX_PREDEFINED_TERMS  10   /* Maximum number of predefined search terms */
#define SEARCH_UI_CONTEXT_CHARS         60   /* Characters of context to show around match */

/*
 * Search UI Modes
 */
typedef enum {
    SEARCH_MODE_TERM_SELECTION,   /* Selecting a predefined search term */
    SEARCH_MODE_SEARCHING,        /* Search in progress */
    SEARCH_MODE_RESULTS,          /* Viewing search results */
    SEARCH_MODE_NO_RESULTS        /* No results found */
} search_ui_mode_t;

/*
 * Search UI State Structure
 */
typedef struct {
    search_context_t *search_ctx;              /* Search engine context (owned) */
    search_ui_mode_t mode;                     /* Current UI mode */

    /* Predefined search terms (for Phase 5 - before keyboard support) */
    char **predefined_terms;                   /* Array of search terms */
    int predefined_count;                      /* Number of predefined terms */
    int selected_term;                         /* Currently selected term index */

    /* UI state */
    bool case_sensitive;                       /* Case-sensitive search toggle */
    bool needs_redraw;                         /* Redraw flag */
} search_ui_state_t;

/*
 * Search UI Action Result
 */
typedef enum {
    SEARCH_UI_ACTION_NONE,          /* No action */
    SEARCH_UI_ACTION_REDRAW,        /* UI needs redraw */
    SEARCH_UI_ACTION_NAVIGATE,      /* Navigate to result page */
    SEARCH_UI_ACTION_EXIT           /* Exit search UI */
} search_ui_action_t;

/*
 * Search UI Initialization and Cleanup
 */

/**
 * Create a new search UI state
 *
 * @param text: Text to search (must remain valid during search UI lifetime)
 * @param text_length: Length of text
 * @param pagination: Pagination context (must remain valid)
 * @param predefined_terms: Array of predefined search terms (copied internally)
 * @param term_count: Number of predefined terms
 * @return: Pointer to search UI state, or NULL on error
 */
search_ui_state_t* search_ui_create(const char *text, size_t text_length,
                                    pagination_t *pagination,
                                    const char **predefined_terms, int term_count);

/**
 * Free search UI state and associated resources
 *
 * @param ui: Search UI state to free (may be NULL)
 */
void search_ui_free(search_ui_state_t *ui);

/*
 * Search UI Rendering
 */

/**
 * Render the search UI to framebuffer
 *
 * @param ui: Search UI state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int search_ui_render(search_ui_state_t *ui, framebuffer_t *fb);

/**
 * Render term selection mode
 *
 * @param ui: Search UI state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int search_ui_render_term_selection(search_ui_state_t *ui, framebuffer_t *fb);

/**
 * Render search results mode
 *
 * @param ui: Search UI state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int search_ui_render_results(search_ui_state_t *ui, framebuffer_t *fb);

/**
 * Render "no results" message
 *
 * @param ui: Search UI state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int search_ui_render_no_results(search_ui_state_t *ui, framebuffer_t *fb);

/**
 * Render searching/loading message
 *
 * @param ui: Search UI state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int search_ui_render_searching(search_ui_state_t *ui, framebuffer_t *fb);

/*
 * Search UI Input Handling
 */

/**
 * Handle button event
 *
 * @param ui: Search UI state
 * @param event: Button event
 * @return: Search UI action indicating what happened
 */
search_ui_action_t search_ui_handle_event(search_ui_state_t *ui, const button_event_t *event);

/*
 * Search UI State Queries
 */

/**
 * Get the page number to navigate to (if action is NAVIGATE)
 *
 * @param ui: Search UI state
 * @return: Page number (0-based), or -1 if not applicable
 */
int search_ui_get_target_page(search_ui_state_t *ui);

/**
 * Check if UI needs redraw
 *
 * @param ui: Search UI state
 * @return: true if redraw needed, false otherwise
 */
bool search_ui_needs_redraw(search_ui_state_t *ui);

/**
 * Clear the needs_redraw flag
 *
 * @param ui: Search UI state
 */
void search_ui_clear_redraw_flag(search_ui_state_t *ui);

#endif /* SEARCH_UI_H */
