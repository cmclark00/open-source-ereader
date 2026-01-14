---
type: planning
title: Phase 04 Planning - EPUB and PDF Support
created: 2026-01-14
tags:
  - planning
  - phase-04
  - epub
  - pdf
  - file-formats
  - reading-experience
related:
  - "[[PHASE_03_LOG]]"
  - "[[EREADER_DESIGN]]"
  - "[[USER_GUIDE]]"
---

# Phase 04 Planning: EPUB and PDF Support

This document outlines the design and implementation plan for Phase 4 of the e-reader project: adding EPUB and PDF file format support with enhanced navigation features.

## Phase Goal

Extend the e-reader application to support industry-standard e-book formats (EPUB 2.0/3.0 and basic PDF), enabling users to read a much wider variety of content. Implement enhanced navigation features including table of contents, jump-to-page, and basic search functionality.

## Prerequisites

Phase 04 assumes successful completion of Phase 03:
- ✓ Basic e-reader application working (TXT support)
- ✓ Text rendering engine operational
- ✓ Book management system functional
- ✓ Menu and reader UI components complete
- ✓ Bookmark system working
- ✓ Buildroot package integration complete

## Overview

Phase 04 transforms the e-reader from a simple TXT reader into a capable e-book reader by:

1. **EPUB Support:** Parse and render EPUB 2.0/3.0 files with HTML/CSS content
2. **PDF Support:** Extract and render text from PDF files (basic support)
3. **Enhanced Navigation:** Table of contents, jump-to-page, search within book
4. **Library Enhancements:** Multi-format support, book metadata display, cover thumbnails (if feasible)
5. **Reading Experience:** Improved pagination, partial refresh optimization, reading statistics

## File Format Support

### EPUB 2.0/3.0 Support

**EPUB Structure:**
- Container: ZIP archive with specific directory structure
- Content: XHTML/HTML5 files with CSS styling
- Metadata: OPF (Open Packaging Format) file with book info
- Navigation: NCX (EPUB 2) or Navigation Document (EPUB 3) for TOC
- Manifest: List of all files in the EPUB

**Key Components to Implement:**

1. **EPUB Parser**
   - Unzip EPUB file (use libzip)
   - Parse container.xml to find OPF file
   - Parse OPF file for metadata, spine, manifest
   - Extract TOC from NCX or navigation document
   - Load chapter content in reading order

2. **HTML/CSS Renderer (Simplified)**
   - HTML parser (use libxml2 with HTML5 parsing)
   - CSS subset: font-size, font-weight, text-align, margins
   - Render to plain text with basic formatting
   - Preserve paragraph breaks, headings, lists
   - Handle images: Skip or show placeholder (e-paper constraints)

3. **EPUB-Specific Features**
   - Chapter navigation (previous/next chapter)
   - Table of contents display and navigation
   - Metadata extraction (title, author, publisher, language)
   - Cover image extraction (optional: display in library)

**Technical Challenges:**
- HTML parsing complexity: Need robust parser (libxml2)
- CSS rendering: Decide which CSS properties to support
- Memory management: EPUB files can be large (20-50MB)
- Performance: HTML parsing may be slower than TXT
- Image handling: E-paper limitations (1-bit, slow refresh)

**Implementation Strategy:**
- Phase 4.1: Basic EPUB parsing (extract text, no formatting)
- Phase 4.2: HTML rendering (paragraphs, headings, basic formatting)
- Phase 4.3: CSS support (subset of properties)
- Phase 4.4: TOC and chapter navigation

### PDF Support (Basic)

**PDF Structure:**
- Container: Self-contained binary format
- Content: Mix of text, images, vector graphics
- Metadata: Info dictionary with title, author, etc.
- Navigation: Optional outline (bookmarks/TOC)

**Key Components to Implement:**

1. **PDF Text Extraction**
   - Use poppler-glib or mupdf library
   - Extract text from each page
   - Preserve reading order (left-to-right, top-to-bottom)
   - Handle multi-column layouts (best effort)

2. **PDF Rendering (Text-Only)**
   - Convert PDF pages to plain text
   - Paginate extracted text for e-paper display
   - Display page numbers from original PDF
   - Skip images or show "[Image]" placeholder

3. **PDF-Specific Features**
   - Page-by-page navigation (original PDF pages)
   - Outline/TOC extraction (if present)
   - Metadata extraction (title, author, page count)
   - PDF page number display vs e-reader page number

