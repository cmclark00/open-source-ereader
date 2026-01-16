/*
 * wifi_menu.c - WiFi Settings Menu UI Implementation
 *
 * Implements the WiFi settings menu UI for network management.
 *
 * Phase 06: WiFi and Network Features
 */

#include "wifi_menu.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal helper function prototypes */
static void wifi_menu_draw_separator_line(framebuffer_t *fb, int line_number);
static void wifi_menu_render_status_bar(framebuffer_t *fb, const char *title);
static void wifi_menu_render_hints(framebuffer_t *fb, const char *hints);

/*
 * WiFi Menu Initialization and Cleanup
 */

wifi_menu_state_t* wifi_menu_create(void) {
    wifi_menu_state_t *menu = (wifi_menu_state_t*)calloc(1, sizeof(wifi_menu_state_t));
    if (!menu) {
        return NULL;
    }

    menu->state = WIFI_STATE_MAIN;
    menu->previous_state = WIFI_STATE_MAIN;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->network_count = 0;
    menu->saved_network_count = 0;
    menu->selected_network_index = -1;
    menu->needs_redraw = true;
    menu->scan_in_progress = false;
    menu->connect_in_progress = false;
    menu->connection_success = false;
    menu->refresh_counter = 0;
    menu->connection_password[0] = '\0';
    menu->result_message[0] = '\0';

    /* Initialize WiFi status */
    menu->wifi_status.state = WIFI_STATE_DISCONNECTED;
    menu->wifi_status.connected_ssid[0] = '\0';
    menu->wifi_status.ip_address[0] = '\0';
    menu->wifi_status.signal_strength = -100;
    menu->wifi_status.last_error = WIFI_ERROR_NONE;
    menu->wifi_status.error_message[0] = '\0';

    /* Update initial status */
    wifi_menu_update_status(menu);

    return menu;
}

void wifi_menu_free(wifi_menu_state_t *menu) {
    if (menu) {
        free(menu);
    }
}

void wifi_menu_reset(wifi_menu_state_t *menu) {
    if (!menu) {
        return;
    }

    menu->state = WIFI_STATE_MAIN;
    menu->previous_state = WIFI_STATE_MAIN;
    menu->selected_index = 0;
    menu->scroll_offset = 0;
    menu->selected_network_index = -1;
    menu->needs_redraw = true;
    menu->scan_in_progress = false;
    menu->connect_in_progress = false;
    menu->connection_password[0] = '\0';

    /* Update status */
    wifi_menu_update_status(menu);
}

int wifi_menu_init_manager(void) {
    return wifi_manager_init();
}

void wifi_menu_cleanup_manager(void) {
    wifi_manager_cleanup();
}

/*
 * WiFi Menu Rendering
 */

int wifi_menu_render(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Dispatch to appropriate rendering function based on state */
    int result = WIFI_MENU_SUCCESS;
    switch (menu->state) {
        case WIFI_STATE_MAIN:
            result = wifi_menu_render_main(menu, fb);
            break;
        case WIFI_STATE_SCAN:
            result = wifi_menu_render_scan(menu, fb);
            break;
        case WIFI_STATE_LIST:
            result = wifi_menu_render_list(menu, fb);
            break;
        case WIFI_STATE_CONNECT:
            result = wifi_menu_render_connect(menu, fb);
            break;
        case WIFI_STATE_CONNECTING:
            result = wifi_menu_render_connecting(menu, fb);
            break;
        case WIFI_STATE_RESULT:
            result = wifi_menu_render_result(menu, fb);
            break;
        case WIFI_STATE_SAVED_NETWORKS:
            result = wifi_menu_render_saved_networks(menu, fb);
            break;
        default:
            result = WIFI_MENU_ERROR_INVALID_STATE;
            break;
    }

    if (result == WIFI_MENU_SUCCESS) {
        menu->needs_redraw = false;
        menu->refresh_counter++;
    }

    return result;
}

