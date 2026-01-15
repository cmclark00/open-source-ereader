---
type: testing
title: E-Book Format Testing Guide
created: 2026-01-14
tags:
  - testing
  - formats
  - epub
  - pdf
related:
  - "[[EBOOK_LIBRARIES]]"
  - "[[FORMAT_SUPPORT]]"
---

# E-Book Format Testing Guide

This document describes how to test the e-reader's multi-format support (TXT, EPUB, PDF) and documents known limitations and performance characteristics.

## Test Files

The following sample books are included in `board/ereader/rootfs-overlay/books/` for testing:

### Plain Text (.txt)
- **welcome.txt** - Initial welcome message (3.9 KB)
- **gutenberg-sample.txt** - Sample text from Project Gutenberg (11.5 KB)
- **sample-readme.txt** - Test document with Lorem Ipsum text (744 bytes)

### EPUB (.epub)
- **alice-in-wonderland.epub** - "Alice's Adventures in Wonderland" by Lewis Carroll (185 KB)
  - Source: Project Gutenberg (EPUB3 with images)
  - Tests: EPUB container parsing, OPF metadata extraction, HTML chapter rendering

### PDF (.pdf)
- **test-document.pdf** - Simple single-page PDF document (13 KB)
  - Tests: PDF validation, text extraction with pdftotext, metadata parsing

## Testing Each Format

### Testing Plain Text (.txt)

Plain text is the simplest format and serves as the baseline.

**Test Steps:**
1. Power on the device
2. Wait for library menu to appear (~10-30 seconds)
3. Use UP/DOWN buttons to navigate to a .txt file (indicated by `[T]`)
4. Press SELECT to open the book
5. Use UP/DOWN to navigate pages
6. Press BACK to return to menu

**Expected Behavior:**
- Text wraps correctly at display boundaries (400×300 pixels)
- Pages display proper pagination with word wrapping
- Status bar shows format indicator `[T]`, title, and page number
- Navigation is instant (no loading delay)
- Bookmarks save and restore position on exit/reopen

**Known Limitations:**
- Fixed 8×16 bitmap font (no font size adjustment yet)
- Maximum file size: 10 MB
- No special formatting (bold, italics, colors)

### Testing EPUB (.epub)

EPUB files are ZIP containers with HTML content and metadata.

**Test Steps:**
1. Navigate to an EPUB file in the library (indicated by `[E]`)
2. Note that the title from EPUB metadata is displayed (not filename)
3. Press SELECT to open the book
4. Use UP/DOWN to navigate pages
5. Verify status bar shows `[E]` indicator
6. Test multiple chapters if present
7. Press BACK to return to menu

**Expected Behavior:**
- EPUB container is validated and opened successfully
- Title and author metadata extracted from OPF file
- HTML tags are stripped from content
- HTML entities (e.g., `&amp;`, `&lt;`, `&quot;`) are decoded correctly
- Text from all chapters flows continuously
- Reading order follows spine definition in OPF
- Page navigation works smoothly across chapter boundaries

**Known Limitations:**
- Images are not displayed (text-only rendering)
- Complex HTML formatting is stripped (only plain text extracted)
- CSS styles are ignored
- Tables, lists, and special formatting appear as plain text
- EPUB 3 features (video, audio, MathML) not supported
- Some EPUB3 books may have compatibility issues

**Performance Notes:**
- Initial load time: ~1-3 seconds depending on EPUB size
- Memory usage: EPUB content is loaded into memory after extraction
- Larger EPUBs (>5 MB) may cause slower loading on Pi Zero W

### Testing PDF (.pdf)

PDF files are rendered using Poppler's pdftotext utility for text extraction.

**Test Steps:**
1. Navigate to a PDF file in the library (indicated by `[P]`)
2. Note that title from PDF metadata is displayed if available
3. Press SELECT to open the book
4. Use UP/DOWN to navigate pages
5. Verify status bar shows `[P]` indicator
6. Test with multi-page PDFs if available
7. Press BACK to return to menu

**Expected Behavior:**
- PDF is validated using pdfinfo
- Metadata (title, author, subject, creator) extracted correctly
- Text extracted page by page using pdftotext
- Page count displayed accurately
- Text flows naturally despite PDF layout

**Known Limitations:**
- **Text-only rendering** - Images, graphics, and visual layouts are not displayed
- **Layout issues** - PDFs with complex layouts (multi-column, tables) may have garbled text order
- **Scanned PDFs** - OCR is not performed; scanned PDFs with no text layer will appear empty
- **Font issues** - Special fonts and symbols may not extract correctly
- **Forms and annotations** - Interactive elements are ignored
- **Password-protected PDFs** - Encrypted PDFs cannot be opened
- **Performance** - Large PDFs (>100 pages, >10 MB) may load slowly

