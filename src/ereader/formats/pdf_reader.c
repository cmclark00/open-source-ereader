/*
 * pdf_reader.c - PDF Format Reader Implementation
 *
 * PDF text extraction using Poppler's pdftotext utility.
 * Extracts plain text from PDF files for display on e-reader.
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

#include "pdf_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
 * Internal Constants
 */
#define PDFTOTEXT_CMD "/usr/bin/pdftotext"
#define PDFINFO_CMD "/usr/bin/pdfinfo"
#define TEMP_FILE_TEMPLATE "/tmp/pdf_extract_XXXXXX"
#define MAX_COMMAND_LENGTH 1024

/*
 * Internal Helper Functions
 */

/* Check if a file exists */
static int file_exists(const char *filepath) {
    struct stat st;
    return (stat(filepath, &st) == 0);
}

/* Read entire file into memory */
static char* read_file_to_string(const char *filepath, size_t *out_size) {
    FILE *file;
    char *buffer;
    size_t file_size;
    size_t bytes_read;

    file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "pdf: Failed to open file: %s\n", filepath);
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Check size limit */
    if (file_size > PDF_MAX_TEXT_SIZE) {
        fprintf(stderr, "pdf: File too large: %zu bytes (max %d)\n",
                file_size, PDF_MAX_TEXT_SIZE);
        fclose(file);
        return NULL;
    }

    /* Allocate buffer */
    buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "pdf: Out of memory allocating %zu bytes\n", file_size);
        fclose(file);
        return NULL;
    }

    /* Read file */
    bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size) {
        fprintf(stderr, "pdf: Read error: expected %zu bytes, got %zu\n",
                file_size, bytes_read);
        free(buffer);
        return NULL;
    }

    /* Null-terminate */
    buffer[file_size] = '\0';
    if (out_size) {
        *out_size = file_size;
    }

    return buffer;
}

/* Execute command and return exit code */
static int execute_command(const char *command) {
    int status;

    status = system(command);

    if (status == -1) {
        fprintf(stderr, "pdf: Failed to execute command\n");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return -1;
}

/* Parse pdfinfo output for metadata */
static int parse_pdfinfo_output(const char *output, pdf_metadata_t *metadata) {
    char line[512];
    const char *ptr = output;
    char *end;

    /* Initialize metadata */
    memset(metadata, 0, sizeof(pdf_metadata_t));

    /* Parse line by line */
    while (*ptr) {
        /* Extract line */
        end = strchr(ptr, '\n');
        if (end) {
            size_t len = end - ptr;
            if (len >= sizeof(line)) {
                len = sizeof(line) - 1;
            }
            memcpy(line, ptr, len);
            line[len] = '\0';
            ptr = end + 1;
        } else {
            strncpy(line, ptr, sizeof(line) - 1);
            line[sizeof(line) - 1] = '\0';
            ptr += strlen(ptr);
        }

        /* Parse metadata fields */
        if (strncmp(line, "Title:", 6) == 0) {
            char *value = line + 6;
            while (*value == ' ' || *value == '\t') value++;
            strncpy(metadata->title, value, sizeof(metadata->title) - 1);
        } else if (strncmp(line, "Author:", 7) == 0) {
            char *value = line + 7;
            while (*value == ' ' || *value == '\t') value++;
            strncpy(metadata->author, value, sizeof(metadata->author) - 1);
        } else if (strncmp(line, "Subject:", 8) == 0) {
            char *value = line + 8;
            while (*value == ' ' || *value == '\t') value++;
            strncpy(metadata->subject, value, sizeof(metadata->subject) - 1);
        } else if (strncmp(line, "Creator:", 8) == 0) {
            char *value = line + 8;
            while (*value == ' ' || *value == '\t') value++;
            strncpy(metadata->creator, value, sizeof(metadata->creator) - 1);
        }
    }

    return PDF_SUCCESS;
}

/* Parse pdfinfo output for page count */
static int parse_page_count(const char *output) {
    const char *pages_line;
    int page_count;

    pages_line = strstr(output, "Pages:");
    if (!pages_line) {
        return -1;
    }

    if (sscanf(pages_line, "Pages: %d", &page_count) != 1) {
        return -1;
    }

    return page_count;
}

/*
 * Public API Functions
 */

int pdf_validate(const char *filepath) {
    char command[MAX_COMMAND_LENGTH];
    int result;

    if (!filepath) {
        return PDF_ERROR_NOT_FOUND;
    }

    /* Check if file exists */
    if (!file_exists(filepath)) {
        return PDF_ERROR_NOT_FOUND;
    }

    /* Check if file has PDF magic number */
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return PDF_ERROR_NOT_FOUND;
    }

    char magic[5] = {0};
    size_t read = fread(magic, 1, 4, file);
    fclose(file);

    if (read != 4 || memcmp(magic, "%PDF", 4) != 0) {
        return PDF_ERROR_INVALID_FORMAT;
    }

    /* Try to get info (validates PDF structure) */
    snprintf(command, sizeof(command), "%s \"%s\" > /dev/null 2>&1",
             PDFINFO_CMD, filepath);

    result = execute_command(command);

    if (result != 0) {
        return PDF_ERROR_CORRUPT_FILE;
    }

    return PDF_SUCCESS;
}

