/*
 * search_ui.c - E-Reader Search User Interface Implementation
 *
 * Implements UI for text search within books.
 *
 * Phase 05: Polish and Advanced Features
 */

#include "search_ui.h"
#include "../rendering/text_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal helper function prototypes */
static void search_ui_execute_search(search_ui_state_t *ui);
static void search_ui_render_header(framebuffer_t *fb, const char *title);
static void search_ui_render_hints(framebuffer_t *fb, const char *hints);

/*
 * Search UI Initialization and Cleanup
 */

search_ui_state_t* search_ui_create(const char *text, size_t text_length,
                                    pagination_t *pagination,
                                    const char **predefined_terms, int term_count) {
    if (!text || !pagination || term_count < 0 || term_count > SEARCH_UI_MAX_PREDEFINED_TERMS) {
        return NULL;
    }

    search_ui_state_t *ui = (search_ui_state_t*)calloc(1, sizeof(search_ui_state_t));
    if (!ui) {
        return NULL;
    }

    /* Create search engine context */
    ui->search_ctx = search_create(text, text_length, pagination);
    if (!ui->search_ctx) {
        free(ui);
        return NULL;
    }

    /* Copy predefined search terms */
    if (term_count > 0 && predefined_terms) {
        ui->predefined_terms = (char**)malloc(sizeof(char*) * term_count);
        if (!ui->predefined_terms) {
            search_free(ui->search_ctx);
            free(ui);
            return NULL;
        }

        ui->predefined_count = 0;
        for (int i = 0; i < term_count; i++) {
            if (predefined_terms[i]) {
                ui->predefined_terms[i] = strdup(predefined_terms[i]);
                if (!ui->predefined_terms[i]) {
                    /* Clean up on error */
                    for (int j = 0; j < i; j++) {
                        free(ui->predefined_terms[j]);
                    }
                    free(ui->predefined_terms);
                    search_free(ui->search_ctx);
                    free(ui);
                    return NULL;
                }
                ui->predefined_count++;
            }
        }
    } else {
        ui->predefined_terms = NULL;
        ui->predefined_count = 0;
    }

    ui->mode = SEARCH_MODE_TERM_SELECTION;
    ui->selected_term = 0;
    ui->case_sensitive = false;
    ui->needs_redraw = true;

    return ui;
}

void search_ui_free(search_ui_state_t *ui) {
    if (ui) {
        if (ui->search_ctx) {
            search_free(ui->search_ctx);
        }
        if (ui->predefined_terms) {
            for (int i = 0; i < ui->predefined_count; i++) {
                free(ui->predefined_terms[i]);
            }
            free(ui->predefined_terms);
        }
        free(ui);
    }
}

/*
 * Search UI Rendering
 */

int search_ui_render(search_ui_state_t *ui, framebuffer_t *fb) {
    if (!ui || !fb) {
        return -1;
    }

    /* Clear framebuffer */
    fb_clear(fb, COLOR_WHITE);

    /* Render based on current mode */
    switch (ui->mode) {
        case SEARCH_MODE_TERM_SELECTION:
            return search_ui_render_term_selection(ui, fb);

        case SEARCH_MODE_SEARCHING:
            return search_ui_render_searching(ui, fb);

        case SEARCH_MODE_RESULTS:
            return search_ui_render_results(ui, fb);

        case SEARCH_MODE_NO_RESULTS:
            return search_ui_render_no_results(ui, fb);

        default:
            return -1;
    }
}

int search_ui_render_term_selection(search_ui_state_t *ui, framebuffer_t *fb) {
    if (!ui || !fb) {
        return -1;
    }

    /* Render header */
    search_ui_render_header(fb, "Search");

    /* Render instructions */
    int y = 40;
    text_render_string(fb, 10, y, "Select search term:", COLOR_BLACK);
    y += 24;

    /* Render predefined terms or message */
    if (ui->predefined_count == 0) {
        y += 20;
        text_render_string(fb, 10, y, "No predefined search terms.", COLOR_BLACK);
        y += 20;
        text_render_string(fb, 10, y, "Text input via buttons", COLOR_BLACK);
        y += 16;
        text_render_string(fb, 10, y, "deferred to Phase 6.", COLOR_BLACK);
    } else {
        for (int i = 0; i < ui->predefined_count && y < 240; i++) {
            char line[128];
            if (i == ui->selected_term) {
                snprintf(line, sizeof(line), "> %s", ui->predefined_terms[i]);
            } else {
                snprintf(line, sizeof(line), "  %s", ui->predefined_terms[i]);
            }
            text_render_string(fb, 10, y, line, COLOR_BLACK);
            y += 18;
        }
    }

    /* Render case sensitivity toggle */
    y = 220;
    char case_toggle[64];
    snprintf(case_toggle, sizeof(case_toggle), "Case sensitive: %s",
             ui->case_sensitive ? "ON" : "OFF");
    text_render_string(fb, 10, y, case_toggle, COLOR_BLACK);

    /* Render control hints */
    if (ui->predefined_count > 0) {
        search_ui_render_hints(fb, "UP/DN:Select SELECT:Search MENU:Toggle BACK:Exit");
    } else {
        search_ui_render_hints(fb, "BACK:Exit");
    }

    return 0;
}

