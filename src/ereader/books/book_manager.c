/*
 * book_manager.c
 *
 * E-Reader book file management system implementation
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

#include "book_manager.h"
#include "../formats/format_interface.h"
#include "../ui/ui_components.h"
#include "../rendering/text_renderer.h"
#include "../rendering/framebuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

/*
 * Internal helper functions
 */

/* Compare function for qsort - alphabetical by filename */
static int compare_books(const void *a, const void *b) {
    const book_metadata_t *book_a = (const book_metadata_t *)a;
    const book_metadata_t *book_b = (const book_metadata_t *)b;
    return strcasecmp(book_a->filename, book_b->filename);
}

/* Compare function for qsort - alphabetical by filename for bookmarks */
static int compare_bookmarks(const void *a, const void *b) {
    const bookmark_t *bm_a = (const bookmark_t *)a;
    const bookmark_t *bm_b = (const bookmark_t *)b;
    return strcasecmp(bm_a->filename, bm_b->filename);
}

/* Resize book list capacity */
static int book_list_resize(book_list_t *list, int new_capacity) {
    book_metadata_t *new_books = realloc(list->books,
                                         new_capacity * sizeof(book_metadata_t));
    if (!new_books) {
        return BOOK_ERROR_OUT_OF_MEMORY;
    }
    list->books = new_books;
    list->capacity = new_capacity;
    return BOOK_SUCCESS;
}

/* Resize bookmark list capacity */
static int bookmark_list_resize(bookmark_list_t *list, int new_capacity) {
    bookmark_t *new_bookmarks = realloc(list->bookmarks,
                                        new_capacity * sizeof(bookmark_t));
    if (!new_bookmarks) {
        return BOOK_ERROR_OUT_OF_MEMORY;
    }
    list->bookmarks = new_bookmarks;
    list->capacity = new_capacity;
    return BOOK_SUCCESS;
}

/*
 * Book List Management
 */

book_list_t* book_list_create(void) {
    book_list_t *list = malloc(sizeof(book_list_t));
    if (!list) {
        return NULL;
    }

    list->count = 0;
    list->capacity = 32;  /* Initial capacity */
    list->books = malloc(list->capacity * sizeof(book_metadata_t));

    if (!list->books) {
        free(list);
        return NULL;
    }

    return list;
}

void book_list_free(book_list_t *list) {
    if (list) {
        free(list->books);
        free(list);
    }
}

