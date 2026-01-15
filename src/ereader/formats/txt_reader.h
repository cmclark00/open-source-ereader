/*
 * txt_reader.h - Plain Text Format Reader
 *
 * Simple reader for plain text (.txt) files.
 * This format reader wraps the existing book_load functionality
 * to provide a consistent interface with other formats.
 *
 * Phase 04: EPUB and PDF Support
 */

#ifndef TXT_READER_H
#define TXT_READER_H

#include <stdint.h>
#include <stddef.h>
#include "format_interface.h"

/*
 * TXT Book Structure
 * Contains plain text book data
 */
typedef struct {
    char filepath[512];        /* Path to TXT file */
    char title[256];           /* Title (derived from filename) */
    char *text;                /* Full text content (null-terminated) */
    size_t text_length;        /* Length of text in bytes */
    long file_size;            /* Original file size */
} txt_book_t;

/*
 * Core TXT Functions
 */

/**
 * Validate a TXT file
 * @param filepath: Path to TXT file
 * @return: FORMAT_SUCCESS on valid file, error code otherwise
 */
int txt_validate(const char *filepath);

/**
 * Open and load a TXT file
 * @param filepath: Path to TXT file
 * @return: Pointer to txt_book_t structure, or NULL on error
 * @note: Caller must free with txt_close()
 */
txt_book_t* txt_open(const char *filepath);

/**
 * Close TXT book and free all associated memory
 * @param book: TXT book structure to free
 */
void txt_close(txt_book_t *book);

/**
 * Extract text (for TXT files, this is a no-op as text is already loaded)
 * @param book: TXT book structure
 * @return: FORMAT_SUCCESS
 */
int txt_extract_text(txt_book_t *book);

/**
 * Get text from TXT book
 * @param book: TXT book structure
 * @param out_text: Output pointer to text (do not free, owned by book)
 * @param out_length: Output length of text
 * @return: FORMAT_SUCCESS if text available, error code otherwise
 */
int txt_get_text(txt_book_t *book, const char **out_text, size_t *out_length);

/**
 * Get metadata from TXT book
 * @param book: TXT book structure
 * @param metadata: Output metadata structure
 * @return: FORMAT_SUCCESS
 */
int txt_get_metadata(txt_book_t *book, format_metadata_t *metadata);

/**
 * Check if a file has .txt extension
 * @param filename: Filename to check
 * @return: 1 if .txt file, 0 otherwise
 */
int txt_is_txt_file(const char *filename);

/*
 * Format Interface
 * External interface definition for format registry
 */
extern const book_format_interface_t txt_format_interface;

#endif /* TXT_READER_H */