int pdf_extract_metadata(const char *filepath, pdf_metadata_t *metadata) {
    char command[MAX_COMMAND_LENGTH];
    char temp_file[] = TEMP_FILE_TEMPLATE;
    int fd;
    char *output;
    size_t output_size;
    int result;

    if (!filepath || !metadata) {
        return PDF_ERROR_NOT_FOUND;
    }

    /* Create temporary file */
    fd = mkstemp(temp_file);
    if (fd == -1) {
        fprintf(stderr, "pdf: Failed to create temporary file\n");
        return PDF_ERROR_READ_FAILED;
    }
    close(fd);

    /* Run pdfinfo */
    snprintf(command, sizeof(command), "%s \"%s\" > \"%s\" 2>/dev/null",
             PDFINFO_CMD, filepath, temp_file);

    result = execute_command(command);

    if (result != 0) {
        unlink(temp_file);
        return PDF_ERROR_EXTRACTION_FAILED;
    }

    /* Read output */
    output = read_file_to_string(temp_file, &output_size);
    unlink(temp_file);

    if (!output) {
        return PDF_ERROR_READ_FAILED;
    }

    /* Parse metadata */
    result = parse_pdfinfo_output(output, metadata);
    free(output);

    return result;
}

pdf_document_t* pdf_open(const char *filepath) {
    pdf_document_t *doc;
    char command[MAX_COMMAND_LENGTH];
    char temp_file[] = TEMP_FILE_TEMPLATE;
    int fd;
    char *output;
    size_t output_size;
    int result;
    int page_count;

    if (!filepath) {
        fprintf(stderr, "pdf: NULL filepath\n");
        return NULL;
    }

    /* Validate PDF */
    result = pdf_validate(filepath);
    if (result != PDF_SUCCESS) {
        fprintf(stderr, "pdf: Validation failed: %s\n", pdf_error_string(result));
        return NULL;
    }

    /* Allocate document structure */
    doc = calloc(1, sizeof(pdf_document_t));
    if (!doc) {
        fprintf(stderr, "pdf: Out of memory\n");
        return NULL;
    }

    /* Copy filepath */
    strncpy(doc->filepath, filepath, sizeof(doc->filepath) - 1);

    /* Extract metadata */
    pdf_extract_metadata(filepath, &doc->metadata);

    /* Get page count */
    fd = mkstemp(temp_file);
    if (fd == -1) {
        fprintf(stderr, "pdf: Failed to create temporary file\n");
        free(doc);
        return NULL;
    }
    close(fd);

    snprintf(command, sizeof(command), "%s \"%s\" > \"%s\" 2>/dev/null",
             PDFINFO_CMD, filepath, temp_file);

    result = execute_command(command);

    if (result != 0) {
        unlink(temp_file);
        free(doc);
        fprintf(stderr, "pdf: Failed to get page count\n");
        return NULL;
    }

    output = read_file_to_string(temp_file, &output_size);
    unlink(temp_file);

    if (!output) {
        free(doc);
        return NULL;
    }

    page_count = parse_page_count(output);
    free(output);

    if (page_count <= 0 || page_count > PDF_MAX_PAGES) {
        fprintf(stderr, "pdf: Invalid page count: %d\n", page_count);
        free(doc);
        return NULL;
    }

    doc->page_count = page_count;

    /* Allocate pages array */
    doc->pages = calloc(page_count, sizeof(pdf_page_t));
    if (!doc->pages) {
        fprintf(stderr, "pdf: Out of memory allocating pages\n");
        free(doc);
        return NULL;
    }

    /* Initialize page numbers */
    for (int i = 0; i < page_count; i++) {
        doc->pages[i].page_number = i + 1;
    }

    return doc;
}

void pdf_close(pdf_document_t *doc) {
    if (!doc) {
        return;
    }

    /* Free page text */
    if (doc->pages) {
        for (int i = 0; i < doc->page_count; i++) {
            free(doc->pages[i].text);
        }
        free(doc->pages);
    }

    /* Free full text */
    free(doc->full_text);

    /* Free document */
    free(doc);
}