int book_list_scan(book_list_t *list, const char *books_dir, void *fb_ptr) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char filepath[MAX_BOOK_PATH];
    framebuffer_t *fb = (framebuffer_t *)fb_ptr;
    progress_bar_t progress;
    int total_files = 0;
    int processed_files = 0;
    bool show_progress = (fb != NULL);

    /* Reset the list */
    list->count = 0;

    /* Open directory */
    dir = opendir(books_dir);
    if (!dir) {
        fprintf(stderr, "book_list_scan: Failed to open directory %s: %s\n",
                books_dir, strerror(errno));
        return BOOK_ERROR_INVALID_PATH;
    }

    /* First pass: Count total potential book files */
    if (show_progress) {
        while ((entry = readdir(dir)) != NULL) {
            /* Skip hidden files and directories */
            if (entry->d_name[0] == '.') {
                continue;
            }

            /* Check if file is a supported format */
            book_format_type_t format = format_detect_type(entry->d_name);
            if (format != BOOK_FORMAT_UNKNOWN) {
                total_files++;
            }
        }

        /* Rewind directory for second pass */
        rewinddir(dir);

        /* Initialize progress bar if we have files to scan */
        if (total_files > 0) {
            progress_bar_init(&progress, 60, 140, 280, true);
            progress_bar_set_value(&progress, 0, total_files);
        }
    }

    /* Scan for supported book files (.txt, .epub, .pdf) */
    while ((entry = readdir(dir)) != NULL) {
        book_format_type_t format;

        /* Skip hidden files and directories */
        if (entry->d_name[0] == '.') {
            continue;
        }

        /* Check if file is a supported format */
        format = format_detect_type(entry->d_name);
        if (format == BOOK_FORMAT_UNKNOWN) {
            continue;
        }

        /* Build full path */
        snprintf(filepath, sizeof(filepath), "%s/%s", books_dir, entry->d_name);

        /* Get file stats */
        if (stat(filepath, &st) != 0) {
            fprintf(stderr, "book_list_scan: stat failed for %s: %s\n",
                    filepath, strerror(errno));
            if (show_progress) processed_files++;
            continue;
        }

        /* Skip directories */
        if (S_ISDIR(st.st_mode)) {
            if (show_progress) processed_files++;
            continue;
        }

        /* Skip empty files */
        if (st.st_size == 0) {
            fprintf(stderr, "book_list_scan: Skipping empty file %s\n", filepath);
            if (show_progress) processed_files++;
            continue;
        }

        /* Check if we need to resize */
        if (list->count >= list->capacity) {
            int new_capacity = list->capacity * 2;
            if (new_capacity > MAX_BOOKS) {
                new_capacity = MAX_BOOKS;
            }
            if (list->count >= new_capacity) {
                fprintf(stderr, "book_list_scan: Maximum book limit reached (%d)\n",
                        MAX_BOOKS);
                break;
            }
            if (book_list_resize(list, new_capacity) != BOOK_SUCCESS) {
                fprintf(stderr, "book_list_scan: Failed to resize list\n");
                break;
            }
        }

        /* Add to list */
        book_metadata_t *book = &list->books[list->count];
        strncpy(book->filename, entry->d_name, MAX_FILENAME_LENGTH - 1);
        book->filename[MAX_FILENAME_LENGTH - 1] = '\0';
        strncpy(book->filepath, filepath, MAX_BOOK_PATH - 1);
        book->filepath[MAX_BOOK_PATH - 1] = '\0';
        book->size = st.st_size;
        book->modified = st.st_mtime;
        book->bookmark_page = 0;  /* Will be updated from bookmarks */
        book->format = format;    /* Store detected format */

        /* Initialize metadata fields with defaults */
        strncpy(book->title, entry->d_name, 255);
        book->title[255] = '\0';
        book->author[0] = '\0';

        /* Try to extract metadata for EPUB/PDF formats */
        if (format == BOOK_FORMAT_EPUB || format == BOOK_FORMAT_PDF) {
            const book_format_interface_t *interface = format_get_interface(format);
            if (interface && interface->open && interface->get_metadata && interface->close) {
                format_handle_t handle = interface->open(filepath);
                if (handle) {
                    format_metadata_t metadata;
                    if (interface->get_metadata(handle, &metadata) == FORMAT_SUCCESS) {
                        /* Copy title if available and non-empty */
                        if (metadata.title[0] != '\0') {
                            strncpy(book->title, metadata.title, 255);
                            book->title[255] = '\0';
                        }
                        /* Copy author if available */
                        if (metadata.author[0] != '\0') {
                            strncpy(book->author, metadata.author, 255);
                            book->author[255] = '\0';
                        }
                    }
                    interface->close(handle);
                }
            }
        }

        list->count++;

        /* Update progress bar (only every 3 files to reduce e-paper refreshes) */
        if (show_progress && total_files > 0) {
            processed_files++;
            if (processed_files % 3 == 0 || processed_files == total_files) {
                progress_bar_set_value(&progress, processed_files, total_files);
                fb_clear(fb, COLOR_WHITE);
                text_render_string(fb, 100, 100, "Scanning library...", COLOR_BLACK);
                progress_bar_render(&progress, fb);
                /* Note: Caller should handle display refresh */
            }
        }
    }

    closedir(dir);

    if (list->count == 0) {
        return BOOK_ERROR_NO_BOOKS;
    }

    return BOOK_SUCCESS;
}

void book_list_sort(book_list_t *list) {
    if (list && list->count > 0) {
        qsort(list->books, list->count, sizeof(book_metadata_t), compare_books);
    }
}

book_metadata_t* book_list_get(book_list_t *list, int index) {
    if (!list || index < 0 || index >= list->count) {
        return NULL;
    }
    return &list->books[index];
}

int book_list_find(book_list_t *list, const char *filename) {
    if (!list || !filename) {
        return -1;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->books[i].filename, filename) == 0) {
            return i;
        }
    }

    return -1;
}

/*
 * Book Loading and Unloading
 */

int book_validate(const char *filepath, long *out_size) {
    struct stat st;

    if (!filepath) {
        return BOOK_ERROR_INVALID_PATH;
    }

    /* Check if file exists and is readable */
    if (access(filepath, R_OK) != 0) {
        return BOOK_ERROR_NOT_FOUND;
    }

    /* Get file size */
    if (stat(filepath, &st) != 0) {
        return BOOK_ERROR_READ_FAILED;
    }

    /* Check if it's a regular file */
    if (!S_ISREG(st.st_mode)) {
        return BOOK_ERROR_INVALID_PATH;
    }

    /* Check file size */
    if (st.st_size == 0) {
        return BOOK_ERROR_EMPTY_FILE;
    }

    if (st.st_size > MAX_BOOK_SIZE) {
        return BOOK_ERROR_TOO_LARGE;
    }

    if (out_size) {
        *out_size = st.st_size;
    }

    return BOOK_SUCCESS;
}

