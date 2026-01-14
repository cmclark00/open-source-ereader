# Phase 03: Basic E-Reader Application

This phase implements the core e-reader application with a simple menu system and text file reading capability. By the end of this phase, you'll have a functional e-reader that boots directly to a menu, lists text files, and displays them on the e-paper screen with page navigation.

## Tasks

- [x] Design e-reader application architecture:
  - Create `docs/architecture/EREADER_DESIGN.md` with front matter (type: architecture, tags: [software, design]):
    - Application state machine (boot → menu → reading → menu)
    - Screen rendering pipeline (text layout → framebuffer → e-paper refresh)
    - File system structure for storing books (/books/ directory)
    - Memory constraints and optimization strategies for Pi Zero W (512MB RAM)
  - Link to `[[Input-System]]` and `[[Display-Specs]]` documents
  - **Completion Notes**: Successfully created comprehensive EREADER_DESIGN.md (850+ lines) with structured YAML front matter (type: architecture, tags: [software, design, ereader, phase-03]) and complete architecture documentation including: (1) Four-state application state machine (STARTUP, MENU_LIBRARY, READING, EMPTY) with detailed state definitions, button handling tables, and transition diagram, (2) Complete screen rendering pipeline from application state → text layout → framebuffer → display driver with detailed algorithms for word wrapping and pagination, (3) File system structure with /books/ directory for content, /etc/ereader/bookmarks.txt for reading positions, detailed format specifications and error handling, (4) Memory constraints analysis for Pi Zero W (512MB RAM, ~200MB userspace available) with three optimization strategies: full file loading (Phase 03 approach, max 10MB books), chunked reading (future), and memory-mapped files (future), (5) Detailed rendering specifications: 400×300 pixel display, 8×16 font yielding 18 lines × 50 chars, framebuffer structure (15KB 1-bit buffer), pixel operations, text rendering, and UI elements (status bar, hints), (6) Performance targets (< 10s boot, < 100ms button response, < 6s page turn), error handling strategy, testing approach, and future enhancement roadmap. Document includes wiki-link cross-references to [[INPUT_SYSTEM]], [[DISPLAY_SPECS]], [[BUTTON_LAYOUT]], and [[PHASE_02_LOG]]. All required sub-items completed: state machine, rendering pipeline, file system structure, memory optimization, and cross-references.

- [x] Create text rendering engine:
  - Create `src/ereader/rendering/` directory
  - Write `text_renderer.c` with functions:
    - Load bitmap font (8x16 or similar embedded font)
    - Render string to framebuffer with word wrapping
    - Calculate text layout (lines per page, characters per line)
    - Pagination: split text into screen-sized pages
  - Write `text_renderer.h` with rendering configuration (font size, margins, line spacing)
  - Write `framebuffer.c` for low-level framebuffer operations (clear, pixel draw, rect fill)
  - Write `framebuffer.h` with framebuffer structure and color definitions
  - **Completion Notes**: Successfully created complete text rendering engine (954 total lines) in src/ereader/rendering/. **framebuffer.h** (109 lines): Defines framebuffer_t structure for 400x300 1-bit display (15KB buffer), COLOR_WHITE/BLACK definitions, and complete API: fb_init(), fb_clear(), fb_set_pixel(), fb_get_pixel(), fb_draw_hline(), fb_draw_vline(), fb_draw_rect(), fb_invert_region(), fb_copy_to_buffer(). **framebuffer.c** (171 lines): Implements all low-level operations with proper bit manipulation (MSB=leftmost pixel), bounds checking, region clipping, and pixel inversion. **text_renderer.h** (162 lines): Defines rendering config (margins 20/20/10/10px, line spacing 2px), calculated constants (47 chars/line, 14 lines/page, 18px line height), text_page_t and pagination_t structures, and complete text rendering API. **text_renderer.c** (512 lines): Implements full engine with embedded 8x16 font (ASCII 32-90, 944 bytes), text_render_char(), text_render_string() with newline/tab support, text_render_wrapped() with intelligent word wrapping and long-word breaking, text_calculate_layout() for line splitting, text_create_pagination() for automatic page distribution, text_free_pagination(), text_get_page(), text_render_page(), plus utility functions text_measure_width() and text_chars_in_width(). All functions include error handling, bounds checking, and proper memory management. Ready for integration with book manager.

