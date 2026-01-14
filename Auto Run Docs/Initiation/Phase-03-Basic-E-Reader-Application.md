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

- [ ] Create text rendering engine:
  - Create `src/ereader/rendering/` directory
  - Write `text_renderer.c` with functions:
    - Load bitmap font (8x16 or similar embedded font)
    - Render string to framebuffer with word wrapping
    - Calculate text layout (lines per page, characters per line)
    - Pagination: split text into screen-sized pages
  - Write `text_renderer.h` with rendering configuration (font size, margins, line spacing)
  - Write `framebuffer.c` for low-level framebuffer operations (clear, pixel draw, rect fill)
  - Write `framebuffer.h` with framebuffer structure and color definitions

- [ ] Implement book file management:
  - Create `src/ereader/books/` directory
  - Write `book_manager.c` with functions:
    - Scan /books/ directory for .txt files
    - Load book metadata (filename, size, last read position)
    - Read book content into memory (with chunking for large files)
    - Save/load reading position to /etc/ereader/bookmarks.txt
  - Write `book_manager.h` with book structure and API
  - Handle edge cases: empty directory, corrupt files, insufficient memory

- [ ] Create menu system:
  - Create `src/ereader/ui/` directory
  - Write `menu.c` with functions:
    - Render menu items (book list) to framebuffer
    - Handle scrolling (up/down buttons) with selection highlight
    - Calculate visible items per screen (depends on font size)
    - Menu state management (selected index, scroll offset)
  - Write `menu.h` with menu structures and callbacks
  - Implement simple menu: list books, select to open, back button returns to menu

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
