/*
 * wifi_menu.h - WiFi Settings Menu UI
 *
 * Provides a scrollable WiFi settings menu UI for network management.
 * Includes network scanning, connection management, password entry, and status display.
 *
 * Phase 06: WiFi and Network Features
 */

#ifndef WIFI_MENU_H
#define WIFI_MENU_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../network/wifi_manager.h"
#include "../../button-test/button_input.h"

/*
 * WiFi Menu Configuration Constants
 */
#define WIFI_MENU_TITLE         "WiFi Settings"
#define WIFI_MENU_HINTS_MAIN    "UP/DOWN:Navigate  SELECT:Choose  BACK:Exit"
#define WIFI_MENU_HINTS_LIST    "UP/DOWN:Select  SELECT:Connect  MENU:Scan  BACK:Return"
#define WIFI_MENU_HINTS_SCAN    "BACK:Cancel"
#define WIFI_MENU_HINTS_CONNECT "UP/DOWN:Select  SELECT:Confirm  BACK:Cancel"
#define WIFI_MENU_HINTS_RESULT  "SELECT:Continue  BACK:Return"

/* Layout Constants (based on 400x300 display, 8x16 font) */
#define WIFI_STATUS_BAR_LINE    0       /* Line 0: Status bar with title */
#define WIFI_SEPARATOR_1_LINE   1       /* Line 1: Separator */
#define WIFI_CONTENT_START_LINE 2       /* Line 2: Content start */
#define WIFI_CONTENT_END_LINE   15      /* Line 15: Content end */
#define WIFI_SEPARATOR_2_LINE   16      /* Line 16: Separator */
#define WIFI_HINTS_LINE         17      /* Line 17: Control hints */

#define WIFI_VISIBLE_ITEMS      11      /* Number of items visible in list (lines 4-14) */
#define WIFI_VISIBLE_NETWORKS   11      /* Number of networks visible at once */

/* Selection marker */
#define WIFI_SELECTION_MARKER   "> "
#define WIFI_SELECTION_MARKER_LEN 2

/* Signal strength bar characters (8 levels) */
#define WIFI_SIGNAL_CHAR_FULL   0xE2    /* █ Full block */
#define WIFI_SIGNAL_CHAR_EMPTY  0xE2    /* ░ Light shade */
#define WIFI_SIGNAL_BARS        8       /* 8 bars for signal strength */

/* WiFi menu states - extends the application state machine */
typedef enum {
    WIFI_STATE_MAIN = 0,        /* Main WiFi menu (status overview) */
    WIFI_STATE_SCAN,            /* Scanning for networks (progress) */
    WIFI_STATE_LIST,            /* Network list display */
    WIFI_STATE_CONNECT,         /* Connection dialog */
    WIFI_STATE_PASSWORD,        /* Password entry using text input */
    WIFI_STATE_CONNECTING,      /* Connecting progress */
    WIFI_STATE_RESULT,          /* Connection result (success/error) */
    WIFI_STATE_SAVED_NETWORKS,  /* Saved networks management */
    WIFI_STATE_COUNT            /* Total number of WiFi states */
} wifi_menu_state_enum_t;

/* Main menu options */
typedef enum {
    WIFI_MAIN_OPTION_SCAN = 0,
    WIFI_MAIN_OPTION_DISCONNECT,
    WIFI_MAIN_OPTION_SAVED_NETWORKS,
    WIFI_MAIN_OPTION_BACK,
    WIFI_MAIN_OPTION_COUNT
} wifi_main_option_t;

/* Main menu options when disconnected */
typedef enum {
    WIFI_MAIN_OPTION_SCAN_DISCONNECTED = 0,
    WIFI_MAIN_OPTION_SAVED_NETWORKS_DISCONNECTED,
    WIFI_MAIN_OPTION_BACK_DISCONNECTED,
    WIFI_MAIN_OPTION_COUNT_DISCONNECTED
} wifi_main_option_disconnected_t;

/* Connect dialog options */
typedef enum {
    WIFI_CONNECT_OPTION_CONNECT = 0,
    WIFI_CONNECT_OPTION_ENTER_PASSWORD,
    WIFI_CONNECT_OPTION_CANCEL,
    WIFI_CONNECT_OPTION_COUNT
} wifi_connect_option_t;