- [x] Implement book file management:
  - Create `src/ereader/books/` directory
  - Write `book_manager.c` with functions:
    - Scan /books/ directory for .txt files
    - Load book metadata (filename, size, last read position)
    - Read book content into memory (with chunking for large files)
    - Save/load reading position to /etc/ereader/bookmarks.txt
  - Write `book_manager.h` with book structure and API
  - Handle edge cases: empty directory, corrupt files, insufficient memory
  - **Completion Notes**: Successfully created complete book management system (647 total lines) in src/ereader/books/. **book_manager.h** (169 lines): Defines comprehensive structures (book_metadata_t, book_list_t, book_t, bookmark_t, bookmark_list_t), configuration constants (MAX_BOOK_SIZE=10MB, BOOKS_DIR="/books", BOOKMARKS_FILE="/etc/ereader/bookmarks.txt"), error codes (7 distinct error types), and complete API with 23 functions covering book list management, loading/unloading, bookmark persistence, and utilities. **book_manager.c** (478 lines): Implements full functionality: (1) Book scanning with book_list_scan() that recursively scans /books/ for .txt files, filters hidden files, validates file stats, handles dynamic capacity expansion up to MAX_BOOKS=1000, and sorts alphabetically; (2) Book loading with book_load() that validates file size limits, performs full-file read into memory, null-terminates text, and extracts basename; (3) Bookmark management with bookmark_list_load/save() that parses CSV format (filename,page,timestamp), handles missing files gracefully, creates /etc/ereader/ directory on demand, and bookmark_update/get/remove() for CRUD operations; (4) Edge case handling: empty directory returns BOOK_ERROR_NO_BOOKS, corrupt files logged and skipped during scan, files >10MB rejected with BOOK_ERROR_TOO_LARGE, empty files skipped, out-of-memory conditions return BOOK_ERROR_OUT_OF_MEMORY; (5) Utility functions: book_error_string() for human-readable errors, book_basename() for path parsing, book_is_txt_file() for case-insensitive .txt extension checking, book_format_size() for display formatting (B/KB/MB/GB), and book_validate() for pre-load file checking. All memory allocations use dynamic resizing with initial capacity of 32 entries. Bookmarks file uses simple CSV format with comments support. System ready for integration with menu and reader UI components.

- [x] Create menu system:
  - Create `src/ereader/ui/` directory
  - Write `menu.c` with functions:
    - Render menu items (book list) to framebuffer
    - Handle scrolling (up/down buttons) with selection highlight
    - Calculate visible items per screen (depends on font size)
    - Menu state management (selected index, scroll offset)
  - Write `menu.h` with menu structures and callbacks
  - Implement simple menu: list books, select to open, back button returns to menu
  - **Completion Notes**: Successfully created complete menu system (779 total lines) in src/ereader/ui/. **menu.h** (277 lines): Defines comprehensive menu architecture with MENU_VISIBLE_ITEMS=14 (lines 2-15 on 400x300 display), menu_state_t structure (book_list, bookmarks, selected_index, scroll_offset, needs_redraw flag), menu_action_t enum (NONE, REDRAW, SELECT_BOOK, EXIT) for event handling results, and complete API with 23 functions covering: (1) Initialization: menu_create() with book_list/bookmarks references, menu_free(), menu_reset(); (2) Rendering: menu_render() for full screen, menu_render_status_bar() with title + page indicator [current/total], menu_render_items() with selection highlight and scrolling, menu_render_hints() for control display, menu_render_empty() for zero-books state; (3) Navigation: menu_handle_event() dispatcher, menu_move_up/down() with wrapping (top↔bottom), menu_page_up/down() for fast navigation, menu_get_selected_book/index() accessors; (4) State queries: menu_needs_redraw(), menu_get_current_page/total_pages() for pagination. **menu.c** (502 lines): Implements full functionality: (1) Status bar with left-aligned "E-Reader Library" title and right-aligned page indicator "[1/3]" format, separator lines using fb_draw_hline(); (2) Item rendering with 14 visible items per screen, "> " selection marker for highlighted item, fb_invert_region() for selection highlighting (inverted background), truncated titles with "..." for long filenames (max 47 chars minus marker), proper scrolling with scroll_offset management; (3) Navigation with menu_move_up/down() implementing wrap-around behavior (top→bottom, bottom→top), automatic scroll adjustment to keep selection visible, page up/down moving by MENU_VISIBLE_ITEMS; (4) Event handling that processes BUTTON_PRESS events only (ignores release/repeat), UP/DOWN for navigation with redraw flag, SELECT for book opening, BACK for exit, MENU reserved for future; (5) Empty state rendering with centered "No books found." message and "Copy .txt files to /books/" hint at screen center; (6) Pagination logic calculating current page from selected_index, total pages from book count with ceiling division; (7) Helper functions: menu_truncate_title() with "..." ellipsis, menu_format_page_indicator() for [X/Y] format, menu_calculate_total_pages() with ceiling division, menu_draw_separator_line() for horizontal rules. All rendering uses text_renderer.h API (text_render_string) and framebuffer.h operations (fb_clear, fb_invert_region, fb_draw_hline). Integrates with book_manager.h (book_list_t, book_metadata_t) and button_input.h (button_event_t, BUTTON_* codes). Error handling via menu_error_t enum (NULL_POINTER, INVALID_STATE, RENDER_FAILED, NO_BOOKS). System ready for integration with reader.c and main application loop.

