/*
 * search_engine.c - E-Reader Text Search Engine Implementation
 *
 * Implements text search capabilities within the currently loaded book.
 * Provides case-sensitive and case-insensitive search with result navigation.
 *
 * Features:
 * - Full-text search within loaded book content
 * - Case-sensitive and case-insensitive matching
 * - Multiple result tracking (up to MAX_SEARCH_RESULTS)
 * - Page number calculation for each result
 * - Forward/backward navigation through results
 * - Circular navigation (wraps from last to first)
 *
 * Search Algorithm:
 * - Uses standard strstr() for pattern matching
 * - Linear scan through entire text (O(n*m) worst case)
 * - For case-insensitive search, creates lowercase copies of both text and search term
 * - Maps each match offset to its corresponding page number via pagination system
 *
 * Performance Considerations:
 * - Case-insensitive search requires O(n) memory for lowercase copy of entire book
 * - Search is performed on full book text, not just current page
 * - Large books (>1MB) may take 1-2 seconds to search on Pi Zero W
 * - Results are limited to MAX_SEARCH_RESULTS (1000) to prevent memory issues
 *
 * Phase 05: Polish and Advanced Features
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

/**
 * Execute a text search within the book
 *
 * Searches for all occurrences of the specified term in the book text and
 * maps each match to its corresponding page number for navigation.
 *
 * Algorithm:
 * 1. Validate and store search parameters
 * 2. For case-insensitive search: create lowercase copies of text and term
 * 3. Use strstr() to find all matches (linear scan through text)
 * 4. For each match: calculate offset and map to page number
 * 5. Store results up to MAX_SEARCH_RESULTS limit
 * 6. Clean up temporary buffers and set search active state
 *
 * Case-Insensitive Search Strategy:
 * - Allocates full lowercase copy of book text (memory-intensive for large books)
 * - Alternative would be character-by-character comparison (slower but less memory)
 * - Current approach prioritizes search speed over memory usage
 *
 * Offset-to-Page Mapping:
 * - Each match offset is converted to page number via pagination system
 * - Pagination system maintains page boundaries (start_offset, end_offset)
 * - Binary search would be more efficient but linear search is simpler
 *   for small page counts (typically <1000 pages)
 *
 * Parameters:
 *   ctx            - Search context containing text and pagination data
 *   term           - Search term (1 to MAX_SEARCH_TERM_LENGTH-1 characters)
 *   case_sensitive - true for case-sensitive, false for case-insensitive
 *
 * Returns:
 *   Number of results found (>0) on success
 *   SEARCH_ERROR_NULL_POINTER if ctx or term is NULL
 *   SEARCH_ERROR_INVALID_TERM if term is empty or too long
 *   SEARCH_ERROR_OUT_OF_MEMORY if lowercase copy allocation fails
 *   SEARCH_ERROR_NO_RESULTS if no matches found
 *
 * Side effects:
 *   - Clears any previous search results
 *   - Sets ctx->search_active to true if results found
 *   - Allocates and frees temporary memory for case-insensitive search
 *
 * Performance:
 *   - Case-sensitive: O(n*m) where n=text length, m=term length
 *   - Case-insensitive: O(n) additional for lowercase conversion
 *   - Typical large book (500KB): 1-2 seconds on Pi Zero W
 */
