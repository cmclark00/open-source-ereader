/*
 * wifi_manager.h - WiFi Network Management
 *
 * This module provides WiFi scanning, connection, and status management.
 * It interfaces with the system's wpa_supplicant, iw, and dhcpcd utilities
 * to control the wlan0 interface.
 *
 * Key features:
 * - Scan for available networks
 * - Connect to WPA/WPA2-PSK and open networks
 * - Monitor connection status and signal strength
 * - Manage saved network credentials
 * - Test network connectivity
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

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>

/*
 * Maximum lengths for WiFi data
 */
#define MAX_SSID_LENGTH 32
#define MAX_BSSID_LENGTH 18
#define MAX_PASSWORD_LENGTH 63
#define MAX_NETWORKS 50
#define MAX_IP_LENGTH 16
#define MAX_ERROR_MESSAGE_LENGTH 128

/*
 * WiFi security types
 */
typedef enum {
    WIFI_SECURITY_OPEN,           /* No encryption */
    WIFI_SECURITY_WEP,            /* WEP (legacy, insecure) */
    WIFI_SECURITY_WPA_PSK,        /* WPA-PSK */
    WIFI_SECURITY_WPA2_PSK,       /* WPA2-PSK (most common) */
    WIFI_SECURITY_WPA_WPA2_PSK    /* WPA/WPA2 mixed mode */
} wifi_security_t;

/*
 * WiFi connection states
 */
typedef enum {
    WIFI_STATE_DISCONNECTED,      /* Not connected to any network */
    WIFI_STATE_CONNECTING,        /* Connection in progress */
    WIFI_STATE_CONNECTED,         /* Connected and have IP address */
    WIFI_STATE_ERROR              /* Error state */
} wifi_state_t;

/*
 * WiFi error codes
 */
typedef enum {
    WIFI_ERROR_NONE = 0,          /* No error */
    WIFI_ERROR_AUTH_FAILED,       /* Authentication failed (wrong password) */
    WIFI_ERROR_NETWORK_NOT_FOUND, /* SSID not found or out of range */
    WIFI_ERROR_DHCP_TIMEOUT,      /* Could not obtain IP address */
    WIFI_ERROR_TIMEOUT,           /* Connection timeout */
    WIFI_ERROR_INTERFACE_DOWN,    /* wlan0 interface is down */
    WIFI_ERROR_SCAN_FAILED,       /* Scan operation failed */
    WIFI_ERROR_CONFIG_WRITE,      /* Failed to write configuration */
    WIFI_ERROR_COMMAND_FAILED,    /* System command execution failed */
    WIFI_ERROR_INVALID_PARAM,     /* Invalid parameter */
    WIFI_ERROR_UNKNOWN            /* Unknown error */
} wifi_error_t;

/*
 * WiFi network information
 *
 * Represents a single WiFi network discovered during scanning
 * or stored in saved networks.
 */
typedef struct {
    char ssid[MAX_SSID_LENGTH + 1];       /* Network name (null-terminated) */
    char bssid[MAX_BSSID_LENGTH];         /* MAC address (XX:XX:XX:XX:XX:XX) */
    int signal_strength;                  /* Signal strength in dBm (-30 to -90) */
    wifi_security_t security;             /* Security type */
    int frequency;                        /* Frequency in MHz (2400-5900) */
    bool is_saved;                        /* Has saved credentials */
} wifi_network_t;

/*
 * WiFi connection status
 *
 * Contains current WiFi connection state and information.
 */
typedef struct {
    wifi_state_t state;                           /* Connection state */
    char connected_ssid[MAX_SSID_LENGTH + 1];     /* Currently connected SSID */
    int signal_strength;                          /* Current signal strength (dBm) */
    char ip_address[MAX_IP_LENGTH];               /* IP address (e.g., "192.168.1.42") */
    wifi_error_t last_error;                      /* Last error code */
    char error_message[MAX_ERROR_MESSAGE_LENGTH]; /* Human-readable error message */
} wifi_status_t;

/*
 * Function prototypes
 */

/**
 * Initialize WiFi manager
 *
 * Sets up the WiFi manager and checks if wlan0 interface exists.
 * This should be called once at application startup.
 *
 * Returns:
 *   0 on success
 *   -1 on failure (wlan0 not available, or system error)
 */
int wifi_manager_init(void);

/**
 * Cleanup WiFi manager
 *
 * Frees any resources allocated by the WiFi manager.
 * Should be called at application shutdown.
 */
void wifi_manager_cleanup(void);

/**
 * Scan for available WiFi networks
 *
 * Executes a WiFi scan using 'iw dev wlan0 scan' and parses the results.
 * Networks are sorted by signal strength (strongest first).
 *
 * This operation takes 3-10 seconds to complete.
 *
 * Parameters:
 *   networks - Array to store discovered networks
 *   max_networks - Maximum number of networks to return (typically MAX_NETWORKS)
 *   count - Output: Number of networks found
 *
 * Returns:
 *   0 on success (count contains number of networks found, may be 0)
 *   -1 on failure (scan command failed, check errno)
 *
 * Example:
 *   wifi_network_t networks[MAX_NETWORKS];
 *   int count;
 *   if (wifi_manager_scan(networks, MAX_NETWORKS, &count) == 0) {
 *       printf("Found %d networks\n", count);
 *   }
 */
int wifi_manager_scan(wifi_network_t *networks, int max_networks, int *count);