int wifi_menu_render_main(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Render title */
    wifi_menu_render_status_bar(fb, WIFI_MENU_TITLE);
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render WiFi status */
    int line = WIFI_CONTENT_START_LINE;

    char status_line[64];
    if (menu->wifi_status.state == WIFI_STATE_CONNECTED) {
        snprintf(status_line, sizeof(status_line), "Status: Connected");
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        /* Network name */
        snprintf(status_line, sizeof(status_line), "  Network: %s", menu->wifi_status.connected_ssid);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        /* Signal strength */
        char signal_bars[WIFI_SIGNAL_BARS + 1];
        wifi_menu_format_signal_bars(menu->wifi_status.signal_strength, signal_bars);
        int percentage = wifi_signal_to_percentage(menu->wifi_status.signal_strength);
        snprintf(status_line, sizeof(status_line), "  Signal: %s (%d%%)", signal_bars, percentage);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        /* IP address */
        snprintf(status_line, sizeof(status_line), "  IP: %s", menu->wifi_status.ip_address);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        line++; /* Empty line */

        /* Menu options - connected */
        int option = 0;
        const char *marker;

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sScan for Networks", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sDisconnect", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sSaved Networks", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sBack", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);
    } else {
        /* Disconnected */
        snprintf(status_line, sizeof(status_line), "Status: Disconnected");
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        line += 2; /* Empty lines */

        /* Menu options - disconnected */
        int option = 0;
        const char *marker;

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sScan for Networks", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sSaved Networks", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == option++) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(status_line, sizeof(status_line), "%sBack", marker);
        text_render_string(fb, status_line, 0, line++, COLOR_BLACK);
    }

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, WIFI_MENU_HINTS_MAIN);

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_scan(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Render title */
    wifi_menu_render_status_bar(fb, "Scanning for Networks");
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render scanning message */
    int line = WIFI_CONTENT_START_LINE + 4;
    text_render_string(fb, "         Scanning...", 0, line++, COLOR_BLACK);
    line++;
    text_render_string(fb, "  Please wait, this may take", 0, line++, COLOR_BLACK);
    text_render_string(fb, "      up to 10 seconds.", 0, line++, COLOR_BLACK);

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, WIFI_MENU_HINTS_SCAN);

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_list(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Render title with page indicator */
    char title[64];
    int total_pages = (menu->network_count + WIFI_VISIBLE_NETWORKS - 1) / WIFI_VISIBLE_NETWORKS;
    int current_page = (menu->selected_index / WIFI_VISIBLE_NETWORKS) + 1;

    if (total_pages > 1) {
        snprintf(title, sizeof(title), "Available Networks [%d/%d]", current_page, total_pages);
    } else {
        snprintf(title, sizeof(title), "Available Networks");
    }
    wifi_menu_render_status_bar(fb, title);
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render network list */
    int line = WIFI_CONTENT_START_LINE;

    if (menu->network_count == 0) {
        /* No networks found */
        line += 4;
        text_render_string(fb, "  No networks found.", 0, line++, COLOR_BLACK);
        line++;
        text_render_string(fb, "  Press MENU to scan again.", 0, line++, COLOR_BLACK);
    } else {
        /* Render visible networks */
        for (int i = 0; i < WIFI_VISIBLE_NETWORKS && (menu->scroll_offset + i) < menu->network_count; i++) {
            int net_index = menu->scroll_offset + i;
            wifi_network_t *net = &menu->networks[net_index];

            char net_line[64];
            char signal_bars[WIFI_SIGNAL_BARS + 1];
            wifi_menu_format_signal_bars(net->signal_strength, signal_bars);

            const char *marker = (net_index == menu->selected_index) ? WIFI_SELECTION_MARKER : "  ";
            const char *security = (net->security != WIFI_SECURITY_OPEN) ? " *" : "";

            /* Truncate SSID if too long */
            char ssid_display[24];
            if (strlen(net->ssid) > 20) {
                snprintf(ssid_display, sizeof(ssid_display), "%.17s...", net->ssid);
            } else {
                snprintf(ssid_display, sizeof(ssid_display), "%s", net->ssid);
            }

            snprintf(net_line, sizeof(net_line), "%s%-20s %s%s", marker, ssid_display, signal_bars, security);
            text_render_string(fb, net_line, 0, line++, COLOR_BLACK);
        }
    }

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, WIFI_MENU_HINTS_LIST);

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_connect(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Get selected network */
    if (menu->selected_network_index < 0 || menu->selected_network_index >= menu->network_count) {
        return WIFI_MENU_ERROR_INVALID_STATE;
    }
    wifi_network_t *net = &menu->networks[menu->selected_network_index];

    /* Render title */
    wifi_menu_render_status_bar(fb, "Connect to Network");
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render network info */
    int line = WIFI_CONTENT_START_LINE;
    char info_line[64];

    snprintf(info_line, sizeof(info_line), "Network: %s", net->ssid);
    text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

    snprintf(info_line, sizeof(info_line), "Security: %s", wifi_security_to_string(net->security));
    text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

    char signal_bars[WIFI_SIGNAL_BARS + 1];
    wifi_menu_format_signal_bars(net->signal_strength, signal_bars);
    int percentage = wifi_signal_to_percentage(net->signal_strength);
    snprintf(info_line, sizeof(info_line), "Signal: %s (%d%%)", signal_bars, percentage);
    text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

    line++; /* Empty line */

    /* Check if network is saved or open */
    if (net->security == WIFI_SECURITY_OPEN) {
        /* Open network */
        text_render_string(fb, "This is an open network. Your", 0, line++, COLOR_BLACK);
        text_render_string(fb, "connection will not be encrypted.", 0, line++, COLOR_BLACK);
        line++;
        text_render_string(fb, "Connect to this network?", 0, line++, COLOR_BLACK);
        line++;

        const char *marker;
        marker = (menu->selected_index == WIFI_CONNECT_OPTION_CONNECT) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sYes, Connect", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == WIFI_CONNECT_OPTION_CANCEL) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sCancel", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
    } else if (net->is_saved) {
        /* Saved network */
        text_render_string(fb, "Saved password found.", 0, line++, COLOR_BLACK);
        line++;
        text_render_string(fb, "Connect using saved password?", 0, line++, COLOR_BLACK);
        line++;

        const char *marker;
        marker = (menu->selected_index == WIFI_CONNECT_OPTION_CONNECT) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sYes, Connect", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == WIFI_CONNECT_OPTION_ENTER_PASSWORD) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sEnter New Password", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == WIFI_CONNECT_OPTION_CANCEL) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sCancel", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
    } else {
        /* Secured network, no saved password */
        text_render_string(fb, "This network requires a password.", 0, line++, COLOR_BLACK);
        line++;
        text_render_string(fb, "NOTE: Password entry via SD card", 0, line++, COLOR_BLACK);
        text_render_string(fb, "is recommended (see User Guide).", 0, line++, COLOR_BLACK);
        line++;

        const char *marker;
        marker = (menu->selected_index == WIFI_CONNECT_OPTION_ENTER_PASSWORD) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sEnter Password (TODO)", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == WIFI_CONNECT_OPTION_CANCEL) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(info_line, sizeof(info_line), "%sCancel", marker);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
    }

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, WIFI_MENU_HINTS_CONNECT);

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_connecting(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Get selected network */
    if (menu->selected_network_index < 0 || menu->selected_network_index >= menu->network_count) {
        return WIFI_MENU_ERROR_INVALID_STATE;
    }
    wifi_network_t *net = &menu->networks[menu->selected_network_index];

    /* Render title */
    wifi_menu_render_status_bar(fb, "Connecting to Network");
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render connection info */
    int line = WIFI_CONTENT_START_LINE + 2;
    char info_line[64];

    snprintf(info_line, sizeof(info_line), "  Network: %s", net->ssid);
    text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

    snprintf(info_line, sizeof(info_line), "  Security: %s", wifi_security_to_string(net->security));
    text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
    line += 2;

    text_render_string(fb, "        Connecting...", 0, line++, COLOR_BLACK);
    line++;
    text_render_string(fb, " This may take up to 30 seconds.", 0, line++, COLOR_BLACK);

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, "BACK:Cancel");

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_result(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Render title */
    const char *title = menu->connection_success ? "Connection Successful" : "Connection Failed";
    wifi_menu_render_status_bar(fb, title);
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render result message */
    int line = WIFI_CONTENT_START_LINE + 2;

    if (menu->connection_success) {
        /* Success */
        char info_line[64];

        snprintf(info_line, sizeof(info_line), "  Connected to %s", menu->wifi_status.connected_ssid);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
        line++;

        snprintf(info_line, sizeof(info_line), "  IP Address: %s", menu->wifi_status.ip_address);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);

        char signal_bars[WIFI_SIGNAL_BARS + 1];
        wifi_menu_format_signal_bars(menu->wifi_status.signal_strength, signal_bars);
        int percentage = wifi_signal_to_percentage(menu->wifi_status.signal_strength);
        snprintf(info_line, sizeof(info_line), "  Signal: %s (%d%%)", signal_bars, percentage);
        text_render_string(fb, info_line, 0, line++, COLOR_BLACK);
        line += 2;

        text_render_string(fb, " You can now download books from", 0, line++, COLOR_BLACK);
        text_render_string(fb, "     the online library.", 0, line++, COLOR_BLACK);
    } else {
        /* Error */
        text_render_string(fb, "  Could not connect", 0, line++, COLOR_BLACK);
        line++;

        if (strlen(menu->result_message) > 0) {
            /* Display error message (may span multiple lines) */
            char *msg = menu->result_message;
            char line_buf[50];
            int pos = 0;

            while (*msg && line < WIFI_CONTENT_END_LINE - 1) {
                /* Copy up to 45 characters per line */
                int i = 0;
                while (*msg && i < 45 && *msg != '\n') {
                    line_buf[i++] = *msg++;
                }
                line_buf[i] = '\0';

                if (*msg == '\n') msg++; /* Skip newline */

                text_render_string(fb, line_buf, 2, line++, COLOR_BLACK);
            }
        }

        line++;
        const char *marker = (menu->selected_index == 0) ? WIFI_SELECTION_MARKER : "  ";
        char option_line[64];
        snprintf(option_line, sizeof(option_line), "%sTry Again", marker);
        text_render_string(fb, option_line, 0, line++, COLOR_BLACK);

        marker = (menu->selected_index == 1) ? WIFI_SELECTION_MARKER : "  ";
        snprintf(option_line, sizeof(option_line), "%sReturn to Network List", marker);
        text_render_string(fb, option_line, 0, line++, COLOR_BLACK);
    }

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, WIFI_MENU_HINTS_RESULT);

    return WIFI_MENU_SUCCESS;
}

