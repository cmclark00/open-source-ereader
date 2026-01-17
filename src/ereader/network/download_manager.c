/*
 * download_manager.c - HTTP/HTTPS Download Manager Implementation
 *
 * This module implements book download functionality using wget.
 * It provides progress tracking, error handling, and file validation.
 *
 * Author: E-Reader Project
 * License: Open Source
 */

#include "download_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <libgen.h>
#include <ctype.h>

/* Global download progress state */
static download_progress_t g_download_progress = {
    .state = DOWNLOAD_STATE_IDLE,
    .bytes_downloaded = 0,
    .total_bytes = 0,
    .progress_percentage = 0,
    .download_speed = 0,
    .eta_seconds = -1,
    .current_file = {0},
    .last_error = DOWNLOAD_ERROR_NONE,
    .error_message = {0}
};

/* Progress file for wget output parsing */
#define PROGRESS_FILE "/tmp/wget_progress.txt"
#define WGET_LOG_FILE "/tmp/wget_log.txt"

/**
 * Initialize download manager
 */
int download_manager_init(void) {
    /* Check if download directory exists */
    struct stat st;
    if (stat(DOWNLOAD_DIRECTORY, &st) != 0) {
        /* Create directory if it doesn't exist */
        if (mkdir(DOWNLOAD_DIRECTORY, 0755) != 0) {
            fprintf(stderr, "download_manager: Failed to create %s: %s\n",
                    DOWNLOAD_DIRECTORY, strerror(errno));
            return -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "download_manager: %s exists but is not a directory\n",
                DOWNLOAD_DIRECTORY);
        return -1;
    }

    /* Initialize progress state */
    memset(&g_download_progress, 0, sizeof(g_download_progress));
    g_download_progress.state = DOWNLOAD_STATE_IDLE;
    g_download_progress.eta_seconds = -1;

    return 0;
}

/**
 * Cleanup download manager
 */
void download_manager_cleanup(void) {
    /* Cancel any active download */
    if (download_manager_is_active()) {
        download_manager_cancel();
    }

    /* Clean up temporary files */
    unlink(PROGRESS_FILE);
    unlink(WGET_LOG_FILE);
}

/**
 * Extract filename from URL
 */
int download_manager_extract_filename(const char *url, char *filename, int max_len) {
    if (!url || !filename || max_len <= 0) {
        return -1;
    }

    /* Find last '/' in URL */
    const char *last_slash = strrchr(url, '/');
    if (!last_slash || last_slash[1] == '\0') {
        return -1; /* No filename in URL */
    }

    /* Extract filename */
    const char *url_filename = last_slash + 1;

    /* Remove query parameters (everything after '?') */
    const char *query = strchr(url_filename, '?');
    int name_len;
    if (query) {
        name_len = query - url_filename;
    } else {
        name_len = strlen(url_filename);
    }

    /* Check length */
    if (name_len >= max_len) {
        return -1; /* Filename too long */
    }

    /* Copy filename */
    strncpy(filename, url_filename, name_len);
    filename[name_len] = '\0';

    return 0;
}

/**
 * Get default download options
 */
download_options_t download_manager_get_default_options(void) {
    download_options_t options = {0};
    options.timeout_seconds = 30;
    options.overwrite_existing = false;
    options.validate_file = true;
    options.max_file_size = 10 * 1024 * 1024; /* 10 MB default max */
    snprintf(options.user_agent, sizeof(options.user_agent),
             "OpenSourceEReader/1.0 (Raspberry Pi; educational project)");
    return options;
}

/**
 * Parse wget progress output
 *
 * Parses wget's progress output to update download progress.
 * Expected format from wget --progress=bar:force:
 * "     0K .......... .......... ..........  3%  123K 2s"
 */
