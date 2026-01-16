/*
 * power_manager.c - E-Reader Power Management Implementation
 *
 * Implements idle tracking, auto-sleep, and power management features.
 *
 * Author: E-Reader Project
 * License: Open Source
 */

#include "power_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Include display driver for sleep/wake operations */
#include "../../display-test/epd_driver.h"

/*
 * Initialize power manager
 */
power_manager_state_t* power_manager_init(uint32_t timeout_minutes) {
    power_manager_state_t *state = NULL;

    /* Allocate state structure */
    state = calloc(1, sizeof(power_manager_state_t));
    if (state == NULL) {
        fprintf(stderr, "power_manager_init: Failed to allocate state\n");
        return NULL;
    }

    /* Initialize idle tracking */
    state->last_activity_time = time(NULL);

    /* Convert timeout from minutes to seconds */
    if (timeout_minutes == 0) {
        /* Never sleep */
        state->idle_timeout_seconds = 0;
        state->auto_sleep_enabled = false;
    } else {
        state->idle_timeout_seconds = timeout_minutes * 60;
        state->auto_sleep_enabled = true;
    }

    /* Initialize sleep state */
    state->is_sleeping = false;
    state->warning_displayed = false;

    printf("Power manager initialized (timeout: %u minutes)\n", timeout_minutes);

    return state;
}

/*
 * Clean up power manager
 */
void power_manager_cleanup(power_manager_state_t *state) {
    if (state == NULL) {
        return;
    }

    /* Wake up if sleeping to ensure display is in good state */
    if (state->is_sleeping) {
        power_manager_wake(state);
    }

    free(state);
    printf("Power manager cleaned up\n");
}

/*
 * Update power manager (call in main loop)
 */
bool power_manager_update(power_manager_state_t *state) {
    if (state == NULL) {
        return false;
    }

    /* If already sleeping, no update needed */
    if (state->is_sleeping) {
        return false;
    }

    /* If auto-sleep disabled, nothing to do */
    if (!state->auto_sleep_enabled || state->idle_timeout_seconds == 0) {
        return false;
    }

    /* Calculate idle time */
    time_t current_time = time(NULL);
    time_t idle_seconds = current_time - state->last_activity_time;

    /* Check if we should show warning */
    uint32_t seconds_until_sleep = 0;
    if (idle_seconds < (time_t)state->idle_timeout_seconds) {
        seconds_until_sleep = state->idle_timeout_seconds - idle_seconds;
    }

    /* Show warning if within threshold and not already shown */
    if (seconds_until_sleep > 0 &&
        seconds_until_sleep <= SLEEP_WARNING_SECONDS &&
        !state->warning_displayed) {
        /* Note: Warning display is handled by caller via should_show_warning() */
        state->warning_displayed = true;
    }

    /* Check if timeout exceeded */
    if (idle_seconds >= (time_t)state->idle_timeout_seconds) {
        printf("Power manager: Idle timeout exceeded, entering sleep mode\n");
        return true;  /* Signal that sleep should be entered */
    }

    return false;
}

/*
 * Reset idle timer
 */
void power_manager_reset_idle(power_manager_state_t *state) {
    if (state == NULL) {
        return;
    }

    state->last_activity_time = time(NULL);
    state->warning_displayed = false;  /* Reset warning flag on activity */
}

/*
 * Enter sleep mode
 */
int power_manager_enter_sleep(power_manager_state_t *state) {
    if (state == NULL) {
        return -1;
    }

    if (state->is_sleeping) {
        /* Already sleeping */
        return 0;
    }

    printf("Entering sleep mode...\n");

    /* Clear display to white (blank e-paper screen) */
    if (epd_clear() != 0) {
        fprintf(stderr, "power_manager: Failed to clear display\n");
        return -1;
    }

    /* Sleep the display (puts it in low-power mode) */
    if (epd_sleep() != 0) {
        fprintf(stderr, "power_manager: Failed to sleep display\n");
        return -1;
    }

    /* Mark as sleeping */
    state->is_sleeping = true;

    printf("Sleep mode active\n");

    return 0;
}

/*
 * Wake from sleep mode
 */
int power_manager_wake(power_manager_state_t *state) {
    if (state == NULL) {
        return -1;
    }

    if (!state->is_sleeping) {
        /* Not sleeping */
        return 0;
    }

    printf("Waking from sleep mode...\n");

    /* Re-initialize display hardware */
    if (epd_display_init() != 0) {
        fprintf(stderr, "power_manager: Failed to wake display\n");
        return -1;
    }

    /* Clear sleep state */
    state->is_sleeping = false;

    /* Reset idle timer */
    power_manager_reset_idle(state);

    printf("Wake complete\n");

    return 0;
}

/*
 * Check if currently in sleep mode
 */
bool power_manager_is_sleeping(const power_manager_state_t *state) {
    if (state == NULL) {
        return false;
    }

    return state->is_sleeping;
}

/*
 * Get seconds until sleep
 */
uint32_t power_manager_get_seconds_until_sleep(const power_manager_state_t *state) {
    if (state == NULL || state->is_sleeping || !state->auto_sleep_enabled) {
        return 0;
    }

    if (state->idle_timeout_seconds == 0) {
        return 0;  /* Sleep disabled */
    }

    time_t current_time = time(NULL);
    time_t idle_seconds = current_time - state->last_activity_time;

    if (idle_seconds >= (time_t)state->idle_timeout_seconds) {
        return 0;  /* Should already be sleeping */
    }

    return state->idle_timeout_seconds - idle_seconds;
}

/*
 * Check if sleep warning should be displayed
 */
bool power_manager_should_show_warning(power_manager_state_t *state) {
    if (state == NULL || state->is_sleeping || !state->auto_sleep_enabled) {
        return false;
    }

    if (state->warning_displayed) {
        return false;  /* Already shown */
    }

    uint32_t seconds_until_sleep = power_manager_get_seconds_until_sleep(state);

    return (seconds_until_sleep > 0 && seconds_until_sleep <= SLEEP_WARNING_SECONDS);
}

/*
 * Mark warning as displayed
 */
void power_manager_mark_warning_shown(power_manager_state_t *state) {
    if (state == NULL) {
        return;
    }

    state->warning_displayed = true;
}

/*
 * Set auto-sleep timeout
 */
void power_manager_set_timeout(power_manager_state_t *state, uint32_t timeout_minutes) {
    if (state == NULL) {
        return;
    }

    if (timeout_minutes == 0) {
        /* Disable auto-sleep */
        state->idle_timeout_seconds = 0;
        state->auto_sleep_enabled = false;
    } else {
        /* Convert minutes to seconds */
        state->idle_timeout_seconds = timeout_minutes * 60;
        state->auto_sleep_enabled = true;
    }

    /* Reset warning flag when timeout changes */
    state->warning_displayed = false;

    printf("Auto-sleep timeout set to %u minutes\n", timeout_minutes);
}

/*
 * Enable or disable auto-sleep
 */
void power_manager_set_enabled(power_manager_state_t *state, bool enabled) {
    if (state == NULL) {
        return;
    }

    state->auto_sleep_enabled = enabled;

    /* Reset warning flag when state changes */
    state->warning_displayed = false;

    printf("Auto-sleep %s\n", enabled ? "enabled" : "disabled");
}

/*
 * Check if auto-sleep is enabled
 */
bool power_manager_is_enabled(const power_manager_state_t *state) {
    if (state == NULL) {
        return false;
    }

    return state->auto_sleep_enabled;
}
