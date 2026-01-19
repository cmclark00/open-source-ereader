/*
 * pdf_reader.h - PDF Format Reader
 *
 * PDF text extraction for embedded e-reader.
 * Uses Poppler's pdftotext utility for text extraction.
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

#ifndef PDF_READER_H
#define PDF_READER_H

#include <stdint.h>
#include <stddef.h>

/*
 * Configuration Constants
 */
#define PDF_MAX_PATH_LENGTH 512
#define PDF_MAX_TEXT_SIZE (50 * 1024 * 1024)  /* 50MB max extracted text */
#define PDF_MAX_PAGES 10000

/*
 * Error Codes
 */
typedef enum {
    PDF_SUCCESS = 0,
    PDF_ERROR_NOT_FOUND = -1,
    PDF_ERROR_INVALID_FORMAT = -2,
    PDF_ERROR_CORRUPT_FILE = -3,
    PDF_ERROR_EXTRACTION_FAILED = -4,
    PDF_ERROR_OUT_OF_MEMORY = -5,
    PDF_ERROR_TOO_LARGE = -6,
    PDF_ERROR_READ_FAILED = -7,
    PDF_ERROR_NO_CONTENT = -8,
    PDF_ERROR_UNSUPPORTED = -9
} pdf_error_t;

/*
 * PDF Page Structure
 * Represents a single page in the PDF
 */
typedef struct {
    int page_number;                   /* Page number (1-indexed) */
    char *text;                        /* Extracted plain text (NULL until extracted) */
    size_t text_length;                /* Length of extracted text */
} pdf_page_t;

/*
 * PDF Metadata Structure
 * Contains document metadata
 */
typedef struct {
    char title[256];                   /* Document title */
    char author[256];                  /* Document author */
    char subject[256];                 /* Document subject */
    char creator[256];                 /* Creator application */
} pdf_metadata_t;

/*
 * PDF Document Structure
 * Contains the full PDF document data
 */
typedef struct {
    char filepath[PDF_MAX_PATH_LENGTH]; /* Path to PDF file */
    pdf_metadata_t metadata;            /* Document metadata */
    pdf_page_t *pages;                  /* Array of pages (lazy loaded) */
    int page_count;                     /* Number of pages */
    char *full_text;                    /* All extracted text concatenated */
    size_t full_text_length;            /* Total text length */
} pdf_document_t;

/*
 * Core PDF Functions
 */

/**
 * Validate a PDF file (check if it's a valid PDF)
 * @param filepath: Path to PDF file
 * @return: PDF_SUCCESS on valid PDF, error code otherwise
 */
int pdf_validate(const char *filepath);

/**
 * Open and parse a PDF file
 * @param filepath: Path to PDF file
 * @return: Pointer to pdf_document_t structure, or NULL on error
 * @note: Caller must free with pdf_close()
 */
pdf_document_t* pdf_open(const char *filepath);

/**
 * Close PDF document and free all associated memory
 * @param doc: PDF document structure to free
 */
void pdf_close(pdf_document_t *doc);

/**
 * Get page count from PDF document
 * @param doc: PDF document structure
 * @return: Number of pages, or -1 on error
 */
int pdf_get_page_count(pdf_document_t *doc);

/**
 * Extract text from a single PDF page
 * @param doc: PDF document structure
 * @param page_number: Page number (1-indexed)
 * @return: PDF_SUCCESS on success, error code on failure
 * @note: Text stored in doc->pages[page_number-1].text
 */
int pdf_extract_page(pdf_document_t *doc, int page_number);

/**
 * Extract all text from PDF document
 * Extracts text from all pages and concatenates into doc->full_text
 * @param doc: PDF document structure
 * @return: PDF_SUCCESS on success, error code on failure
 */
int pdf_extract_text(pdf_document_t *doc);

/**
 * Get page by number
 * @param doc: PDF document structure
 * @param page_number: Page number (1-indexed)
 * @return: Pointer to page structure, or NULL if invalid page number
 */
pdf_page_t* pdf_get_page(pdf_document_t *doc, int page_number);

/**
 * Get extracted full text from PDF
 * @param doc: PDF document structure
 * @param out_text: Output pointer to text (do not free, owned by doc)
 * @param out_length: Output length of text
 * @return: PDF_SUCCESS if text available, error code otherwise
 * @note: Must call pdf_extract_text() first
 */
int pdf_get_text(pdf_document_t *doc, const char **out_text, size_t *out_length);

/*
 * Utility Functions
 */

/**
 * Get human-readable error message for error code
 * @param error: Error code
 * @return: Error message string
 */
const char* pdf_error_string(pdf_error_t error);

/**
 * Check if a file has .pdf extension
 * @param filename: Filename to check
 * @return: 1 if .pdf file, 0 otherwise
 */
int pdf_is_pdf_file(const char *filename);

/**
 * Extract metadata from PDF using pdfinfo
 * @param filepath: Path to PDF file
 * @param metadata: Output metadata structure
 * @return: PDF_SUCCESS on success, error code on failure
 */
int pdf_extract_metadata(const char *filepath, pdf_metadata_t *metadata);

#endif /* PDF_READER_H */
