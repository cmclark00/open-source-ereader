/*
 * text_input.c - Simple Text Input System Implementation
 *
 * Sequential character selection for entering text with physical buttons.
 * Character set: A-Z, a-z, 0-9, symbols, SPACE, BACKSPACE, DONE, CANCEL
 *
 * Phase 06: WiFi and Network Features
 */

#include "text_input.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Character Set for Sequential Navigation
 *
 * Total: 93 characters
 * - 26 uppercase letters (A-Z)
 * - 26 lowercase letters (a-z)
 * - 10 digits (0-9)
 * - 28 symbols (common punctuation and special characters)
 * - 1 space
 * - 2 control characters (BACKSPACE, DONE, CANCEL)
 */
static const char TEXT_INPUT_CHARSET[] = {
    /* Uppercase letters (0-25) */
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

    /* Lowercase letters (26-51) */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

    /* Digits (52-61) */
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    /* Common symbols (62-89) */
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '=', '+',
    '[', ']', '{', '}', '\\', '|', ';', ':', '\'', '"', '<', '>', ',', '.',
    '/', '?', '~', '`',

    /* Space (90) */
    ' ',

    /* Special control characters (91-93) - represented as special values */
    '\x08',  /* BACKSPACE (91) */
    '\x0D',  /* DONE (92) - using carriage return as marker */
    '\x1B'   /* CANCEL (93) - using escape as marker */
};

/* Display names for special characters */
static const char* TEXT_INPUT_CHAR_NAMES[] = {
    [' '] = "SPACE",
    ['\x08'] = "BACKSPACE",
    ['\x0D'] = "DONE",
    ['\x1B'] = "CANCEL"
};

/* Animation configuration */
#define BLINK_RATE      30      /* Frames per blink cycle (at 60fps, ~0.5 seconds) */

/*
 * Internal Helper Functions
 */

static void text_input_render_title(text_input_state_t *input, framebuffer_t *fb);
static void text_input_render_text_display(text_input_state_t *input, framebuffer_t *fb);
static void text_input_render_character_selector(text_input_state_t *input, framebuffer_t *fb);
static void text_input_render_hints(text_input_state_t *input, framebuffer_t *fb);
static void text_input_draw_box(framebuffer_t *fb, int x, int y, int width, int height);

/*
 * Public API Implementation
 */

text_input_state_t* text_input_create(const char *title, text_input_mode_t mode, int min_length, int max_length) {
    if (!title || max_length < 0 || max_length >= TEXT_INPUT_MAX_LENGTH) {
        return NULL;
    }

    text_input_state_t *input = (text_input_state_t*)calloc(1, sizeof(text_input_state_t));
    if (!input) {
        return NULL;
    }

    /* Copy title */
    strncpy(input->title, title, sizeof(input->title) - 1);
    input->title[sizeof(input->title) - 1] = '\0';

    /* Initialize settings */
    input->mode = mode;
    input->min_length = (min_length < 0) ? 0 : min_length;
    input->max_length = (max_length == 0) ? TEXT_INPUT_MAX_LENGTH - 1 : max_length;

    /* Initialize state */
    input->text[0] = '\0';
    input->text_length = 0;
    input->current_char_index = 0;
    input->current_char = TEXT_INPUT_CHARSET[0];
    input->result = TEXT_INPUT_RESULT_NONE;
    input->needs_redraw = true;
    input->blink_counter = 0;
    input->prompt[0] = '\0';

    return input;
}

void text_input_free(text_input_state_t *input) {
    if (input) {
        free(input);
    }
}

void text_input_reset(text_input_state_t *input) {
    if (!input) {
        return;
    }

    input->text[0] = '\0';
    input->text_length = 0;
    input->current_char_index = 0;
    input->current_char = TEXT_INPUT_CHARSET[0];
    input->result = TEXT_INPUT_RESULT_NONE;
    input->needs_redraw = true;
    input->blink_counter = 0;
}

void text_input_set_prompt(text_input_state_t *input, const char *prompt) {
    if (!input || !prompt) {
        return;
    }

    strncpy(input->prompt, prompt, sizeof(input->prompt) - 1);
    input->prompt[sizeof(input->prompt) - 1] = '\0';
    input->needs_redraw = true;
}

int text_input_render(text_input_state_t *input, framebuffer_t *fb) {
    if (!input || !fb) {
        return -1;
    }

    /* Clear screen */
    framebuffer_clear(fb, COLOR_WHITE);

    /* Render components */
    text_input_render_title(input, fb);
    text_input_render_text_display(input, fb);
    text_input_render_character_selector(input, fb);
    text_input_render_hints(input, fb);

    /* Clear redraw flag */
    input->needs_redraw = false;

    return 0;
}

