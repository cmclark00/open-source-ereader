/*
 * format_interface.c - Generic Book Format Interface Implementation
 *
 * Implements the format registry and utility functions
 *
 * Phase 04: EPUB and PDF Support
 */

#include "format_interface.h"
#include "txt_reader.h"
#include "epub_reader.h"
#include "pdf_reader.h"
#include <string.h>
#include <strings.h>
#include <ctype.h>

/*
 * Format Registry
 * Array of all registered format interfaces
 */
static const book_format_interface_t *format_registry[4] = {
    NULL,  /* BOOK_FORMAT_UNKNOWN */
    NULL,  /* BOOK_FORMAT_TXT - will be set in format_init() */
    NULL,  /* BOOK_FORMAT_EPUB - will be set in format_init() */
    NULL   /* BOOK_FORMAT_PDF - will be set in format_init() */
};

/* External format interfaces (defined in txt_reader.c, epub_reader.c, pdf_reader.c) */
extern const book_format_interface_t txt_format_interface;
extern const book_format_interface_t epub_format_interface;
extern const book_format_interface_t pdf_format_interface;

/*
 * Format Registry Functions
 */

int format_init(void) {
    /* Register all format interfaces */
    format_registry[BOOK_FORMAT_TXT] = &txt_format_interface;
    format_registry[BOOK_FORMAT_EPUB] = &epub_format_interface;
    format_registry[BOOK_FORMAT_PDF] = &pdf_format_interface;
    return FORMAT_SUCCESS;
}

const book_format_interface_t* format_get_interface(book_format_type_t type) {
    if (type <= BOOK_FORMAT_UNKNOWN || type > BOOK_FORMAT_PDF) {
        return NULL;
    }
    return format_registry[type];
}

book_format_type_t format_detect_type(const char *filename) {
    if (!filename) {
        return BOOK_FORMAT_UNKNOWN;
    }

    size_t len = strlen(filename);
    if (len < 5) {  /* Minimum: "a.txt" */
        return BOOK_FORMAT_UNKNOWN;
    }

    /* Find the extension */
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        return BOOK_FORMAT_UNKNOWN;
    }

    /* Check each format (case-insensitive) */
    if (strcasecmp(ext, ".txt") == 0) {
        return BOOK_FORMAT_TXT;
    } else if (strcasecmp(ext, ".epub") == 0) {
        return BOOK_FORMAT_EPUB;
    } else if (strcasecmp(ext, ".pdf") == 0) {
        return BOOK_FORMAT_PDF;
    }

    return BOOK_FORMAT_UNKNOWN;
}

const char* format_get_type_name(book_format_type_t type) {
    switch (type) {
        case BOOK_FORMAT_TXT:
            return "TXT";
        case BOOK_FORMAT_EPUB:
            return "EPUB";
        case BOOK_FORMAT_PDF:
            return "PDF";
        case BOOK_FORMAT_UNKNOWN:
        default:
            return "Unknown";
    }
}

char format_get_type_indicator(book_format_type_t type) {
    switch (type) {
        case BOOK_FORMAT_TXT:
            return 'T';
        case BOOK_FORMAT_EPUB:
            return 'E';
        case BOOK_FORMAT_PDF:
            return 'P';
        case BOOK_FORMAT_UNKNOWN:
        default:
            return '?';
    }
}

int format_is_supported(const char *filename) {
    book_format_type_t type = format_detect_type(filename);
    return (type != BOOK_FORMAT_UNKNOWN);
}

const char* format_error_string(format_error_t error) {
    switch (error) {
        case FORMAT_SUCCESS:
            return "Success";
        case FORMAT_ERROR_NOT_FOUND:
            return "File not found";
        case FORMAT_ERROR_INVALID_FORMAT:
            return "Invalid file format";
        case FORMAT_ERROR_CORRUPT_FILE:
            return "Corrupt file";
        case FORMAT_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case FORMAT_ERROR_TOO_LARGE:
            return "File too large";
        case FORMAT_ERROR_READ_FAILED:
            return "Read failed";
        case FORMAT_ERROR_NO_CONTENT:
            return "No content";
        case FORMAT_ERROR_UNSUPPORTED:
            return "Unsupported feature";
        default:
            return "Unknown error";
    }
}