/**
 * Connect to a WiFi network
 *
 * Connects to the specified network using the provided password.
 * For open networks, password should be NULL or empty string.
 *
 * This operation:
 * 1. Updates /etc/wpa_supplicant.conf with network credentials
 * 2. Reconfigures wpa_supplicant (wpa_cli reconfigure)
 * 3. Waits for connection (up to 30 seconds)
 * 4. Obtains IP address via DHCP
 *
 * This is a blocking operation that may take 5-30 seconds.
 *
 * Parameters:
 *   ssid - Network SSID (null-terminated string, max 32 chars)
 *   password - Network password (null-terminated, max 63 chars, or NULL for open)
 *
 * Returns:
 *   0 on success (connected and obtained IP address)
 *   -1 on failure (see wifi_manager_get_status for error details)
 *
 * Example:
 *   if (wifi_manager_connect("MyHomeWiFi", "mypassword123") == 0) {
 *       printf("Connected successfully\n");
 *   }
 */
int wifi_manager_connect(const char *ssid, const char *password);

/**
 * Disconnect from current WiFi network
 *
 * Disconnects from the current network and stops wpa_supplicant.
 *
 * Returns:
 *   0 on success
 *   -1 on failure
 */
int wifi_manager_disconnect(void);

/**
 * Get current WiFi connection status
 *
 * Retrieves the current connection state, signal strength, IP address,
 * and any error information.
 *
 * This function parses:
 * - 'iw dev wlan0 link' for connection status and signal
 * - 'ip addr show wlan0' for IP address
 *
 * Parameters:
 *   status - Output: Current WiFi status
 *
 * Returns:
 *   0 on success (status contains current state)
 *   -1 on failure (system error)
 *
 * Example:
 *   wifi_status_t status;
 *   wifi_manager_get_status(&status);
 *   if (status.state == WIFI_STATE_CONNECTED) {
 *       printf("Connected to %s (%s)\n", status.connected_ssid, status.ip_address);
 *   }
 */
int wifi_manager_get_status(wifi_status_t *status);

/**
 * Read current network SSID
 *
 * Returns the SSID of the currently connected network.
 * If not connected, returns an empty string.
 *
 * Parameters:
 *   ssid - Output buffer for SSID (must be at least MAX_SSID_LENGTH + 1 bytes)
 *   max_len - Size of ssid buffer
 *
 * Returns:
 *   0 on success
 *   -1 on failure
 */
int wifi_manager_get_current_ssid(char *ssid, int max_len);

/**
 * Read current signal strength
 *
 * Returns the signal strength in dBm for the current connection.
 * If not connected, returns -100 dBm.
 *
 * Returns:
 *   Signal strength in dBm (-30 to -90 typical range)
 *   -100 if not connected
 */
int wifi_manager_get_signal_strength(void);

/**
 * Forget saved network credentials
 *
 * Removes the specified network from /etc/wpa_supplicant.conf.
 * If currently connected to this network, disconnects first.
 *
 * Parameters:
 *   ssid - Network SSID to forget
 *
 * Returns:
 *   0 on success
 *   -1 on failure
 */
int wifi_manager_forget_network(const char *ssid);

/**
 * List saved networks
 *
 * Reads /etc/wpa_supplicant.conf and returns a list of saved networks.
 *
 * Parameters:
 *   networks - Array to store saved networks
 *   max_networks - Maximum number of networks to return
 *   count - Output: Number of saved networks found
 *
 * Returns:
 *   0 on success
 *   -1 on failure
 */
int wifi_manager_list_saved(wifi_network_t *networks, int max_networks, int *count);

/**
 * Test network connectivity
 *
 * Tests connectivity by pinging the default gateway or a well-known IP.
 * This verifies that the network connection is working.
 *
 * Returns:
 *   0 on success (network is working)
 *   -1 on failure (no connectivity)
 */
int wifi_manager_test_connection(void);

/*
 * Utility functions
 */

/**
 * Convert WiFi security type to human-readable string
 *
 * Parameters:
 *   security - Security type
 *
 * Returns:
 *   String representation (e.g., "WPA2-PSK", "Open")
 */
const char* wifi_security_to_string(wifi_security_t security);

/**
 * Convert WiFi state to human-readable string
 *
 * Parameters:
 *   state - WiFi state
 *
 * Returns:
 *   String representation (e.g., "Connected", "Disconnected")
 */
const char* wifi_state_to_string(wifi_state_t state);

/**
 * Convert WiFi error code to human-readable string
 *
 * Parameters:
 *   error - Error code
 *
 * Returns:
 *   String representation (e.g., "Authentication failed")
 */
const char* wifi_error_to_string(wifi_error_t error);

/**
 * Convert signal strength (dBm) to quality percentage
 *
 * Maps signal strength to a 0-100% quality scale.
 *
 * Parameters:
 *   dbm - Signal strength in dBm
 *
 * Returns:
 *   Quality percentage (0-100)
 */
int wifi_signal_to_percentage(int dbm);

/**
 * Convert signal strength to bar count (for UI display)
 *
 * Maps signal strength to 8-level bar indicator (as per WIFI_UI.md spec).
 *
 * Parameters:
 *   dbm - Signal strength in dBm
 *
 * Returns:
 *   Bar count (1-8), where 8 is strongest, 1 is weakest
 */
int wifi_signal_to_bars(int dbm);

#endif /* WIFI_MANAGER_H */
