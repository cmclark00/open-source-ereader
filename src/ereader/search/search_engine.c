/*
 * search_engine.c - E-Reader Text Search Engine Implementation
 *
 * Implements text search capabilities within the currently loaded book.
 *
 * Phase 05: Polish and Advanced Features
 */

#include "search_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Internal helper function prototypes */
static char* search_to_lowercase(const char *str);
static int search_strstr_case_insensitive(const char *haystack, const char *needle, size_t haystack_len);

/*
 * Search Context Management
 */

search_context_t* search_create(const char *text, size_t text_length, pagination_t *pagination) {
    if (!text || !pagination) {
        return NULL;
    }

    search_context_t *ctx = (search_context_t*)calloc(1, sizeof(search_context_t));
    if (!ctx) {
        return NULL;
    }

    ctx->source_text = text;
    ctx->source_length = text_length;
    ctx->pagination = pagination;
    ctx->current_result = -1;
    ctx->search_active = false;
    ctx->case_sensitive = false;

    /* Allocate results array */
    ctx->results = (search_result_t*)malloc(sizeof(search_result_t) * MAX_SEARCH_RESULTS);
    if (!ctx->results) {
        free(ctx);
        return NULL;
    }

    ctx->result_count = 0;
    ctx->search_term[0] = '\0';

    return ctx;
}

void search_free(search_context_t *ctx) {
    if (ctx) {
        if (ctx->results) {
            free(ctx->results);
        }
        free(ctx);
    }
}

void search_clear(search_context_t *ctx) {
    if (!ctx) {
        return;
    }

    ctx->result_count = 0;
    ctx->current_result = -1;
    ctx->search_active = false;
    ctx->search_term[0] = '\0';
}

/*
 * Search Operations
 */

int search_execute(search_context_t *ctx, const char *term, bool case_sensitive) {
    if (!ctx || !term) {
        return SEARCH_ERROR_NULL_POINTER;
    }

    /* Validate search term */
    size_t term_len = strlen(term);
    if (term_len == 0 || term_len >= MAX_SEARCH_TERM_LENGTH) {
        return SEARCH_ERROR_INVALID_TERM;
    }

    /* Clear previous search results */
    search_clear(ctx);

    /* Store search parameters */
    strncpy(ctx->search_term, term, MAX_SEARCH_TERM_LENGTH - 1);
    ctx->search_term[MAX_SEARCH_TERM_LENGTH - 1] = '\0';
    ctx->case_sensitive = case_sensitive;

    /* Prepare search strings for case-insensitive search */
    char *search_text = NULL;
    char *search_term = NULL;

    if (!case_sensitive) {
        search_text = search_to_lowercase(ctx->source_text);
        search_term = search_to_lowercase(term);
        if (!search_text || !search_term) {
            free(search_text);
            free(search_term);
            return SEARCH_ERROR_OUT_OF_MEMORY;
        }
    }

    /* Search for all occurrences */
    const char *text_to_search = case_sensitive ? ctx->source_text : search_text;
    const char *term_to_search = case_sensitive ? term : search_term;
    const char *current_pos = text_to_search;
    const char *text_end = text_to_search + ctx->source_length;
    int offset = 0;

    while (current_pos < text_end && ctx->result_count < MAX_SEARCH_RESULTS) {
        const char *match = strstr(current_pos, term_to_search);
        if (!match) {
            break;
        }

        /* Calculate offset in original text */
        offset = (int)(match - text_to_search);

        /* Find the page number for this offset */
        int page = search_find_page_for_offset(ctx->pagination, offset);
        if (page >= 0) {
            /* Store result */
            ctx->results[ctx->result_count].offset = offset;
            ctx->results[ctx->result_count].page_number = page;
            ctx->results[ctx->result_count].line_in_page = 0;  /* TODO: Calculate if needed */
            ctx->results[ctx->result_count].char_in_line = 0;  /* TODO: Calculate if needed */
            ctx->result_count++;
        }

        /* Move to next character after this match */
        current_pos = match + 1;
    }

    /* Clean up case-insensitive search buffers */
    if (!case_sensitive) {
        free(search_text);
        free(search_term);
    }

    /* Set search as active if we found results */
    if (ctx->result_count > 0) {
        ctx->search_active = true;
        ctx->current_result = 0;  /* Start at first result */
        return ctx->result_count;
    } else {
        ctx->search_active = false;
        return SEARCH_ERROR_NO_RESULTS;
    }
}