static void parse_wget_progress(const char *line) {
    /* Look for percentage in the line */
    const char *percent_pos = strchr(line, '%');
    if (!percent_pos) {
        return;
    }

    /* Parse percentage (work backwards from %) */
    const char *p = percent_pos - 1;
    while (p >= line && isspace(*p)) p--;
    const char *percent_end = p + 1;
    while (p >= line && isdigit(*p)) p--;

    if (p < percent_end - 1) {
        int percentage = atoi(p + 1);
        if (percentage >= 0 && percentage <= 100) {
            g_download_progress.progress_percentage = percentage;
        }
    }

    /* Try to parse speed and ETA */
    const char *speed_pos = percent_pos + 1;
    while (*speed_pos && isspace(*speed_pos)) speed_pos++;

    /* Speed format: "123K" or "1.2M" */
    if (*speed_pos && isdigit(*speed_pos)) {
        float speed_val = atof(speed_pos);
        const char *unit = speed_pos;
        while (*unit && (isdigit(*unit) || *unit == '.')) unit++;

        if (*unit == 'K' || *unit == 'k') {
            g_download_progress.download_speed = (uint64_t)(speed_val * 1024);
        } else if (*unit == 'M' || *unit == 'm') {
            g_download_progress.download_speed = (uint64_t)(speed_val * 1024 * 1024);
        }

        /* ETA is typically after speed */
        while (*unit && !isspace(*unit)) unit++;
        while (*unit && isspace(*unit)) unit++;
        if (*unit && isdigit(*unit)) {
            /* Simple ETA parsing - just grab the number */
            int eta = atoi(unit);
            const char *eta_unit = unit;
            while (*eta_unit && isdigit(*eta_unit)) eta_unit++;

            /* Check for 's', 'm', 'h' suffix */
            if (*eta_unit == 'm') {
                eta *= 60;
            } else if (*eta_unit == 'h') {
                eta *= 3600;
            }
            g_download_progress.eta_seconds = eta;
        }
    }
}

/**
 * Monitor wget progress
 *
 * Reads wget's output and updates progress information.
 */
static void monitor_wget_progress(void) {
    FILE *fp = fopen(WGET_LOG_FILE, "r");
    if (!fp) {
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        parse_wget_progress(line);
    }

    fclose(fp);
}

/**
 * Download a file from URL
 */