int pdf_get_page_count(pdf_document_t *doc) {
    if (!doc) {
        return -1;
    }

    return doc->page_count;
}

int pdf_extract_page(pdf_document_t *doc, int page_number) {
    char command[MAX_COMMAND_LENGTH];
    char temp_file[] = TEMP_FILE_TEMPLATE;
    int fd;
    char *text;
    size_t text_size;
    int result;

    if (!doc) {
        return PDF_ERROR_NOT_FOUND;
    }

    if (page_number < 1 || page_number > doc->page_count) {
        fprintf(stderr, "pdf: Invalid page number: %d (valid: 1-%d)\n",
                page_number, doc->page_count);
        return PDF_ERROR_NOT_FOUND;
    }

    int page_index = page_number - 1;

    /* Check if already extracted */
    if (doc->pages[page_index].text) {
        return PDF_SUCCESS;
    }

    /* Create temporary file */
    fd = mkstemp(temp_file);
    if (fd == -1) {
        fprintf(stderr, "pdf: Failed to create temporary file\n");
        return PDF_ERROR_READ_FAILED;
    }
    close(fd);

    /* Extract single page using pdftotext */
    snprintf(command, sizeof(command),
             "%s -f %d -l %d -nopgbrk -q \"%s\" \"%s\" 2>/dev/null",
             PDFTOTEXT_CMD, page_number, page_number,
             doc->filepath, temp_file);

    result = execute_command(command);

    if (result != 0) {
        unlink(temp_file);
        fprintf(stderr, "pdf: Text extraction failed for page %d\n", page_number);
        return PDF_ERROR_EXTRACTION_FAILED;
    }

    /* Read extracted text */
    text = read_file_to_string(temp_file, &text_size);
    unlink(temp_file);

    if (!text) {
        return PDF_ERROR_READ_FAILED;
    }

    /* Store in page structure */
    doc->pages[page_index].text = text;
    doc->pages[page_index].text_length = text_size;

    return PDF_SUCCESS;
}

int pdf_extract_text(pdf_document_t *doc) {
    char command[MAX_COMMAND_LENGTH];
    char temp_file[] = TEMP_FILE_TEMPLATE;
    int fd;
    char *text;
    size_t text_size;
    int result;

    if (!doc) {
        return PDF_ERROR_NOT_FOUND;
    }

    /* Check if already extracted */
    if (doc->full_text) {
        return PDF_SUCCESS;
    }

    /* Create temporary file */
    fd = mkstemp(temp_file);
    if (fd == -1) {
        fprintf(stderr, "pdf: Failed to create temporary file\n");
        return PDF_ERROR_READ_FAILED;
    }
    close(fd);

    /* Extract all pages using pdftotext */
    /* -nopgbrk: Don't insert page breaks between pages */
    /* -q: Quiet mode (no messages) */
    snprintf(command, sizeof(command),
             "%s -nopgbrk -q \"%s\" \"%s\" 2>/dev/null",
             PDFTOTEXT_CMD, doc->filepath, temp_file);

    result = execute_command(command);

    if (result != 0) {
        unlink(temp_file);
        fprintf(stderr, "pdf: Text extraction failed\n");
        return PDF_ERROR_EXTRACTION_FAILED;
    }

    /* Read extracted text */
    text = read_file_to_string(temp_file, &text_size);
    unlink(temp_file);

    if (!text) {
        return PDF_ERROR_READ_FAILED;
    }

    /* Check for empty content */
    if (text_size == 0) {
        free(text);
        return PDF_ERROR_NO_CONTENT;
    }

    /* Store in document */
    doc->full_text = text;
    doc->full_text_length = text_size;

    return PDF_SUCCESS;
}

pdf_page_t* pdf_get_page(pdf_document_t *doc, int page_number) {
    if (!doc) {
        return NULL;
    }

    if (page_number < 1 || page_number > doc->page_count) {
        return NULL;
    }

    return &doc->pages[page_number - 1];
}

int pdf_get_text(pdf_document_t *doc, const char **out_text, size_t *out_length) {
    if (!doc || !out_text || !out_length) {
        return PDF_ERROR_NOT_FOUND;
    }

    if (!doc->full_text) {
        return PDF_ERROR_NO_CONTENT;
    }

    *out_text = doc->full_text;
    *out_length = doc->full_text_length;

    return PDF_SUCCESS;
}

