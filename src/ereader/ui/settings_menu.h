/*
 * settings_menu.h - E-Reader Settings Menu UI
 *
 * Provides a scrollable settings menu UI for configuring e-reader preferences.
 * Settings include font size, line spacing, margins, display mode, and auto-sleep.
 *
 * Phase 05: Polish and Advanced Features
 */

#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../settings/settings_manager.h"
#include "../../button-test/button_input.h"

/*
 * Settings Menu Configuration Constants
 */
#define SETTINGS_MENU_TITLE         "Settings"
#define SETTINGS_MENU_HINTS         "UP/DOWN:Navigate  SELECT:Change  BACK:Save&Exit"

/* Layout Constants (based on 400x300 display, 8x16 font) */
#define SETTINGS_STATUS_BAR_LINE    0       /* Line 0: Status bar with title */
#define SETTINGS_SEPARATOR_1_LINE   1       /* Line 1: Separator */
#define SETTINGS_FIRST_ITEM_LINE    2       /* Line 2: First setting item */
#define SETTINGS_LAST_ITEM_LINE     15      /* Line 15: Last setting item */
#define SETTINGS_SEPARATOR_2_LINE   16      /* Line 16: Separator */
#define SETTINGS_HINTS_LINE         17      /* Line 17: Control hints */

#define SETTINGS_VISIBLE_ITEMS      14      /* Number of items visible at once (lines 2-15) */

/* Selection marker */
#define SETTINGS_SELECTION_MARKER   "> "
#define SETTINGS_SELECTION_MARKER_LEN 2

/* Setting item IDs */
typedef enum {
    SETTING_ITEM_FONT_SIZE = 0,
    SETTING_ITEM_LINE_SPACING,
    SETTING_ITEM_MARGINS,
    SETTING_ITEM_DISPLAY_MODE,
    SETTING_ITEM_AUTO_SLEEP,
    SETTING_ITEM_WIFI,          /* WiFi settings */
    SETTING_ITEM_COUNT          /* Total number of settings */
} setting_item_t;

/* Error codes */
typedef enum {
    SETTINGS_MENU_SUCCESS = 0,
    SETTINGS_MENU_ERROR_NULL_POINTER = -1,
    SETTINGS_MENU_ERROR_INVALID_STATE = -2,
    SETTINGS_MENU_ERROR_RENDER_FAILED = -3,
    SETTINGS_MENU_ERROR_SAVE_FAILED = -4
} settings_menu_error_t;

/* Settings menu action result - what happened after handling an event */
typedef enum {
    SETTINGS_MENU_ACTION_NONE,          /* No action taken */
    SETTINGS_MENU_ACTION_REDRAW,        /* Menu needs to be redrawn */
    SETTINGS_MENU_ACTION_VALUE_CHANGED, /* A setting value was changed */
    SETTINGS_MENU_ACTION_SAVE_EXIT      /* User wants to save and exit */
} settings_menu_action_t;

/*
 * Settings Menu State Structure
 *
 * Maintains current menu state including settings, selection, and scroll position
 */
typedef struct {
    settings_t *settings;           /* Pointer to settings (not owned by menu) */

    int selected_index;             /* Currently highlighted setting (0-based) */
    int scroll_offset;              /* First visible item index (0-based) */
    int visible_items;              /* Number of items that fit on screen (14) */
    int total_items;                /* Total number of settings (SETTING_ITEM_COUNT) */

    bool needs_redraw;              /* Flag indicating full redraw is needed */
    bool settings_changed;          /* Flag indicating settings were modified */
    int refresh_counter;            /* Counter for forcing full refresh (prevent ghosting) */
} settings_menu_state_t;

/*
 * Settings Menu Initialization and Cleanup
 */

/**
 * Create and initialize a new settings menu state
 *
 * @param settings: Pointer to settings structure (must remain valid during menu lifetime)
 * @return: Pointer to settings menu state, or NULL on error
 */
settings_menu_state_t* settings_menu_create(settings_t *settings);

/**
 * Free settings menu state and associated resources
 *
 * @param menu: Settings menu state to free (may be NULL)
 */
void settings_menu_free(settings_menu_state_t *menu);

/**
 * Reset menu state to initial position
 *
 * @param menu: Settings menu state to reset
 */
void settings_menu_reset(settings_menu_state_t *menu);

/*
 * Settings Menu Rendering
 */

/**
 * Render the complete settings menu to framebuffer
 *
 * Draws status bar, settings list, value indicators, and control hints.
 *
 * @param menu: Settings menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int settings_menu_render(settings_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render just the status bar (title)
 *
 * @param menu: Settings menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int settings_menu_render_status_bar(settings_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the settings items (with selection highlight and current values)
 *
 * @param menu: Settings menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int settings_menu_render_items(settings_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render control hints at bottom of screen
 *
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int settings_menu_render_hints(framebuffer_t *fb);

/*
 * Settings Menu Navigation and Input Handling
 */

/**
 * Handle button event and update menu state
 *
 * @param menu: Settings menu state
 * @param event: Button event to handle
 * @return: Settings menu action indicating what happened
 */
settings_menu_action_t settings_menu_handle_event(settings_menu_state_t *menu, const button_event_t *event);

/**
 * Move selection up one item
 *
 * @param menu: Settings menu state
 * @return: true if selection changed, false otherwise
 */
bool settings_menu_move_up(settings_menu_state_t *menu);

/**
 * Move selection down one item
 *
 * @param menu: Settings menu state
 * @return: true if selection changed, false otherwise
 */
bool settings_menu_move_down(settings_menu_state_t *menu);

/**
 * Cycle the currently selected setting to its next value
 *
 * @param menu: Settings menu state
 * @return: true if value changed, false otherwise
 */
bool settings_menu_cycle_value(settings_menu_state_t *menu);

/**
 * Get currently selected setting item
 *
 * @param menu: Settings menu state
 * @return: Currently selected setting item ID
 */
setting_item_t settings_menu_get_selected_item(settings_menu_state_t *menu);

/*
 * Settings Menu State Queries
 */

/**
 * Check if menu needs to be redrawn
 *
 * @param menu: Settings menu state
 * @return: true if redraw needed, false otherwise
 */
bool settings_menu_needs_redraw(settings_menu_state_t *menu);

/**
 * Clear the needs_redraw flag
 *
 * @param menu: Settings menu state
 */
void settings_menu_clear_redraw_flag(settings_menu_state_t *menu);

/**
 * Check if any settings were changed
 *
 * @param menu: Settings menu state
 * @return: true if settings were modified, false otherwise
 */
bool settings_menu_settings_changed(settings_menu_state_t *menu);

/**
 * Save settings to persistent storage
 *
 * @param menu: Settings menu state
 * @return: 0 on success, negative error code on failure
 */
int settings_menu_save_settings(settings_menu_state_t *menu);

/*
 * Utility Functions
 */

/**
 * Get setting name string
 *
 * @param item: Setting item ID
 * @return: Human-readable setting name
 */
const char* settings_menu_get_setting_name(setting_item_t item);

/**
 * Get current value string for a setting
 *
 * @param menu: Settings menu state
 * @param item: Setting item ID
 * @param buffer: Output buffer
 * @param buffer_size: Size of output buffer
 */
void settings_menu_get_value_string(settings_menu_state_t *menu, setting_item_t item,
                                    char *buffer, size_t buffer_size);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* settings_menu_error_string(settings_menu_error_t error);

#endif /* SETTINGS_MENU_H */