int wifi_menu_render_saved_networks(wifi_menu_state_t *menu, framebuffer_t *fb) {
    if (!menu || !fb) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    /* Render title */
    char title[64];
    snprintf(title, sizeof(title), "Saved Networks [%d]", menu->saved_network_count);
    wifi_menu_render_status_bar(fb, title);
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_1_LINE);

    /* Render saved networks list */
    int line = WIFI_CONTENT_START_LINE;

    if (menu->saved_network_count == 0) {
        /* No saved networks */
        line += 4;
        text_render_string(fb, "  No saved networks.", 0, line++, COLOR_BLACK);
    } else {
        /* Render saved networks */
        for (int i = 0; i < menu->saved_network_count && i < WIFI_VISIBLE_NETWORKS; i++) {
            wifi_network_t *net = &menu->saved_networks[i];

            char net_line[64];
            const char *marker = (i == menu->selected_index) ? WIFI_SELECTION_MARKER : "  ";

            /* Check if this is the current network */
            bool is_current = (menu->wifi_status.state == WIFI_STATE_CONNECTED &&
                              strcmp(net->ssid, menu->wifi_status.connected_ssid) == 0);
            const char *active = is_current ? " (active)" : "";

            snprintf(net_line, sizeof(net_line), "%s%-22s%s", marker, net->ssid, active);
            text_render_string(fb, net_line, 0, line++, COLOR_BLACK);
        }
    }

    /* Render hints */
    wifi_menu_draw_separator_line(fb, WIFI_SEPARATOR_2_LINE);
    wifi_menu_render_hints(fb, "SELECT:Options  BACK:Return");

    return WIFI_MENU_SUCCESS;
}