**Technical Challenges:**
- Library size: poppler/mupdf are large dependencies
- PDF complexity: Many PDFs have complex layouts (tables, columns)
- Performance: PDF parsing is CPU-intensive
- Text extraction quality: Some PDFs have poor text extraction (scanned PDFs)
- Binary size: PDF libraries add significant size to image

**Implementation Strategy:**
- Phase 4.1: Use mupdf (smaller than poppler, good text extraction)
- Phase 4.2: Extract text from PDF pages sequentially
- Phase 4.3: Cache extracted text to avoid re-parsing
- Phase 4.4: Display original PDF page numbers alongside e-reader pages

**Limitations (Out of Scope for Phase 4):**
- No graphical PDF rendering (too slow on e-paper)
- No form filling or annotations
- No zoom/pan for PDF content (text-only extraction)
- Scanned PDFs without OCR will show no text

### Library Dependencies

**Required Libraries:**

| Library | Purpose | Size Impact | License |
|---------|---------|-------------|---------|
| libzip | EPUB unzipping | +200KB | BSD |
| libxml2 | HTML/XML parsing | +1.5MB | MIT |
| mupdf | PDF text extraction | +3MB | AGPL v3 |
| zlib | Compression (already present) | 0 (existing) | zlib |
| libjpeg | Image decoding (optional) | +400KB | IJG |
| libpng | Image decoding (optional) | +200KB | libpng |

**Total Size Impact:** ~5-6MB added to image size

**Buildroot Integration:**
- Add BR2_PACKAGE_LIBZIP=y
- Add BR2_PACKAGE_LIBXML2=y
- Add BR2_PACKAGE_MUPDF=y (or compile custom lightweight version)
- Update ereader.mk dependencies

**Alternative: Lightweight Approach**
- Phase 4 could initially support EPUB only (defer PDF to Phase 5)
- Reduces dependencies: Skip mupdf, use minizip instead of libzip
- Trade-off: Less functionality but smaller image size

## Enhanced Navigation Features

### Table of Contents (TOC)

**Feature Description:**
- Display book's table of contents in a menu
- Navigate to any chapter/section by selecting from TOC
- Show current chapter in status bar while reading

**Implementation:**

1. **TOC Data Structure**
   ```c
   typedef struct {
       char *title;           // Chapter title
       int page;              // Starting page in e-reader pagination
       int level;             // Indentation level (0-3)
       char *href;            // EPUB: link, PDF: page number
   } toc_entry_t;

   typedef struct {
       toc_entry_t *entries;  // Array of TOC entries
       int count;             // Number of entries
       int capacity;          // Allocated capacity
   } toc_t;
   ```

2. **TOC Extraction**
   - EPUB: Parse NCX file or navigation document
   - PDF: Extract outline if present
   - TXT: Generate TOC from headings (lines starting with '#' or all caps)

3. **TOC UI**
   - New state: READING_TOC (sub-state of READING)
   - Render TOC as scrollable menu (similar to library menu)
   - Indent entries based on level
   - SELECT jumps to chapter, BACK returns to reading
   - Button: MENU button while reading opens TOC

**User Experience:**
- Reader screen → Press MENU → TOC appears
- Navigate TOC with UP/DOWN
- SELECT entry → Jump to chapter
- BACK → Return to previous reading position

### Jump to Page/Location

**Feature Description:**
- Jump to specific page number
- Jump to percentage of book (25%, 50%, 75%, etc.)
- Jump to beginning/end shortcuts

**Implementation:**

1. **Jump Menu UI**
   - New state: READING_JUMP (sub-state of READING)
   - Display menu: "Jump to: Beginning, 25%, 50%, 75%, End, Page X"
   - Button: Long-press DOWN opens jump menu

2. **Jump Actions**
   - Calculate target page from percentage
   - Validate page number in range
   - Update reader state to target page
   - Set needs_redraw flag

**User Experience:**
- Reader screen → Long-press DOWN → Jump menu appears
- Navigate with UP/DOWN, SELECT to jump
- Immediate feedback (jump happens instantly)

### Search Within Book

**Feature Description:**
- Search for text string within current book
- Display results as list with context
- Jump to search result location

**Implementation:**

1. **Search Function**
   ```c
   typedef struct {
       int page;              // Page where match found
       int line;              // Line number within page
       int offset;            // Character offset within line
       char *context;         // Snippet with match highlighted
   } search_result_t;

   typedef struct {
       search_result_t *results;
       int count;
       int capacity;
       char *query;           // Search query string
   } search_results_t;
   ```

