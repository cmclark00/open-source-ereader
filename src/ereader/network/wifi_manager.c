/*
 * wifi_manager.c - WiFi Network Management Implementation
 *
 * Implements WiFi scanning, connection, and status management by interfacing
 * with wpa_supplicant, iw, and dhcpcd system utilities.
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

#include "wifi_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/* Configuration paths */
#define WPA_SUPPLICANT_CONF "/etc/wpa_supplicant.conf"
#define WIFI_INTERFACE "wlan0"
#define NETWORK_LOG_FILE "/var/log/network.log"

/* Command timeouts (seconds) */
#define SCAN_TIMEOUT 10
#define CONNECT_TIMEOUT 30
#define DHCP_TIMEOUT 15

/* Buffer sizes */
#define CMD_BUFFER_SIZE 512
#define LINE_BUFFER_SIZE 256
#define OUTPUT_BUFFER_SIZE 8192

/* Global state */
static bool initialized = false;
static wifi_status_t current_status = {
    .state = WIFI_STATE_DISCONNECTED,
    .connected_ssid = "",
    .signal_strength = -100,
    .ip_address = "",
    .last_error = WIFI_ERROR_NONE,
    .error_message = ""
};

/*
 * Helper function: Execute a system command and capture output
 */
static int execute_command(const char *cmd, char *output, int output_size) {
    FILE *fp;
    char buffer[LINE_BUFFER_SIZE];
    int bytes_written = 0;

    if (output) {
        output[0] = '\0';
    }

    fp = popen(cmd, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to execute command: %s\n", cmd);
        return -1;
    }

    /* Read command output */
    if (output) {
        while (fgets(buffer, sizeof(buffer), fp) != NULL && bytes_written < output_size - 1) {
            int len = strlen(buffer);
            int space_left = output_size - bytes_written - 1;
            if (len > space_left) {
                len = space_left;
            }
            memcpy(output + bytes_written, buffer, len);
            bytes_written += len;
        }
        output[bytes_written] = '\0';
    } else {
        /* Discard output */
        while (fgets(buffer, sizeof(buffer), fp) != NULL);
    }

    int status = pclose(fp);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

/*
 * Helper function: Trim whitespace from string
 */
static void trim_whitespace(char *str) {
    if (str == NULL) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }

    /* Trim trailing whitespace */
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    *(end + 1) = '\0';

    /* Move trimmed string to beginning */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/*
 * Helper function: Parse signal strength from iw scan output
 */
static int parse_signal_dbm(const char *signal_str) {
    /* Format: "signal: -65.00 dBm" */
    const char *dbm_pos = strstr(signal_str, "-");
    if (dbm_pos) {
        return atoi(dbm_pos);
    }
    return -100;
}

/*
 * Helper function: Parse security type from iw scan output
 */
static wifi_security_t parse_security(const char *info) {
    if (strstr(info, "WPA2")) {
        return WIFI_SECURITY_WPA2_PSK;
    } else if (strstr(info, "WPA")) {
        return WIFI_SECURITY_WPA_PSK;
    } else if (strstr(info, "WEP")) {
        return WIFI_SECURITY_WEP;
    }
    return WIFI_SECURITY_OPEN;
}

/*
 * Helper function: Check if network is saved in wpa_supplicant.conf
 */
static bool is_network_saved(const char *ssid) {
    FILE *fp = fopen(WPA_SUPPLICANT_CONF, "r");
    if (fp == NULL) {
        return false;
    }

    char line[LINE_BUFFER_SIZE];
    bool found = false;

    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_whitespace(line);
        if (strstr(line, "ssid=") && strstr(line, ssid)) {
            found = true;
            break;
        }
    }

    fclose(fp);
    return found;
}

/*
 * Helper function: Compare networks for sorting (by signal strength)
 */
static int compare_networks(const void *a, const void *b) {
    const wifi_network_t *na = (const wifi_network_t *)a;
    const wifi_network_t *nb = (const wifi_network_t *)b;
    /* Sort descending (strongest first) */
    return nb->signal_strength - na->signal_strength;
}

/*
 * Initialize WiFi manager
 */