int search_execute(search_context_t *ctx, const char *term, bool case_sensitive) {
    if (!ctx || !term) {
        return SEARCH_ERROR_NULL_POINTER;
    }

    /* Validate search term length */
    size_t term_len = strlen(term);
    if (term_len == 0 || term_len >= MAX_SEARCH_TERM_LENGTH) {
        return SEARCH_ERROR_INVALID_TERM;
    }

    /* Clear previous search results and reset state */
    search_clear(ctx);

    /* Store search parameters for later reference */
    strncpy(ctx->search_term, term, MAX_SEARCH_TERM_LENGTH - 1);
    ctx->search_term[MAX_SEARCH_TERM_LENGTH - 1] = '\0';
    ctx->case_sensitive = case_sensitive;

    /* Prepare search strings based on case sensitivity */
    char *search_text = NULL;
    char *search_term = NULL;

    if (!case_sensitive) {
        /* Create lowercase copies for case-insensitive matching */
        search_text = search_to_lowercase(ctx->source_text);
        search_term = search_to_lowercase(term);
        if (!search_text || !search_term) {
            /* Memory allocation failed */
            free(search_text);
            free(search_term);
            return SEARCH_ERROR_OUT_OF_MEMORY;
        }
    }

    /* Select appropriate text buffers for searching */
    const char *text_to_search = case_sensitive ? ctx->source_text : search_text;
    const char *term_to_search = case_sensitive ? term : search_term;
    const char *current_pos = text_to_search;
    const char *text_end = text_to_search + ctx->source_length;
    int offset = 0;

    /* Search for all occurrences using strstr() */
    while (current_pos < text_end && ctx->result_count < MAX_SEARCH_RESULTS) {
        /* Find next occurrence of search term */
        const char *match = strstr(current_pos, term_to_search);
        if (!match) {
            break;  /* No more matches */
        }

        /* Calculate character offset in original text */
        offset = (int)(match - text_to_search);

        /* Map offset to page number via pagination system */
        int page = search_find_page_for_offset(ctx->pagination, offset);
        if (page >= 0) {
            /* Store valid result */
            ctx->results[ctx->result_count].offset = offset;
            ctx->results[ctx->result_count].page_number = page;

            /* Line/character position within page - currently unused but reserved
             * for future features like highlighting search results in text.
             * Calculation would require parsing page content to count newlines
             * and characters, which adds complexity and performance overhead.
             * For now, these are set to 0 as the page number is sufficient
             * for navigation purposes. */
            ctx->results[ctx->result_count].line_in_page = 0;
            ctx->results[ctx->result_count].char_in_line = 0;

            ctx->result_count++;
        }

        /* Move to next character to allow overlapping matches
         * Example: searching "AA" in "AAAA" finds 3 matches at positions 0, 1, 2 */
        current_pos = match + 1;
    }

    /* Clean up temporary buffers used for case-insensitive search */
    if (!case_sensitive) {
        free(search_text);
        free(search_term);
    }

    /* Update search state and return results */
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

/**
 * Find which page contains a given character offset
 *
 * Maps a character offset in the book text to its corresponding page number.
 * This is used to navigate to search results and bookmarks.
 *
 * Algorithm:
 * - Linear search through all pages checking offset ranges
 * - Each page has start_offset and end_offset defining its text span
 * - Returns first page whose range contains the offset
 *
 * Example:
 *   Page 0: offsets 0-1000
 *   Page 1: offsets 1001-2000
 *   Page 2: offsets 2001-3000
 *   search_find_page_for_offset(pagination, 1500) returns 1
 *
 * Optimization Possibilities:
 * - Binary search would be O(log n) instead of O(n) for large page counts
 * - However, typical books have <1000 pages, so linear search is fast enough
 * - Binary search would require pages to be sorted by offset (currently guaranteed)
 *
 * Parameters:
 *   pagination - Pagination structure containing page offset information
 *   offset     - Character offset in book text (0-based)
 *
 * Returns:
 *   Page number (0-based index) if offset is found
 *   -1 if offset is invalid or not found in any page
 *
 * Performance:
 *   O(n) where n is page count
 *   Typical book with 500 pages: <0.1ms on Pi Zero W
 */
int search_find_page_for_offset(pagination_t *pagination, int offset) {
    if (!pagination || offset < 0) {
        return -1;
    }

    /* Linear search through pages to find which one contains this offset
     * Pages are ordered sequentially, so we could optimize with binary search
     * if performance becomes an issue for very large books */
    for (int i = 0; i < pagination->page_count; i++) {
        text_page_t *page = text_get_page(pagination, i);
        if (!page) {
            continue;  /* Skip invalid pages */
        }

        /* Check if offset falls within this page's range (inclusive bounds) */
        if (offset >= page->start_offset && offset <= page->end_offset) {
            return i;  /* Found the page */
        }
    }

    return -1;  /* Offset not found in any page (invalid offset or pagination error) */
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