book_t* book_load(const char *filepath) {
    FILE *f;
    book_t *book;
    long size;
    size_t bytes_read;
    int validation_result;

    /* Validate file */
    validation_result = book_validate(filepath, &size);
    if (validation_result != BOOK_SUCCESS) {
        fprintf(stderr, "book_load: Validation failed for %s: %s\n",
                filepath, book_error_string(validation_result));
        return NULL;
    }

    /* Open file */
    f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "book_load: Failed to open %s: %s\n",
                filepath, strerror(errno));
        return NULL;
    }

    /* Allocate book structure */
    book = malloc(sizeof(book_t));
    if (!book) {
        fprintf(stderr, "book_load: Failed to allocate book structure\n");
        fclose(f);
        return NULL;
    }

    /* Allocate text buffer */
    book->text = malloc(size + 1);
    if (!book->text) {
        fprintf(stderr, "book_load: Failed to allocate %ld bytes for text\n", size);
        free(book);
        fclose(f);
        return NULL;
    }

    /* Read file content */
    bytes_read = fread(book->text, 1, size, f);
    fclose(f);

    if (bytes_read != (size_t)size) {
        fprintf(stderr, "book_load: Read error: expected %ld bytes, got %zu\n",
                size, bytes_read);
        free(book->text);
        free(book);
        return NULL;
    }

    /* Null-terminate */
    book->text[size] = '\0';
    book->text_length = size;

    /* Extract filename */
    const char *basename = book_basename(filepath);
    strncpy(book->filename, basename, MAX_FILENAME_LENGTH - 1);
    book->filename[MAX_FILENAME_LENGTH - 1] = '\0';

    return book;
}

void book_unload(book_t *book) {
    if (book) {
        free(book->text);
        free(book);
    }
}

/*
 * Bookmark Management
 */

bookmark_list_t* bookmark_list_create(void) {
    bookmark_list_t *list = malloc(sizeof(bookmark_list_t));
    if (!list) {
        return NULL;
    }

    list->count = 0;
    list->capacity = 32;
    list->bookmarks = malloc(list->capacity * sizeof(bookmark_t));

    if (!list->bookmarks) {
        free(list);
        return NULL;
    }

    return list;
}

void bookmark_list_free(bookmark_list_t *list) {
    if (list) {
        free(list->bookmarks);
        free(list);
    }
}

int bookmark_list_load(bookmark_list_t *list, const char *filepath) {
    FILE *f;
    char line[512];
    int line_num = 0;

    /* Reset list */
    list->count = 0;

    /* Open file */
    f = fopen(filepath, "r");
    if (!f) {
        /* File doesn't exist yet - this is okay */
        if (errno == ENOENT) {
            return BOOK_SUCCESS;
        }
        fprintf(stderr, "bookmark_list_load: Failed to open %s: %s\n",
                filepath, strerror(errno));
        return BOOK_ERROR_READ_FAILED;
    }

    /* Read line by line */
    while (fgets(line, sizeof(line), f)) {
        line_num++;

        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') {
            continue;
        }

        /* Parse CSV: filename,page,timestamp */
        char filename[MAX_FILENAME_LENGTH];
        int page;
        long timestamp;

        int fields = sscanf(line, "%255[^,],%d,%ld", filename, &page, &timestamp);

        if (fields != 3) {
            fprintf(stderr, "bookmark_list_load: Parse error at line %d\n", line_num);
            continue;
        }

        /* Validate page number */
        if (page < 0) {
            fprintf(stderr, "bookmark_list_load: Invalid page %d at line %d\n",
                    page, line_num);
            continue;
        }

        /* Check if we need to resize */
        if (list->count >= list->capacity) {
            int new_capacity = list->capacity * 2;
            if (bookmark_list_resize(list, new_capacity) != BOOK_SUCCESS) {
                fprintf(stderr, "bookmark_list_load: Failed to resize list\n");
                break;
            }
        }

        /* Add bookmark */
        bookmark_t *bm = &list->bookmarks[list->count];
        strncpy(bm->filename, filename, MAX_FILENAME_LENGTH - 1);
        bm->filename[MAX_FILENAME_LENGTH - 1] = '\0';
        bm->page = page;
        bm->timestamp = (time_t)timestamp;

        list->count++;
    }

    fclose(f);
    return BOOK_SUCCESS;
}