int wifi_manager_init(void) {
    if (initialized) {
        return 0;
    }

    /* Check if wlan0 interface exists */
    char output[OUTPUT_BUFFER_SIZE];
    if (execute_command("ip link show " WIFI_INTERFACE " 2>/dev/null", output, sizeof(output)) != 0) {
        fprintf(stderr, "WiFi interface " WIFI_INTERFACE " not found\n");
        return -1;
    }

    /* Bring interface up if down */
    execute_command("ip link set " WIFI_INTERFACE " up 2>/dev/null", NULL, 0);

    initialized = true;

    /* Get initial status */
    wifi_manager_get_status(&current_status);

    return 0;
}

/*
 * Cleanup WiFi manager
 */
void wifi_manager_cleanup(void) {
    initialized = false;
}

/*
 * Scan for available WiFi networks
 */
int wifi_manager_scan(wifi_network_t *networks, int max_networks, int *count) {
    if (!initialized || networks == NULL || count == NULL) {
        return -1;
    }

    *count = 0;

    /* Trigger scan */
    char output[OUTPUT_BUFFER_SIZE];
    if (execute_command("iw dev " WIFI_INTERFACE " scan 2>/dev/null", output, sizeof(output)) != 0) {
        fprintf(stderr, "WiFi scan failed\n");
        current_status.last_error = WIFI_ERROR_SCAN_FAILED;
        return -1;
    }

    /* Parse scan results */
    char *line = strtok(output, "\n");
    wifi_network_t current_network = {0};
    bool parsing_network = false;

    while (line != NULL && *count < max_networks) {
        trim_whitespace(line);

        /* New BSS (network) entry */
        if (strncmp(line, "BSS ", 4) == 0) {
            /* Save previous network if valid */
            if (parsing_network && current_network.ssid[0] != '\0') {
                current_network.is_saved = is_network_saved(current_network.ssid);
                memcpy(&networks[*count], &current_network, sizeof(wifi_network_t));
                (*count)++;
            }

            /* Start new network */
            memset(&current_network, 0, sizeof(wifi_network_t));
            parsing_network = true;

            /* Extract BSSID (MAC address) */
            char *bssid_start = line + 4;
            char *bssid_end = strchr(bssid_start, '(');
            if (bssid_end) {
                int bssid_len = bssid_end - bssid_start;
                if (bssid_len > MAX_BSSID_LENGTH - 1) {
                    bssid_len = MAX_BSSID_LENGTH - 1;
                }
                strncpy(current_network.bssid, bssid_start, bssid_len);
                current_network.bssid[bssid_len] = '\0';
                trim_whitespace(current_network.bssid);
            }

            current_network.signal_strength = -100;
            current_network.security = WIFI_SECURITY_OPEN;
        }
        /* SSID */
        else if (strstr(line, "SSID:") == line) {
            char *ssid_start = line + 5;
            trim_whitespace(ssid_start);
            strncpy(current_network.ssid, ssid_start, MAX_SSID_LENGTH);
            current_network.ssid[MAX_SSID_LENGTH] = '\0';
        }
        /* Signal strength */
        else if (strstr(line, "signal:")) {
            current_network.signal_strength = parse_signal_dbm(line);
        }
        /* Frequency */
        else if (strstr(line, "freq:")) {
            char *freq_start = strstr(line, "freq:") + 5;
            current_network.frequency = atoi(freq_start);
        }
        /* Security */
        else if (strstr(line, "WPA") || strstr(line, "RSN") || strstr(line, "WEP")) {
            current_network.security = parse_security(line);
        }

        line = strtok(NULL, "\n");
    }

    /* Save last network */
    if (parsing_network && current_network.ssid[0] != '\0' && *count < max_networks) {
        current_network.is_saved = is_network_saved(current_network.ssid);
        memcpy(&networks[*count], &current_network, sizeof(wifi_network_t));
        (*count)++;
    }

    /* Sort by signal strength (strongest first) */
    if (*count > 1) {
        qsort(networks, *count, sizeof(wifi_network_t), compare_networks);
    }

    return 0;
}

/*
 * Connect to a WiFi network
 */