- [ ] Implement reading view:
  - Write `src/ereader/ui/reader.c` with functions:
    - Display current page of text on screen
    - Page navigation (UP=previous page, DOWN=next page)
    - Display status bar (page X of Y, book title)
    - Back button returns to menu
  - Write `src/ereader/ui/reader.h` with reader state structure
  - Handle edge cases: first page, last page, empty book

- [ ] Create main application loop:
  - Write `src/ereader/main.c` with:
    - Initialize display driver from Phase 1
    - Initialize button input from Phase 2
    - Create application state machine (MENU, READING, SETTINGS)
    - Main event loop: process button events, update screen, handle state transitions
    - Graceful shutdown on SIGTERM/SIGINT
  - Write `src/ereader/ereader.h` with application-wide definitions
  - Create `src/ereader/Makefile` linking all modules together

- [ ] Add sample content and init scripts:
  - Create `board/ereader/rootfs-overlay/books/` directory
  - Add sample .txt files: `welcome.txt` with usage instructions, `gutenberg-sample.txt` with public domain text
  - Create `board/ereader/rootfs-overlay/etc/init.d/S99ereader` init script:
    - Start ereader application on boot
    - Redirect output to /var/log/ereader.log for debugging
    - Restart on crash (simple watchdog)
  - Make init script executable in post-build.sh

- [ ] Integrate e-reader application into Buildroot:
  - Create `package/ereader/ereader.mk` package definition:
    - Build all ereader source files
    - Install binary to /usr/bin/ereader
    - Install init script
  - Create `package/ereader/Config.in`
  - Add dependencies: display-test (for driver code), button-test (for input code)
  - Update `configs/ereader_rpi0w_defconfig` to enable ereader package

- [ ] Build complete system and create user guide:
  - Rebuild Buildroot with full ereader application
  - Test boot-to-reader flow: power on → see menu → select book → read pages → return to menu
  - Create `docs/USER_GUIDE.md` with:
    - How to add books to the e-reader (mount SD card, copy .txt files to /books/)
    - Button functions in each mode (menu vs reading)
    - How to navigate and read
    - Current limitations (only .txt files, no WiFi yet)
  - Update `README.md` with Phase 3 completion status

- [ ] Document completion and plan Phase 4:
  - Update `docs/progress/PHASE_03_LOG.md` with:
    - Working features: boot-to-reader, menu navigation, text display, page turning
    - Performance notes: boot time, page refresh time, responsiveness
    - Bugs or issues discovered
  - Create `docs/progress/PHASE_04_PLANNING.md` outlining EPUB support implementation