/* Error codes */
typedef enum {
    WIFI_MENU_SUCCESS = 0,
    WIFI_MENU_ERROR_NULL_POINTER = -1,
    WIFI_MENU_ERROR_INVALID_STATE = -2,
    WIFI_MENU_ERROR_RENDER_FAILED = -3,
    WIFI_MENU_ERROR_SCAN_FAILED = -4,
    WIFI_MENU_ERROR_CONNECT_FAILED = -5,
    WIFI_MENU_ERROR_WIFI_MANAGER = -6
} wifi_menu_error_t;

/* WiFi menu action result - what happened after handling an event */
typedef enum {
    WIFI_MENU_ACTION_NONE,          /* No action taken */
    WIFI_MENU_ACTION_REDRAW,        /* Menu needs to be redrawn */
    WIFI_MENU_ACTION_STATE_CHANGE,  /* State changed */
    WIFI_MENU_ACTION_EXIT           /* User wants to exit WiFi menu */
} wifi_menu_action_t;

/*
 * WiFi Menu State Structure
 *
 * Maintains current menu state including WiFi status, networks, and UI state
 */
typedef struct {
    /* WiFi manager state */
    wifi_status_t wifi_status;          /* Current WiFi connection status */
    wifi_network_t networks[MAX_NETWORKS]; /* Scanned networks */
    int network_count;                  /* Number of networks found */
    wifi_network_t saved_networks[MAX_NETWORKS]; /* Saved networks */
    int saved_network_count;            /* Number of saved networks */

    /* UI state */
    wifi_menu_state_enum_t state;       /* Current WiFi menu state */
    wifi_menu_state_enum_t previous_state; /* Previous state for back navigation */

    /* Navigation state */
    int selected_index;                 /* Currently highlighted item (0-based) */
    int scroll_offset;                  /* First visible item index (0-based) */

    /* Selected network for connection */
    int selected_network_index;         /* Index of network to connect to */
    char connection_password[MAX_PASSWORD_LENGTH + 1]; /* Password buffer */

    /* Text input for password entry */
    struct text_input_state *text_input; /* Text input state (NULL when not in use) */

    /* Flags */
    bool needs_redraw;                  /* Flag indicating full redraw is needed */
    bool scan_in_progress;              /* Scan operation in progress */
    bool connect_in_progress;           /* Connection operation in progress */

    /* Connection result */
    bool connection_success;            /* Last connection attempt succeeded */
    char result_message[128];           /* Result message to display */

    /* Refresh counter */
    int refresh_counter;                /* Counter for forcing full refresh */
} wifi_menu_state_t;

/*
 * WiFi Menu Initialization and Cleanup
 */

/**
 * Create and initialize a new WiFi menu state
 *
 * @return: Pointer to WiFi menu state, or NULL on error
 */
wifi_menu_state_t* wifi_menu_create(void);

/**
 * Free WiFi menu state and associated resources
 *
 * @param menu: WiFi menu state to free (may be NULL)
 */
void wifi_menu_free(wifi_menu_state_t *menu);

/**
 * Reset menu state to initial position
 *
 * @param menu: WiFi menu state to reset
 */
void wifi_menu_reset(wifi_menu_state_t *menu);

/**
 * Initialize WiFi manager (should be called once at app startup)
 *
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_init_manager(void);

/**
 * Cleanup WiFi manager (should be called at app shutdown)
 */
void wifi_menu_cleanup_manager(void);

/*
 * WiFi Menu Rendering
 */

/**
 * Render the complete WiFi menu to framebuffer
 *
 * Dispatches to appropriate rendering function based on current state.
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the main WiFi menu (status overview)
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_main(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the network scan progress screen
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_scan(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the network list
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_list(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the connection dialog
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_connect(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the connecting progress screen
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_connecting(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the connection result screen
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_result(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the saved networks list
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_saved_networks(wifi_menu_state_t *menu, framebuffer_t *fb);

/**
 * Render the password entry screen
 *
 * @param menu: WiFi menu state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_render_password(wifi_menu_state_t *menu, framebuffer_t *fb);

/*
 * WiFi Menu Navigation and Input Handling
 */