int search_ui_render_results(search_ui_state_t *ui, framebuffer_t *fb) {
    if (!ui || !fb) {
        return -1;
    }

    search_context_t *ctx = ui->search_ctx;
    if (!ctx || !search_is_active(ctx)) {
        return -1;
    }

    /* Render header */
    char header[128];
    snprintf(header, sizeof(header), "Search: \"%s\"", search_get_term(ctx));
    search_ui_render_header(fb, header);

    /* Render result counter */
    int y = 40;
    char result_info[128];
    int current = search_get_current_index(ctx);
    int total = search_get_result_count(ctx);
    snprintf(result_info, sizeof(result_info), "Result %d of %d", current + 1, total);
    text_render_string(fb, 10, y, result_info, COLOR_BLACK);
    y += 24;

    /* Get current result */
    search_result_t *result = search_get_current_result(ctx);
    if (result) {
        /* Show page number */
        char page_info[64];
        snprintf(page_info, sizeof(page_info), "Page: %d", result->page_number + 1);
        text_render_string(fb, 10, y, page_info, COLOR_BLACK);
        y += 24;

        /* Show context around the match */
        const char *text = ctx->source_text;
        int offset = result->offset;
        int term_len = strlen(search_get_term(ctx));

        /* Calculate context window */
        int context_before = 30;
        int context_after = 30;
        int start = (offset - context_before < 0) ? 0 : offset - context_before;
        int end = offset + term_len + context_after;
        if (end > (int)ctx->source_length) {
            end = ctx->source_length;
        }

        /* Extract context */
        char context[256];
        int ctx_len = end - start;
        if (ctx_len > 0 && ctx_len < (int)sizeof(context) - 1) {
            strncpy(context, text + start, ctx_len);
            context[ctx_len] = '\0';

            /* Replace newlines with spaces for display */
            for (int i = 0; i < ctx_len; i++) {
                if (context[i] == '\n' || context[i] == '\r') {
                    context[i] = ' ';
                }
            }

            /* Render context */
            y += 10;
            text_render_string(fb, 10, y, "Context:", COLOR_BLACK);
            y += 18;

            /* Word-wrap the context */
            char *line_start = context;
            char *line_end;
            while (*line_start && y < 240) {
                /* Find end of line that fits */
                int chars_per_line = text_renderer_get_chars_per_line();
                line_end = line_start + chars_per_line;
                if (line_end > context + ctx_len) {
                    line_end = context + ctx_len;
                }

                /* Break at word boundary if possible */
                if (*line_end && line_end > line_start) {
                    while (line_end > line_start && *line_end != ' ') {
                        line_end--;
                    }
                    if (line_end == line_start) {
                        line_end = line_start + chars_per_line;
                    }
                }

                /* Render line */
                char line_buf[128];
                int line_len = line_end - line_start;
                if (line_len > 0 && line_len < (int)sizeof(line_buf) - 1) {
                    strncpy(line_buf, line_start, line_len);
                    line_buf[line_len] = '\0';
                    text_render_string(fb, 15, y, line_buf, COLOR_BLACK);
                    y += 16;
                }

                line_start = line_end;
                while (*line_start == ' ') {
                    line_start++;
                }
            }
        }
    }

    /* Render control hints */
    search_ui_render_hints(fb, "UP:Prev DN:Next SEL:Go BACK:Exit");

    return 0;
}

int search_ui_render_no_results(search_ui_state_t *ui, framebuffer_t *fb) {
    if (!ui || !fb) {
        return -1;
    }

    /* Render header */
    search_ui_render_header(fb, "Search");

    /* Render message */
    int y = 100;
    text_render_string(fb, 10, y, "No results found for:", COLOR_BLACK);
    y += 24;

    char term_display[128];
    snprintf(term_display, sizeof(term_display), "\"%s\"", search_get_term(ui->search_ctx));
    text_render_string(fb, 10, y, term_display, COLOR_BLACK);

    /* Render control hints */
    search_ui_render_hints(fb, "BACK:Return to search");

    return 0;
}

int search_ui_render_searching(search_ui_state_t *ui, framebuffer_t *fb) {
    if (!ui || !fb) {
        return -1;
    }

    /* Render header */
    search_ui_render_header(fb, "Search");

    /* Render message */
    int y = 100;
    text_render_string(fb, 10, y, "Searching...", COLOR_BLACK);

    return 0;
}

/*
 * Search UI Input Handling
 */