2. **Search UI**
   - New state: READING_SEARCH (sub-state of READING)
   - Input: Use button combinations to enter letters (like T9)
   - Alternative: Reserve search for SSH/keyboard input only
   - Display: List of results with page numbers and context
   - Navigation: Select result to jump to page

3. **Search Algorithm**
   - Case-insensitive substring search
   - Search across all book content
   - Limit results to first 100 matches (performance)
   - Cache results for repeated searches

**User Experience (Simplified for Phase 4):**
- Text search via configuration file: `/etc/ereader/search.txt`
- User writes search query to file via SSH
- Press MENU → Search → Displays cached results
- Future: On-screen keyboard or external keyboard support

**Alternative (Phase 4):**
- Defer interactive text input to Phase 5
- Phase 4: Focus on EPUB/PDF support and TOC navigation
- Search can be added in Phase 5 with keyboard support

## Enhanced Book Management

### Multi-Format Detection

**Implementation:**
- Update `book_manager.c` to detect file types by extension
- Support: .txt, .epub, .pdf
- Add book type field to book_metadata_t
- Filter in library by type (optional setting)

```c
typedef enum {
    BOOK_TYPE_TXT,
    BOOK_TYPE_EPUB,
    BOOK_TYPE_PDF,
    BOOK_TYPE_UNKNOWN
} book_type_t;

typedef struct {
    char *filename;
    char *basename;
    book_type_t type;       // NEW: Book file type
    size_t size;
    bool is_valid;
} book_metadata_t;
```

### Book Metadata Display

**Feature Description:**
- Show book information screen before opening
- Display: Title, author, format, file size, page count
- Button: Long-press SELECT on book in library

**Implementation:**

1. **Metadata Extraction**
   - TXT: Filename only (no embedded metadata)
   - EPUB: Parse OPF metadata (title, creator, language, date)
   - PDF: Extract Info dictionary (Title, Author, Subject, Creator)

2. **Metadata Cache**
   - Cache extracted metadata in `/var/cache/ereader/metadata.db`
   - SQLite database (optional) or simple text file (CSV)
   - Avoid re-parsing files on each library scan

3. **Book Info UI**
   - New state: MENU_BOOK_INFO
   - Display metadata in formatted layout
   - Button: BACK returns to library, SELECT opens book

**Example Display:**
```
┌─────────────────────────────┐
│  Book Information           │
│                             │
│  Title: Frankenstein        │
│  Author: Mary Shelley       │
│  Format: EPUB 3.0           │
│  Size: 2.3 MB               │
│  Pages: 324                 │
│  Language: English          │
│                             │
│  SELECT: Open  BACK: Cancel │
└─────────────────────────────┘
```

### Cover Thumbnails (Stretch Goal)

**Feature Description:**
- Display small book cover image in library menu
- 50x75 pixel thumbnail next to book title

**Technical Challenges:**
- Image decoding: Requires libjpeg/libpng
- Image scaling: Resize to thumbnail dimensions
- 1-bit conversion: Dithering for acceptable quality
- Memory: Cache thumbnails or generate on-demand
- Performance: Generating thumbnails is slow

**Implementation (If Time Permits):**
- Extract cover image from EPUB/PDF
- Scale to 50x75 pixels
- Apply Floyd-Steinberg dithering for 1-bit conversion
- Cache in `/var/cache/ereader/covers/`
- Render thumbnail in library menu (left side of title)

**Recommendation:** Defer to Phase 5 (significant complexity, low priority)

## Reading Experience Enhancements

### Partial Display Refresh Optimization

**Goal:** Reduce page turn latency from 2-6s to <1s for text-only updates

**UC8176 Partial Refresh Mode:**
- Controller supports partial refresh waveform
- Updates only changed regions
- Faster: ~300-500ms vs 2-6s for full refresh
- Trade-off: Ghosting increases with multiple partial refreshes

**Implementation:**

1. **EPD Driver Updates (`epd_driver.c`)**
   - Add `epd_partial_refresh()` function
   - Use partial update waveform (LUT adjustment)
   - Track refresh count (force full refresh every N partials)

2. **Framebuffer Dirty Region Tracking**
   - Track changed regions (min/max x, y coordinates)
   - Only refresh changed regions
   - Clear dirty flags after refresh