const char* pdf_error_string(pdf_error_t error) {
    switch (error) {
        case PDF_SUCCESS:
            return "Success";
        case PDF_ERROR_NOT_FOUND:
            return "File not found";
        case PDF_ERROR_INVALID_FORMAT:
            return "Invalid PDF format";
        case PDF_ERROR_CORRUPT_FILE:
            return "Corrupt PDF file";
        case PDF_ERROR_EXTRACTION_FAILED:
            return "Text extraction failed";
        case PDF_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case PDF_ERROR_TOO_LARGE:
            return "File too large";
        case PDF_ERROR_READ_FAILED:
            return "Read failed";
        case PDF_ERROR_NO_CONTENT:
            return "No text content";
        case PDF_ERROR_UNSUPPORTED:
            return "Unsupported feature";
        default:
            return "Unknown error";
    }
}

int pdf_is_pdf_file(const char *filename) {
    size_t len;
    const char *ext;

    if (!filename) {
        return 0;
    }

    len = strlen(filename);
    if (len < 5) {  /* Minimum: "a.pdf" */
        return 0;
    }

    ext = filename + len - 4;

    return (strcasecmp(ext, ".pdf") == 0);
}

/*
 * Format Interface Implementation
 */

#include "format_interface.h"

/* Convert PDF error codes to format error codes */
static format_error_t pdf_to_format_error(pdf_error_t error) {
    switch (error) {
        case PDF_SUCCESS:
            return FORMAT_SUCCESS;
        case PDF_ERROR_NOT_FOUND:
            return FORMAT_ERROR_NOT_FOUND;
        case PDF_ERROR_INVALID_FORMAT:
            return FORMAT_ERROR_INVALID_FORMAT;
        case PDF_ERROR_CORRUPT_FILE:
        case PDF_ERROR_EXTRACTION_FAILED:
            return FORMAT_ERROR_CORRUPT_FILE;
        case PDF_ERROR_OUT_OF_MEMORY:
            return FORMAT_ERROR_OUT_OF_MEMORY;
        case PDF_ERROR_TOO_LARGE:
            return FORMAT_ERROR_TOO_LARGE;
        case PDF_ERROR_READ_FAILED:
            return FORMAT_ERROR_READ_FAILED;
        case PDF_ERROR_NO_CONTENT:
            return FORMAT_ERROR_NO_CONTENT;
        case PDF_ERROR_UNSUPPORTED:
            return FORMAT_ERROR_UNSUPPORTED;
        default:
            return FORMAT_ERROR_INVALID_FORMAT;
    }
}

static int pdf_interface_validate(const char *filepath) {
    int result = pdf_validate(filepath);
    return pdf_to_format_error(result);
}

static format_handle_t pdf_interface_open(const char *filepath) {
    return (format_handle_t)pdf_open(filepath);
}

static void pdf_interface_close(format_handle_t handle) {
    pdf_close((pdf_document_t*)handle);
}

static int pdf_interface_extract_text(format_handle_t handle) {
    int result = pdf_extract_text((pdf_document_t*)handle);
    return pdf_to_format_error(result);
}

static int pdf_interface_get_text(format_handle_t handle, const char **out_text, size_t *out_length) {
    int result = pdf_get_text((pdf_document_t*)handle, out_text, out_length);
    return pdf_to_format_error(result);
}

static int pdf_interface_get_metadata(format_handle_t handle, format_metadata_t *metadata) {
    pdf_document_t *doc = (pdf_document_t*)handle;

    if (!doc || !metadata) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }

    memset(metadata, 0, sizeof(format_metadata_t));

    /* Copy metadata from PDF */
    strncpy(metadata->title, doc->metadata.title, sizeof(metadata->title) - 1);
    metadata->title[sizeof(metadata->title) - 1] = '\0';

    strncpy(metadata->author, doc->metadata.author, sizeof(metadata->author) - 1);
    metadata->author[sizeof(metadata->author) - 1] = '\0';

    /* PDFs don't typically have language metadata */
    metadata->language[0] = '\0';

    /* Page count from PDF */
    metadata->page_count = doc->page_count;

    /* File size - approximate from full text length */
    metadata->file_size = doc->full_text_length;

    return FORMAT_SUCCESS;
}

static int pdf_interface_get_page_count(format_handle_t handle) {
    pdf_document_t *doc = (pdf_document_t*)handle;
    if (!doc) {
        return -1;
    }
    return doc->page_count;
}

/*
 * Format Interface Definition
 */
const book_format_interface_t pdf_format_interface = {
    .type = BOOK_FORMAT_PDF,
    .name = "PDF",
    .extension = ".pdf",
    .validate = pdf_interface_validate,
    .open = pdf_interface_open,
    .close = pdf_interface_close,
    .extract_text = pdf_interface_extract_text,
    .get_text = pdf_interface_get_text,
    .get_metadata = pdf_interface_get_metadata,
    .get_page_count = pdf_interface_get_page_count
};