/*
 * WiFi Menu Navigation and Input Handling
 */

wifi_menu_action_t wifi_menu_handle_event(wifi_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return WIFI_MENU_ACTION_NONE;
    }

    /* Only handle PRESS events */
    if (event->type != BUTTON_EVENT_PRESS) {
        return WIFI_MENU_ACTION_NONE;
    }

    /* Dispatch to appropriate handler based on state */
    switch (menu->state) {
        case WIFI_STATE_MAIN:
            return wifi_menu_handle_main(menu, event);
        case WIFI_STATE_SCAN:
            /* Can only cancel during scan */
            if (event->button == BUTTON_BACK) {
                wifi_menu_change_state(menu, WIFI_STATE_MAIN);
                return WIFI_MENU_ACTION_STATE_CHANGE;
            }
            break;
        case WIFI_STATE_LIST:
            return wifi_menu_handle_list(menu, event);
        case WIFI_STATE_CONNECT:
            return wifi_menu_handle_connect(menu, event);
        case WIFI_STATE_CONNECTING:
            /* Can only cancel during connection */
            if (event->button == BUTTON_BACK) {
                /* TODO: Cancel connection attempt */
                wifi_menu_change_state(menu, WIFI_STATE_LIST);
                return WIFI_MENU_ACTION_STATE_CHANGE;
            }
            break;
        case WIFI_STATE_RESULT:
            return wifi_menu_handle_result(menu, event);
        case WIFI_STATE_SAVED_NETWORKS:
            /* TODO: Implement saved networks navigation */
            if (event->button == BUTTON_BACK) {
                wifi_menu_change_state(menu, WIFI_STATE_MAIN);
                return WIFI_MENU_ACTION_STATE_CHANGE;
            }
            break;
        default:
            break;
    }

    return WIFI_MENU_ACTION_NONE;
}

