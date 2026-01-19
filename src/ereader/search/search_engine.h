/*
 * search_engine.h - E-Reader Text Search Engine
 *
 * Provides text search capabilities within the currently loaded book.
 * Supports finding all occurrences of search terms and navigating between matches.
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

#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "../books/book_manager.h"
#include "../rendering/text_renderer.h"

/*
 * Configuration Constants
 */
#define MAX_SEARCH_TERM_LENGTH  64      /* Maximum length of search term */
#define MAX_SEARCH_RESULTS     1000     /* Maximum number of search results */

/*
 * Search Result Structure
 * Represents a single occurrence of the search term in the text
 */
typedef struct {
    int offset;              /* Character offset in source text */
    int page_number;         /* Page number where match occurs (0-based) */
    int line_in_page;        /* Line number within the page (0-based) */
    int char_in_line;        /* Character position within the line */
} search_result_t;

/*
 * Search Context Structure
 * Maintains state for an active search operation
 */
typedef struct {
    char search_term[MAX_SEARCH_TERM_LENGTH];  /* Current search term */
    bool case_sensitive;                        /* Case-sensitive search flag */

    search_result_t *results;                   /* Array of search results */
    int result_count;                           /* Number of results found */
    int current_result;                         /* Current result index (-1 if none) */

    const char *source_text;                    /* Pointer to text being searched (not owned) */
    size_t source_length;                       /* Length of source text */
    pagination_t *pagination;                   /* Pagination context (not owned) */

    bool search_active;                         /* Whether a search is currently active */
} search_context_t;

/*
 * Error Codes
 */
typedef enum {
    SEARCH_SUCCESS = 0,
    SEARCH_ERROR_NULL_POINTER = -1,
    SEARCH_ERROR_INVALID_TERM = -2,
    SEARCH_ERROR_OUT_OF_MEMORY = -3,
    SEARCH_ERROR_NO_RESULTS = -4,
    SEARCH_ERROR_NO_ACTIVE_SEARCH = -5
} search_error_t;

/*
 * Search Context Management
 */

/**
 * Create a new search context
 *
 * @param text: Source text to search (must remain valid during search context lifetime)
 * @param text_length: Length of source text in bytes
 * @param pagination: Pagination context for mapping offsets to pages (must remain valid)
 * @return: Pointer to search context, or NULL on error
 */
search_context_t* search_create(const char *text, size_t text_length, pagination_t *pagination);

/**
 * Free search context and associated resources
 *
 * @param ctx: Search context to free (may be NULL)
 */
void search_free(search_context_t *ctx);

/**
 * Clear current search results
 *
 * @param ctx: Search context
 */
void search_clear(search_context_t *ctx);

/*
 * Search Operations
 */

/**
 * Execute a search for the given term
 *
 * This finds all occurrences of the search term in the text and populates
 * the results array. The current result is set to the first match.
 *
 * @param ctx: Search context
 * @param term: Search term (null-terminated string)
 * @param case_sensitive: Whether to perform case-sensitive search
 * @return: Number of results found (0 if no matches), or negative error code
 */
int search_execute(search_context_t *ctx, const char *term, bool case_sensitive);

/**
 * Navigate to the next search result
 *
 * @param ctx: Search context
 * @return: Index of new current result, or -1 if at end or no active search
 */
int search_next_result(search_context_t *ctx);

/**
 * Navigate to the previous search result
 *
 * @param ctx: Search context
 * @return: Index of new current result, or -1 if at beginning or no active search
 */
int search_prev_result(search_context_t *ctx);

/**
 * Get the current search result
 *
 * @param ctx: Search context
 * @return: Pointer to current search result, or NULL if no active search
 */
search_result_t* search_get_current_result(search_context_t *ctx);

/**
 * Get a specific search result by index
 *
 * @param ctx: Search context
 * @param index: Result index (0-based)
 * @return: Pointer to search result, or NULL if invalid index
 */
search_result_t* search_get_result(search_context_t *ctx, int index);

/*
 * Search State Queries
 */

/**
 * Check if a search is currently active
 *
 * @param ctx: Search context
 * @return: true if search is active, false otherwise
 */
bool search_is_active(search_context_t *ctx);

/**
 * Get the number of search results
 *
 * @param ctx: Search context
 * @return: Number of results found in current search
 */
int search_get_result_count(search_context_t *ctx);

/**
 * Get the current result index
 *
 * @param ctx: Search context
 * @return: Current result index (0-based), or -1 if no current result
 */
int search_get_current_index(search_context_t *ctx);

/**
 * Get the current search term
 *
 * @param ctx: Search context
 * @return: Current search term (empty string if no active search)
 */
const char* search_get_term(search_context_t *ctx);

/*
 * Utility Functions
 */

/**
 * Find the page number for a given text offset
 *
 * Helper function to map a character offset in the source text
 * to the corresponding page number in the pagination.
 *
 * @param pagination: Pagination context
 * @param offset: Character offset in source text
 * @return: Page number (0-based), or -1 if not found
 */
int search_find_page_for_offset(pagination_t *pagination, int offset);

/**
 * Get error message for error code
 *
 * @param error: Error code
 * @return: Human-readable error message
 */
const char* search_error_string(search_error_t error);

#endif /* SEARCH_ENGINE_H */
