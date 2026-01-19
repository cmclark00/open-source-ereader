/*
 * time_sync.c - Network Time Synchronization Implementation
 *
 * Implements automatic time synchronization using chrony NTP client.
 * Integrates with WiFi manager to sync time when network becomes available.
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

#include "time_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/* Buffer sizes */
#define CMD_BUFFER_SIZE 256
#define LINE_BUFFER_SIZE 128
#define OUTPUT_BUFFER_SIZE 2048

/* Timeout values (seconds) */
#define SYNC_TIMEOUT 30
#define CHRONY_START_TIMEOUT 5

/* Global state */
static bool initialized = false;
static time_sync_status_t current_status = {
    .state = TIME_SYNC_STATE_NOT_SYNCED,
    .error = TIME_SYNC_ERROR_NONE,
    .last_sync_time = 0,
    .offset_seconds = 0,
    .server = "",
    .chrony_running = false
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
        fprintf(stderr, "time_sync: Failed to execute command: %s\n", cmd);
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
 * Helper function: Check if chronyd is running
 */
static bool is_chronyd_running(void) {
    char output[OUTPUT_BUFFER_SIZE];
    int ret = execute_command("pidof chronyd", output, sizeof(output));
    return (ret == 0 && strlen(output) > 0);
}

/*
 * Helper function: Start chronyd daemon
 */
static int start_chronyd(void) {
    if (is_chronyd_running()) {
        return 0; /* Already running */
    }

    int ret = execute_command("/usr/sbin/chronyd -d > /dev/null 2>&1 &", NULL, 0);
    if (ret != 0) {
        fprintf(stderr, "time_sync: Failed to start chronyd\n");
        return -1;
    }

    /* Wait for chronyd to start */
    for (int i = 0; i < CHRONY_START_TIMEOUT; i++) {
        sleep(1);
        if (is_chronyd_running()) {
            current_status.chrony_running = true;
            return 0;
        }
    }

    fprintf(stderr, "time_sync: chronyd failed to start within timeout\n");
    return -1;
}

/*
 * Helper function: Stop chronyd daemon
 */
static int stop_chronyd(void) {
    if (!is_chronyd_running()) {
        return 0; /* Already stopped */
    }

    int ret = execute_command("killall chronyd", NULL, 0);
    if (ret != 0) {
        fprintf(stderr, "time_sync: Failed to stop chronyd\n");
        return -1;
    }

    current_status.chrony_running = false;
    return 0;
}

/*
 * Helper function: Parse chronyc tracking output
 */
static int parse_tracking_output(const char *output, time_sync_status_t *status) {
    char line[LINE_BUFFER_SIZE];
    const char *ptr = output;

    while (*ptr) {
        /* Extract one line */
        const char *line_end = strchr(ptr, '\n');
        if (line_end == NULL) {
            line_end = ptr + strlen(ptr);
        }

        int line_len = line_end - ptr;
        if (line_len >= LINE_BUFFER_SIZE) {
            line_len = LINE_BUFFER_SIZE - 1;
        }

        strncpy(line, ptr, line_len);
        line[line_len] = '\0';

        /* Parse fields */
        if (strstr(line, "Reference ID") != NULL) {
            /* Extract server hostname/IP */
            char *paren = strchr(line, '(');
            if (paren) {
                char *close = strchr(paren, ')');
                if (close) {
                    int len = close - paren - 1;
                    if (len > 0 && len < 63) {
                        strncpy(status->server, paren + 1, len);
                        status->server[len] = '\0';
                    }
                }
            }
        } else if (strstr(line, "System time") != NULL) {
            /* Extract offset in seconds */
            char *seconds_ptr = strstr(line, "seconds");
            if (seconds_ptr) {
                /* Search backwards for the number */
                char *num_start = seconds_ptr - 1;
                while (num_start > line && (*num_start == ' ' || *num_start == '\t')) {
                    num_start--;
                }
                while (num_start > line && ((*num_start >= '0' && *num_start <= '9') || *num_start == '.' || *num_start == '-')) {
                    num_start--;
                }
                num_start++;

                status->offset_seconds = (int)atof(num_start);
            }
        }

        /* Move to next line */
        ptr = (*line_end == '\0') ? line_end : line_end + 1;
    }

    return 0;
}

/*
 * Initialize the time sync module
 */
int time_sync_init(void) {
    if (initialized) {
        return 0; /* Already initialized */
    }

    /* Start chronyd daemon */
    if (start_chronyd() != 0) {
        return -1;
    }

    initialized = true;
    printf("time_sync: Initialized successfully\n");
    return 0;
}

/*
 * Shutdown the time sync module
 */
int time_sync_shutdown(void) {
    if (!initialized) {
        return 0; /* Already shut down */
    }

    stop_chronyd();
    initialized = false;
    printf("time_sync: Shut down successfully\n");
    return 0;
}

/*
 * Trigger an immediate time sync
 */
int time_sync_trigger(void) {
    if (!initialized) {
        fprintf(stderr, "time_sync: Not initialized\n");
        return -1;
    }

    if (!is_chronyd_running()) {
        current_status.state = TIME_SYNC_STATE_ERROR;
        current_status.error = TIME_SYNC_ERROR_CHRONY_FAILED;
        fprintf(stderr, "time_sync: chronyd not running\n");
        return -1;
    }

    /* Force immediate sync using chronyc makestep */
    char output[OUTPUT_BUFFER_SIZE];
    current_status.state = TIME_SYNC_STATE_SYNCING;

    int ret = execute_command("chronyc makestep 2>&1", output, sizeof(output));

    if (ret != 0) {
        current_status.state = TIME_SYNC_STATE_ERROR;
        current_status.error = TIME_SYNC_ERROR_CHRONY_FAILED;
        fprintf(stderr, "time_sync: chronyc makestep failed: %s\n", output);
        return -1;
    }

    /* Wait a moment for sync to complete */
    sleep(2);

    /* Check if sync was successful by querying tracking */
    ret = execute_command("chronyc tracking 2>&1", output, sizeof(output));

    if (ret != 0 || strstr(output, "Not synchronised") != NULL) {
        current_status.state = TIME_SYNC_STATE_ERROR;
        current_status.error = TIME_SYNC_ERROR_NTP_FAILED;
        fprintf(stderr, "time_sync: NTP sync failed\n");
        return -1;
    }

    /* Parse tracking output */
    parse_tracking_output(output, &current_status);

    /* Update status */
    current_status.state = TIME_SYNC_STATE_SYNCED;
    current_status.error = TIME_SYNC_ERROR_NONE;
    current_status.last_sync_time = time(NULL);

    printf("time_sync: Time synchronized successfully from %s\n", current_status.server);
    return 0;
}

/*
 * Get current time sync status
 */
int time_sync_get_status(time_sync_status_t *status) {
    if (!initialized || status == NULL) {
        return -1;
    }

    /* Check if chronyd is still running */
    current_status.chrony_running = is_chronyd_running();

    /* Copy current status */
    memcpy(status, &current_status, sizeof(time_sync_status_t));
    return 0;
}

/*
 * Check if time has been synced
 */
bool time_sync_is_synced(void) {
    if (!initialized) {
        return false;
    }

    return (current_status.state == TIME_SYNC_STATE_SYNCED);
}

/*
 * Callback function to be called when WiFi connects
 */
int time_sync_on_wifi_connect(void) {
    printf("time_sync: WiFi connected, triggering time sync\n");

    /* Wait a moment for network to stabilize */
    sleep(2);

    return time_sync_trigger();
}

/*
 * Convert time_sync_state_t to string
 */
const char *time_sync_state_to_string(time_sync_state_t state) {
    switch (state) {
        case TIME_SYNC_STATE_NOT_SYNCED: return "Not Synced";
        case TIME_SYNC_STATE_SYNCING:    return "Syncing";
        case TIME_SYNC_STATE_SYNCED:     return "Synced";
        case TIME_SYNC_STATE_ERROR:      return "Error";
        default:                         return "Unknown";
    }
}

/*
 * Convert time_sync_error_t to string
 */
const char *time_sync_error_to_string(time_sync_error_t error) {
    switch (error) {
        case TIME_SYNC_ERROR_NONE:           return "No error";
        case TIME_SYNC_ERROR_NO_NETWORK:     return "No network connection";
        case TIME_SYNC_ERROR_TIMEOUT:        return "Sync timeout";
        case TIME_SYNC_ERROR_NTP_FAILED:     return "NTP server unreachable";
        case TIME_SYNC_ERROR_CHRONY_FAILED:  return "chronyd failed";
        case TIME_SYNC_ERROR_UNKNOWN:        return "Unknown error";
        default:                             return "Unknown error";
    }
}