wifi_menu_action_t wifi_menu_handle_main(wifi_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return WIFI_MENU_ACTION_NONE;
    }

    bool is_connected = (menu->wifi_status.state == WIFI_STATE_CONNECTED);
    int max_options = is_connected ? WIFI_MAIN_OPTION_COUNT : WIFI_MAIN_OPTION_COUNT_DISCONNECTED;

    switch (event->button) {
        case BUTTON_UP:
            if (menu->selected_index > 0) {
                menu->selected_index--;
                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_DOWN:
            if (menu->selected_index < max_options - 1) {
                menu->selected_index++;
                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_SELECT:
            if (is_connected) {
                /* Connected menu options */
                switch (menu->selected_index) {
                    case WIFI_MAIN_OPTION_SCAN:
                        wifi_menu_change_state(menu, WIFI_STATE_SCAN);
                        wifi_menu_start_scan(menu);
                        return WIFI_MENU_ACTION_STATE_CHANGE;
                    case WIFI_MAIN_OPTION_DISCONNECT:
                        wifi_menu_disconnect(menu);
                        menu->needs_redraw = true;
                        return WIFI_MENU_ACTION_REDRAW;
                    case WIFI_MAIN_OPTION_SAVED_NETWORKS:
                        wifi_menu_change_state(menu, WIFI_STATE_SAVED_NETWORKS);
                        /* Load saved networks */
                        wifi_manager_list_saved(menu->saved_networks, MAX_NETWORKS, &menu->saved_network_count);
                        return WIFI_MENU_ACTION_STATE_CHANGE;
                    case WIFI_MAIN_OPTION_BACK:
                        return WIFI_MENU_ACTION_EXIT;
                }
            } else {
                /* Disconnected menu options */
                switch (menu->selected_index) {
                    case WIFI_MAIN_OPTION_SCAN_DISCONNECTED:
                        wifi_menu_change_state(menu, WIFI_STATE_SCAN);
                        wifi_menu_start_scan(menu);
                        return WIFI_MENU_ACTION_STATE_CHANGE;
                    case WIFI_MAIN_OPTION_SAVED_NETWORKS_DISCONNECTED:
                        wifi_menu_change_state(menu, WIFI_STATE_SAVED_NETWORKS);
                        /* Load saved networks */
                        wifi_manager_list_saved(menu->saved_networks, MAX_NETWORKS, &menu->saved_network_count);
                        return WIFI_MENU_ACTION_STATE_CHANGE;
                    case WIFI_MAIN_OPTION_BACK_DISCONNECTED:
                        return WIFI_MENU_ACTION_EXIT;
                }
            }
            break;

        case BUTTON_BACK:
        case BUTTON_MENU:
            return WIFI_MENU_ACTION_EXIT;

        default:
            break;
    }

    return WIFI_MENU_ACTION_NONE;
}

wifi_menu_action_t wifi_menu_handle_list(wifi_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return WIFI_MENU_ACTION_NONE;
    }

    switch (event->button) {
        case BUTTON_UP:
            if (menu->selected_index > 0) {
                menu->selected_index--;

                /* Adjust scroll offset if needed */
                if (menu->selected_index < menu->scroll_offset) {
                    menu->scroll_offset = menu->selected_index;
                }

                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_DOWN:
            if (menu->selected_index < menu->network_count - 1) {
                menu->selected_index++;

                /* Adjust scroll offset if needed */
                if (menu->selected_index >= menu->scroll_offset + WIFI_VISIBLE_NETWORKS) {
                    menu->scroll_offset = menu->selected_index - WIFI_VISIBLE_NETWORKS + 1;
                }

                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_SELECT:
            if (menu->network_count > 0) {
                /* Select network and go to connect dialog */
                menu->selected_network_index = menu->selected_index;
                wifi_menu_change_state(menu, WIFI_STATE_CONNECT);

                /* Reset selected index for connect dialog options */
                menu->selected_index = 0;
                return WIFI_MENU_ACTION_STATE_CHANGE;
            }
            break;

        case BUTTON_MENU:
            /* Re-scan */
            wifi_menu_change_state(menu, WIFI_STATE_SCAN);
            wifi_menu_start_scan(menu);
            return WIFI_MENU_ACTION_STATE_CHANGE;

        case BUTTON_BACK:
            wifi_menu_change_state(menu, WIFI_STATE_MAIN);
            return WIFI_MENU_ACTION_STATE_CHANGE;

        default:
            break;
    }

    return WIFI_MENU_ACTION_NONE;
}

wifi_menu_action_t wifi_menu_handle_connect(wifi_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return WIFI_MENU_ACTION_NONE;
    }

    wifi_network_t *net = &menu->networks[menu->selected_network_index];
    int max_options;

    if (net->security == WIFI_SECURITY_OPEN) {
        max_options = 2; /* Connect, Cancel */
    } else if (net->is_saved) {
        max_options = 3; /* Connect, Enter Password, Cancel */
    } else {
        max_options = 2; /* Enter Password, Cancel */
    }

    switch (event->button) {
        case BUTTON_UP:
            if (menu->selected_index > 0) {
                menu->selected_index--;
                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_DOWN:
            if (menu->selected_index < max_options - 1) {
                menu->selected_index++;
                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;
            }
            break;

        case BUTTON_SELECT:
            if (net->security == WIFI_SECURITY_OPEN) {
                /* Open network */
                if (menu->selected_index == 0) {
                    /* Connect */
                    wifi_menu_change_state(menu, WIFI_STATE_CONNECTING);
                    wifi_menu_connect_network(menu);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                } else {
                    /* Cancel */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                }
            } else if (net->is_saved) {
                /* Saved network */
                if (menu->selected_index == WIFI_CONNECT_OPTION_CONNECT) {
                    /* Connect with saved password */
                    wifi_menu_change_state(menu, WIFI_STATE_CONNECTING);
                    wifi_menu_connect_network(menu);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                } else if (menu->selected_index == WIFI_CONNECT_OPTION_ENTER_PASSWORD) {
                    /* TODO: Enter new password */
                    /* For now, just return to list */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                } else {
                    /* Cancel */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                }
            } else {
                /* Secured network without saved password */
                if (menu->selected_index == 0) {
                    /* TODO: Enter password */
                    /* For now, just return to list */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                } else {
                    /* Cancel */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                }
            }
            break;

        case BUTTON_BACK:
            wifi_menu_change_state(menu, WIFI_STATE_LIST);
            return WIFI_MENU_ACTION_STATE_CHANGE;

        default:
            break;
    }

    return WIFI_MENU_ACTION_NONE;
}

wifi_menu_action_t wifi_menu_handle_result(wifi_menu_state_t *menu, const button_event_t *event) {
    if (!menu || !event) {
        return WIFI_MENU_ACTION_NONE;
    }

    if (menu->connection_success) {
        /* Success - any button returns to main */
        if (event->button == BUTTON_SELECT || event->button == BUTTON_BACK) {
            wifi_menu_change_state(menu, WIFI_STATE_MAIN);
            return WIFI_MENU_ACTION_STATE_CHANGE;
        }
    } else {
        /* Error - provide options */
        switch (event->button) {
            case BUTTON_UP:
            case BUTTON_DOWN:
                menu->selected_index = 1 - menu->selected_index; /* Toggle between 0 and 1 */
                menu->needs_redraw = true;
                return WIFI_MENU_ACTION_REDRAW;

            case BUTTON_SELECT:
                if (menu->selected_index == 0) {
                    /* Try Again */
                    wifi_menu_change_state(menu, WIFI_STATE_CONNECT);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                } else {
                    /* Return to Network List */
                    wifi_menu_change_state(menu, WIFI_STATE_LIST);
                    return WIFI_MENU_ACTION_STATE_CHANGE;
                }
                break;

            case BUTTON_BACK:
                wifi_menu_change_state(menu, WIFI_STATE_MAIN);
                return WIFI_MENU_ACTION_STATE_CHANGE;

            default:
                break;
        }
    }

    return WIFI_MENU_ACTION_NONE;
}

/*
 * WiFi Operations
 */

int wifi_menu_start_scan(wifi_menu_state_t *menu) {
    if (!menu) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    menu->scan_in_progress = true;
    menu->network_count = 0;
    menu->selected_index = 0;
    menu->scroll_offset = 0;

    /* Perform scan */
    int result = wifi_manager_scan(menu->networks, MAX_NETWORKS, &menu->network_count);

    menu->scan_in_progress = false;

    if (result == 0) {
        /* Scan successful, transition to network list */
        wifi_menu_change_state(menu, WIFI_STATE_LIST);
        return WIFI_MENU_SUCCESS;
    } else {
        /* Scan failed, return to main */
        wifi_menu_change_state(menu, WIFI_STATE_MAIN);
        return WIFI_MENU_ERROR_SCAN_FAILED;
    }
}

int wifi_menu_connect_network(wifi_menu_state_t *menu) {
    if (!menu) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    if (menu->selected_network_index < 0 || menu->selected_network_index >= menu->network_count) {
        return WIFI_MENU_ERROR_INVALID_STATE;
    }

    wifi_network_t *net = &menu->networks[menu->selected_network_index];
    menu->connect_in_progress = true;

    /* Connect to network */
    const char *password = (net->security == WIFI_SECURITY_OPEN) ? NULL : "";
    int result = wifi_manager_connect(net->ssid, password);

    menu->connect_in_progress = false;

    /* Update status */
    wifi_menu_update_status(menu);

    /* Prepare result message */
    if (result == 0) {
        menu->connection_success = true;
        snprintf(menu->result_message, sizeof(menu->result_message), "Successfully connected to %s", net->ssid);
    } else {
        menu->connection_success = false;

        /* Format error message based on WiFi status */
        if (menu->wifi_status.last_error == WIFI_ERROR_AUTH_FAILED) {
            snprintf(menu->result_message, sizeof(menu->result_message),
                    "Error: Authentication failed\n(incorrect password)");
        } else if (menu->wifi_status.last_error == WIFI_ERROR_NETWORK_NOT_FOUND) {
            snprintf(menu->result_message, sizeof(menu->result_message),
                    "Error: Network not found or\nsignal too weak");
        } else if (menu->wifi_status.last_error == WIFI_ERROR_DHCP_TIMEOUT) {
            snprintf(menu->result_message, sizeof(menu->result_message),
                    "Error: Could not obtain IP\naddress (DHCP timeout)");
        } else {
            snprintf(menu->result_message, sizeof(menu->result_message),
                    "Error: %s", menu->wifi_status.error_message);
        }
    }

    /* Transition to result screen */
    wifi_menu_change_state(menu, WIFI_STATE_RESULT);
    menu->selected_index = 0;

    return (result == 0) ? WIFI_MENU_SUCCESS : WIFI_MENU_ERROR_CONNECT_FAILED;
}

int wifi_menu_disconnect(wifi_menu_state_t *menu) {
    if (!menu) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    int result = wifi_manager_disconnect();

    /* Update status */
    wifi_menu_update_status(menu);

    return (result == 0) ? WIFI_MENU_SUCCESS : WIFI_MENU_ERROR_WIFI_MANAGER;
}

int wifi_menu_update_status(wifi_menu_state_t *menu) {
    if (!menu) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    int result = wifi_manager_get_status(&menu->wifi_status);

    /* Check if networks have saved credentials */
    for (int i = 0; i < menu->network_count; i++) {
        /* Check if this network is in saved networks */
        wifi_network_t saved[MAX_NETWORKS];
        int saved_count;
        if (wifi_manager_list_saved(saved, MAX_NETWORKS, &saved_count) == 0) {
            menu->networks[i].is_saved = false;
            for (int j = 0; j < saved_count; j++) {
                if (strcmp(menu->networks[i].ssid, saved[j].ssid) == 0) {
                    menu->networks[i].is_saved = true;
                    break;
                }
            }
        }
    }

    return (result == 0) ? WIFI_MENU_SUCCESS : WIFI_MENU_ERROR_WIFI_MANAGER;
}

int wifi_menu_change_state(wifi_menu_state_t *menu, wifi_menu_state_enum_t new_state) {
    if (!menu) {
        return WIFI_MENU_ERROR_NULL_POINTER;
    }

    menu->previous_state = menu->state;
    menu->state = new_state;
    menu->needs_redraw = true;
    menu->selected_index = 0;

    return WIFI_MENU_SUCCESS;
}

/*
 * WiFi Menu State Queries
 */

bool wifi_menu_needs_redraw(wifi_menu_state_t *menu) {
    if (!menu) {
        return false;
    }
    return menu->needs_redraw;
}

void wifi_menu_clear_redraw_flag(wifi_menu_state_t *menu) {
    if (menu) {
        menu->needs_redraw = false;
    }
}

wifi_menu_state_enum_t wifi_menu_get_state(wifi_menu_state_t *menu) {
    if (!menu) {
        return WIFI_STATE_MAIN;
    }
    return menu->state;
}

bool wifi_menu_is_connected(wifi_menu_state_t *menu) {
    if (!menu) {
        return false;
    }
    return (menu->wifi_status.state == WIFI_STATE_CONNECTED);
}

/*
 * Utility Functions
 */

void wifi_menu_format_signal_bars(int signal_dbm, char *buffer) {
    if (!buffer) {
        return;
    }

    int bars = wifi_signal_to_bars(signal_dbm);

    for (int i = 0; i < WIFI_SIGNAL_BARS; i++) {
        buffer[i] = (i < bars) ? '#' : '-';
    }
    buffer[WIFI_SIGNAL_BARS] = '\0';
}

const char* wifi_menu_state_to_string(wifi_menu_state_enum_t state) {
    switch (state) {
        case WIFI_STATE_MAIN:           return "MAIN";
        case WIFI_STATE_SCAN:           return "SCAN";
        case WIFI_STATE_LIST:           return "LIST";
        case WIFI_STATE_CONNECT:        return "CONNECT";
        case WIFI_STATE_CONNECTING:     return "CONNECTING";
        case WIFI_STATE_RESULT:         return "RESULT";
        case WIFI_STATE_SAVED_NETWORKS: return "SAVED_NETWORKS";
        default:                        return "UNKNOWN";
    }
}

const char* wifi_menu_error_string(wifi_menu_error_t error) {
    switch (error) {
        case WIFI_MENU_SUCCESS:             return "Success";
        case WIFI_MENU_ERROR_NULL_POINTER:  return "Null pointer error";
        case WIFI_MENU_ERROR_INVALID_STATE: return "Invalid state";
        case WIFI_MENU_ERROR_RENDER_FAILED: return "Render failed";
        case WIFI_MENU_ERROR_SCAN_FAILED:   return "Scan failed";
        case WIFI_MENU_ERROR_CONNECT_FAILED: return "Connection failed";
        case WIFI_MENU_ERROR_WIFI_MANAGER:  return "WiFi manager error";
        default:                            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

static void wifi_menu_draw_separator_line(framebuffer_t *fb, int line_number) {
    if (!fb) {
        return;
    }

    int chars_per_line = fb->width / FONT_WIDTH;
    char separator[64];

    for (int i = 0; i < chars_per_line && i < (int)sizeof(separator) - 1; i++) {
        separator[i] = '-';
    }
    separator[chars_per_line < (int)sizeof(separator) ? chars_per_line : (int)sizeof(separator) - 1] = '\0';

    text_render_string(fb, separator, 0, line_number, COLOR_BLACK);
}

static void wifi_menu_render_status_bar(framebuffer_t *fb, const char *title) {
    if (!fb || !title) {
        return;
    }

    int title_len = strlen(title);
    int x_offset = (fb->width / FONT_WIDTH - title_len) / 2;

    text_render_string(fb, title, x_offset, WIFI_STATUS_BAR_LINE, COLOR_BLACK);
}

static void wifi_menu_render_hints(framebuffer_t *fb, const char *hints) {
    if (!fb || !hints) {
        return;
    }

    int hints_len = strlen(hints);
    int x_offset = (fb->width / FONT_WIDTH - hints_len) / 2;

    text_render_string(fb, hints, x_offset, WIFI_HINTS_LINE, COLOR_BLACK);
}