int wifi_manager_connect(const char *ssid, const char *password) {
    if (!initialized || ssid == NULL || ssid[0] == '\0') {
        current_status.last_error = WIFI_ERROR_INVALID_PARAM;
        snprintf(current_status.error_message, MAX_ERROR_MESSAGE_LENGTH,
                 "Invalid parameters");
        return -1;
    }

    /* Update status to connecting */
    current_status.state = WIFI_STATE_CONNECTING;
    strncpy(current_status.connected_ssid, ssid, MAX_SSID_LENGTH);
    current_status.last_error = WIFI_ERROR_NONE;

    /* Build wpa_supplicant network configuration */
    char network_config[512];
    if (password == NULL || password[0] == '\0') {
        /* Open network */
        snprintf(network_config, sizeof(network_config),
                 "network={\n"
                 "    ssid=\"%s\"\n"
                 "    key_mgmt=NONE\n"
                 "}\n",
                 ssid);
    } else {
        /* Secured network - use wpa_passphrase to generate PSK */
        char cmd[CMD_BUFFER_SIZE];
        char psk_output[OUTPUT_BUFFER_SIZE];

        snprintf(cmd, sizeof(cmd), "wpa_passphrase \"%s\" \"%s\" 2>/dev/null", ssid, password);
        if (execute_command(cmd, psk_output, sizeof(psk_output)) != 0) {
            current_status.state = WIFI_STATE_ERROR;
            current_status.last_error = WIFI_ERROR_CONFIG_WRITE;
            snprintf(current_status.error_message, MAX_ERROR_MESSAGE_LENGTH,
                     "Failed to generate network configuration");
            return -1;
        }

        /* Use the wpa_passphrase output directly */
        strncpy(network_config, psk_output, sizeof(network_config) - 1);
        network_config[sizeof(network_config) - 1] = '\0';
    }

    /* Read existing wpa_supplicant.conf */
    FILE *fp = fopen(WPA_SUPPLICANT_CONF, "r");
    char *conf_content = NULL;
    size_t conf_size = 0;

    if (fp != NULL) {
        /* Read existing content */
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        conf_content = malloc(conf_size + 1);
        if (conf_content) {
            fread(conf_content, 1, conf_size, fp);
            conf_content[conf_size] = '\0';
        }
        fclose(fp);
    }

    /* Write updated configuration */
    fp = fopen(WPA_SUPPLICANT_CONF, "w");
    if (fp == NULL) {
        free(conf_content);
        current_status.state = WIFI_STATE_ERROR;
        current_status.last_error = WIFI_ERROR_CONFIG_WRITE;
        snprintf(current_status.error_message, MAX_ERROR_MESSAGE_LENGTH,
                 "Failed to write configuration file");
        return -1;
    }

    /* Write header if this is a new file */
    if (conf_content == NULL) {
        fprintf(fp, "ctrl_interface=/var/run/wpa_supplicant\n");
        fprintf(fp, "update_config=1\n\n");
    } else {
        /* Write existing content (excluding old networks with same SSID) */
        char *line_start = conf_content;
        char *line_end;
        bool in_network_block = false;
        bool skip_block = false;

        while ((line_end = strchr(line_start, '\n')) != NULL) {
            *line_end = '\0';

            if (strstr(line_start, "network={")) {
                in_network_block = true;
                skip_block = false;
            } else if (in_network_block && strstr(line_start, "ssid=") && strstr(line_start, ssid)) {
                skip_block = true;  /* Skip this network block */
            } else if (in_network_block && strstr(line_start, "}")) {
                in_network_block = false;
                if (skip_block) {
                    line_start = line_end + 1;
                    continue;  /* Skip the closing brace */
                }
            }

            if (!skip_block) {
                fprintf(fp, "%s\n", line_start);
            }

            line_start = line_end + 1;
        }

        free(conf_content);
    }

    /* Append new network configuration */
    fprintf(fp, "%s\n", network_config);
    fclose(fp);

    /* Reconfigure wpa_supplicant */
    if (execute_command("wpa_cli -i " WIFI_INTERFACE " reconfigure 2>/dev/null", NULL, 0) != 0) {
        fprintf(stderr, "Warning: wpa_cli reconfigure failed, trying to restart wpa_supplicant\n");
        execute_command("/etc/init.d/S40network restart 2>/dev/null", NULL, 0);
    }

    /* Wait for connection (up to 30 seconds) */
    int attempts = 0;
    bool connected = false;

    for (attempts = 0; attempts < 30; attempts++) {
        sleep(1);

        /* Check connection status */
        char status_output[OUTPUT_BUFFER_SIZE];
        if (execute_command("iw dev " WIFI_INTERFACE " link 2>/dev/null",
                          status_output, sizeof(status_output)) == 0) {
            if (strstr(status_output, "Connected to")) {
                connected = true;
                break;
            }
        }
    }

    if (!connected) {
        current_status.state = WIFI_STATE_ERROR;
        current_status.last_error = WIFI_ERROR_TIMEOUT;
        snprintf(current_status.error_message, MAX_ERROR_MESSAGE_LENGTH,
                 "Connection timeout");
        return -1;
    }

    /* Request DHCP lease */
    char cmd[CMD_BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "dhcpcd -4 " WIFI_INTERFACE " 2>/dev/null");
    execute_command(cmd, NULL, 0);

    /* Wait for IP address (up to 15 seconds) */
    bool got_ip = false;
    for (attempts = 0; attempts < 15; attempts++) {
        sleep(1);

        char ip_output[OUTPUT_BUFFER_SIZE];
        if (execute_command("ip addr show " WIFI_INTERFACE " 2>/dev/null",
                          ip_output, sizeof(ip_output)) == 0) {
            if (strstr(ip_output, "inet ")) {
                got_ip = true;
                break;
            }
        }
    }

    if (!got_ip) {
        current_status.state = WIFI_STATE_ERROR;
        current_status.last_error = WIFI_ERROR_DHCP_TIMEOUT;
        snprintf(current_status.error_message, MAX_ERROR_MESSAGE_LENGTH,
                 "Could not obtain IP address");
        return -1;
    }

    /* Update status */
    current_status.state = WIFI_STATE_CONNECTED;
    current_status.last_error = WIFI_ERROR_NONE;
    wifi_manager_get_status(&current_status);

    return 0;
}