int download_manager_download(const char *url, const char *filename,
                               const download_options_t *options) {
    if (!url) {
        g_download_progress.last_error = DOWNLOAD_ERROR_INVALID_URL;
        snprintf(g_download_progress.error_message,
                 sizeof(g_download_progress.error_message),
                 "Invalid URL (NULL)");
        return -1;
    }

    /* Get default options if none provided */
    download_options_t default_opts = download_manager_get_default_options();
    const download_options_t *opts = options ? options : &default_opts;

    /* Extract filename if not provided */
    char auto_filename[MAX_FILENAME_LENGTH];
    const char *final_filename = filename;
    if (!final_filename) {
        if (download_manager_extract_filename(url, auto_filename,
                                               sizeof(auto_filename)) != 0) {
            g_download_progress.last_error = DOWNLOAD_ERROR_INVALID_URL;
            snprintf(g_download_progress.error_message,
                     sizeof(g_download_progress.error_message),
                     "Cannot extract filename from URL");
            return -1;
        }
        final_filename = auto_filename;
    }

    /* Build full destination path */
    char dest_path[MAX_PATH_LENGTH];
    snprintf(dest_path, sizeof(dest_path), "%s/%s",
             DOWNLOAD_DIRECTORY, final_filename);

    /* Check if file already exists */
    if (!opts->overwrite_existing && access(dest_path, F_OK) == 0) {
        g_download_progress.last_error = DOWNLOAD_ERROR_FILE_EXISTS;
        snprintf(g_download_progress.error_message,
                 sizeof(g_download_progress.error_message),
                 "File already exists: %s", final_filename);
        return -1;
    }

    /* Initialize progress */
    g_download_progress.state = DOWNLOAD_STATE_CONNECTING;
    g_download_progress.bytes_downloaded = 0;
    g_download_progress.total_bytes = 0;
    g_download_progress.progress_percentage = 0;
    g_download_progress.download_speed = 0;
    g_download_progress.eta_seconds = -1;
    g_download_progress.last_error = DOWNLOAD_ERROR_NONE;
    strncpy(g_download_progress.current_file, final_filename,
            sizeof(g_download_progress.current_file) - 1);
    g_download_progress.error_message[0] = '\0';

    /* Clean up old log files */
    unlink(WGET_LOG_FILE);

    /* Build wget command */
    char wget_cmd[2048];
    snprintf(wget_cmd, sizeof(wget_cmd),
             "wget -T %d -t 3 --progress=bar:force "
             "-U \"%s\" "
             "-O \"%s\" "
             "\"%s\" "
             "2>&1 | tee \"%s\"",
             opts->timeout_seconds,
             opts->user_agent,
             dest_path,
             url,
             WGET_LOG_FILE);

    /* Execute wget */
    g_download_progress.state = DOWNLOAD_STATE_DOWNLOADING;

    int ret = system(wget_cmd);

    /* Check wget exit status */
    if (ret != 0) {
        g_download_progress.state = DOWNLOAD_STATE_FAILED;

        /* Try to determine error from exit code */
        /* wget exit codes: 0=success, 1=generic, 2=parse error, 3=file I/O,
           4=network, 5=SSL, 6=auth, 7=protocol, 8=server error */
        int exit_code = WEXITSTATUS(ret);

        switch (exit_code) {
            case 1:
            case 4:
                g_download_progress.last_error = DOWNLOAD_ERROR_NETWORK_TIMEOUT;
                snprintf(g_download_progress.error_message,
                         sizeof(g_download_progress.error_message),
                         "Network timeout or connection failed");
                break;
            case 3:
                g_download_progress.last_error = DOWNLOAD_ERROR_WRITE_FAILED;
                snprintf(g_download_progress.error_message,
                         sizeof(g_download_progress.error_message),
                         "Failed to write file (disk full?)");
                break;
            case 8:
                /* Check for 404 in log */
                FILE *log = fopen(WGET_LOG_FILE, "r");
                if (log) {
                    char line[256];
                    bool found_404 = false;
                    while (fgets(line, sizeof(line), log)) {
                        if (strstr(line, "404") || strstr(line, "Not Found")) {
                            found_404 = true;
                            break;
                        }
                    }
                    fclose(log);

                    if (found_404) {
                        g_download_progress.last_error = DOWNLOAD_ERROR_HTTP_404;
                        snprintf(g_download_progress.error_message,
                                 sizeof(g_download_progress.error_message),
                                 "File not found (HTTP 404)");
                    } else {
                        g_download_progress.last_error = DOWNLOAD_ERROR_HTTP_ERROR;
                        snprintf(g_download_progress.error_message,
                                 sizeof(g_download_progress.error_message),
                                 "HTTP error");
                    }
                }
                break;
            default:
                g_download_progress.last_error = DOWNLOAD_ERROR_UNKNOWN;
                snprintf(g_download_progress.error_message,
                         sizeof(g_download_progress.error_message),
                         "Download failed (wget exit code %d)", exit_code);
        }

        /* Remove partial file */
        unlink(dest_path);
        return -1;
    }

    /* Validate file if requested */
    if (opts->validate_file) {
        g_download_progress.state = DOWNLOAD_STATE_VALIDATING;
        file_format_t format;
        if (download_manager_validate_file(dest_path, &format) != 0) {
            g_download_progress.state = DOWNLOAD_STATE_FAILED;
            g_download_progress.last_error = DOWNLOAD_ERROR_INVALID_FILE;
            snprintf(g_download_progress.error_message,
                     sizeof(g_download_progress.error_message),
                     "Downloaded file is invalid or corrupt");
            unlink(dest_path);
            return -1;
        }
    }

    /* Success! */
    g_download_progress.state = DOWNLOAD_STATE_COMPLETED;
    g_download_progress.progress_percentage = 100;

    /* Get final file size */
    struct stat st;
    if (stat(dest_path, &st) == 0) {
        g_download_progress.bytes_downloaded = st.st_size;
        g_download_progress.total_bytes = st.st_size;
    }

    return 0;
}

/**
 * Get current download progress
 */