3. **Refresh Strategy**
   - Page navigation: Partial refresh (text changed, status bar changed)
   - Menu navigation: Partial refresh (selection highlight only)
   - Menu change: Full refresh (new content)
   - Every 10th refresh: Full refresh (clear ghosting)

4. **User Control**
   - Setting: "Refresh mode" (Full always / Partial with auto-full / Partial only)
   - Manual full refresh: Long-press BACK (force clean screen)

**Expected Impact:**
- Page turns: 2-6s → 300-500ms (5-10x faster)
- Menu navigation: 2-4s → 200-300ms (10x faster)
- Improved responsiveness and user experience

### Reading Statistics

**Feature Description:**
- Track reading time per book
- Display reading progress percentage
- Show estimated time remaining
- Count pages read per session

**Implementation:**

1. **Statistics Data Structure**
   ```c
   typedef struct {
       char *filename;
       int total_pages;
       int pages_read;          // Highest page reached
       int reading_time_seconds; // Total time spent reading
       int session_start_page;  // Page at session start
       int session_pages_read;  // Pages read this session
       time_t last_read_time;   // Timestamp of last read
   } reading_stats_t;
   ```

2. **Statistics Tracking**
   - Update on page change (record page number)
   - Update on reader exit (save reading time)
   - Calculate reading speed (pages per minute)
   - Store in `/etc/ereader/stats.txt` (CSV format)

3. **Statistics Display**
   - Show in status bar: "Progress: 65% | Est. 45 min"
   - Button: Long-press SELECT in reader → Show stats screen
   - Stats screen: Pages read, time spent, reading speed, est. remaining

**Example Status Bar:**
```
┌─────────────────────────────┐
│ Frankenstein     [123/324]  │
│ Progress: 38%  Est: 2h 15m  │
```

**Example Stats Screen:**
```
┌─────────────────────────────┐
│  Reading Statistics         │
│                             │
│  Progress: 38% (123/324)    │
│  Pages read: 123            │
│  Time spent: 3h 42m         │
│  Reading speed: 18 pgs/hr   │
│  Estimated remaining: 2h 15m│
│                             │
│  BACK: Return to reading    │
└─────────────────────────────┘
```

### Improved Pagination Algorithm

**Current Limitation:**
- Phase 3 pagination: Even distribution, no awareness of chapter breaks
- Result: Chapters may split awkwardly across pages

**Improvement:**
- Detect chapter boundaries (EPUB chapters, PDF page breaks, TXT blank lines)
- Prefer page breaks at chapter boundaries
- Allow slightly uneven page lengths to preserve chapter integrity

**Implementation:**
- Add `prefer_break_at_offset` array to pagination context
- Mark chapter/section boundaries during text layout
- Pagination algorithm favors breaks near preferred offsets
- Trade-off: Some pages may be shorter/longer (±2 lines)

## User Interface Updates

### Reading Mode Enhancements

**New Button Mappings (Reading Mode):**

| Button | Action (Short Press) | Action (Long Press) |
|--------|----------------------|---------------------|
| UP | Previous page | N/A |
| DOWN | Next page | Jump menu |
| SELECT | Manual bookmark | Show statistics |
| BACK | Return to library | Full refresh |
| MENU | Table of contents | N/A |

**Long-press detection:**
- Long press: Button held for >500ms
- Emit BUTTON_LONG_PRESS event after timeout
- Handle in reader event loop

### Library Mode Enhancements

**New Button Mappings (Library Mode):**

| Button | Action (Short Press) | Action (Long Press) |
|--------|----------------------|---------------------|
| UP | Previous item | Jump to top |
| DOWN | Next item | Jump to bottom |
| SELECT | Open book | Show book info |
| BACK | Exit application | N/A |
| MENU | Settings menu | N/A |

### Settings Menu (New Feature)

**Menu Structure:**
```
Settings
├── Display
│   ├── Refresh Mode (Full / Partial / Auto)
│   ├── Font Size (8×16 only in Phase 4)
│   └── Line Spacing (Current / Compact / Relaxed)
├── Reading
│   ├── Chapter Break Pagination (On / Off)
│   ├── Show Reading Statistics (On / Off)
│   └── Auto-save Bookmarks (On / Off)
├── System
│   ├── System Information
│   ├── Storage Info
│   ├── Clear Cache
│   └── Reboot / Shutdown
└── About
    ├── Version Information
    ├── Credits
    └── License
```

**Implementation:**
- Nested menu system (similar to library, but hierarchical)
- Settings stored in `/etc/ereader/settings.conf` (INI format)
- Load settings on application start
- Apply settings in real-time (e.g., refresh mode change)

