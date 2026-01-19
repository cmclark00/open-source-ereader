/*
 * epub_reader.h - EPUB Format Reader
 *
 * EPUB parser and text extractor for embedded e-reader.
 * Uses libzip for ZIP archive access and libxml2 for XML parsing.
 *
 * Phase 04: EPUB and PDF Support
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

#ifndef EPUB_READER_H
#define EPUB_READER_H

#include <stdint.h>
#include <stddef.h>

/*
 * Configuration Constants
 */
#define EPUB_MAX_PATH_LENGTH 512
#define EPUB_MAX_CHAPTERS 500
#define EPUB_MAX_TEXT_SIZE (20 * 1024 * 1024)  /* 20MB max extracted text */

/*
 * Error Codes
 */
typedef enum {
    EPUB_SUCCESS = 0,
    EPUB_ERROR_NOT_FOUND = -1,
    EPUB_ERROR_INVALID_FORMAT = -2,
    EPUB_ERROR_CORRUPT_ZIP = -3,
    EPUB_ERROR_MISSING_CONTAINER = -4,
    EPUB_ERROR_MISSING_OPF = -5,
    EPUB_ERROR_PARSE_ERROR = -6,
    EPUB_ERROR_OUT_OF_MEMORY = -7,
    EPUB_ERROR_TOO_LARGE = -8,
    EPUB_ERROR_READ_FAILED = -9,
    EPUB_ERROR_NO_CONTENT = -10
} epub_error_t;

/*
 * EPUB Chapter Structure
 * Represents a single chapter/spine item in reading order
 */
typedef struct {
    char id[128];                      /* Manifest item ID */
    char href[EPUB_MAX_PATH_LENGTH];   /* Path to chapter file in ZIP */
    char *text;                        /* Extracted plain text (NULL until extracted) */
    size_t text_length;                /* Length of extracted text */
} epub_chapter_t;

/*
 * EPUB Metadata Structure
 * Contains book metadata from content.opf
 */
typedef struct {
    char title[256];                   /* Book title */
    char author[256];                  /* Primary author */
    char language[16];                 /* Language code (e.g., "en") */
    char identifier[128];              /* ISBN or other identifier */
} epub_metadata_t;

/*
 * EPUB Book Structure
 * Contains the full EPUB book data
 */
typedef struct {
    char filepath[EPUB_MAX_PATH_LENGTH]; /* Path to EPUB file */
    epub_metadata_t metadata;            /* Book metadata */
    epub_chapter_t *chapters;            /* Array of chapters in reading order */
    int chapter_count;                   /* Number of chapters */
    char *full_text;                     /* All extracted text concatenated */
    size_t full_text_length;             /* Total text length */
    void *zip_handle;                    /* Internal: libzip handle */
} epub_book_t;

/*
 * Core EPUB Functions
 */

/**
 * Validate an EPUB file (check if it's a valid ZIP with EPUB structure)
 * @param filepath: Path to EPUB file
 * @return: EPUB_SUCCESS on valid EPUB, error code otherwise
 */
int epub_validate(const char *filepath);

/**
 * Open and parse an EPUB file
 * @param filepath: Path to EPUB file
 * @return: Pointer to epub_book_t structure, or NULL on error
 * @note: Caller must free with epub_close()
 */
epub_book_t* epub_open(const char *filepath);

/**
 * Close EPUB book and free all associated memory
 * @param book: EPUB book structure to free
 */
void epub_close(epub_book_t *book);

/**
 * Extract all text from EPUB book
 * Extracts text from all chapters, strips HTML, decodes entities,
 * and concatenates into book->full_text
 * @param book: EPUB book structure
 * @return: EPUB_SUCCESS on success, error code on failure
 */
int epub_extract_text(epub_book_t *book);

/**
 * Get chapter by index
 * @param book: EPUB book structure
 * @param index: Chapter index (0-based)
 * @return: Pointer to chapter structure, or NULL if invalid index
 */
epub_chapter_t* epub_get_chapter(epub_book_t *book, int index);

/**
 * Get extracted full text from EPUB
 * @param book: EPUB book structure
 * @param out_text: Output pointer to text (do not free, owned by book)
 * @param out_length: Output length of text
 * @return: EPUB_SUCCESS if text available, error code otherwise
 * @note: Must call epub_extract_text() first
 */
int epub_get_text(epub_book_t *book, const char **out_text, size_t *out_length);

/*
 * Utility Functions
 */

/**
 * Get human-readable error message for error code
 * @param error: Error code
 * @return: Error message string
 */
const char* epub_error_string(epub_error_t error);

/**
 * Check if a file has .epub extension
 * @param filename: Filename to check
 * @return: 1 if .epub file, 0 otherwise
 */
int epub_is_epub_file(const char *filename);

/*
 * Internal Helper Functions (exposed for testing)
 */

/**
 * Parse META-INF/container.xml to find content.opf path
 * @param zip_handle: libzip handle
 * @param opf_path: Output buffer for OPF path
 * @param opf_path_size: Size of output buffer
 * @return: EPUB_SUCCESS on success, error code on failure
 */
int epub_parse_container(void *zip_handle, char *opf_path, size_t opf_path_size);

/**
 * Parse content.opf to extract metadata and reading order
 * @param zip_handle: libzip handle
 * @param opf_path: Path to content.opf in ZIP
 * @param book: EPUB book structure to populate
 * @return: EPUB_SUCCESS on success, error code on failure
 */
int epub_parse_opf(void *zip_handle, const char *opf_path, epub_book_t *book);

/**
 * Extract text from a single XHTML/HTML chapter
 * @param html: HTML/XHTML content
 * @param html_length: Length of HTML content
 * @param out_text: Output buffer for plain text (allocated by function)
 * @param out_length: Output length of plain text
 * @return: EPUB_SUCCESS on success, error code on failure
 * @note: Caller must free out_text
 */
int epub_extract_html_text(const char *html, size_t html_length,
                           char **out_text, size_t *out_length);

/**
 * Decode HTML entities in text (e.g., &amp; &lt; &gt; &#39; etc.)
 * @param text: Input text with entities
 * @param text_length: Length of input text
 * @param out_text: Output buffer (allocated by function)
 * @param out_length: Output length
 * @return: EPUB_SUCCESS on success, error code on failure
 * @note: Caller must free out_text
 */
int epub_decode_entities(const char *text, size_t text_length,
                         char **out_text, size_t *out_length);

#endif /* EPUB_READER_H */