int download_manager_get_progress(download_progress_t *progress) {
    if (!progress) {
        return -1;
    }

    /* Update progress from wget output if downloading */
    if (g_download_progress.state == DOWNLOAD_STATE_DOWNLOADING) {
        monitor_wget_progress();
    }

    /* Copy current progress */
    memcpy(progress, &g_download_progress, sizeof(download_progress_t));

    return 0;
}

/**
 * Cancel active download
 */
int download_manager_cancel(void) {
    if (!download_manager_is_active()) {
        return -1; /* No active download */
    }

    /* Kill any running wget processes */
    system("pkill -9 wget");

    /* Update state */
    g_download_progress.state = DOWNLOAD_STATE_CANCELLED;
    g_download_progress.last_error = DOWNLOAD_ERROR_CANCELLED;
    snprintf(g_download_progress.error_message,
             sizeof(g_download_progress.error_message),
             "Download cancelled by user");

    /* Remove partial file */
    char dest_path[MAX_PATH_LENGTH];
    snprintf(dest_path, sizeof(dest_path), "%s/%s",
             DOWNLOAD_DIRECTORY, g_download_progress.current_file);
    unlink(dest_path);

    return 0;
}

/**
 * Check if download is active
 */
bool download_manager_is_active(void) {
    return (g_download_progress.state == DOWNLOAD_STATE_CONNECTING ||
            g_download_progress.state == DOWNLOAD_STATE_DOWNLOADING ||
            g_download_progress.state == DOWNLOAD_STATE_VALIDATING);
}

/**
 * Get available disk space
 */
uint64_t download_manager_get_available_space(void) {
    struct statvfs stat;

    if (statvfs(DOWNLOAD_DIRECTORY, &stat) != 0) {
        return 0; /* Error */
    }

    /* Available space = fragment size × available fragments */
    return (uint64_t)stat.f_bavail * (uint64_t)stat.f_frsize;
}

/**
 * Validate downloaded file
 */
int download_manager_validate_file(const char *filepath, file_format_t *format) {
    if (!filepath) {
        return -1;
    }

    /* Check file exists and is readable */
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        return -1;
    }

    /* Read file signature (first few bytes) */
    unsigned char sig[16];
    size_t bytes_read = fread(sig, 1, sizeof(sig), fp);
    fclose(fp);

    if (bytes_read < 4) {
        return -1; /* File too small */
    }

    /* Detect file format by signature */
    file_format_t detected_format = FILE_FORMAT_UNKNOWN;

    /* Check for EPUB signature (ZIP file starting with PK) */
    if (sig[0] == 0x50 && sig[1] == 0x4B &&
        (sig[2] == 0x03 || sig[2] == 0x05) && sig[3] == 0x04) {
        detected_format = FILE_FORMAT_EPUB;
    }
    /* Check for PDF signature */
    else if (sig[0] == 0x25 && sig[1] == 0x50 && sig[2] == 0x44 && sig[3] == 0x46) {
        detected_format = FILE_FORMAT_PDF;
    }
    /* Assume plain text for everything else */
    else {
        /* For text files, just check if it's mostly ASCII/UTF-8 */
        bool looks_like_text = true;
        for (size_t i = 0; i < bytes_read; i++) {
            unsigned char c = sig[i];
            /* Allow printable ASCII, newlines, tabs, and UTF-8 continuation bytes */
            if (!(c >= 0x20 && c <= 0x7E) &&  /* Printable ASCII */
                c != '\n' && c != '\r' && c != '\t' &&  /* Whitespace */
                !(c >= 0x80 && c <= 0xBF)) {  /* UTF-8 continuation */
                looks_like_text = false;
                break;
            }
        }

        if (looks_like_text) {
            detected_format = FILE_FORMAT_TXT;
        } else {
            return -1; /* Unknown binary format */
        }
    }

    /* Return detected format */
    if (format) {
        *format = detected_format;
    }

    /* Check file size is reasonable (not zero, not too large) */
    struct stat st;
    if (stat(filepath, &st) != 0) {
        return -1;
    }

    if (st.st_size == 0) {
        return -1; /* Empty file */
    }

    if (st.st_size > 50 * 1024 * 1024) {
        /* Files larger than 50MB are suspicious for e-books */
        return -1;
    }

    return 0; /* File is valid */
}

