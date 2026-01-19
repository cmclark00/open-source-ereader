/*
 * time_sync.h - Network Time Synchronization
 *
 * This module provides automatic time synchronization using NTP (chrony).
 * It triggers time sync when WiFi connects and provides status monitoring.
 *
 * Key features:
 * - Automatic time sync on WiFi connection
 * - Manual time sync trigger
 * - Time sync status monitoring
 * - Integration with chrony NTP client
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

#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <stdbool.h>
#include <time.h>

/*
 * Time sync states
 */
typedef enum {
    TIME_SYNC_STATE_NOT_SYNCED,    /* Time has not been synced yet */
    TIME_SYNC_STATE_SYNCING,       /* Sync in progress */
    TIME_SYNC_STATE_SYNCED,        /* Time successfully synced */
    TIME_SYNC_STATE_ERROR          /* Sync failed */
} time_sync_state_t;

/*
 * Time sync error codes
 */
typedef enum {
    TIME_SYNC_ERROR_NONE = 0,      /* No error */
    TIME_SYNC_ERROR_NO_NETWORK,    /* No network connection */
    TIME_SYNC_ERROR_TIMEOUT,       /* Sync timeout */
    TIME_SYNC_ERROR_NTP_FAILED,    /* NTP server unreachable */
    TIME_SYNC_ERROR_CHRONY_FAILED, /* chronyc command failed */
    TIME_SYNC_ERROR_UNKNOWN        /* Unknown error */
} time_sync_error_t;

/*
 * Time sync status information
 */
typedef struct {
    time_sync_state_t state;       /* Current sync state */
    time_sync_error_t error;       /* Last error (if state is ERROR) */
    time_t last_sync_time;         /* Timestamp of last successful sync (0 if never synced) */
    int offset_seconds;            /* Time offset from NTP server in seconds */
    char server[64];               /* NTP server hostname/IP */
    bool chrony_running;           /* Whether chronyd daemon is running */
} time_sync_status_t;

/*
 * Initialize the time sync module
 *
 * Starts the chronyd daemon if not already running.
 *
 * Returns:
 *   0 on success, -1 on failure
 */
int time_sync_init(void);

/*
 * Shutdown the time sync module
 *
 * Stops the chronyd daemon.
 *
 * Returns:
 *   0 on success, -1 on failure
 */
int time_sync_shutdown(void);

/*
 * Trigger an immediate time sync
 *
 * Forces chronyd to sync with NTP servers immediately.
 * This is non-blocking - use time_sync_get_status() to check progress.
 *
 * Returns:
 *   0 on success (sync started), -1 on failure
 */
int time_sync_trigger(void);

/*
 * Get current time sync status
 *
 * Parameters:
 *   status - Pointer to time_sync_status_t to fill with current status
 *
 * Returns:
 *   0 on success, -1 on failure
 */
int time_sync_get_status(time_sync_status_t *status);

/*
 * Check if time has been synced
 *
 * Returns:
 *   true if time is synced, false otherwise
 */
bool time_sync_is_synced(void);

/*
 * Callback function to be called when WiFi connects
 *
 * This is typically called by the WiFi manager when a connection
 * is established. It triggers an automatic time sync.
 *
 * Returns:
 *   0 on success, -1 on failure
 */
int time_sync_on_wifi_connect(void);

/*
 * Convert time_sync_state_t to string
 *
 * Parameters:
 *   state - The state to convert
 *
 * Returns:
 *   String representation of the state
 */
const char *time_sync_state_to_string(time_sync_state_t state);

/*
 * Convert time_sync_error_t to string
 *
 * Parameters:
 *   error - The error to convert
 *
 * Returns:
 *   String representation of the error
 */
const char *time_sync_error_to_string(time_sync_error_t error);

#endif /* TIME_SYNC_H */
