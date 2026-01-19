/*
 * download_manager.h - HTTP/HTTPS Download Manager
 *
 * This module provides book download functionality using wget.
 * It handles HTTP/HTTPS downloads with progress tracking, error handling,
 * and file validation.
 *
 * Key features:
 * - HTTP/HTTPS download with progress tracking
 * - Save files to /books/ directory
 * - Validate downloaded files (format and integrity)
 * - Handle network timeouts and errors
 * - Disk space checking
 * - Bandwidth estimation
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

#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Maximum lengths for download data
 */
#define MAX_URL_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512
#define MAX_ERROR_MESSAGE_LENGTH 256

/*
 * Download directory
 */
#define DOWNLOAD_DIRECTORY "/books"

/*
 * Download states
 */
typedef enum {
    DOWNLOAD_STATE_IDLE,           /* No download in progress */
    DOWNLOAD_STATE_CONNECTING,     /* Connecting to server */
    DOWNLOAD_STATE_DOWNLOADING,    /* Download in progress */
    DOWNLOAD_STATE_VALIDATING,     /* Validating downloaded file */
    DOWNLOAD_STATE_COMPLETED,      /* Download successful */
    DOWNLOAD_STATE_FAILED,         /* Download failed */
    DOWNLOAD_STATE_CANCELLED       /* Download cancelled by user */
} download_state_t;

/*
 * Download error codes
 */
typedef enum {
    DOWNLOAD_ERROR_NONE = 0,           /* No error */
    DOWNLOAD_ERROR_INVALID_URL,        /* Invalid or malformed URL */
    DOWNLOAD_ERROR_NETWORK_TIMEOUT,    /* Network connection timeout */
    DOWNLOAD_ERROR_DNS_FAILED,         /* DNS resolution failed */
    DOWNLOAD_ERROR_CONNECTION_FAILED,  /* Failed to connect to server */
    DOWNLOAD_ERROR_HTTP_404,           /* HTTP 404 Not Found */
    DOWNLOAD_ERROR_HTTP_403,           /* HTTP 403 Forbidden */
    DOWNLOAD_ERROR_HTTP_ERROR,         /* Other HTTP error */
    DOWNLOAD_ERROR_DISK_FULL,          /* Insufficient disk space */
    DOWNLOAD_ERROR_WRITE_FAILED,       /* Failed to write file */
    DOWNLOAD_ERROR_INVALID_FILE,       /* Downloaded file is invalid */
    DOWNLOAD_ERROR_FILE_EXISTS,        /* File already exists */
    DOWNLOAD_ERROR_CANCELLED,          /* Download cancelled */
    DOWNLOAD_ERROR_UNKNOWN             /* Unknown error */
} download_error_t;

/*
 * Supported file formats for validation
 */
typedef enum {
    FILE_FORMAT_TXT,     /* Plain text (.txt) */
    FILE_FORMAT_EPUB,    /* EPUB e-book (.epub) */
    FILE_FORMAT_PDF,     /* PDF document (.pdf) */
    FILE_FORMAT_UNKNOWN  /* Unknown or unsupported format */
} file_format_t;

/*
 * Download progress information
 *
 * Contains real-time download progress data including bytes transferred,
 * download speed, and estimated time remaining.
 */
typedef struct {
    download_state_t state;            /* Current download state */
    uint64_t bytes_downloaded;         /* Bytes downloaded so far */
    uint64_t total_bytes;              /* Total file size (0 if unknown) */
    int progress_percentage;           /* Progress percentage (0-100) */
    uint64_t download_speed;           /* Download speed in bytes/sec */
    int eta_seconds;                   /* Estimated time remaining (seconds, -1 if unknown) */
    char current_file[MAX_FILENAME_LENGTH]; /* Currently downloading file */
    download_error_t last_error;       /* Last error code */
    char error_message[MAX_ERROR_MESSAGE_LENGTH]; /* Human-readable error message */
} download_progress_t;

/*
 * Download options
 *
 * Configuration options for download operations.
 */
typedef struct {
    int timeout_seconds;               /* Network timeout in seconds (default: 30) */
    bool overwrite_existing;           /* Overwrite existing files (default: false) */
    bool validate_file;                /* Validate file after download (default: true) */
    uint64_t max_file_size;            /* Maximum file size in bytes (0 = no limit) */
    char user_agent[128];              /* User-Agent header for HTTP requests */
} download_options_t;

/*
 * Function prototypes
 */

/**
 * Initialize download manager
 *
 * Sets up the download manager and checks if the download directory exists.
 * Creates the directory if it doesn't exist.
 *
 * Returns:
 *   0 on success
 *   -1 on failure (cannot create download directory, or system error)
 */
int download_manager_init(void);

/**
 * Cleanup download manager
 *
 * Frees any resources allocated by the download manager.
 * Cancels any active downloads.
 */
void download_manager_cleanup(void);

/**
 * Download a file from URL
 *
 * Downloads a file from the specified URL to the download directory.
 * This is a blocking operation that may take several seconds or minutes
 * depending on file size and network speed.
 *
 * The filename is extracted from the URL or can be specified explicitly.
 * Progress can be monitored by calling download_manager_get_progress()
 * from another thread.
 *
 * Parameters:
 *   url - Full URL to download (http:// or https://)
 *   filename - Optional custom filename (NULL to extract from URL)
 *   options - Download options (NULL for defaults)
 *
 * Returns:
 *   0 on success (file downloaded and validated)
 *   -1 on failure (check download_manager_get_progress for error details)
 *
 * Example:
 *   download_options_t opts = {0};
 *   opts.timeout_seconds = 60;
 *   opts.validate_file = true;
 *
 *   if (download_manager_download(
 *       "https://www.gutenberg.org/ebooks/11.txt.utf-8",
 *       "alice.txt",
 *       &opts) == 0) {
 *       printf("Download complete!\n");
 *   }
 */