/*
 * Disconnect from current WiFi network
 */
int wifi_manager_disconnect(void) {
    if (!initialized) {
        return -1;
    }

    /* Disconnect interface */
    execute_command("wpa_cli -i " WIFI_INTERFACE " disconnect 2>/dev/null", NULL, 0);
    execute_command("dhcpcd -k " WIFI_INTERFACE " 2>/dev/null", NULL, 0);

    /* Update status */
    current_status.state = WIFI_STATE_DISCONNECTED;
    current_status.connected_ssid[0] = '\0';
    current_status.ip_address[0] = '\0';
    current_status.signal_strength = -100;

    return 0;
}

/*
 * Get current WiFi connection status
 */
int wifi_manager_get_status(wifi_status_t *status) {
    if (!initialized || status == NULL) {
        return -1;
    }

    /* Check link status */
    char output[OUTPUT_BUFFER_SIZE];
    if (execute_command("iw dev " WIFI_INTERFACE " link 2>/dev/null",
                       output, sizeof(output)) == 0) {

        if (strstr(output, "Not connected")) {
            current_status.state = WIFI_STATE_DISCONNECTED;
            current_status.connected_ssid[0] = '\0';
            current_status.signal_strength = -100;
        } else if (strstr(output, "Connected to")) {
            current_status.state = WIFI_STATE_CONNECTED;

            /* Extract SSID */
            char *ssid_line = strstr(output, "SSID:");
            if (ssid_line) {
                ssid_line += 5;
                char *ssid_end = strchr(ssid_line, '\n');
                if (ssid_end) {
                    int len = ssid_end - ssid_line;
                    if (len > MAX_SSID_LENGTH) len = MAX_SSID_LENGTH;
                    strncpy(current_status.connected_ssid, ssid_line, len);
                    current_status.connected_ssid[len] = '\0';
                    trim_whitespace(current_status.connected_ssid);
                }
            }

            /* Extract signal strength */
            char *signal_line = strstr(output, "signal:");
            if (signal_line) {
                current_status.signal_strength = parse_signal_dbm(signal_line);
            }
        }
    }

    /* Get IP address if connected */
    if (current_status.state == WIFI_STATE_CONNECTED) {
        if (execute_command("ip addr show " WIFI_INTERFACE " 2>/dev/null",
                          output, sizeof(output)) == 0) {
            char *inet_line = strstr(output, "inet ");
            if (inet_line) {
                inet_line += 5;
                char *slash = strchr(inet_line, '/');
                if (slash) {
                    int len = slash - inet_line;
                    if (len > MAX_IP_LENGTH - 1) len = MAX_IP_LENGTH - 1;
                    strncpy(current_status.ip_address, inet_line, len);
                    current_status.ip_address[len] = '\0';
                    trim_whitespace(current_status.ip_address);
                }
            } else {
                /* No IP address yet, might still be getting DHCP */
                current_status.ip_address[0] = '\0';
            }
        }
    } else {
        current_status.ip_address[0] = '\0';
    }

    /* Copy to output */
    memcpy(status, &current_status, sizeof(wifi_status_t));

    return 0;
}

