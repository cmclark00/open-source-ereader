/*
 * txt_reader.c - Plain Text Format Reader Implementation
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

#include "txt_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Configuration */
#define MAX_TXT_SIZE (10 * 1024 * 1024)  /* 10MB limit for TXT files */

/*
 * Internal helper to extract title from filename
 */
static void extract_title(const char *filepath, char *title, size_t title_size) {
    /* Get basename */
    const char *basename = strrchr(filepath, '/');
    if (basename) {
        basename++;
    } else {
        basename = filepath;
    }

    /* Copy filename without extension */
    strncpy(title, basename, title_size - 1);
    title[title_size - 1] = '\0';

    /* Remove .txt extension */
    char *ext = strrchr(title, '.');
    if (ext && strcasecmp(ext, ".txt") == 0) {
        *ext = '\0';
    }
}

/*
 * Core TXT Functions
 */

int txt_validate(const char *filepath) {
    struct stat st;

    if (!filepath) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }

    /* Check if file exists and is readable */
    if (access(filepath, R_OK) != 0) {
        return FORMAT_ERROR_NOT_FOUND;
    }

    /* Get file size */
    if (stat(filepath, &st) != 0) {
        return FORMAT_ERROR_READ_FAILED;
    }

    /* Check if it's a regular file */
    if (!S_ISREG(st.st_mode)) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }

    /* Check file size */
    if (st.st_size == 0) {
        return FORMAT_ERROR_NO_CONTENT;
    }

    if (st.st_size > MAX_TXT_SIZE) {
        return FORMAT_ERROR_TOO_LARGE;
    }

    return FORMAT_SUCCESS;
}

txt_book_t* txt_open(const char *filepath) {
    FILE *f;
    txt_book_t *book;
    struct stat st;
    size_t bytes_read;
    int validation_result;

    /* Validate file */
    validation_result = txt_validate(filepath);
    if (validation_result != FORMAT_SUCCESS) {
        fprintf(stderr, "txt_open: Validation failed for %s: %s\n",
                filepath, format_error_string(validation_result));
        return NULL;
    }

    /* Get file size */
    if (stat(filepath, &st) != 0) {
        fprintf(stderr, "txt_open: stat failed for %s: %s\n",
                filepath, strerror(errno));
        return NULL;
    }

    /* Open file */
    f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "txt_open: Failed to open %s: %s\n",
                filepath, strerror(errno));
        return NULL;
    }

    /* Allocate book structure */
    book = malloc(sizeof(txt_book_t));
    if (!book) {
        fprintf(stderr, "txt_open: Failed to allocate book structure\n");
        fclose(f);
        return NULL;
    }

    /* Initialize book structure */
    memset(book, 0, sizeof(txt_book_t));
    strncpy(book->filepath, filepath, sizeof(book->filepath) - 1);
    book->filepath[sizeof(book->filepath) - 1] = '\0';
    book->file_size = st.st_size;

    /* Extract title from filename */
    extract_title(filepath, book->title, sizeof(book->title));

    /* Allocate text buffer */
    book->text = malloc(st.st_size + 1);
    if (!book->text) {
        fprintf(stderr, "txt_open: Failed to allocate %ld bytes for text\n", st.st_size);
        free(book);
        fclose(f);
        return NULL;
    }

    /* Read file content */
    bytes_read = fread(book->text, 1, st.st_size, f);
    fclose(f);

    if (bytes_read != (size_t)st.st_size) {
        fprintf(stderr, "txt_open: Read error: expected %ld bytes, got %zu\n",
                st.st_size, bytes_read);
        free(book->text);
        free(book);
        return NULL;
    }

    /* Null-terminate */
    book->text[st.st_size] = '\0';
    book->text_length = st.st_size;

    return book;
}

void txt_close(txt_book_t *book) {
    if (book) {
        free(book->text);
        free(book);
    }
}

int txt_extract_text(txt_book_t *book) {
    /* For TXT files, text is already extracted during open */
    if (!book) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }
    return FORMAT_SUCCESS;
}

int txt_get_text(txt_book_t *book, const char **out_text, size_t *out_length) {
    if (!book || !out_text || !out_length) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }

    if (!book->text) {
        return FORMAT_ERROR_NO_CONTENT;
    }

    *out_text = book->text;
    *out_length = book->text_length;
    return FORMAT_SUCCESS;
}

int txt_get_metadata(txt_book_t *book, format_metadata_t *metadata) {
    if (!book || !metadata) {
        return FORMAT_ERROR_INVALID_FORMAT;
    }

    memset(metadata, 0, sizeof(format_metadata_t));

    /* Set title from filename */
    strncpy(metadata->title, book->title, sizeof(metadata->title) - 1);
    metadata->title[sizeof(metadata->title) - 1] = '\0';

    /* TXT files don't have author or language metadata */
    metadata->author[0] = '\0';
    metadata->language[0] = '\0';

    /* Page count is not known until pagination (depends on rendering) */
    metadata->page_count = 0;

    /* File size */
    metadata->file_size = book->file_size;

    return FORMAT_SUCCESS;
}

int txt_is_txt_file(const char *filename) {
    if (!filename) {
        return 0;
    }

    size_t len = strlen(filename);
    if (len < 5) {  /* Minimum: "a.txt" */
        return 0;
    }

    /* Check for .txt extension (case-insensitive) */
    const char *ext = filename + len - 4;
    return (strcasecmp(ext, ".txt") == 0);
}

/*
 * Format Interface Implementation
 * Wrappers that convert between void* handles and txt_book_t*
 */

static int txt_interface_validate(const char *filepath) {
    return txt_validate(filepath);
}

static format_handle_t txt_interface_open(const char *filepath) {
    return (format_handle_t)txt_open(filepath);
}

static void txt_interface_close(format_handle_t handle) {
    txt_close((txt_book_t*)handle);
}

static int txt_interface_extract_text(format_handle_t handle) {
    return txt_extract_text((txt_book_t*)handle);
}

static int txt_interface_get_text(format_handle_t handle, const char **out_text, size_t *out_length) {
    return txt_get_text((txt_book_t*)handle, out_text, out_length);
}

static int txt_interface_get_metadata(format_handle_t handle, format_metadata_t *metadata) {
    return txt_get_metadata((txt_book_t*)handle, metadata);
}

static int txt_interface_get_page_count(format_handle_t handle) {
    /* Page count not available for TXT until pagination */
    (void)handle;
    return -1;
}

/*
 * Format Interface Definition
 */
const book_format_interface_t txt_format_interface = {
    .type = BOOK_FORMAT_TXT,
    .name = "Plain Text",
    .extension = ".txt",
    .validate = txt_interface_validate,
    .open = txt_interface_open,
    .close = txt_interface_close,
    .extract_text = txt_interface_extract_text,
    .get_text = txt_interface_get_text,
    .get_metadata = txt_interface_get_metadata,
    .get_page_count = txt_interface_get_page_count
};