int text_input_handle_button(text_input_state_t *input, button_t button) {
    if (!input) {
        return -1;
    }

    /* Ignore input if already done */
    if (input->result != TEXT_INPUT_RESULT_NONE) {
        return 0;
    }

    switch (button) {
        case BUTTON_UP:
            /* Previous character (with wrap-around) */
            input->current_char_index--;
            if (input->current_char_index < 0) {
                input->current_char_index = TEXT_INPUT_CHARSET_SIZE - 1;
            }
            input->current_char = TEXT_INPUT_CHARSET[input->current_char_index];
            input->needs_redraw = true;
            break;

        case BUTTON_DOWN:
            /* Next character (with wrap-around) */
            input->current_char_index++;
            if (input->current_char_index >= TEXT_INPUT_CHARSET_SIZE) {
                input->current_char_index = 0;
            }
            input->current_char = TEXT_INPUT_CHARSET[input->current_char_index];
            input->needs_redraw = true;
            break;

        case BUTTON_SELECT:
            /* Select current character */
            if (input->current_char == '\x08') {
                /* BACKSPACE */
                if (input->text_length > 0) {
                    input->text_length--;
                    input->text[input->text_length] = '\0';
                    input->needs_redraw = true;
                }
            } else if (input->current_char == '\x0D') {
                /* DONE */
                if (input->text_length >= input->min_length) {
                    input->result = TEXT_INPUT_RESULT_DONE;
                    input->needs_redraw = true;
                }
                /* If length too short, ignore (could add visual feedback) */
            } else if (input->current_char == '\x1B') {
                /* CANCEL */
                input->result = TEXT_INPUT_RESULT_CANCELLED;
                input->needs_redraw = true;
            } else {
                /* Regular character */
                if (input->text_length < input->max_length) {
                    input->text[input->text_length] = input->current_char;
                    input->text_length++;
                    input->text[input->text_length] = '\0';
                    input->needs_redraw = true;
                }
                /* If at max length, ignore (could add visual feedback) */
            }
            break;

        case BUTTON_BACK:
            /* Cancel input */
            input->result = TEXT_INPUT_RESULT_CANCELLED;
            input->needs_redraw = true;
            break;

        case BUTTON_MENU:
            /* Quick access to backspace */
            if (input->text_length > 0) {
                input->text_length--;
                input->text[input->text_length] = '\0';
                input->needs_redraw = true;
            }
            break;

        default:
            break;
    }

    return 0;
}

bool text_input_is_done(text_input_state_t *input) {
    if (!input) {
        return false;
    }
    return input->result != TEXT_INPUT_RESULT_NONE;
}

text_input_result_t text_input_get_result(text_input_state_t *input) {
    if (!input) {
        return TEXT_INPUT_RESULT_NONE;
    }
    return input->result;
}

const char* text_input_get_text(text_input_state_t *input) {
    if (!input) {
        return NULL;
    }
    return input->text;
}

int text_input_get_length(text_input_state_t *input) {
    if (!input) {
        return 0;
    }
    return input->text_length;
}

bool text_input_needs_redraw(text_input_state_t *input) {
    if (!input) {
        return false;
    }
    return input->needs_redraw;
}

void text_input_clear_redraw_flag(text_input_state_t *input) {
    if (input) {
        input->needs_redraw = false;
    }
}

void text_input_update_animation(text_input_state_t *input) {
    if (!input) {
        return;
    }

    input->blink_counter++;
    if (input->blink_counter >= BLINK_RATE * 2) {
        input->blink_counter = 0;
    }

    /* Request redraw if cursor should blink */
    if (input->blink_counter % BLINK_RATE == 0) {
        input->needs_redraw = true;
    }
}

char text_input_get_char_at_index(int index) {
    if (index < 0 || index >= TEXT_INPUT_CHARSET_SIZE) {
        return '\0';
    }
    return TEXT_INPUT_CHARSET[index];
}

const char* text_input_get_char_name(char c) {
    /* Check special characters */
    if (c == ' ') return "SPACE";
    if (c == '\x08') return "BACKSPACE";
    if (c == '\x0D') return "DONE";
    if (c == '\x1B') return "CANCEL";

    /* Regular character - return NULL (caller should display character itself) */
    return NULL;
}

/*
 * Internal Rendering Functions
 */

static void text_input_render_title(text_input_state_t *input, framebuffer_t *fb) {
    /* Draw title at top center */
    int title_width = strlen(input->title) * 8;  /* 8 pixels per char */
    int x = (FB_WIDTH - title_width) / 2;

    text_render_string(fb, x, 10, input->title, COLOR_BLACK);

    /* Draw separator line */
    for (int i = 10; i < FB_WIDTH - 10; i++) {
        framebuffer_set_pixel(fb, i, 28, COLOR_BLACK);
    }
}