/*
 * Read current network SSID
 */
int wifi_manager_get_current_ssid(char *ssid, int max_len) {
    if (!initialized || ssid == NULL || max_len < 1) {
        return -1;
    }

    wifi_status_t status;
    if (wifi_manager_get_status(&status) == 0) {
        strncpy(ssid, status.connected_ssid, max_len - 1);
        ssid[max_len - 1] = '\0';
        return 0;
    }

    return -1;
}

/*
 * Read current signal strength
 */
int wifi_manager_get_signal_strength(void) {
    wifi_status_t status;
    if (wifi_manager_get_status(&status) == 0) {
        return status.signal_strength;
    }
    return -100;
}

/*
 * Forget saved network credentials
 */
int wifi_manager_forget_network(const char *ssid) {
    if (!initialized || ssid == NULL || ssid[0] == '\0') {
        return -1;
    }

    /* If connected to this network, disconnect first */
    if (strcmp(current_status.connected_ssid, ssid) == 0) {
        wifi_manager_disconnect();
    }

    /* Read wpa_supplicant.conf */
    FILE *fp = fopen(WPA_SUPPLICANT_CONF, "r");
    if (fp == NULL) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t conf_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *conf_content = malloc(conf_size + 1);
    if (conf_content == NULL) {
        fclose(fp);
        return -1;
    }

    fread(conf_content, 1, conf_size, fp);
    conf_content[conf_size] = '\0';
    fclose(fp);

    /* Write back without the specified network */
    fp = fopen(WPA_SUPPLICANT_CONF, "w");
    if (fp == NULL) {
        free(conf_content);
        return -1;
    }

    char *line_start = conf_content;
    char *line_end;
    bool in_network_block = false;
    bool skip_block = false;

    while ((line_end = strchr(line_start, '\n')) != NULL) {
        *line_end = '\0';

        if (strstr(line_start, "network={")) {
            in_network_block = true;
            skip_block = false;
        } else if (in_network_block && strstr(line_start, "ssid=") && strstr(line_start, ssid)) {
            skip_block = true;
        } else if (in_network_block && strstr(line_start, "}")) {
            in_network_block = false;
            if (skip_block) {
                line_start = line_end + 1;
                continue;
            }
        }

        if (!skip_block) {
            fprintf(fp, "%s\n", line_start);
        }

        line_start = line_end + 1;
    }

    fclose(fp);
    free(conf_content);

    /* Reconfigure wpa_supplicant */
    execute_command("wpa_cli -i " WIFI_INTERFACE " reconfigure 2>/dev/null", NULL, 0);

    return 0;
}

/*
 * List saved networks
 */
