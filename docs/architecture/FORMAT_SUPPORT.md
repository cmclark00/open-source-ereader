---
type: architecture
title: E-Book Format Support Architecture
created: 2026-01-14
tags:
  - architecture
  - formats
  - epub
  - pdf
  - phase-04
related:
  - "[[EREADER_DESIGN]]"
  - "[[USER_GUIDE]]"
  - "[[EBOOK_LIBRARIES]]"
---

# E-Book Format Support Architecture

This document describes the format abstraction layer that enables the e-reader to support multiple book formats (TXT, EPUB, PDF) through a unified interface.

## Table of Contents

1. [Overview](#overview)
2. [Format Detection](#format-detection)
3. [Format Abstraction Layer](#format-abstraction-layer)
4. [Format Implementations](#format-implementations)
5. [Adding New Formats](#adding-new-formats)
6. [Memory Management](#memory-management)
7. [Error Handling](#error-handling)

---

## Overview

### Design Goals

The format support system was designed with the following principles:

1. **Abstraction**: UI layer doesn't know about specific formats
2. **Extensibility**: Easy to add new formats without changing UI code
3. **Consistency**: All formats provide text through the same interface
4. **Efficiency**: Minimal memory footprint on embedded hardware
5. **Reliability**: Graceful handling of corrupt or invalid files

### Supported Formats

| Format | Extension | Library | Text Extraction | Metadata Support |
|--------|-----------|---------|----------------|------------------|
| Plain Text | `.txt` | Native C | Direct read | Filename only |
| EPUB | `.epub` | libzip + libxml2 | HTML stripping | Title, author from OPF |
| PDF | `.pdf` | Poppler utils | pdftotext | Title, author, subject, creator |

---

## Format Detection

### File Extension Matching

Format detection is performed by `format_detect_type()` in `src/ereader/formats/format_interface.c`:

```c
book_format_t format_detect_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return FORMAT_UNKNOWN;

    if (strcasecmp(ext, ".txt") == 0) return FORMAT_TXT;
    if (strcasecmp(ext, ".epub") == 0) return FORMAT_EPUB;
    if (strcasecmp(ext, ".pdf") == 0) return FORMAT_PDF;

    return FORMAT_UNKNOWN;
}
```

### Case-Insensitive Matching

Extensions are matched case-insensitively, so `.TXT`, `.Txt`, and `.txt` are all recognized.

### Detection During Scan

Books are scanned and format-detected during library initialization:

1. `book_list_scan()` iterates through `/books/` directory
2. For each file, `format_detect_type()` determines the format
3. Format type is stored in `book_metadata_t.format`
4. If format is `FORMAT_UNKNOWN`, the file is skipped

---

## Format Abstraction Layer

### Format Interface Structure

All formats implement the `format_interface_t` function pointer structure:

```c
typedef struct {
    // Validation
    bool (*validate)(const char *filepath);

    // Lifecycle
    void* (*open)(const char *filepath);
    void (*close)(void *handle);

    // Text extraction
    int (*extract_text)(void *handle);
    char* (*get_text)(void *handle);

    // Metadata
    void (*get_metadata)(void *handle, book_metadata_t *metadata);

    // Pagination
    int (*get_page_count)(void *handle);
} format_interface_t;
```

### Function Descriptions

| Function | Purpose | Returns |
|----------|---------|---------|
| `validate()` | Check if file is valid format | `true` if valid |
| `open()` | Open file and create handle | Opaque handle pointer |
| `close()` | Close file and free resources | void |
| `extract_text()` | Extract all text from book | 0 on success, -1 on error |
| `get_text()` | Get extracted text buffer | Pointer to text (null-terminated) |
| `get_metadata()` | Fill metadata structure | void |
| `get_page_count()` | Calculate total pages | Page count (0-based) |

### Getting Format Interface

The `format_get_interface()` function returns the appropriate interface:

```c
const format_interface_t* format_get_interface(book_format_t format) {
    switch (format) {
        case FORMAT_TXT:  return &txt_format_interface;
        case FORMAT_EPUB: return &epub_format_interface;
        case FORMAT_PDF:  return &pdf_format_interface;
        default:          return NULL;
    }
}
```

### Usage Pattern

UI code uses formats through the abstraction layer:

```c
// Detect format
book_format_t format = format_detect_type(filename);

// Get interface
const format_interface_t *iface = format_get_interface(format);

// Open book
void *handle = iface->open(filepath);

// Extract text
if (iface->extract_text(handle) == 0) {
    char *text = iface->get_text(handle);
    // Use text...
}

// Get metadata
book_metadata_t metadata;
iface->get_metadata(handle, &metadata);

// Close
iface->close(handle);
```

---

## Format Implementations

### Plain Text (TXT)

**Implementation**: `src/ereader/formats/txt_reader.c`

**Text Extraction**:
- Reads entire file into memory
- Detects line endings (LF, CRLF, CR)
- Normalizes to Unix line endings (LF)
- Maximum file size: 10 MB

**Metadata**:
- Title: Filename without extension
- Author: Not available
- Format: `FORMAT_TXT`

**Memory Usage**: File size + overhead (~1KB)

**Performance**: Fastest format, < 100ms for typical books

### EPUB

**Implementation**: `src/ereader/formats/epub_reader.c`

**Text Extraction Process**:

1. **Validate ZIP**: Check if file is valid ZIP archive
2. **Parse container.xml**: Find OPF file location in `META-INF/container.xml`
3. **Parse OPF**: Extract metadata and spine (reading order) from `content.opf`
4. **Extract chapters**: For each spine item:
   - Extract XHTML/HTML file from ZIP
   - Strip HTML tags
   - Decode HTML entities (&amp;, &lt;, etc.)
   - Append to text buffer
5. **Join chapters**: Concatenate all chapter text with separators

**Libraries Used**:
- `libzip`: ZIP archive extraction
- `libxml2`: XML parsing (container.xml, content.opf)
- Custom HTML stripper: Tag removal and entity decoding

**Metadata Extraction**:
```xml
<!-- Parsed from content.opf -->
<metadata xmlns:dc="http://purl.org/dc/elements/1.1/">
    <dc:title>Pride and Prejudice</dc:title>
    <dc:creator>Jane Austen</dc:creator>
</metadata>
```

**Supported EPUB Versions**:
- EPUB 2.0: Full support
- EPUB 3.0: Basic support (no multimedia, JavaScript, or complex features)

**Limitations**:
- Images are ignored
- CSS styling is ignored
- JavaScript and interactive content not supported
- Complex layouts converted to linear text
- Footnotes may appear inline instead of at bottom

**Memory Usage**: ~2x file size during extraction (temporary buffers)

**Performance**: 1-3 seconds for typical books (depends on chapter count)

### PDF

**Implementation**: `src/ereader/formats/pdf_reader.c`

**Text Extraction Process**:

1. **Validate PDF**: Run `pdfinfo` to check file validity
2. **Extract metadata**: Parse `pdfinfo` output for title, author, etc.
3. **Extract text**: Run `pdftotext -layout -nopgbrk` to extract all text
4. **Clean text**: Remove page headers/footers if detected
5. **Store text**: Keep extracted text in memory

**Tools Used** (Poppler utils):
- `pdfinfo`: Metadata extraction and validation
- `pdftotext`: Text extraction with layout preservation

**Command Line Examples**:
```bash
# Get metadata and page count
pdfinfo /books/manual.pdf

# Extract text (all pages)
pdftotext -layout -nopgbrk /books/manual.pdf /tmp/manual.txt

# Extract single page (for lazy loading)
pdftotext -layout -nopgbrk -f 5 -l 5 /books/manual.pdf /tmp/page5.txt
```

**Metadata Extraction**:
```
Title:          User Manual
Author:         John Doe
Subject:        Technical Documentation
Creator:        LaTeX with hyperref
Producer:       pdfTeX-1.40.21
CreationDate:   Mon Jan 13 10:30:00 2026
Pages:          42
```

**Limitations**:
- Text-based PDFs only (scanned PDFs require OCR)
- Images and diagrams ignored
- Complex multi-column layouts may not preserve reading order
- Tables may be reformatted or broken
- Page numbers in extracted text != original PDF page numbers

**Memory Usage**: ~3x file size (original + extracted text + temp files)

**Performance**: 2-5 seconds (depends on page count and complexity)

---

## Adding New Formats

To add support for a new book format (e.g., MOBI, AZW3, HTML):

### Step 1: Add Format Enum

Edit `src/ereader/formats/format_interface.h`:

```c
typedef enum {
    FORMAT_UNKNOWN = 0,
    FORMAT_TXT,
    FORMAT_EPUB,
    FORMAT_PDF,
    FORMAT_MOBI,  // <-- Add new format
    FORMAT_COUNT
} book_format_t;
```

### Step 2: Create Format Reader

Create `src/ereader/formats/mobi_reader.c` and `mobi_reader.h`:

```c
// mobi_reader.h
#ifndef MOBI_READER_H
#define MOBI_READER_H

#include "format_interface.h"

// Public interface
extern const format_interface_t mobi_format_interface;

#endif
```

```c
// mobi_reader.c
#include "mobi_reader.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *text;
    char *filepath;
    book_metadata_t metadata;
} mobi_handle_t;

static bool mobi_validate(const char *filepath) {
    // Check if file is valid MOBI
    // Return true if valid
}

static void* mobi_open(const char *filepath) {
    mobi_handle_t *handle = calloc(1, sizeof(mobi_handle_t));
    handle->filepath = strdup(filepath);
    return handle;
}

static int mobi_extract_text(void *handle_ptr) {
    mobi_handle_t *handle = (mobi_handle_t*)handle_ptr;
    // Extract text from MOBI file
    // Store in handle->text
    // Return 0 on success, -1 on error
}

static char* mobi_get_text(void *handle_ptr) {
    mobi_handle_t *handle = (mobi_handle_t*)handle_ptr;
    return handle->text;
}

static void mobi_get_metadata(void *handle_ptr, book_metadata_t *metadata) {
    mobi_handle_t *handle = (mobi_handle_t*)handle_ptr;
    *metadata = handle->metadata;
}

static int mobi_get_page_count(void *handle_ptr) {
    // Calculate based on text length
    mobi_handle_t *handle = (mobi_handle_t*)handle_ptr;
    if (!handle->text) return 0;
    return strlen(handle->text) / (47 * 14); // chars per page
}

static void mobi_close(void *handle_ptr) {
    mobi_handle_t *handle = (mobi_handle_t*)handle_ptr;
    free(handle->text);
    free(handle->filepath);
    free(handle);
}

const format_interface_t mobi_format_interface = {
    .validate = mobi_validate,
    .open = mobi_open,
    .close = mobi_close,
    .extract_text = mobi_extract_text,
    .get_text = mobi_get_text,
    .get_metadata = mobi_get_metadata,
    .get_page_count = mobi_get_page_count
};
```

### Step 3: Register Format

Edit `src/ereader/formats/format_interface.c`:

```c
#include "mobi_reader.h"  // Add include

// Update format_detect_type()
book_format_t format_detect_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return FORMAT_UNKNOWN;

    if (strcasecmp(ext, ".txt") == 0) return FORMAT_TXT;
    if (strcasecmp(ext, ".epub") == 0) return FORMAT_EPUB;
    if (strcasecmp(ext, ".pdf") == 0) return FORMAT_PDF;
    if (strcasecmp(ext, ".mobi") == 0) return FORMAT_MOBI;  // Add

    return FORMAT_UNKNOWN;
}

// Update format_get_interface()
const format_interface_t* format_get_interface(book_format_t format) {
    switch (format) {
        case FORMAT_TXT:  return &txt_format_interface;
        case FORMAT_EPUB: return &epub_format_interface;
        case FORMAT_PDF:  return &pdf_format_interface;
        case FORMAT_MOBI: return &mobi_format_interface;  // Add
        default:          return NULL;
    }
}

// Update format_get_type_indicator()
const char* format_get_type_indicator(book_format_t format) {
    switch (format) {
        case FORMAT_TXT:  return "[T]";
        case FORMAT_EPUB: return "[E]";
        case FORMAT_PDF:  return "[P]";
        case FORMAT_MOBI: return "[M]";  // Add
        default:          return "[?]";
    }
}
```

### Step 4: Update Build System

Edit `src/Makefile`:

```makefile
SOURCES = main.c \
          ereader/display/epd_driver.c \
          ereader/input/button_manager.c \
          ereader/ui/renderer.c \
          ereader/ui/text_renderer.c \
          ereader/ui/menu.c \
          ereader/ui/reader.c \
          ereader/books/book_manager.c \
          ereader/books/bookmark_manager.c \
          ereader/formats/format_interface.c \
          ereader/formats/txt_reader.c \
          ereader/formats/epub_reader.c \
          ereader/formats/pdf_reader.c \
          ereader/formats/mobi_reader.c  # Add this

# Add any required libraries
LIBS = -lzip -lxml2 -lmobi  # If using libmobi
```

### Step 5: Add Library to Buildroot (if needed)

If the format requires a new library:

1. Check if Buildroot has the package: `ls buildroot/package/ | grep mobi`
2. If not, create `buildroot/package/libmobi/libmobi.mk`
3. Add `BR2_PACKAGE_LIBMOBI=y` to `configs/ereader_rpi0w_defconfig`
4. Document in `docs/buildroot/CUSTOM_PACKAGES.md`

### Step 6: Test

1. Add sample MOBI file to `board/ereader/rootfs-overlay/books/`
2. Rebuild: `make`
3. Test format detection, metadata extraction, and text rendering
4. Verify memory usage is acceptable (<150 MB total)
5. Test with corrupt files to ensure graceful error handling

### Step 7: Document

Update documentation:
- `docs/USER_GUIDE.md`: Add format to supported formats list
- `docs/testing/FORMAT_TESTING.md`: Add testing procedures
- `docs/architecture/FORMAT_SUPPORT.md`: Document implementation
- `docs/research/EBOOK_LIBRARIES.md`: Add library evaluation

---

## Memory Management

### Memory Constraints

Raspberry Pi Zero W has 512 MB RAM total:
- ~200 MB for Linux kernel and services
- ~100 MB for display framebuffer and caching
- **~200 MB available for application**

### Per-Format Memory Budget

| Format | File Size | Memory Usage | Notes |
|--------|-----------|--------------|-------|
| TXT | 1 MB | ~1 MB | Direct read into buffer |
| EPUB | 500 KB | ~1-1.5 MB | ZIP extraction + XML parsing |
| PDF | 2 MB | ~6 MB | Original + extracted text + temp files |

### Memory Management Strategies

**1. Lazy Loading**
- Don't extract text until book is opened
- Library scan only reads filenames and basic metadata
- Full text extraction happens on-demand

**2. Single Book in Memory**
- Only one book is fully loaded at a time
- When switching books, previous book is freed
- Reduces memory pressure significantly

**3. Streaming for Large Files**
- Files >10 MB are rejected during scan
- Prevents out-of-memory crashes
- User is prompted to split large books

**4. Temporary File Cleanup**
- PDF extraction creates temp files in `/tmp`
- Files are deleted immediately after processing
- `/tmp` is mounted as tmpfs (RAM-backed)

**5. Error Recovery**
- If `malloc()` fails, gracefully abort operation
- Return to library menu instead of crashing
- Log error for debugging

### Example Memory Flow (EPUB)

```
Initial state: 50 MB used
↓
Open EPUB (500 KB file)
↓
Allocate ZIP handle: +10 KB
↓
Extract container.xml: +5 KB
↓
Extract content.opf: +20 KB
↓
Extract 20 chapters: +800 KB (accumulated)
↓
Join text: +700 KB
↓
Free ZIP handles: -35 KB
↓
Final state: 51.5 MB used (1.5 MB for book)
```

---

## Error Handling

### Validation Strategy

All formats implement multi-level validation:

**Level 1: Extension check** (during scan)
- Fast, happens for all files
- Filters out non-book files immediately

**Level 2: Format validation** (before opening)
- Checks file magic bytes or structure
- Prevents crashes from corrupt files
- `validate()` function in interface

**Level 3: Content validation** (during extraction)
- Verifies internal structure is correct
- Handles missing files, invalid XML, etc.
- Returns error codes for UI to display

### Error Types and Handling

| Error Type | Detection | Handling |
|------------|-----------|----------|
| File not found | `open()` | Show "File not found" error, return to menu |
| Corrupt file | `validate()` | Skip file during scan, log error |
| Invalid format | `extract_text()` | Show "Invalid format" error, return to menu |
| Missing content | `extract_text()` | Show "No text found" error, return to menu |
| Out of memory | `malloc()` | Show "Out of memory" error, return to menu |
| Extraction failure | External tools | Log stderr, show generic error to user |

### Error Messages

User-friendly error messages are displayed on-screen:

```c
// Example error handling in book opening
void* handle = iface->open(filepath);
if (!handle) {
    renderer_show_message("Error: Could not open book");
    return;
}

if (iface->extract_text(handle) != 0) {
    renderer_show_message("Error: Could not read book content");
    iface->close(handle);
    return;
}
```

### Logging

Detailed errors are logged to `/var/log/ereader.log`:

```
[2026-01-14 10:30:15] ERROR: EPUB extraction failed for alice.epub: ZIP file corrupted
[2026-01-14 10:30:20] ERROR: PDF validation failed for manual.pdf: Not a PDF file
[2026-01-14 10:30:25] WARN: Large file skipped: encyclopedia.epub (15.2 MB > 10 MB limit)
```

---

## Performance Considerations

### Optimization Techniques

**1. Caching**
- Extracted text is cached in handle
- Re-rendering pages doesn't re-extract text
- Metadata is cached after first extraction

**2. Minimal XML Parsing**
- Only parse required XML files (container.xml, content.opf)
- Don't parse NCX, nav.xhtml unless needed
- Use SAX-style parsing for large files

**3. Efficient String Operations**
- Pre-allocate buffers when size is known
- Use `memmove()` instead of repeated `strcat()`
- Avoid excessive `realloc()` calls

**4. External Tool Optimization**
- PDF: Use `-layout -nopgbrk` flags for better text extraction
- Redirect stderr to `/dev/null` to avoid logging overhead
- Use temporary files instead of pipes for large data

### Benchmark Results (Raspberry Pi Zero W)

| Operation | TXT | EPUB | PDF |
|-----------|-----|------|-----|
| Library scan (100 books) | 0.5s | 1.2s | 2.0s |
| Open book | 50ms | 800ms | 1.5s |
| Extract text (average) | 100ms | 2.0s | 3.5s |
| Render page | 50ms | 50ms | 50ms |
| Page turn | 2.5s | 2.5s | 2.5s |

Note: Page turn time is dominated by e-paper refresh (2-4 seconds), not format processing.

---

## Future Enhancements

### Planned Features

**Phase 5**:
- Table of contents navigation (EPUB NCX/nav.xhtml parsing)
- Chapter markers in status bar
- Fast chapter skip buttons

**Phase 6**:
- Format conversion using Calibre libraries
- Download EPUB/PDF from online libraries
- Auto-convert PDF to EPUB for better text flow

**Phase 7**:
- Image extraction and display (grayscale only)
- Footnote navigation
- Multi-column layout support

### Additional Formats

Potential formats to add:

- **MOBI/AZW3**: Using libmobi or KindleUnpack
- **HTML**: Direct parsing with libxml2
- **RTF**: Using unrtf or custom parser
- **Markdown**: Rendering to plain text
- **Comic books (CBZ/CBR)**: Image extraction only

---

## References

### Code Locations

- **Format interface**: `src/ereader/formats/format_interface.{c,h}`
- **TXT reader**: `src/ereader/formats/txt_reader.{c,h}`
- **EPUB reader**: `src/ereader/formats/epub_reader.{c,h}`
- **PDF reader**: `src/ereader/formats/pdf_reader.{c,h}`
- **Book manager**: `src/ereader/books/book_manager.c` (uses format layer)
- **Menu UI**: `src/ereader/ui/menu.c` (displays format indicators)
- **Reader UI**: `src/ereader/ui/reader.c` (renders extracted text)

### Related Documentation

- [[EREADER_DESIGN]] - Overall application architecture
- [[USER_GUIDE]] - User-facing documentation
- [[EBOOK_LIBRARIES]] - Library evaluation and selection
- [[CUSTOM_PACKAGES]] - Buildroot package configuration
- [[FORMAT_TESTING]] - Testing procedures for formats

### External Resources

- EPUB specification: https://www.w3.org/publishing/epub3/
- PDF reference: https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf
- Poppler utils: https://poppler.freedesktop.org/
- libzip documentation: https://libzip.org/documentation/
- libxml2 documentation: http://www.xmlsoft.org/

---

*Last updated: 2026-01-14 (Phase 04)*