**Performance Notes:**
- Initial load time: ~2-5 seconds for metadata extraction
- Page extraction: ~0.5-1 second per page (lazy loading)
- Memory usage: Only current page text is kept in memory
- Large PDFs (>50 MB) may cause extraction delays

### Testing Format Detection

The e-reader automatically detects formats by file extension.

**Test Steps:**
1. Ensure the `/books/` directory contains files with mixed extensions:
   - `.txt` - Plain text
   - `.epub` - EPUB books
   - `.pdf` - PDF documents
2. Boot to the library menu
3. Verify each book shows the correct format indicator:
   - `[T]` for TXT
   - `[E]` for EPUB
   - `[P]` for PDF
4. Verify metadata is displayed where available (EPUB/PDF titles)

**Expected Behavior:**
- All supported formats are detected and listed
- Format indicators appear correctly
- Files are sorted alphabetically regardless of format
- Invalid or corrupt files are skipped with warning in logs

## Testing Corrupt or Invalid Files

**Test Cases:**
1. **Corrupt ZIP/EPUB:** Create a `.epub` file with invalid ZIP structure
   - Expected: Book fails to open, returns to menu with error
2. **Corrupt PDF:** Create a `.pdf` file with invalid PDF structure
   - Expected: Book fails to open, returns to menu with error
3. **Empty files:** Create empty files with valid extensions
   - Expected: Book opens but shows no content or error message
4. **Wrong extension:** Rename a `.txt` file to `.epub`
   - Expected: EPUB parser fails, returns to menu with error

## Performance Testing

### Memory Usage
- **TXT:** Minimal (entire file loaded, <10 MB max)
- **EPUB:** Moderate (extracted text from all chapters, typically <5 MB)
- **PDF:** Low (lazy loading, only current page in memory)

### Boot Time Impact
- Adding EPUB/PDF libraries increases boot time by ~2-5 seconds
- libzip + libxml2 footprint: ~1.6 MB
- Poppler footprint: ~5-7 MB
- Total format support overhead: ~7-9 MB in rootfs

### Battery Life
- Format parsing is one-time cost at book open
- Page navigation uses same rendering engine for all formats
- PDF extraction may use slightly more CPU (pdftotext subprocess)

## Test Results Template

Use this template to document test results:

```
Date: YYYY-MM-DD
Build: [Git commit hash]
Device: Raspberry Pi Zero W

| Format | File Name | Size | Result | Notes |
|--------|-----------|------|--------|-------|
| TXT    | welcome.txt | 3.9 KB | PASS | Fast loading |
| TXT    | gutenberg-sample.txt | 11.5 KB | PASS | Pagination correct |
| EPUB   | alice-in-wonderland.epub | 185 KB | PASS | Title extracted, 2s load time |
| PDF    | test-document.pdf | 13 KB | PASS | Single page, instant |
```

## Known Issues

### EPUB Issues
1. **Image-heavy EPUBs:** Books with many images will have missing content (images ignored)
2. **Complex HTML:** Nested tables, advanced CSS formatting may not render correctly
3. **Right-to-left text:** RTL languages (Arabic, Hebrew) not properly supported
4. **Special characters:** Some Unicode characters may not display correctly

### PDF Issues
1. **Multi-column layouts:** Text extraction order may be wrong (reads left-to-right across columns)
2. **Scanned documents:** PDFs without text layer appear blank (no OCR)
3. **Large files:** PDFs over 50 MB may cause memory issues or slow extraction
4. **Complex formatting:** Tables, sidebars, footnotes may have garbled text order

## Adding New Test Files

To add new test files for verification:

1. Copy files to `board/ereader/rootfs-overlay/books/`
2. Rebuild the rootfs: `cd buildroot && make clean-ereader-app && make`
3. Flash the updated SD card image
4. Boot and verify files appear in library

**Recommended Test Files:**
- Small EPUB (<1 MB) - Tests basic parsing
- Large EPUB (5-10 MB) - Tests memory limits
- Simple PDF (1-5 pages) - Tests basic extraction
- Complex PDF (50+ pages, multi-column) - Tests layout handling
- Scanned PDF - Tests text layer detection

## Regression Testing

After any changes to format parsers, verify:

1. All sample books still open correctly
2. Metadata extraction still works
3. Page navigation remains smooth
4. No memory leaks (test opening/closing books repeatedly)
5. Error handling for corrupt files still functions

## Future Testing Requirements

As format support improves, add tests for:

- **Image rendering** - EPUB/PDF images displayed
- **Font formatting** - Bold, italic, font sizes
- **Tables and lists** - Structured content rendering
- **Hyperlinks** - Cross-references and external links
- **Annotations** - Highlight and note support
- **RTL languages** - Proper right-to-left text flow