int search_next_result(search_context_t *ctx) {
    if (!ctx || !ctx->search_active || ctx->result_count == 0) {
        return -1;
    }

    if (ctx->current_result < ctx->result_count - 1) {
        ctx->current_result++;
        return ctx->current_result;
    }

    /* Wrap around to first result */
    ctx->current_result = 0;
    return ctx->current_result;
}

int search_prev_result(search_context_t *ctx) {
    if (!ctx || !ctx->search_active || ctx->result_count == 0) {
        return -1;
    }

    if (ctx->current_result > 0) {
        ctx->current_result--;
        return ctx->current_result;
    }

    /* Wrap around to last result */
    ctx->current_result = ctx->result_count - 1;
    return ctx->current_result;
}

search_result_t* search_get_current_result(search_context_t *ctx) {
    if (!ctx || !ctx->search_active || ctx->current_result < 0) {
        return NULL;
    }

    return &ctx->results[ctx->current_result];
}

search_result_t* search_get_result(search_context_t *ctx, int index) {
    if (!ctx || index < 0 || index >= ctx->result_count) {
        return NULL;
    }

    return &ctx->results[index];
}

/*
 * Search State Queries
 */

bool search_is_active(search_context_t *ctx) {
    if (!ctx) {
        return false;
    }
    return ctx->search_active;
}

int search_get_result_count(search_context_t *ctx) {
    if (!ctx) {
        return 0;
    }
    return ctx->result_count;
}

int search_get_current_index(search_context_t *ctx) {
    if (!ctx || !ctx->search_active) {
        return -1;
    }
    return ctx->current_result;
}

const char* search_get_term(search_context_t *ctx) {
    if (!ctx) {
        return "";
    }
    return ctx->search_term;
}

/*
 * Utility Functions
 */

int search_find_page_for_offset(pagination_t *pagination, int offset) {
    if (!pagination || offset < 0) {
        return -1;
    }

    /* Search through pages to find which one contains this offset */
    for (int i = 0; i < pagination->page_count; i++) {
        text_page_t *page = text_get_page(pagination, i);
        if (!page) {
            continue;
        }

        /* Check if offset falls within this page's range */
        if (offset >= page->start_offset && offset <= page->end_offset) {
            return i;
        }
    }

    return -1;  /* Offset not found in any page */
}

const char* search_error_string(search_error_t error) {
    switch (error) {
        case SEARCH_SUCCESS:
            return "Success";
        case SEARCH_ERROR_NULL_POINTER:
            return "Null pointer error";
        case SEARCH_ERROR_INVALID_TERM:
            return "Invalid search term";
        case SEARCH_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case SEARCH_ERROR_NO_RESULTS:
            return "No results found";
        case SEARCH_ERROR_NO_ACTIVE_SEARCH:
            return "No active search";
        default:
            return "Unknown error";
    }
}

/*
 * Internal Helper Functions
 */

/**
 * Convert a string to lowercase (allocates new string)
 *
 * @param str: String to convert
 * @return: Newly allocated lowercase string, or NULL on error (caller must free)
 */
static char* search_to_lowercase(const char *str) {
    if (!str) {
        return NULL;
    }

    size_t len = strlen(str);
    char *result = (char*)malloc(len + 1);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = tolower((unsigned char)str[i]);
    }
    result[len] = '\0';

    return result;
}
