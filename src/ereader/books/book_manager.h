/*
 * book_manager.h
 *
 * E-Reader book file management system
 * Handles book discovery, loading, and reading position tracking
 *
 * Phase 03: Basic E-Reader Application
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

#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include <stdint.h>
#include <time.h>

/*
 * Configuration constants
 */
#define MAX_FILENAME_LENGTH 256
#define MAX_BOOK_PATH 512
#define MAX_BOOKS 1000
#define MAX_BOOK_SIZE (10 * 1024 * 1024)  /* 10MB limit for Phase 03 */
#define BOOKS_DIR "/books"
#define BOOKMARKS_FILE "/etc/ereader/bookmarks.txt"

/*
 * Book format type (forward declaration from format_interface.h)
 */
typedef enum {
    BOOK_FORMAT_UNKNOWN = 0,
    BOOK_FORMAT_TXT,
    BOOK_FORMAT_EPUB,
    BOOK_FORMAT_PDF
} book_format_type_t;

/*
 * Book metadata structure
 * Stores information about a single book file without loading the full content
 */
typedef struct {
    char filename[MAX_FILENAME_LENGTH];  /* Basename of the file (e.g., "book.txt") */
    char filepath[MAX_BOOK_PATH];        /* Full path to the file */
    long size;                           /* File size in bytes */
    time_t modified;                     /* Last modification time */
    int bookmark_page;                   /* Last read page (0 if never read) */
    book_format_type_t format;           /* File format (TXT, EPUB, PDF) */
    char title[256];                     /* Book title (extracted from metadata, or filename) */
    char author[256];                    /* Author name (extracted from metadata, empty if N/A) */
} book_metadata_t;

/*
 * Book list structure
 * Contains all discovered books in the library
 */
typedef struct {
    book_metadata_t *books;              /* Array of book metadata */
    int count;                           /* Number of books in the list */
    int capacity;                        /* Allocated capacity */
} book_list_t;

/*
 * Loaded book structure
 * Contains the full text content of a book currently being read
 */
typedef struct {
    char filename[MAX_FILENAME_LENGTH];  /* Book filename */
    char *text;                          /* Full text content (null-terminated) */
    size_t text_length;                  /* Length of text in bytes */
} book_t;

/*
 * Bookmark structure
 * Stores reading position for a single book
 */
typedef struct {
    char filename[MAX_FILENAME_LENGTH];  /* Book filename */
    int page;                            /* Last read page (0-based) */
    time_t timestamp;                    /* When this position was saved */
} bookmark_t;

/*
 * Bookmark list structure
 * Contains all bookmarks
 */
typedef struct {
    bookmark_t *bookmarks;               /* Array of bookmarks */
    int count;                           /* Number of bookmarks */
    int capacity;                        /* Allocated capacity */
} bookmark_list_t;

/*
 * Error codes
 */
typedef enum {
    BOOK_SUCCESS = 0,
    BOOK_ERROR_NOT_FOUND = -1,
    BOOK_ERROR_TOO_LARGE = -2,
    BOOK_ERROR_READ_FAILED = -3,
    BOOK_ERROR_OUT_OF_MEMORY = -4,
    BOOK_ERROR_INVALID_PATH = -5,
    BOOK_ERROR_EMPTY_FILE = -6,
    BOOK_ERROR_NO_BOOKS = -7
} book_error_t;

/*
 * Book List Management
 */

/* Create a new book list */
book_list_t* book_list_create(void);

/* Free a book list */
void book_list_free(book_list_t *list);

/* Scan the books directory and populate the list */
int book_list_scan(book_list_t *list, const char *books_dir);

/* Sort books alphabetically by filename */
void book_list_sort(book_list_t *list);

/* Get book metadata by index */
book_metadata_t* book_list_get(book_list_t *list, int index);

/* Find book by filename */
int book_list_find(book_list_t *list, const char *filename);

/*
 * Book Loading and Unloading
 */

/* Load a book's full text into memory */
book_t* book_load(const char *filepath);

/* Unload a book and free its memory */
void book_unload(book_t *book);

/* Check if a book file is valid (exists, readable, not too large) */
int book_validate(const char *filepath, long *out_size);

/*
 * Bookmark Management
 */

/* Create a new bookmark list */
bookmark_list_t* bookmark_list_create(void);

/* Free a bookmark list */
void bookmark_list_free(bookmark_list_t *list);

/* Load bookmarks from file */
int bookmark_list_load(bookmark_list_t *list, const char *filepath);

/* Save bookmarks to file */
int bookmark_list_save(bookmark_list_t *list, const char *filepath);

/* Update or create a bookmark for a book */
int bookmark_update(bookmark_list_t *list, const char *filename, int page);

/* Get bookmark page for a book (returns -1 if not found) */
int bookmark_get(bookmark_list_t *list, const char *filename);

/* Remove a bookmark */
int bookmark_remove(bookmark_list_t *list, const char *filename);

/*
 * Utility Functions
 */

/* Get a human-readable error message for an error code */
const char* book_error_string(book_error_t error);

/* Extract filename from a full path */
const char* book_basename(const char *path);

/* Check if a filename has .txt extension */
int book_is_txt_file(const char *filename);

/* Format file size for display (e.g., "1.5 MB") */
void book_format_size(long size, char *buf, size_t buf_len);

#endif /* BOOK_MANAGER_H */