/*
 * Utility functions
 */

/**
 * Convert download state to string
 */
const char* download_state_to_string(download_state_t state) {
    switch (state) {
        case DOWNLOAD_STATE_IDLE:        return "Idle";
        case DOWNLOAD_STATE_CONNECTING:  return "Connecting";
        case DOWNLOAD_STATE_DOWNLOADING: return "Downloading";
        case DOWNLOAD_STATE_VALIDATING:  return "Validating";
        case DOWNLOAD_STATE_COMPLETED:   return "Completed";
        case DOWNLOAD_STATE_FAILED:      return "Failed";
        case DOWNLOAD_STATE_CANCELLED:   return "Cancelled";
        default:                         return "Unknown";
    }
}

/**
 * Convert download error to string
 */
const char* download_error_to_string(download_error_t error) {
    switch (error) {
        case DOWNLOAD_ERROR_NONE:               return "No error";
        case DOWNLOAD_ERROR_INVALID_URL:        return "Invalid URL";
        case DOWNLOAD_ERROR_NETWORK_TIMEOUT:    return "Network timeout";
        case DOWNLOAD_ERROR_DNS_FAILED:         return "DNS resolution failed";
        case DOWNLOAD_ERROR_CONNECTION_FAILED:  return "Connection failed";
        case DOWNLOAD_ERROR_HTTP_404:           return "File not found (404)";
        case DOWNLOAD_ERROR_HTTP_403:           return "Access forbidden (403)";
        case DOWNLOAD_ERROR_HTTP_ERROR:         return "HTTP error";
        case DOWNLOAD_ERROR_DISK_FULL:          return "Disk full";
        case DOWNLOAD_ERROR_WRITE_FAILED:       return "Write failed";
        case DOWNLOAD_ERROR_INVALID_FILE:       return "Invalid file";
        case DOWNLOAD_ERROR_FILE_EXISTS:        return "File already exists";
        case DOWNLOAD_ERROR_CANCELLED:          return "Cancelled";
        case DOWNLOAD_ERROR_UNKNOWN:            return "Unknown error";
        default:                                return "Invalid error code";
    }
}

/**
 * Convert file format to string
 */
const char* file_format_to_string(file_format_t format) {
    switch (format) {
        case FILE_FORMAT_TXT:     return "TXT";
        case FILE_FORMAT_EPUB:    return "EPUB";
        case FILE_FORMAT_PDF:     return "PDF";
        case FILE_FORMAT_UNKNOWN: return "Unknown";
        default:                  return "Invalid";
    }
}

/**
 * Format bytes as human-readable size
 */
void download_manager_format_size(uint64_t bytes, char *buffer, int buffer_size) {
    if (!buffer || buffer_size <= 0) {
        return;
    }

    if (bytes < 1024) {
        snprintf(buffer, buffer_size, "%llu B", (unsigned long long)bytes);
    } else if (bytes < 1024 * 1024) {
        snprintf(buffer, buffer_size, "%.1f KB", bytes / 1024.0);
    } else if (bytes < 1024 * 1024 * 1024) {
        snprintf(buffer, buffer_size, "%.1f MB", bytes / (1024.0 * 1024.0));
    } else {
        snprintf(buffer, buffer_size, "%.2f GB", bytes / (1024.0 * 1024.0 * 1024.0));
    }
}

/**
 * Format time duration as human-readable string
 */
void download_manager_format_time(int seconds, char *buffer, int buffer_size) {
    if (!buffer || buffer_size <= 0) {
        return;
    }

    if (seconds < 0) {
        snprintf(buffer, buffer_size, "Unknown");
    } else if (seconds < 60) {
        snprintf(buffer, buffer_size, "%ds", seconds);
    } else if (seconds < 3600) {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        snprintf(buffer, buffer_size, "%dm %ds", minutes, secs);
    } else {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        snprintf(buffer, buffer_size, "%dh %dm", hours, minutes);
    }
}