search_ui_action_t search_ui_handle_event(search_ui_state_t *ui, const button_event_t *event) {
    if (!ui || !event) {
        return SEARCH_UI_ACTION_NONE;
    }

    /* Only handle PRESS events */
    if (event->type != BUTTON_EVENT_PRESS) {
        return SEARCH_UI_ACTION_NONE;
    }

    switch (ui->mode) {
        case SEARCH_MODE_TERM_SELECTION:
            /* Handle term selection mode */
            switch (event->button) {
                case BUTTON_UP:
                    if (ui->predefined_count > 0 && ui->selected_term > 0) {
                        ui->selected_term--;
                        ui->needs_redraw = true;
                        return SEARCH_UI_ACTION_REDRAW;
                    }
                    break;

                case BUTTON_DOWN:
                    if (ui->predefined_count > 0 && ui->selected_term < ui->predefined_count - 1) {
                        ui->selected_term++;
                        ui->needs_redraw = true;
                        return SEARCH_UI_ACTION_REDRAW;
                    }
                    break;

                case BUTTON_SELECT:
                    /* Execute search */
                    if (ui->predefined_count > 0) {
                        search_ui_execute_search(ui);
                        ui->needs_redraw = true;
                        return SEARCH_UI_ACTION_REDRAW;
                    }
                    break;

                case BUTTON_MENU:
                    /* Toggle case sensitivity */
                    ui->case_sensitive = !ui->case_sensitive;
                    ui->needs_redraw = true;
                    return SEARCH_UI_ACTION_REDRAW;

                case BUTTON_BACK:
                    return SEARCH_UI_ACTION_EXIT;

                default:
                    break;
            }
            break;

        case SEARCH_MODE_RESULTS:
            /* Handle results navigation */
            switch (event->button) {
                case BUTTON_UP:
                    search_prev_result(ui->search_ctx);
                    ui->needs_redraw = true;
                    return SEARCH_UI_ACTION_REDRAW;

                case BUTTON_DOWN:
                    search_next_result(ui->search_ctx);
                    ui->needs_redraw = true;
                    return SEARCH_UI_ACTION_REDRAW;

                case BUTTON_SELECT:
                    /* Navigate to result page */
                    return SEARCH_UI_ACTION_NAVIGATE;

                case BUTTON_BACK:
                    /* Return to term selection */
                    ui->mode = SEARCH_MODE_TERM_SELECTION;
                    ui->needs_redraw = true;
                    return SEARCH_UI_ACTION_REDRAW;

                default:
                    break;
            }
            break;

        case SEARCH_MODE_NO_RESULTS:
            /* Return to term selection on any button */
            if (event->button == BUTTON_BACK || event->button == BUTTON_SELECT) {
                ui->mode = SEARCH_MODE_TERM_SELECTION;
                ui->needs_redraw = true;
                return SEARCH_UI_ACTION_REDRAW;
            }
            break;

        case SEARCH_MODE_SEARCHING:
            /* Ignore input while searching */
            break;
    }

    return SEARCH_UI_ACTION_NONE;
}

/*
 * Search UI State Queries
 */

int search_ui_get_target_page(search_ui_state_t *ui) {
    if (!ui || !ui->search_ctx) {
        return -1;
    }

    search_result_t *result = search_get_current_result(ui->search_ctx);
    if (result) {
        return result->page_number;
    }

    return -1;
}

bool search_ui_needs_redraw(search_ui_state_t *ui) {
    if (!ui) {
        return false;
    }
    return ui->needs_redraw;
}

void search_ui_clear_redraw_flag(search_ui_state_t *ui) {
    if (ui) {
        ui->needs_redraw = false;
    }
}

/*
 * Internal Helper Functions
 */

static void search_ui_execute_search(search_ui_state_t *ui) {
    if (!ui || ui->predefined_count == 0) {
        return;
    }

    /* Set to searching mode */
    ui->mode = SEARCH_MODE_SEARCHING;

    /* Get selected term */
    const char *term = ui->predefined_terms[ui->selected_term];

    /* Execute search */
    int result = search_execute(ui->search_ctx, term, ui->case_sensitive);

    /* Update mode based on results */
    if (result > 0) {
        ui->mode = SEARCH_MODE_RESULTS;
    } else {
        ui->mode = SEARCH_MODE_NO_RESULTS;
    }
}

static void search_ui_render_header(framebuffer_t *fb, const char *title) {
    if (!fb || !title) {
        return;
    }

    /* Render title */
    text_render_string(fb, 10, 10, title, COLOR_BLACK);

    /* Draw separator line */
    int font_height = text_renderer_get_font_height();
    int y = 10 + font_height + 4;
    fb_draw_line(fb, 0, y, FB_WIDTH - 1, y, COLOR_BLACK);
}

static void search_ui_render_hints(framebuffer_t *fb, const char *hints) {
    if (!fb || !hints) {
        return;
    }

    /* Draw separator line */
    int y = FB_HEIGHT - 30;
    fb_draw_line(fb, 0, y, FB_WIDTH - 1, y, COLOR_BLACK);

    /* Render hints */
    y += 6;
    text_render_string(fb, 10, y, hints, COLOR_BLACK);
}
