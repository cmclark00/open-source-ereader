/*
 * power_manager.h - E-Reader Power Management
 *
 * This module provides power management features including:
 * - Idle timer tracking (reset on any button press)
 * - Automatic sleep mode after configured timeout
 * - Sleep mode: clear screen, disable display, wait for button press
 * - Wake on any button press
 * - Integration with settings system for configurable timeout
 *
 * Sleep workflow:
 * 1. Idle timer counts time since last button press
 * 2. When timeout approaches, display warning message
 * 3. When timeout reached, enter sleep mode:
 *    - Clear screen to blank
 *    - Power down display
 *    - Enter low-power state
 * 4. Wake on any button press:
 *    - Re-initialize display
 *    - Restore previous application state
 *    - Reset idle timer
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

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/*
 * Sleep warning threshold (seconds before sleep to show warning)
 */
#define SLEEP_WARNING_SECONDS 30

/*
 * Power manager state
 *
 * Tracks idle time, sleep state, and power configuration.
 */
typedef struct {
    /* Idle tracking */
    time_t last_activity_time;     /* Timestamp of last button press */
    uint32_t idle_timeout_seconds;  /* Timeout in seconds (0 = never sleep) */

    /* Sleep state */
    bool is_sleeping;               /* Currently in sleep mode */
    bool warning_displayed;         /* Sleep warning has been shown */

    /* Configuration */
    bool auto_sleep_enabled;        /* Auto-sleep feature enabled */

    /* Display callback for warning message */
    void (*warning_callback)(uint32_t seconds_remaining);

} power_manager_state_t;

/*
 * Initialize power manager
 *
 * Sets up idle tracking and configures timeout from settings.
 *
 * Parameters:
 *   timeout_minutes - Auto-sleep timeout in minutes (0 = never sleep)
 *
 * Returns:
 *   Pointer to power manager state on success
 *   NULL on failure
 */
power_manager_state_t* power_manager_init(uint32_t timeout_minutes);

/*
 * Clean up power manager and free resources
 *
 * Parameters:
 *   state - Power manager state (may be NULL)
 */
void power_manager_cleanup(power_manager_state_t *state);

/*
 * Update power manager (call in main loop)
 *
 * Checks idle time and triggers sleep mode if timeout exceeded.
 * Should be called regularly from main event loop.
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   true if sleep mode should be entered
 *   false otherwise
 */
bool power_manager_update(power_manager_state_t *state);

/*
 * Reset idle timer (call on any user activity)
 *
 * Should be called whenever a button is pressed or any user
 * interaction occurs.
 *
 * Parameters:
 *   state - Power manager state
 */
void power_manager_reset_idle(power_manager_state_t *state);

/*
 * Enter sleep mode
 *
 * Puts device into low-power state:
 * - Clears display to blank
 * - Powers down display hardware
 * - Sets sleeping flag
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int power_manager_enter_sleep(power_manager_state_t *state);

/*
 * Wake from sleep mode
 *
 * Restores device from low-power state:
 * - Re-initializes display hardware
 * - Resets idle timer
 * - Clears sleeping flag
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   0 on success
 *   -1 on error
 */
int power_manager_wake(power_manager_state_t *state);

/*
 * Check if currently in sleep mode
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   true if sleeping
 *   false otherwise
 */
bool power_manager_is_sleeping(const power_manager_state_t *state);

/*
 * Get seconds until sleep
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   Number of seconds until sleep (0 if sleeping or disabled)
 */
uint32_t power_manager_get_seconds_until_sleep(const power_manager_state_t *state);

/*
 * Check if sleep warning should be displayed
 *
 * Returns true if we're within warning threshold and warning
 * hasn't been displayed yet.
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   true if warning should be displayed
 *   false otherwise
 */
bool power_manager_should_show_warning(power_manager_state_t *state);

/*
 * Mark warning as displayed
 *
 * Call this after displaying the sleep warning to avoid
 * repeatedly showing it.
 *
 * Parameters:
 *   state - Power manager state
 */
void power_manager_mark_warning_shown(power_manager_state_t *state);

/*
 * Set auto-sleep timeout
 *
 * Updates the timeout value. Can be called at runtime to apply
 * new settings.
 *
 * Parameters:
 *   state - Power manager state
 *   timeout_minutes - New timeout in minutes (0 = never sleep)
 */
void power_manager_set_timeout(power_manager_state_t *state, uint32_t timeout_minutes);

/*
 * Enable or disable auto-sleep
 *
 * Parameters:
 *   state - Power manager state
 *   enabled - true to enable, false to disable
 */
void power_manager_set_enabled(power_manager_state_t *state, bool enabled);

/*
 * Check if auto-sleep is enabled
 *
 * Parameters:
 *   state - Power manager state
 *
 * Returns:
 *   true if enabled
 *   false otherwise
 */
bool power_manager_is_enabled(const power_manager_state_t *state);

#endif /* POWER_MANAGER_H */