static void text_input_render_text_display(text_input_state_t *input, framebuffer_t *fb) {
    /* Display entered text at top of input area */
    char display_text[TEXT_INPUT_MAX_LENGTH + 20];

    if (input->mode == TEXT_INPUT_MODE_PASSWORD && input->text_length > 0) {
        /* Password mode: show first 3 chars, then asterisks */
        int visible_chars = (input->text_length > 3) ? 3 : input->text_length;
        strncpy(display_text, input->text, visible_chars);
        display_text[visible_chars] = '\0';

        /* Add asterisks for remaining characters */
        for (int i = visible_chars; i < input->text_length; i++) {
            strcat(display_text, "*");
        }
    } else {
        /* Normal mode: show full text */
        strcpy(display_text, input->text);
    }

    /* Add cursor if blinking on */
    bool show_cursor = (input->blink_counter < BLINK_RATE);
    if (show_cursor && input->result == TEXT_INPUT_RESULT_NONE) {
        strcat(display_text, "_");
    }

    /* Center the text display */
    int text_width = strlen(display_text) * 8;
    int x = (FB_WIDTH - text_width) / 2;
    if (x < 10) x = 10;  /* Minimum margin */

    text_render_string(fb, x, 45, display_text, COLOR_BLACK);

    /* Show character count */
    char count_text[32];
    snprintf(count_text, sizeof(count_text), "Length: %d/%d", input->text_length, input->max_length);
    int count_width = strlen(count_text) * 8;
    x = (FB_WIDTH - count_width) / 2;
    text_render_string(fb, x, 65, count_text, COLOR_BLACK);
}

static void text_input_render_character_selector(text_input_state_t *input, framebuffer_t *fb) {
    /* Large character display in center */
    int y_center = 130;

    /* Draw box around character */
    text_input_draw_box(fb, FB_WIDTH/2 - 50, y_center - 30, 100, 60);

    /* Display current character */
    const char *char_name = text_input_get_char_name(input->current_char);

    if (char_name) {
        /* Special character - show name */
        int name_width = strlen(char_name) * 8;
        int x = (FB_WIDTH - name_width) / 2;
        text_render_string(fb, x, y_center - 8, char_name, COLOR_BLACK);
    } else {
        /* Regular character - show large */
        char char_str[2] = {input->current_char, '\0'};

        /* Draw character 3x size (simulate by drawing 3x3 grid) */
        int base_x = FB_WIDTH/2 - 12;  /* Center 3x8 = 24 pixels wide */
        int base_y = y_center - 24;    /* Center 3x16 = 48 pixels tall */

        for (int dy = 0; dy < 3; dy++) {
            for (int dx = 0; dx < 3; dx++) {
                text_render_string(fb, base_x + dx * 8, base_y + dy * 16, char_str, COLOR_BLACK);
            }
        }
    }

    /* Show current position in charset */
    char position_text[32];
    snprintf(position_text, sizeof(position_text), "Char %d of %d",
             input->current_char_index + 1, TEXT_INPUT_CHARSET_SIZE);
    int pos_width = strlen(position_text) * 8;
    int x = (FB_WIDTH - pos_width) / 2;
    text_render_string(fb, x, y_center + 40, position_text, COLOR_BLACK);
}

static void text_input_render_hints(text_input_state_t *input, framebuffer_t *fb) {
    /* Show button hints at bottom */
    const char *hint1 = "UP/DOWN: Change char";
    const char *hint2 = "SELECT: Add char";
    const char *hint3 = "MENU: Backspace  BACK: Cancel";

    int y_base = FB_HEIGHT - 50;

    /* Center each hint line */
    int hint1_width = strlen(hint1) * 8;
    int hint2_width = strlen(hint2) * 8;
    int hint3_width = strlen(hint3) * 8;

    text_render_string(fb, (FB_WIDTH - hint1_width) / 2, y_base, hint1, COLOR_BLACK);
    text_render_string(fb, (FB_WIDTH - hint2_width) / 2, y_base + 16, hint2, COLOR_BLACK);
    text_render_string(fb, (FB_WIDTH - hint3_width) / 2, y_base + 32, hint3, COLOR_BLACK);

    /* Show prompt if set */
    if (input->prompt[0] != '\0') {
        int prompt_width = strlen(input->prompt) * 8;
        text_render_string(fb, (FB_WIDTH - prompt_width) / 2, y_base - 20, input->prompt, COLOR_BLACK);
    }
}

static void text_input_draw_box(framebuffer_t *fb, int x, int y, int width, int height) {
    /* Draw rectangle border */
    /* Top line */
    for (int i = x; i < x + width; i++) {
        framebuffer_set_pixel(fb, i, y, COLOR_BLACK);
        framebuffer_set_pixel(fb, i, y + 1, COLOR_BLACK);
    }

    /* Bottom line */
    for (int i = x; i < x + width; i++) {
        framebuffer_set_pixel(fb, i, y + height - 2, COLOR_BLACK);
        framebuffer_set_pixel(fb, i, y + height - 1, COLOR_BLACK);
    }

    /* Left line */
    for (int i = y; i < y + height; i++) {
        framebuffer_set_pixel(fb, x, i, COLOR_BLACK);
        framebuffer_set_pixel(fb, x + 1, i, COLOR_BLACK);
    }

    /* Right line */
    for (int i = y; i < y + height; i++) {
        framebuffer_set_pixel(fb, x + width - 2, i, COLOR_BLACK);
        framebuffer_set_pixel(fb, x + width - 1, i, COLOR_BLACK);
    }
}