## Architecture Changes

### Module Structure Updates

**New Modules:**
```
src/ereader/
├── formats/                 # NEW: File format parsers
│   ├── epub_parser.c/h     # EPUB parsing and rendering
│   ├── pdf_parser.c/h      # PDF text extraction
│   └── format_detector.c/h # File type detection
├── navigation/              # NEW: Enhanced navigation
│   ├── toc.c/h             # Table of contents handling
│   ├── search.c/h          # Text search functionality
│   └── jump.c/h            # Jump-to-page/percentage
├── stats/                   # NEW: Reading statistics
│   └── reading_stats.c/h   # Statistics tracking and display
├── settings/                # NEW: Application settings
│   └── settings.c/h        # Settings management
├── rendering/               # EXISTING: Text rendering
│   ├── framebuffer.c/h     # (No changes)
│   └── text_renderer.c/h   # (Minor updates for formatting)
├── books/                   # EXISTING: Book management
│   └── book_manager.c/h    # (Updates for multi-format)
├── ui/                      # EXISTING: User interface
│   ├── menu.c/h            # (Updates for book info, settings)
│   └── reader.c/h          # (Updates for TOC, jump, stats)
└── main.c, ereader.h        # (Updates for new states)
```

### State Machine Updates

**New States:**
```
Current states (Phase 3):
- STARTUP
- MENU_LIBRARY
- READING
- EMPTY
- ERROR
- SHUTDOWN

New states (Phase 4):
- MENU_SETTINGS         # NEW: Settings menu
- MENU_BOOK_INFO        # NEW: Book info display
- READING_TOC           # NEW: Table of contents
- READING_JUMP          # NEW: Jump menu
- READING_SEARCH        # NEW: Search results (optional)
- READING_STATS         # NEW: Statistics display
```

**State Transitions:**
```
MENU_LIBRARY ←→ MENU_SETTINGS
MENU_LIBRARY ←→ MENU_BOOK_INFO ←→ READING
READING ←→ READING_TOC
READING ←→ READING_JUMP
READING ←→ READING_SEARCH (optional)
READING ←→ READING_STATS
```

### Memory Management Strategy

**Challenge:** EPUB/PDF files can be large (20-50MB), exceeding Phase 3's 10MB limit

**Solution: Chunked Reading**

1. **Don't Load Entire File**
   - EPUB: Load chapters on-demand (not entire book)
   - PDF: Extract pages on-demand (cache recent pages)
   - TXT: Keep full-file loading (Phase 3 approach)

2. **Chapter Caching (EPUB)**
   - Load current chapter + next chapter
   - Unload previous chapters after moving away
   - Cache size: ~2-5 MB (2-3 chapters typical)

3. **Page Caching (PDF)**
   - Extract and cache current page ± 2 pages
   - Cache size: ~500KB (5 pages of text)
   - Evict oldest pages when cache full

4. **Pagination Context**
   - Generate pagination lazily (chapter by chapter)
   - Store page-to-chapter mapping
   - Don't pre-calculate entire book pagination

**Memory Budget (Phase 4):**
- Application: ~2 MB
- Current chapter text: ~2 MB
- Cached chapters: ~3 MB
- Pagination data: ~1 MB
- Framebuffer: ~15 KB
- Total: ~10 MB (comfortable on 512MB system)

## Implementation Plan

### Phase 4 Task Breakdown

**Phase 4.1: EPUB Foundation (Priority 1)**
1. Add libzip and libxml2 to Buildroot configuration
2. Implement EPUB file detection in book_manager.c
3. Create epub_parser.c with basic EPUB extraction
4. Extract plain text from EPUB (no formatting)
5. Test EPUB loading and display

**Phase 4.2: EPUB Rendering (Priority 1)**
6. Implement HTML parsing with libxml2
7. Extract text from HTML with paragraph breaks
8. Handle basic HTML tags (p, h1-h6, br, div)
9. Test EPUB rendering with various books
10. Update pagination to handle chapter boundaries

**Phase 4.3: EPUB Navigation (Priority 1)**
11. Parse TOC from NCX or navigation document
12. Implement toc.c with TOC data structures
13. Create TOC UI in reader
14. Implement chapter navigation (previous/next chapter)
15. Test TOC navigation

**Phase 4.4: PDF Support (Priority 2)**
16. Add mupdf to Buildroot configuration
17. Implement PDF file detection
18. Create pdf_parser.c with text extraction
19. Handle PDF page-by-page extraction
20. Test PDF loading and display