int wifi_manager_list_saved(wifi_network_t *networks, int max_networks, int *count) {
    if (!initialized || networks == NULL || count == NULL) {
        return -1;
    }

    *count = 0;

    FILE *fp = fopen(WPA_SUPPLICANT_CONF, "r");
    if (fp == NULL) {
        return 0;  /* No saved networks */
    }

    char line[LINE_BUFFER_SIZE];
    wifi_network_t current_network = {0};
    bool in_network_block = false;

    while (fgets(line, sizeof(line), fp) != NULL && *count < max_networks) {
        trim_whitespace(line);

        if (strstr(line, "network={")) {
            in_network_block = true;
            memset(&current_network, 0, sizeof(wifi_network_t));
            current_network.is_saved = true;
            current_network.signal_strength = -100;
        } else if (in_network_block && strstr(line, "ssid=")) {
            /* Extract SSID (remove quotes) */
            char *ssid_start = strchr(line, '"');
            if (ssid_start) {
                ssid_start++;
                char *ssid_end = strchr(ssid_start, '"');
                if (ssid_end) {
                    int len = ssid_end - ssid_start;
                    if (len > MAX_SSID_LENGTH) len = MAX_SSID_LENGTH;
                    strncpy(current_network.ssid, ssid_start, len);
                    current_network.ssid[len] = '\0';
                }
            }
        } else if (in_network_block && strstr(line, "}")) {
            in_network_block = false;
            if (current_network.ssid[0] != '\0') {
                memcpy(&networks[*count], &current_network, sizeof(wifi_network_t));
                (*count)++;
            }
        }
    }

    fclose(fp);
    return 0;
}

/*
 * Test network connectivity
 */
int wifi_manager_test_connection(void) {
    if (!initialized) {
        return -1;
    }

    /* Ping gateway */
    char output[OUTPUT_BUFFER_SIZE];
    int result = execute_command("ping -c 1 -W 2 8.8.8.8 2>/dev/null", output, sizeof(output));

    return (result == 0) ? 0 : -1;
}

/*
 * Utility functions
 */

const char* wifi_security_to_string(wifi_security_t security) {
    switch (security) {
        case WIFI_SECURITY_OPEN:           return "Open";
        case WIFI_SECURITY_WEP:            return "WEP";
        case WIFI_SECURITY_WPA_PSK:        return "WPA-PSK";
        case WIFI_SECURITY_WPA2_PSK:       return "WPA2-PSK";
        case WIFI_SECURITY_WPA_WPA2_PSK:   return "WPA/WPA2-PSK";
        default:                           return "Unknown";
    }
}

const char* wifi_state_to_string(wifi_state_t state) {
    switch (state) {
        case WIFI_STATE_DISCONNECTED:  return "Disconnected";
        case WIFI_STATE_CONNECTING:    return "Connecting";
        case WIFI_STATE_CONNECTED:     return "Connected";
        case WIFI_STATE_ERROR:         return "Error";
        default:                       return "Unknown";
    }
}

const char* wifi_error_to_string(wifi_error_t error) {
    switch (error) {
        case WIFI_ERROR_NONE:              return "No error";
        case WIFI_ERROR_AUTH_FAILED:       return "Authentication failed";
        case WIFI_ERROR_NETWORK_NOT_FOUND: return "Network not found";
        case WIFI_ERROR_DHCP_TIMEOUT:      return "DHCP timeout";
        case WIFI_ERROR_TIMEOUT:           return "Connection timeout";
        case WIFI_ERROR_INTERFACE_DOWN:    return "Interface down";
        case WIFI_ERROR_SCAN_FAILED:       return "Scan failed";
        case WIFI_ERROR_CONFIG_WRITE:      return "Configuration write failed";
        case WIFI_ERROR_COMMAND_FAILED:    return "Command execution failed";
        case WIFI_ERROR_INVALID_PARAM:     return "Invalid parameter";
        case WIFI_ERROR_UNKNOWN:           return "Unknown error";
        default:                           return "Unknown error";
    }
}

int wifi_signal_to_percentage(int dbm) {
    if (dbm >= -50) return 100;
    if (dbm <= -100) return 0;
    return 2 * (dbm + 100);
}

int wifi_signal_to_bars(int dbm) {
    /* Map dBm to 8-level bar indicator (as per WIFI_UI.md spec) */
    if (dbm >= -50) return 8;       /* -30 to -50: Excellent */
    if (dbm >= -60) return 7;       /* -50 to -60: Very Good */
    if (dbm >= -67) return 6;       /* -60 to -67: Good */
    if (dbm >= -70) return 5;       /* -67 to -70: OK */
    if (dbm >= -75) return 4;       /* -70 to -75: Fair */
    if (dbm >= -80) return 3;       /* -75 to -80: Weak */
    if (dbm >= -85) return 2;       /* -80 to -85: Very Weak */
    if (dbm >= -90) return 1;       /* -85 to -90: Minimal */
    return 1;                       /* Below -90: Still show 1 bar */
}