/**
 * Handle button event and update menu state
 *
 * Dispatches to appropriate handler based on current state.
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action indicating what happened
 */
wifi_menu_action_t wifi_menu_handle_event(wifi_menu_state_t *menu, const button_event_t *event);

/**
 * Handle button event in main menu state
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action
 */
wifi_menu_action_t wifi_menu_handle_main(wifi_menu_state_t *menu, const button_event_t *event);

/**
 * Handle button event in network list state
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action
 */
wifi_menu_action_t wifi_menu_handle_list(wifi_menu_state_t *menu, const button_event_t *event);

/**
 * Handle button event in connect dialog state
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action
 */
wifi_menu_action_t wifi_menu_handle_connect(wifi_menu_state_t *menu, const button_event_t *event);

/**
 * Handle button event in result screen state
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action
 */
wifi_menu_action_t wifi_menu_handle_result(wifi_menu_state_t *menu, const button_event_t *event);

/**
 * Handle button event in password entry state
 *
 * @param menu: WiFi menu state
 * @param event: Button event to handle
 * @return: WiFi menu action
 */
wifi_menu_action_t wifi_menu_handle_password(wifi_menu_state_t *menu, const button_event_t *event);

/*
 * WiFi Operations
 */

/**
 * Start WiFi network scan
 *
 * Initiates a network scan and updates menu state.
 * This is a blocking operation (3-10 seconds).
 *
 * @param menu: WiFi menu state
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_start_scan(wifi_menu_state_t *menu);

/**
 * Connect to selected network
 *
 * Initiates connection to the selected network.
 * This is a blocking operation (5-30 seconds).
 *
 * @param menu: WiFi menu state
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_connect_network(wifi_menu_state_t *menu);

/**
 * Disconnect from current network
 *
 * @param menu: WiFi menu state
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_disconnect(wifi_menu_state_t *menu);

/**
 * Update WiFi status
 *
 * Refreshes the current WiFi connection status from the manager.
 *
 * @param menu: WiFi menu state
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_update_status(wifi_menu_state_t *menu);

/**
 * Change WiFi menu state
 *
 * Transitions to a new state and performs necessary setup/cleanup.
 *
 * @param menu: WiFi menu state
 * @param new_state: Target state
 * @return: 0 on success, negative error code on failure
 */
int wifi_menu_change_state(wifi_menu_state_t *menu, wifi_menu_state_enum_t new_state);

/*
 * WiFi Menu State Queries
 */

/**
 * Check if menu needs to be redrawn
 *
 * @param menu: WiFi menu state
 * @return: true if redraw needed, false otherwise
 */
bool wifi_menu_needs_redraw(wifi_menu_state_t *menu);

/**
 * Clear the needs_redraw flag
 *
 * @param menu: WiFi menu state
 */
void wifi_menu_clear_redraw_flag(wifi_menu_state_t *menu);

/**
 * Get current WiFi menu state
 *
 * @param menu: WiFi menu state
 * @return: Current state
 */
wifi_menu_state_enum_t wifi_menu_get_state(wifi_menu_state_t *menu);

/**
 * Check if WiFi is currently connected
 *
 * @param menu: WiFi menu state
 * @return: true if connected, false otherwise
 */
bool wifi_menu_is_connected(wifi_menu_state_t *menu);

/*
 * Utility Functions
 */

/**
 * Format signal strength as bar string
 *
 * Creates a string like "████████" or "████░░░░" based on signal strength.
 *
 * @param signal_dbm: Signal strength in dBm
 * @param buffer: Output buffer (must be at least WIFI_SIGNAL_BARS + 1 bytes)
 */
void wifi_menu_format_signal_bars(int signal_dbm, char *buffer);

/**
 * Get WiFi menu state name
 *
 * @param state: WiFi menu state
 * @return: String representation of state
 */
const char* wifi_menu_state_to_string(wifi_menu_state_enum_t state);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* wifi_menu_error_string(wifi_menu_error_t error);

#endif /* WIFI_MENU_H */