int download_manager_download(const char *url, const char *filename,
                               const download_options_t *options);

/**
 * Get current download progress
 *
 * Retrieves the current download state, progress, and error information.
 * This can be called during an active download to monitor progress.
 *
 * Parameters:
 *   progress - Output: Current download progress
 *
 * Returns:
 *   0 on success
 *   -1 on failure (system error)
 *
 * Example:
 *   download_progress_t progress;
 *   download_manager_get_progress(&progress);
 *   printf("Downloaded %llu/%llu bytes (%d%%)\n",
 *          progress.bytes_downloaded,
 *          progress.total_bytes,
 *          progress.progress_percentage);
 */
int download_manager_get_progress(download_progress_t *progress);

/**
 * Cancel active download
 *
 * Cancels the currently active download and cleans up partial files.
 * This is safe to call even if no download is active.
 *
 * Returns:
 *   0 on success
 *   -1 on failure (no active download)
 */
int download_manager_cancel(void);

/**
 * Check if download is active
 *
 * Returns whether a download is currently in progress.
 *
 * Returns:
 *   true if download is active
 *   false if no download is active
 */
bool download_manager_is_active(void);

/**
 * Get available disk space
 *
 * Returns the available disk space in the download directory.
 * Useful for checking before starting a large download.
 *
 * Returns:
 *   Available space in bytes
 *   0 on error
 */
uint64_t download_manager_get_available_space(void);

/**
 * Validate downloaded file
 *
 * Checks if a file is a valid e-book format and not corrupt.
 * Verifies file signature and basic structure.
 *
 * Parameters:
 *   filepath - Full path to file to validate
 *   format - Output: Detected file format
 *
 * Returns:
 *   0 if file is valid
 *   -1 if file is invalid or corrupt
 *
 * Example:
 *   file_format_t format;
 *   if (download_manager_validate_file("/books/alice.txt", &format) == 0) {
 *       printf("Valid %s file\n", file_format_to_string(format));
 *   }
 */
int download_manager_validate_file(const char *filepath, file_format_t *format);

/**
 * Extract filename from URL
 *
 * Extracts the filename from a URL. This is used when no explicit
 * filename is provided to download_manager_download().
 *
 * Parameters:
 *   url - URL to extract filename from
 *   filename - Output buffer for filename (must be at least MAX_FILENAME_LENGTH bytes)
 *   max_len - Size of filename buffer
 *
 * Returns:
 *   0 on success
 *   -1 on failure (invalid URL, no filename found)
 *
 * Example:
 *   char filename[MAX_FILENAME_LENGTH];
 *   download_manager_extract_filename(
 *       "https://www.gutenberg.org/files/11/11-0.txt",
 *       filename, sizeof(filename));
 *   // filename = "11-0.txt"
 */
int download_manager_extract_filename(const char *url, char *filename, int max_len);

/**
 * Get default download options
 *
 * Returns a download_options_t structure with sensible defaults.
 *
 * Returns:
 *   Default options structure
 */
download_options_t download_manager_get_default_options(void);

/*
 * Utility functions
 */

/**
 * Convert download state to human-readable string
 *
 * Parameters:
 *   state - Download state
 *
 * Returns:
 *   String representation (e.g., "Downloading", "Completed")
 */
const char* download_state_to_string(download_state_t state);

/**
 * Convert download error code to human-readable string
 *
 * Parameters:
 *   error - Error code
 *
 * Returns:
 *   String representation (e.g., "Network timeout", "Disk full")
 */
const char* download_error_to_string(download_error_t error);

/**
 * Convert file format to human-readable string
 *
 * Parameters:
 *   format - File format
 *
 * Returns:
 *   String representation (e.g., "TXT", "EPUB", "PDF")
 */
const char* file_format_to_string(file_format_t format);

/**
 * Format bytes as human-readable size
 *
 * Converts byte count to human-readable format (KB, MB, GB).
 *
 * Parameters:
 *   bytes - Number of bytes
 *   buffer - Output buffer (must be at least 32 bytes)
 *   buffer_size - Size of output buffer
 *
 * Example:
 *   char size_str[32];
 *   download_manager_format_size(1536000, size_str, sizeof(size_str));
 *   // size_str = "1.5 MB"
 */
void download_manager_format_size(uint64_t bytes, char *buffer, int buffer_size);

/**
 * Format time duration as human-readable string
 *
 * Converts seconds to human-readable format (e.g., "5m 30s", "1h 23m").
 *
 * Parameters:
 *   seconds - Duration in seconds
 *   buffer - Output buffer (must be at least 32 bytes)
 *   buffer_size - Size of output buffer
 *
 * Example:
 *   char eta_str[32];
 *   download_manager_format_time(90, eta_str, sizeof(eta_str));
 *   // eta_str = "1m 30s"
 */
void download_manager_format_time(int seconds, char *buffer, int buffer_size);

#endif /* DOWNLOAD_MANAGER_H */
