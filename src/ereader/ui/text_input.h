/*
 * text_input.h - Simple Text Input System for E-Reader
 *
 * Implements a sequential character selection system for entering text
 * using only UP/DOWN/SELECT buttons. This is designed for occasional use
 * (e.g., WiFi password entry) and prioritizes simplicity over speed.
 *
 * Usage:
 * 1. Create text input state with text_input_create()
 * 2. In your main loop:
 *    - Call text_input_render() to draw the UI
 *    - Call text_input_handle_button() for each button event
 *    - Check text_input_is_done() to detect completion
 * 3. Retrieve final text with text_input_get_text()
 * 4. Clean up with text_input_free()
 *
 * Phase 06: WiFi and Network Features
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

#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "../rendering/framebuffer.h"
#include "../../button-test/button_input.h"

/*
 * Configuration Constants
 */

#define TEXT_INPUT_MAX_LENGTH   64      /* Maximum text length (63 chars + null) */
#define TEXT_INPUT_MIN_LENGTH   0       /* Minimum text length (0 for optional) */

/* Character set for sequential navigation */
#define TEXT_INPUT_CHARSET_SIZE 93      /* Total number of selectable characters */

/*
 * Special character indices in the character set
 */
typedef enum {
    TEXT_INPUT_CHAR_BACKSPACE = 90,     /* Index of backspace in charset */
    TEXT_INPUT_CHAR_DONE = 91,          /* Index of done in charset */
    TEXT_INPUT_CHAR_CANCEL = 92         /* Index of cancel in charset */
} text_input_special_char_t;

/*
 * Text Input Mode
 */
typedef enum {
    TEXT_INPUT_MODE_NORMAL,             /* Regular text input */
    TEXT_INPUT_MODE_PASSWORD            /* Password input (show asterisks) */
} text_input_mode_t;

/*
 * Text Input Result
 */
typedef enum {
    TEXT_INPUT_RESULT_NONE,             /* Still in progress */
    TEXT_INPUT_RESULT_DONE,             /* User pressed DONE */
    TEXT_INPUT_RESULT_CANCELLED         /* User pressed CANCEL or BACK */
} text_input_result_t;

/*
 * Text Input State
 */
typedef struct {
    /* Input text buffer */
    char text[TEXT_INPUT_MAX_LENGTH];   /* Current input text */
    int text_length;                    /* Current text length */
    int max_length;                     /* Maximum allowed length */
    int min_length;                     /* Minimum required length */

    /* Character selection */
    int current_char_index;             /* Current character in charset (0-92) */
    char current_char;                  /* Currently selected character */

    /* Display configuration */
    text_input_mode_t mode;             /* Normal or password mode */
    char title[64];                     /* Title text (e.g., "Enter Password: MyWiFi") */
    char prompt[64];                    /* Optional prompt text */

    /* State */
    text_input_result_t result;         /* Completion result */
    bool needs_redraw;                  /* UI needs redrawing */

    /* Animation */
    uint32_t blink_counter;             /* Cursor blink animation counter */
} text_input_state_t;

/*
 * Public API Functions
 */

/**
 * Create a new text input state
 *
 * @param title: Title text to display (e.g., "Enter Password: MyWiFi")
 * @param mode: TEXT_INPUT_MODE_NORMAL or TEXT_INPUT_MODE_PASSWORD
 * @param min_length: Minimum required text length (0 for optional)
 * @param max_length: Maximum allowed text length (max TEXT_INPUT_MAX_LENGTH - 1)
 * @return: Pointer to text input state, or NULL on error
 */
text_input_state_t* text_input_create(const char *title, text_input_mode_t mode, int min_length, int max_length);

/**
 * Free text input state
 *
 * @param input: Text input state to free
 */
void text_input_free(text_input_state_t *input);

/**
 * Reset text input state
 *
 * @param input: Text input state to reset
 */
void text_input_reset(text_input_state_t *input);

/**
 * Set the prompt text (optional helper message)
 *
 * @param input: Text input state
 * @param prompt: Prompt text (e.g., "Password must be 8-63 characters")
 */
void text_input_set_prompt(text_input_state_t *input, const char *prompt);

/**
 * Render text input UI to framebuffer
 *
 * Displays:
 * - Title at top
 * - Current input text (with asterisks in password mode)
 * - Current character selection (large, centered)
 * - Navigation hints at bottom
 *
 * @param input: Text input state
 * @param fb: Framebuffer to render to
 * @return: 0 on success, -1 on error
 */
int text_input_render(text_input_state_t *input, framebuffer_t *fb);

/**
 * Handle button input
 *
 * @param input: Text input state
 * @param button: Button that was pressed
 * @return: 0 on success, -1 on error
 */
int text_input_handle_button(text_input_state_t *input, button_t button);

/**
 * Check if text input is complete
 *
 * @param input: Text input state
 * @return: true if user pressed DONE or CANCEL, false otherwise
 */
bool text_input_is_done(text_input_state_t *input);

/**
 * Get the result of text input
 *
 * @param input: Text input state
 * @return: TEXT_INPUT_RESULT_DONE, TEXT_INPUT_RESULT_CANCELLED, or TEXT_INPUT_RESULT_NONE
 */
text_input_result_t text_input_get_result(text_input_state_t *input);

/**
 * Get the current input text
 *
 * @param input: Text input state
 * @return: Pointer to input text (valid until next operation or free)
 */
const char* text_input_get_text(text_input_state_t *input);

/**
 * Get the current text length
 *
 * @param input: Text input state
 * @return: Current length of input text
 */
int text_input_get_length(text_input_state_t *input);

/**
 * Check if text input needs redrawing
 *
 * @param input: Text input state
 * @return: true if UI needs redrawing, false otherwise
 */
bool text_input_needs_redraw(text_input_state_t *input);

/**
 * Clear the needs_redraw flag (call after rendering)
 *
 * @param input: Text input state
 */
void text_input_clear_redraw_flag(text_input_state_t *input);

/**
 * Update animation state (call each frame, ~60fps)
 *
 * @param input: Text input state
 */
void text_input_update_animation(text_input_state_t *input);

/*
 * Utility Functions
 */

/**
 * Get character at index in character set
 *
 * @param index: Character index (0-92)
 * @return: Character at that index
 */
char text_input_get_char_at_index(int index);

/**
 * Get display name for character (e.g., "BACKSPACE", "DONE", "SPACE")
 *
 * @param c: Character to get name for
 * @return: Display name string
 */
const char* text_input_get_char_name(char c);

#endif /* TEXT_INPUT_H */