**Phase 4.5: Enhanced Navigation (Priority 2)**
21. Implement jump-to-page functionality
22. Implement jump-to-percentage
23. Create jump menu UI
24. Test jump functionality

**Phase 4.6: Reading Statistics (Priority 3)**
25. Implement reading_stats.c with time tracking
26. Add statistics display UI
27. Store statistics persistently
28. Test statistics accuracy

**Phase 4.7: Partial Refresh (Priority 2)**
29. Implement partial refresh in epd_driver.c
30. Add dirty region tracking to framebuffer
31. Update UI components to use partial refresh
32. Test partial refresh and ghosting mitigation

**Phase 4.8: Settings Menu (Priority 3)**
33. Implement settings.c with INI file parsing
34. Create settings menu UI
35. Add refresh mode setting
36. Add line spacing setting
37. Test settings persistence

**Phase 4.9: Integration and Testing (Priority 1)**
38. Update main.c with new states
39. Update ereader.h with new data structures
40. Integration testing with TXT, EPUB, PDF
41. Performance testing and optimization
42. Update USER_GUIDE.md with Phase 4 features
43. Update README.md with new capabilities

**Phase 4.10: Documentation (Priority 1)**
44. Create PHASE_04_LOG.md
45. Update EREADER_DESIGN.md with Phase 4 architecture
46. Document EPUB and PDF parsing approach
47. Document new navigation features

### Timeline Estimates

**Development Effort (not calendar time):**

| Task Group | Estimated Effort | Dependencies |
|------------|------------------|--------------|
| EPUB Foundation | 8 hours | Buildroot, book_manager |
| EPUB Rendering | 12 hours | EPUB Foundation |
| EPUB Navigation (TOC) | 8 hours | EPUB Rendering |
| PDF Support | 10 hours | Buildroot, book_manager |
| Enhanced Navigation | 6 hours | Reader UI |
| Reading Statistics | 6 hours | Reader UI |
| Partial Refresh | 8 hours | EPD driver, framebuffer |
| Settings Menu | 6 hours | UI framework |
| Integration & Testing | 12 hours | All components |
| Documentation | 4 hours | Testing complete |
| **Total** | **~80 hours** | |

**Critical Path:**
EPUB Foundation → EPUB Rendering → TOC → Integration → Testing → Documentation

**Parallel Development Opportunities:**
- PDF support (independent of EPUB)
- Reading statistics (independent of formats)
- Settings menu (independent of formats)

### Phased Rollout Strategy

**Option 1: Complete Phase 4 (EPUB + PDF + All Features)**
- Timeline: ~80 hours
- Benefit: Full-featured e-reader
- Risk: Long development cycle, harder to test incrementally

**Option 2: Split into Phase 4a and 4b**
- Phase 4a: EPUB + TOC + Partial Refresh (~40 hours)
- Phase 4b: PDF + Statistics + Settings (~40 hours)
- Benefit: Earlier EPUB support, incremental testing
- Risk: Two release cycles

**Recommendation: Option 2 (Phased Rollout)**
- Phase 4a delivers highest-value feature (EPUB)
- Phase 4b adds nice-to-have features
- Easier testing and validation
- Users can start using EPUB support sooner

## Technical Challenges and Solutions

### Challenge 1: HTML/CSS Rendering Complexity

**Problem:** Full HTML/CSS rendering is extremely complex (browser-level complexity)

**Solution: Simplified Rendering Pipeline**
1. Parse HTML with libxml2 (mature, robust parser)
2. Convert HTML to plain text with formatting markers
3. Support limited tag set: p, br, h1-h6, div, span, em, strong, ul, ol, li
4. Ignore CSS (use default styling for all elements)
5. Render formatted text with text_renderer (extend for bold/italic if time permits)

**Alternative (Phase 4):** Plain text extraction only, defer formatting to Phase 5

### Challenge 2: EPUB Chapter Loading Performance

**Problem:** Loading chapters on-demand may cause latency (user waits for next chapter)

**Solution: Prefetching**
1. Load current chapter + next chapter in background
2. When user turns page near chapter end, next chapter is ready
3. Use separate thread or async I/O (if available)
4. Fallback: Load on-demand with "Loading..." message

### Challenge 3: PDF Text Extraction Quality

**Problem:** Some PDFs have poor text extraction (tables, columns, scanned images)