int bookmark_list_save(bookmark_list_t *list, const char *filepath) {
    FILE *f;

    if (!list || !filepath) {
        return BOOK_ERROR_INVALID_PATH;
    }

    /* Create directory if it doesn't exist */
    char dir[MAX_BOOK_PATH];
    strncpy(dir, filepath, MAX_BOOK_PATH - 1);
    dir[MAX_BOOK_PATH - 1] = '\0';
    char *last_slash = strrchr(dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        mkdir(dir, 0755);  /* Create /etc/ereader if needed */
    }

    /* Open file for writing */
    f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "bookmark_list_save: Failed to open %s: %s\n",
                filepath, strerror(errno));
        return BOOK_ERROR_READ_FAILED;
    }

    /* Write header */
    fprintf(f, "# E-Reader Bookmarks File\n");
    fprintf(f, "# Format: filename,page_number,last_read_timestamp\n");

    /* Write bookmarks */
    for (int i = 0; i < list->count; i++) {
        bookmark_t *bm = &list->bookmarks[i];
        fprintf(f, "%s,%d,%ld\n", bm->filename, bm->page, (long)bm->timestamp);
    }

    fclose(f);
    return BOOK_SUCCESS;
}

int bookmark_update(bookmark_list_t *list, const char *filename, int page) {
    if (!list || !filename || page < 0) {
        return BOOK_ERROR_INVALID_PATH;
    }

    /* Search for existing bookmark */
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->bookmarks[i].filename, filename) == 0) {
            /* Update existing bookmark */
            list->bookmarks[i].page = page;
            list->bookmarks[i].timestamp = time(NULL);
            return BOOK_SUCCESS;
        }
    }

    /* Bookmark not found - create new one */
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        if (bookmark_list_resize(list, new_capacity) != BOOK_SUCCESS) {
            return BOOK_ERROR_OUT_OF_MEMORY;
        }
    }

    bookmark_t *bm = &list->bookmarks[list->count];
    strncpy(bm->filename, filename, MAX_FILENAME_LENGTH - 1);
    bm->filename[MAX_FILENAME_LENGTH - 1] = '\0';
    bm->page = page;
    bm->timestamp = time(NULL);
    list->count++;

    return BOOK_SUCCESS;
}

int bookmark_get(bookmark_list_t *list, const char *filename) {
    if (!list || !filename) {
        return -1;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->bookmarks[i].filename, filename) == 0) {
            return list->bookmarks[i].page;
        }
    }

    return -1;  /* Not found */
}

int bookmark_remove(bookmark_list_t *list, const char *filename) {
    if (!list || !filename) {
        return BOOK_ERROR_INVALID_PATH;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->bookmarks[i].filename, filename) == 0) {
            /* Remove by shifting subsequent bookmarks */
            for (int j = i; j < list->count - 1; j++) {
                list->bookmarks[j] = list->bookmarks[j + 1];
            }
            list->count--;
            return BOOK_SUCCESS;
        }
    }

    return BOOK_ERROR_NOT_FOUND;
}

/*
 * Utility Functions
 */

const char* book_error_string(book_error_t error) {
    switch (error) {
        case BOOK_SUCCESS:
            return "Success";
        case BOOK_ERROR_NOT_FOUND:
            return "Book not found";
        case BOOK_ERROR_TOO_LARGE:
            return "Book file too large";
        case BOOK_ERROR_READ_FAILED:
            return "Failed to read book";
        case BOOK_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case BOOK_ERROR_INVALID_PATH:
            return "Invalid file path";
        case BOOK_ERROR_EMPTY_FILE:
            return "Empty file";
        case BOOK_ERROR_NO_BOOKS:
            return "No books found";
        default:
            return "Unknown error";
    }
}

const char* book_basename(const char *path) {
    if (!path) {
        return "";
    }

    const char *last_slash = strrchr(path, '/');
    if (last_slash) {
        return last_slash + 1;
    }

    return path;
}

int book_is_txt_file(const char *filename) {
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

void book_format_size(long size, char *buf, size_t buf_len) {
    if (!buf || buf_len == 0) {
        return;
    }

    if (size < 1024) {
        snprintf(buf, buf_len, "%ld B", size);
    } else if (size < 1024 * 1024) {
        snprintf(buf, buf_len, "%.1f KB", size / 1024.0);
    } else if (size < 1024 * 1024 * 1024) {
        snprintf(buf, buf_len, "%.1f MB", size / (1024.0 * 1024.0));
    } else {
        snprintf(buf, buf_len, "%.1f GB", size / (1024.0 * 1024.0 * 1024.0));
    }
}
