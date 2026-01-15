/*
 * format_interface.h - Generic Book Format Interface
 *
 * Provides a unified interface for different book formats (TXT, EPUB, PDF).
 * This abstraction layer allows the UI and book manager to work with any
 * format without format-specific code.
 *
 * Phase 04: EPUB and PDF Support
 */

#ifndef FORMAT_INTERFACE_H
#define FORMAT_INTERFACE_H

#include <stdint.h>
#include <stddef.h>

/*
 * Book Format Types
 */
typedef enum {
    BOOK_FORMAT_UNKNOWN = 0,
    BOOK_FORMAT_TXT,
    BOOK_FORMAT_EPUB,
    BOOK_FORMAT_PDF
} book_format_type_t;

/*
 * Book Format Error Codes
 * Unified error codes across all formats
 */
typedef enum {
    FORMAT_SUCCESS = 0,
    FORMAT_ERROR_NOT_FOUND = -1,
    FORMAT_ERROR_INVALID_FORMAT = -2,
    FORMAT_ERROR_CORRUPT_FILE = -3,
    FORMAT_ERROR_OUT_OF_MEMORY = -4,
    FORMAT_ERROR_TOO_LARGE = -5,
    FORMAT_ERROR_READ_FAILED = -6,
    FORMAT_ERROR_NO_CONTENT = -7,
    FORMAT_ERROR_UNSUPPORTED = -8
} format_error_t;

/*
 * Book Metadata Structure
 * Generic metadata that all formats should provide (if available)
 */
typedef struct {
    char title[256];           /* Book title (or filename if not available) */
    char author[256];          /* Author name (empty if not available) */
    char language[16];         /* Language code (e.g., "en", empty if unknown) */
    int page_count;            /* Number of pages (0 if not yet calculated) */
    long file_size;            /* Original file size in bytes */
} format_metadata_t;

/*
 * Book Format Handle
 * Opaque handle to format-specific book data
 */
typedef void* format_handle_t;

/*
 * Book Format Interface
 * Function pointer table for format-specific operations
 * Each format (TXT, EPUB, PDF) implements these functions
 */
typedef struct book_format_interface {
    /* Format identification */
    book_format_type_t type;
    const char *name;          /* Human-readable format name */
    const char *extension;     /* File extension (e.g., ".txt") */

    /*
     * Validate file format
     * Quick check if file is valid for this format
     * @param filepath: Path to book file
     * @return: FORMAT_SUCCESS if valid, error code otherwise
     */
    int (*validate)(const char *filepath);

    /*
     * Open and parse book file
     * @param filepath: Path to book file
     * @return: Format-specific handle, or NULL on error
     */
    format_handle_t (*open)(const char *filepath);

    /*
     * Close book and free all resources
     * @param handle: Format-specific book handle
     */
    void (*close)(format_handle_t handle);

    /*
     * Extract all text from book
     * Populates the book's internal text buffer
     * @param handle: Format-specific book handle
     * @return: FORMAT_SUCCESS on success, error code on failure
     */
    int (*extract_text)(format_handle_t handle);

    /*
     * Get extracted text
     * @param handle: Format-specific book handle
     * @param out_text: Output pointer to text (do not free, owned by handle)
     * @param out_length: Output length of text in bytes
     * @return: FORMAT_SUCCESS if text available, error code otherwise
     * @note: Must call extract_text() first
     */
    int (*get_text)(format_handle_t handle, const char **out_text, size_t *out_length);

    /*
     * Get book metadata
     * @param handle: Format-specific book handle
     * @param metadata: Output metadata structure
     * @return: FORMAT_SUCCESS on success, error code on failure
     */
    int (*get_metadata)(format_handle_t handle, format_metadata_t *metadata);

    /*
     * Get page count
     * @param handle: Format-specific book handle
     * @return: Number of pages, or -1 if not available/calculated yet
     * @note: For text-based formats, pagination may depend on rendering
     */
    int (*get_page_count)(format_handle_t handle);

} book_format_interface_t;

/*
 * Format Registry Functions
 */

/**
 * Get format interface for a specific format type
 * @param type: Format type
 * @return: Pointer to format interface, or NULL if not supported
 */
const book_format_interface_t* format_get_interface(book_format_type_t type);

/**
 * Detect format type from file extension
 * @param filename: Filename to check
 * @return: Detected format type, or BOOK_FORMAT_UNKNOWN
 */
book_format_type_t format_detect_type(const char *filename);

/**
 * Get format type name
 * @param type: Format type
 * @return: Human-readable format name (e.g., "TXT", "EPUB", "PDF")
 */
const char* format_get_type_name(book_format_type_t type);

/**
 * Get format type indicator character for UI display
 * @param type: Format type
 * @return: Single character indicator ('T', 'E', 'P', '?')
 */
char format_get_type_indicator(book_format_type_t type);

/**
 * Check if a filename is a supported book format
 * @param filename: Filename to check
 * @return: 1 if supported, 0 otherwise
 */
int format_is_supported(const char *filename);

/**
 * Get human-readable error message
 * @param error: Error code
 * @return: Error message string
 */
const char* format_error_string(format_error_t error);

/**
 * Initialize format system
 * Registers all available format handlers
 * @return: FORMAT_SUCCESS on success, error code on failure
 */
int format_init(void);

#endif /* FORMAT_INTERFACE_H */