**Solution: Best-Effort Approach**
1. Use mupdf's text extraction (good quality for most PDFs)
2. Display warning for scanned PDFs (no text found)
3. Graceful degradation: If extraction fails, show error message
4. Future: OCR support in Phase 6 (tesseract)

### Challenge 4: Library Dependency Size

**Problem:** libzip + libxml2 + mupdf = ~5MB, increases image size significantly

**Solution: Image Size Optimization**
1. Strip debug symbols from libraries
2. Compile with -Os (optimize for size)
3. Disable unused features in libraries (e.g., XML schema validation)
4. Expected optimized size: ~3MB total

**Alternative:** Phase 4a: EPUB only (skip PDF, defer to 4b)

### Challenge 5: Partial Refresh Ghosting

**Problem:** Partial refreshes cause ghosting, may be worse than full refresh

**Solution: Adaptive Refresh Strategy**
1. Count partial refreshes since last full refresh
2. Force full refresh every 10 partial refreshes
3. User setting: Adjust threshold (5-20 refreshes)
4. Manual full refresh: Long-press BACK

**Testing Required:** Evaluate ghosting on actual hardware, adjust threshold

### Challenge 6: TOC Parsing Variability

**Problem:** EPUB TOC formats vary (EPUB 2 NCX vs EPUB 3 nav.xhtml)

**Solution: Support Both Formats**
1. Try EPUB 3 navigation document first (preferred)
2. Fallback to EPUB 2 NCX if nav document missing
3. Fallback to spine order if no TOC (generate from chapter filenames)
4. Handle malformed TOC gracefully (log warning, use spine)

## Success Criteria

Phase 04 is successful when:

✓ **EPUB Support:**
- Open and read EPUB 2.0 and EPUB 3.0 files
- Extract and display text content
- Navigate via table of contents
- Support chapter-based navigation

✓ **PDF Support:**
- Open and read text-based PDF files
- Extract text from pages
- Display page numbers from original PDF
- Handle basic single-column layouts

✓ **Multi-Format Library:**
- Library lists TXT, EPUB, and PDF files
- Correct format detection by extension
- Open any format from library menu
- All Phase 3 features work with all formats

✓ **Enhanced Navigation:**
- Table of contents displays and works
- Jump-to-page functionality working
- Jump-to-percentage working
- Chapter navigation (previous/next)

✓ **Performance:**
- EPUB loading: <2 seconds for typical book
- PDF loading: <3 seconds for typical book
- Page turns: <1 second with partial refresh
- Memory usage: <50MB total

✓ **Reading Experience:**
- Reading statistics tracking and display
- Settings menu with refresh mode control
- Bookmarks work with all formats
- Status bar shows appropriate info

✓ **Documentation:**
- USER_GUIDE.md updated with EPUB/PDF usage
- EREADER_DESIGN.md updated with Phase 4 architecture
- PHASE_04_LOG.md documents completion

## Testing Plan

### Unit Testing

**EPUB Parser:**
- Parse valid EPUB 2.0 and 3.0 files
- Handle missing metadata gracefully
- Extract TOC from NCX and nav documents
- Load chapters in correct spine order

**PDF Parser:**
- Extract text from single-page PDF
- Extract text from multi-page PDF
- Handle PDF with no text (scanned)
- Extract metadata (title, author)

**TOC System:**
- Build TOC from EPUB NCX
- Build TOC from EPUB nav document
- Build TOC from PDF outline
- Render TOC in UI with indentation

**Navigation:**
- Jump to specific page number
- Jump to percentage (0%, 25%, 50%, 75%, 100%)
- Navigate TOC entries
- Chapter navigation (previous/next)

### Integration Testing

**Multi-Format Library:**
- Library with TXT, EPUB, PDF files
- Open TXT file (Phase 3 functionality preserved)
- Open EPUB file (new functionality)
- Open PDF file (new functionality)

**Complete User Flows:**
1. Boot → Library → Open EPUB → Read → Jump to 50% → Return to library
2. Boot → Library → Open PDF → Read → View TOC → Jump to chapter → Read
3. Boot → Library → Open TXT → Read → Switch to EPUB → Read → Bookmark → Reboot → Resume
4. Boot → Settings → Change refresh mode → Library → Open book → Verify partial refresh

**Bookmark Compatibility:**
- Create bookmark in EPUB, reopen, verify resume
- Create bookmark in PDF, reopen, verify resume
- Mix of TXT, EPUB, PDF bookmarks in bookmarks.txt

### Performance Testing

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| EPUB load time | <2s | Stopwatch from select to first page |
| PDF load time | <3s | Stopwatch from select to first page |
| Page turn (partial) | <1s | Stopwatch with partial refresh |
| Page turn (full) | <6s | Stopwatch with full refresh |
| TOC navigation | <2s | Time from TOC select to chapter load |
| Memory usage (EPUB) | <50MB | free command while reading |
| Memory usage (PDF) | <50MB | free command while reading |

### Format Compatibility Testing

**EPUB Test Suite:**
- EPUB 2.0 with NCX TOC
- EPUB 3.0 with nav document
- EPUB with images (images skipped or placeholders)
- EPUB with complex HTML (tables, lists)
- EPUB with CSS (styles ignored)
- Large EPUB (20+ MB)

**PDF Test Suite:**
- Simple text PDF (1 column)
- Multi-page PDF (100+ pages)
- PDF with outline/TOC
- PDF with no outline
- PDF with images (text extracted, images skipped)
- Scanned PDF (no text, handled gracefully)

**TXT Regression Testing:**
- Verify all Phase 3 TXT functionality still works
- Bookmarks, pagination, navigation
- No performance degradation

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| HTML rendering too complex | High | High | Simplify to text extraction only |
| PDF library size too large | Medium | Medium | Use mupdf (smallest), strip symbols |
| Partial refresh ghosting | Medium | Medium | Adaptive strategy, user control |
| EPUB parsing variability | Medium | Medium | Support multiple TOC formats, fallbacks |
| Memory exhaustion (large books) | Low | High | Chunked loading, memory limits |
| Performance issues (slow parsing) | Medium | Medium | Cache parsed content, optimize |
| Button input complexity | Low | Low | Existing button system sufficient |
| Increased boot time | Medium | Low | Lazy initialization, defer library scan |

## Future Enhancements (Post-Phase 4)

### Phase 5: Reading Experience Enhancements
- Font size adjustment (8×16, 12×24, 16×32)
- Font style selection (serif, sans-serif)
- CSS rendering (subset: font, margins, alignment)
- Image display (grayscale dithering)
- Annotations and highlights
- Dictionary lookup (basic word definitions)

### Phase 6: Network Features
- WiFi configuration (wpa_supplicant)
- Web-based book management (upload via browser)
- Calibre integration (OPDS catalog)
- Book download from Project Gutenberg
- Firmware updates over network

### Phase 7: Advanced Features
- E-ink optimization (multi-level grayscale if supported)
- OCR for scanned PDFs (tesseract)
- Text-to-speech (espeak)
- External keyboard support (USB HID)
- Touch screen support (if hardware upgraded)

## Related Documentation

- `[[PHASE_03_LOG]]` - Phase 3 completion log (prerequisite)
- `[[EREADER_DESIGN]]` - Application architecture (will be updated)
- `[[USER_GUIDE]]` - User documentation (will be updated)
- `[[BUILD_PROCESS]]` - Buildroot build instructions
- `[[CUSTOM_PACKAGES]]` - How to add libraries to Buildroot

## References

### EPUB Specifications
- [EPUB 3.3 Specification](https://www.w3.org/publishing/epub3/epub-spec.html)
- [EPUB Open Container Format](https://www.w3.org/publishing/epub3/epub-ocf.html)
- [EPUB Content Documents](https://www.w3.org/publishing/epub3/epub-contentdocs.html)

### PDF Resources
- [PDF Reference (ISO 32000)](https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf)
- [MuPDF Documentation](https://mupdf.readthedocs.io/en/latest/)
- [Poppler Documentation](https://poppler.freedesktop.org/)

### Libraries
- [libzip Documentation](https://libzip.org/documentation/)
- [libxml2 Documentation](https://gitlab.gnome.org/GNOME/libxml2/-/wikis/home)
- [MuPDF API Reference](https://mupdf.readthedocs.io/en/latest/mupdf-api.html)

### E-Paper Optimization
- [UC8176 Partial Refresh Modes](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- [E-Ink Ghosting Mitigation](https://www.good-display.com/companyfile/32.html)

---

**Document Version**: 1.0
**Created**: 2026-01-14
**Status**: Planning - Ready for Phase 4 development
**Estimated Effort**: ~80 hours (or ~40 hours for Phase 4a, ~40 hours for Phase 4b)
**Recommendation**: Proceed with phased rollout (4a: EPUB first, 4b: PDF later)
