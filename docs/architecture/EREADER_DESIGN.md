---
type: architecture
title: E-Reader Application Design
created: 2026-01-13
tags:
  - architecture
  - software
  - design
  - ereader
  - phase-03
related:
  - "[[INPUT_SYSTEM]]"
  - "[[DISPLAY_SPECS]]"
  - "[[BUTTON_LAYOUT]]"
  - "[[PHASE_02_LOG]]"
---

# E-Reader Application Design

This document describes the complete architecture for the basic e-reader application implemented in Phase 03. It covers the application state machine, screen rendering pipeline, file system structure, and memory optimization strategies for the Raspberry Pi Zero W's constrained 512MB RAM environment.

## Overview

The e-reader application is a minimal, embedded C application designed to:
- Boot directly to a book selection menu
- Read and display .txt files from the `/books/` directory
- Provide simple page navigation using GPIO buttons
- Maintain reading position across sessions
- Operate within severe memory constraints (512MB RAM total, ~200MB available to userspace)

### Design Principles

1. **Simplicity First**: Minimal dependencies, straightforward C code
2. **Memory Efficiency**: Static allocation where possible, careful buffer management
3. **Responsive UI**: < 100ms button-to-action latency despite 5s display refresh
4. **Fault Tolerance**: Graceful handling of missing files, corrupt data, OOM conditions
5. **Testability**: Modular design with clear interfaces for unit testing

## Application State Machine

### State Diagram

```
                    ┌──────────────┐
                    │   STARTUP    │
                    │  (init hw)   │
                    └──────┬───────┘
                           │
                           ▼
              ┌────────────────────────┐
              │     MENU_LIBRARY       │◄──────────┐
              │  (list books)          │           │
              └─────┬────────────┬─────┘           │
                    │            │                 │
        SELECT book │            │ no books        │
                    │            │                 │
                    ▼            ▼                 │
         ┌─────────────┐  ┌──────────┐            │
         │   READING   │  │  EMPTY   │            │
         │ (show page) │  │ (message)│            │
         └──────┬──────┘  └─────┬────┘            │
                │                │                 │
           BACK │                │ MENU            │
                │                │                 │
                └────────────────┴─────────────────┘

```

### State Definitions

#### STARTUP State

**Purpose**: Initialize hardware and load application state.

**Entry Actions**:
1. Initialize SPI and GPIO pins for display
2. Initialize button input system via `/dev/input/event*`
3. Initialize display (power on, send init sequence)
4. Display splash screen: "E-Reader" + version
5. Scan `/books/` directory for .txt files
6. Load reading positions from `/etc/ereader/bookmarks.txt`
7. Transition to MENU_LIBRARY

**Duration**: ~2-3 seconds (including display init)

**Error Handling**:
- Display init failure → Log error, retry once, then show error message
- Button init failure → Log warning, continue (can still use serial console)
- Books directory missing → Create it, transition to EMPTY state

#### MENU_LIBRARY State

**Purpose**: Display list of available books for selection.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ E-Reader Library         [1/3]     │  ← Status bar
├────────────────────────────────────┤
│                                    │
│  > Book Title 1.txt               │  ← Selected item (highlighted)
│    Book Title 2.txt               │
│    Another Book.txt               │
│    Long Book Name That Wraps...   │
│    Science Fiction Story.txt      │
│    Classic Novel.txt              │
│    Poetry Collection.txt          │
│                                    │
│                                    │
├────────────────────────────────────┤
│ ↑/↓:Select  ENTER:Open            │  ← Control hints
└────────────────────────────────────┘
```

**Display Specifications**:
- 400×300 pixels display @ 8×16 pixel font = **18 lines × 50 characters**
- Line 1: Status bar (library name, page indicator)
- Line 2: Separator
- Lines 3-16: Book list (14 visible items)
- Line 17: Separator
- Line 18: Control hints

**State Variables**:
```c
typedef struct {
    book_entry_t *books;        /* Array of book metadata */
    int book_count;             /* Total number of books */
    int selected_index;         /* Currently highlighted book (0-based) */
    int scroll_offset;          /* First visible item index */
    int visible_items;          /* Items per screen (14) */
    int page_number;            /* Current page of results (1-based) */
} menu_state_t;
```

**Button Handling**:

| Button | Action | Behavior |
|--------|--------|----------|
| UP | Move Up | Decrement selected_index; if at top of screen, scroll up; if at top of list, wrap to bottom |
| DOWN | Move Down | Increment selected_index; if at bottom of screen, scroll down; if at bottom of list, wrap to top |
| SELECT | Open Book | Load selected book, transition to READING state |
| BACK | Exit | Reserved for future use (shutdown menu) |
| MENU | System Menu | Reserved for future use (settings, about) |

**Edge Cases**:
- Empty list (0 books): Transition to EMPTY state
- Single book: Still show menu, just one item
- Large list (>14 books): Implement scrolling/pagination

**Refresh Strategy**:
- Full refresh on entry
- Partial refresh on selection change (if supported, else full refresh)
- Full refresh every 10 selections to prevent ghosting

#### READING State

**Purpose**: Display current page of open book with navigation.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Book Title               [12/156]  │  ← Status bar (title + page)
├────────────────────────────────────┤
│ Lorem ipsum dolor sit amet,        │
│ consectetur adipiscing elit. Sed   │
│ do eiusmod tempor incididunt ut    │
│ labore et dolore magna aliqua.     │
│ Ut enim ad minim veniam, quis      │
│ nostrud exercitation ullamco       │
│ laboris nisi ut aliquip ex ea      │
│ commodo consequat. Duis aute       │
│ irure dolor in reprehenderit in    │
│ voluptate velit esse cillum        │
│ dolore eu fugiat nulla pariatur.   │
│ Excepteur sint occaecat cupidatat  │
│ non proident, sunt in culpa qui    │
│ officia deserunt mollit anim id    │
│ est laborum. Sed ut perspiciatis   │
│ unde omnis iste natus error sit.   │
├────────────────────────────────────┤
│ ↑:Prev ↓:Next BACK:Library        │  ← Control hints
└────────────────────────────────────┘
```

**Display Specifications**:
- Line 1: Status bar (book title truncated to 30 chars, page X of Y)
- Line 2: Separator
- Lines 3-17: Text content (15 lines, 50 chars/line = 750 chars/page)
- Line 18: Control hints

**State Variables**:
```c
typedef struct {
    book_t *current_book;       /* Pointer to loaded book */
    char *text_buffer;          /* Full book text in memory */
    int text_length;            /* Total characters in book */
    int current_page;           /* Current page number (0-based) */
    int total_pages;            /* Total pages in book */
    int page_offset;            /* Character offset of current page start */
    char page_content[800];     /* Rendered text for current page */
} reading_state_t;
```

**Button Handling**:

| Button | Action | Behavior |
|--------|--------|----------|
| UP | Previous Page | Decrement current_page; if at page 0, stay at page 0 |
| DOWN | Next Page | Increment current_page; if at last page, stay at last page |
| SELECT | Toggle Menu | Reserved (future: bookmark, TOC) |
| BACK | Return to Library | Save position, close book, transition to MENU_LIBRARY |
| MENU | Quick Settings | Reserved (future: font size) |

**Page Navigation**:
- When page changes, re-render page content
- Full display refresh
- Save reading position to bookmarks file

**Edge Cases**:
- Empty book (0 bytes): Show "Empty file" message
- Very large book (>2MB): Use chunked reading (see Memory Constraints)
- First page: "Previous" button stays at page 0
- Last page: "Next" button stays at last page

#### EMPTY State

**Purpose**: Inform user that no books are available.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ E-Reader Library                   │
├────────────────────────────────────┤
│                                    │
│                                    │
│        No books found.             │
│                                    │
│    Copy .txt files to /books/      │
│    then restart the application.   │
│                                    │
│                                    │
│                                    │
├────────────────────────────────────┤
│ MENU:System                        │
└────────────────────────────────────┘
```

**Button Handling**:
- MENU: Transition to system menu (future: rescan, settings, shutdown)
- All other buttons: Ignore or beep

**Exit Conditions**:
- User adds books and restarts application

### State Transition Table

| From State | Event | To State | Actions |
|------------|-------|----------|---------|
| STARTUP | Init success, books found | MENU_LIBRARY | Display library |
| STARTUP | Init success, no books | EMPTY | Display empty message |
| STARTUP | Init failure | ERROR | Display error, halt |
| MENU_LIBRARY | SELECT book | READING | Load book, display first page |
| MENU_LIBRARY | No books (rescan) | EMPTY | Clear menu |
| READING | BACK button | MENU_LIBRARY | Save position, unload book |
| READING | End of book reached | READING | Stay on last page |
| EMPTY | MENU button | SYSTEM_MENU | Reserved for future |
| Any | Shutdown signal | SHUTDOWN | Save state, cleanup, power off |

## Screen Rendering Pipeline

### Overview

The rendering pipeline transforms application state into pixels on the e-paper display.

```
┌─────────────────┐
│ Application     │  (e.g., "Show page 5 of book")
│ State           │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Text Layout     │  (word wrap, pagination, alignment)
│ Engine          │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Framebuffer     │  (render text/shapes to 400×300 1-bit buffer)
│ Renderer        │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Display Driver  │  (SPI commands to e-paper controller)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ E-Paper Display │  (physical screen update, ~5 seconds)
└─────────────────┘
```

### Text Layout Engine

**Purpose**: Calculate text positioning, word wrapping, and pagination.

**Input**: Raw text string + layout parameters (font size, margins, line spacing)

**Output**: Array of text lines for current page

**Algorithm**:

```c
typedef struct {
    int screen_width;       /* 400 pixels */
    int screen_height;      /* 300 pixels */
    int font_width;         /* 8 pixels (embedded 8×16 font) */
    int font_height;        /* 16 pixels */
    int margin_left;        /* 8 pixels */
    int margin_right;       /* 8 pixels */
    int margin_top;         /* 16 pixels */
    int margin_bottom;      /* 16 pixels */
    int line_spacing;       /* 0 pixels (no extra spacing initially) */
} layout_params_t;

/* Calculate usable area */
int usable_width = screen_width - margin_left - margin_right;  /* 384 px */
int usable_height = screen_height - margin_top - margin_bottom; /* 268 px */

/* Calculate chars per line and lines per page */
int chars_per_line = usable_width / font_width;    /* 48 chars */
int lines_per_page = usable_height / (font_height + line_spacing); /* 16 lines */

/* Characters per page */
int chars_per_page = chars_per_line * lines_per_page; /* 768 chars */
```

**Word Wrapping**:
- Greedy word wrap algorithm
- Break on whitespace (space, tab, newline)
- If word exceeds line length, break mid-word
- Preserve paragraph breaks (double newline)

**Pagination**:
```c
/* Split text into pages */
int total_chars = strlen(text);
int total_pages = (total_chars + chars_per_page - 1) / chars_per_page;

/* Get text for page N */
char* get_page_text(char *text, int page_num) {
    int offset = page_num * chars_per_page;
    if (offset >= total_chars) return NULL;

    /* Copy up to chars_per_page characters */
    static char page_buffer[800];
    int chars_to_copy = min(chars_per_page, total_chars - offset);
    strncpy(page_buffer, text + offset, chars_to_copy);
    page_buffer[chars_to_copy] = '\0';

    return page_buffer;
}
```

**Refinements** (Future):
- Smart pagination: avoid breaking mid-sentence
- Hyphenation for better word wrapping
- Justify text alignment
- Adjust line spacing for readability

### Framebuffer Renderer

**Purpose**: Draw text and UI elements into a memory buffer.

**Framebuffer Format**:
- Dimensions: 400 × 300 pixels
- Color depth: 1-bit (black or white)
- Memory size: `(400 * 300) / 8 = 15,000 bytes = 14.6 KB`
- Pixel packing: 8 pixels per byte, MSB first

**Data Structure**:
```c
#define FB_WIDTH  400
#define FB_HEIGHT 300
#define FB_SIZE   ((FB_WIDTH * FB_HEIGHT) / 8)  /* 15000 bytes */

typedef struct {
    uint8_t pixels[FB_SIZE];  /* 1-bit framebuffer */
} framebuffer_t;
```

**Pixel Operations**:

```c
/* Set pixel at (x, y) to color (0=white, 1=black) */
void fb_set_pixel(framebuffer_t *fb, int x, int y, int color) {
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) return;

    int byte_index = (y * FB_WIDTH + x) / 8;
    int bit_offset = 7 - (x % 8);  /* MSB first */

    if (color) {
        fb->pixels[byte_index] |= (1 << bit_offset);   /* Set bit (black) */
    } else {
        fb->pixels[byte_index] &= ~(1 << bit_offset);  /* Clear bit (white) */
    }
}

/* Get pixel at (x, y) */
int fb_get_pixel(framebuffer_t *fb, int x, int y) {
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) return 0;

    int byte_index = (y * FB_WIDTH + x) / 8;
    int bit_offset = 7 - (x % 8);

    return (fb->pixels[byte_index] >> bit_offset) & 1;
}

/* Clear framebuffer to color */
void fb_clear(framebuffer_t *fb, int color) {
    memset(fb->pixels, color ? 0xFF : 0x00, FB_SIZE);
}

/* Draw horizontal line */
void fb_draw_hline(framebuffer_t *fb, int x, int y, int width, int color) {
    for (int i = 0; i < width; i++) {
        fb_set_pixel(fb, x + i, y, color);
    }
}

/* Draw rectangle (filled) */
void fb_fill_rect(framebuffer_t *fb, int x, int y, int w, int h, int color) {
    for (int dy = 0; dy < h; dy++) {
        fb_draw_hline(fb, x, y + dy, w, color);
    }
}
```

**Text Rendering**:

```c
/* Embedded 8×16 bitmap font (from font.h in display-test) */
extern const unsigned char font_8x16[][16];

/* Draw single character at (x, y) */
void fb_draw_char(framebuffer_t *fb, int x, int y, char c, int color) {
    if (c < 32 || c > 126) c = '?';  /* Printable ASCII only */

    const unsigned char *glyph = font_8x16[c - 32];

    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
            if (glyph[row] & (0x80 >> col)) {
                fb_set_pixel(fb, x + col, y + row, color);
            } else {
                fb_set_pixel(fb, x + col, y + row, !color);  /* Background */
            }
        }
    }
}

/* Draw string at (x, y) */
void fb_draw_string(framebuffer_t *fb, int x, int y, const char *str, int color) {
    int cx = x;
    while (*str) {
        fb_draw_char(fb, cx, y, *str, color);
        cx += 8;  /* Advance by character width */
        str++;
    }
}

/* Draw string centered on screen */
void fb_draw_string_centered(framebuffer_t *fb, int y, const char *str, int color) {
    int len = strlen(str);
    int x = (FB_WIDTH - (len * 8)) / 2;
    fb_draw_string(fb, x, y, str, color);
}
```

**UI Elements**:

```c
/* Draw status bar at top of screen */
void fb_draw_status_bar(framebuffer_t *fb, const char *title, int current, int total) {
    /* Background */
    fb_fill_rect(fb, 0, 0, FB_WIDTH, 18, 1);  /* Black background */

    /* Title (left-aligned, white text on black) */
    fb_draw_string(fb, 8, 2, title, 0);

    /* Page indicator (right-aligned) */
    char page_str[16];
    snprintf(page_str, sizeof(page_str), "[%d/%d]", current, total);
    int x = FB_WIDTH - (strlen(page_str) * 8) - 8;
    fb_draw_string(fb, x, 2, page_str, 0);

    /* Separator line */
    fb_draw_hline(fb, 0, 19, FB_WIDTH, 1);
}

/* Draw control hints at bottom of screen */
void fb_draw_hints(framebuffer_t *fb, const char *hints) {
    /* Separator line */
    fb_draw_hline(fb, 0, 280, FB_WIDTH, 1);

    /* Hints text (centered) */
    fb_draw_string_centered(fb, 284, hints, 1);
}
```

### Display Driver Integration

**Purpose**: Send framebuffer to e-paper display.

**API** (from Phase 01 `epd_driver.c`):

```c
/* Initialize display hardware */
int epd_init(void);

/* Send framebuffer to display and refresh */
int epd_display_frame(const uint8_t *frame_buffer);

/* Clear display to white */
int epd_clear(void);

/* Enter sleep mode (low power) */
int epd_sleep(void);

/* Wake from sleep mode */
int epd_wake(void);
```

**Rendering Function**:

```c
/* Render current application state to display */
void render_screen(app_state_t *app) {
    framebuffer_t fb;
    fb_clear(&fb, 0);  /* Clear to white */

    switch (app->current_state) {
        case STATE_MENU_LIBRARY:
            render_menu_screen(&fb, &app->menu);
            break;

        case STATE_READING:
            render_reading_screen(&fb, &app->reading);
            break;

        case STATE_EMPTY:
            render_empty_screen(&fb);
            break;

        default:
            break;
    }

    /* Send to display (blocks ~5 seconds) */
    epd_display_frame(fb.pixels);
}
```

**Partial Refresh** (Future Optimization):

- UC8176 controller supports partial refresh (V2 displays)
- Update only changed regions
- Reduces refresh time from 5s to ~2s
- Accumulates ghosting, requires periodic full refresh
- Implementation deferred to Phase 05

## File System Structure

### Directory Layout

```
/
├── books/                      # Book storage directory
│   ├── book1.txt               # Text files to read
│   ├── book2.txt
│   └── long-novel.txt
│
├── etc/
│   └── ereader/
│       ├── bookmarks.txt       # Reading positions
│       └── settings.conf       # Application settings (future)
│
├── var/
│   └── log/
│       └── ereader.log         # Application log file
│
└── usr/
    └── bin/
        └── ereader             # Main application binary
```

### Book Storage (`/books/`)

**Purpose**: Centralized location for all readable content.

**Format**: Plain text files with `.txt` extension

**Organization**:
- Flat directory structure (no subdirectories in Phase 03)
- Filenames become book titles in UI
- Sort alphabetically by filename

**Constraints**:
- Maximum filename length: 255 bytes (Linux NAME_MAX)
- Maximum files per directory: 65536 (ext4 limit, practically ~1000 for performance)
- Individual file size: Unlimited by filesystem, limited by available RAM

**Reserved Filenames**:
- `welcome.txt` - Sample welcome/instructions file
- Files starting with `.` - Hidden, ignored by scanner

### Bookmarks File (`/etc/ereader/bookmarks.txt`)

**Purpose**: Persist reading positions across application restarts.

**Format**: CSV-like text file

```
# E-Reader Bookmarks File
# Format: filename,page_number,last_read_timestamp
book1.txt,15,1673634000
long-novel.txt,342,1673637600
another-book.txt,1,1673641200
```

**Fields**:
1. **Filename**: Basename of book file (no path)
2. **Page Number**: 0-based page index
3. **Timestamp**: Unix epoch seconds of last read

**Operations**:

```c
/* Load bookmarks from file */
int load_bookmarks(const char *path, bookmark_t **bookmarks, int *count);

/* Save bookmarks to file */
int save_bookmarks(const char *path, bookmark_t *bookmarks, int count);

/* Update bookmark for specific book */
void update_bookmark(bookmark_t *bookmarks, int count, const char *filename, int page);

/* Get bookmark for specific book (returns page number or 0 if not found) */
int get_bookmark(bookmark_t *bookmarks, int count, const char *filename);
```

**Error Handling**:
- File missing: Create new empty bookmarks file
- Parse error: Skip malformed lines, log warning
- Write error: Log error, continue (lose position for this session)

### Settings File (`/etc/ereader/settings.conf`) - Future

**Purpose**: User preferences (font size, refresh mode, etc.)

**Format**: Simple key=value pairs

```ini
# E-Reader Settings
font_size=16
refresh_mode=full
brightness=100
```

## Memory Constraints and Optimization Strategies

### Memory Budget

**Raspberry Pi Zero W Total RAM**: 512 MB

**Memory Allocation** (estimated):
- Linux kernel: ~80 MB
- Kernel buffers/cache: ~100 MB
- System services (init, logging): ~20 MB
- Display buffers: ~15 KB
- **Available for e-reader app**: ~200-300 MB

**Target Application Memory Footprint**: < 10 MB for typical usage

### Memory Analysis

**Fixed Allocations**:
- Code segment (executable): ~500 KB (estimated)
- Static data (font, strings): ~50 KB
- Framebuffer: 15 KB
- Stack: 1 MB (default)
- **Subtotal**: ~1.5 MB

**Dynamic Allocations**:

| Component | Size | Calculation |
|-----------|------|-------------|
| Book list (metadata) | 100 KB | 1000 books × 100 bytes each |
| Book text buffer | Variable | Depends on file size |
| Bookmarks | 10 KB | 1000 entries × 10 bytes each |
| UI buffers | 5 KB | Temporary string formatting |
| **Subtotal** | 115 KB + book size | |

**Critical Constraint**: Book text buffer

- **Small book** (50 KB): Total app memory ~1.6 MB ✓
- **Medium book** (500 KB): Total app memory ~2.1 MB ✓
- **Large book** (5 MB): Total app memory ~6.5 MB ✓
- **Very large book** (50 MB): Total app memory ~51.5 MB ✓
- **Extreme book** (100+ MB): May exhaust available memory ✗

### Optimization Strategy 1: Full File Loading (Phase 03)

**Approach**: Load entire book into memory.

**Pros**:
- Simple implementation
- Fast page navigation (no disk I/O)
- Easy pagination

**Cons**:
- Memory usage scales with file size
- Large files may cause OOM
- Slower book opening for large files

**Implementation**:

```c
typedef struct {
    char *text;           /* Entire book text */
    int text_length;      /* Number of characters */
    char filename[256];   /* Book filename */
} book_t;

/* Load book into memory */
book_t* book_load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    /* Get file size */
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* Check size limit */
    if (size > MAX_BOOK_SIZE) {
        fprintf(stderr, "Book too large: %ld bytes\n", size);
        fclose(f);
        return NULL;
    }

    /* Allocate and read */
    book_t *book = malloc(sizeof(book_t));
    book->text = malloc(size + 1);
    fread(book->text, 1, size, f);
    book->text[size] = '\0';
    book->text_length = size;

    fclose(f);
    return book;
}

/* Unload book */
void book_unload(book_t *book) {
    if (book) {
        free(book->text);
        free(book);
    }
}
```

**Size Limit**: `MAX_BOOK_SIZE = 10 MB` (conservative, allows 10 books open simultaneously in theory)

### Optimization Strategy 2: Chunked Reading (Future - Phase 04+)

**Approach**: Load only current page + small buffer into memory.

**Pros**:
- Constant memory usage regardless of file size
- Supports arbitrarily large books

**Cons**:
- More complex implementation
- Slower page turns (requires disk I/O)
- Need efficient seeking to byte offsets

**Implementation** (sketch):

```c
typedef struct {
    FILE *file_handle;        /* Open file handle */
    long file_size;           /* Total size in bytes */
    long *page_offsets;       /* Byte offset of each page */
    int total_pages;          /* Number of pages */
    int current_page;         /* Current page index */
    char page_buffer[2048];   /* Current page text */
} book_chunked_t;

/* Open book without loading full text */
book_chunked_t* book_open_chunked(const char *path);

/* Build index of page offsets (scan file once) */
void book_build_page_index(book_chunked_t *book);

/* Load specific page */
void book_load_page(book_chunked_t *book, int page_num);
```

**Deferred to Phase 04**: EPUB/PDF support will require similar chunking strategy.

### Optimization Strategy 3: Memory-Mapped Files (Future)

**Approach**: Use `mmap()` to let OS handle paging.

**Pros**:
- Kernel manages memory efficiently
- Book appears as contiguous memory
- No explicit read() calls

**Cons**:
- Still consumes virtual address space
- Page faults on first access
- Less control over memory usage

**Not implemented in Phase 03** (keep it simple).

### Memory Safety Measures

**Defensive Checks**:

```c
#define MAX_BOOK_SIZE (10 * 1024 * 1024)  /* 10 MB */
#define MAX_BOOKS 1000

/* Check available memory before allocation */
long get_available_memory() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return 0;

    char line[256];
    long available = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "MemAvailable: %ld kB", &available) == 1) {
            break;
        }
    }
    fclose(f);
    return available * 1024;  /* Convert to bytes */
}

/* Safe book loading with memory check */
book_t* book_load_safe(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return NULL;

    /* Check file size */
    if (st.st_size > MAX_BOOK_SIZE) {
        fprintf(stderr, "Book exceeds size limit\n");
        return NULL;
    }

    /* Check available memory */
    long available = get_available_memory();
    if (st.st_size * 2 > available) {  /* 2× for safety margin */
        fprintf(stderr, "Insufficient memory to load book\n");
        return NULL;
    }

    return book_load(path);
}
```

**Error Handling**:
- Show user-friendly error: "Book too large to load"
- Log detailed error to `/var/log/ereader.log`
- Gracefully return to menu

### Book List Optimization

**Problem**: Scanning directory with 1000 books is slow.

**Solution**: Cache metadata in memory.

```c
typedef struct {
    char filename[256];     /* Basename */
    long file_size;         /* Size in bytes */
    time_t modified_time;   /* Last modified timestamp */
} book_entry_t;

typedef struct {
    book_entry_t *entries;  /* Array of book metadata */
    int count;              /* Number of books */
    time_t last_scan;       /* When we last scanned directory */
} book_list_t;

/* Scan /books/ directory */
book_list_t* scan_books_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) return NULL;

    book_list_t *list = malloc(sizeof(book_list_t));
    list->entries = malloc(sizeof(book_entry_t) * MAX_BOOKS);
    list->count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && list->count < MAX_BOOKS) {
        /* Skip non-.txt files */
        if (!has_extension(entry->d_name, ".txt")) continue;

        /* Get file stats */
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) continue;

        /* Store metadata */
        book_entry_t *book = &list->entries[list->count];
        strncpy(book->filename, entry->d_name, sizeof(book->filename) - 1);
        book->file_size = st.st_size;
        book->modified_time = st.st_mtime;
        list->count++;
    }

    closedir(dir);

    /* Sort alphabetically */
    qsort(list->entries, list->count, sizeof(book_entry_t), compare_books);

    list->last_scan = time(NULL);
    return list;
}
```

**Memory Cost**: 1000 books × ~300 bytes = 300 KB (acceptable)

### Stack Usage

**Default stack size**: 1 MB (usually sufficient)

**Potential issues**:
- Deep recursion: Avoided (use iteration)
- Large local buffers: Use static or heap allocation

**No stack overflow expected** with current design.

## Performance Targets

### Boot Time
- **Target**: < 10 seconds from power-on to menu display
- **Breakdown**:
  - Kernel boot: ~5 seconds
  - Init system: ~2 seconds
  - App startup + display init: ~3 seconds

### Button Response Time
- **Target**: < 100 ms from button press to UI state change
- **Note**: Display refresh (5s) happens after state change, so UI feels responsive

### Page Turn Time
- **Target**: < 6 seconds total (1s computation + 5s display refresh)
- **Breakdown**:
  - Render new page: < 100 ms
  - Send to display: ~5 seconds (hardware limit)

### Memory Usage
- **Target**: < 10 MB for typical use case (1-2 MB book)
- **Maximum**: < 50 MB for extreme use case (large book)

## Error Handling and Fault Tolerance

### Critical Errors (Halt Application)
- Display initialization failure (after retries)
- Out of memory during startup
- Corrupted system files

### Recoverable Errors (Log and Continue)
- Missing `/books/` directory → Create it, show EMPTY state
- Missing bookmarks file → Create empty file
- Corrupted bookmark entry → Skip entry, log warning
- Book load failure → Show error message, return to menu
- Button input failure → Log warning, continue (can still work via serial)

### User-Facing Error Messages

**Display Error**:
```
┌────────────────────────────────────┐
│ Display Initialization Failed      │
│                                    │
│ Please check connections and       │
│ restart the device.                │
│                                    │
│ See /var/log/ereader.log for       │
│ details.                           │
└────────────────────────────────────┘
```

**Book Load Error**:
```
┌────────────────────────────────────┐
│ Unable to Open Book                │
│                                    │
│ "very-large-file.txt"              │
│                                    │
│ Reason: File too large (15 MB)     │
│                                    │
│ Press BACK to return to library    │
└────────────────────────────────────┘
```

## Testing Strategy

### Unit Tests
- Text layout functions (word wrap, pagination)
- Framebuffer operations (pixel set/get, text rendering)
- File I/O (book scanning, bookmark load/save)
- State machine transitions

### Integration Tests
- Full application flow: menu → open book → navigate → return
- Memory leak detection (valgrind)
- Large file handling
- Error injection (missing files, corrupted data)

### Hardware Tests
- Actual button presses
- Display refresh timing
- Long-running stability (24+ hours)

## Future Enhancements (Beyond Phase 03)

### Phase 04: EPUB/PDF Support
- Add libzip, libxml2, poppler/mupdf dependencies
- Implement EPUB parser
- Extract text/images from PDF
- More complex pagination

### Phase 05: Polish
- Partial refresh optimization
- Adjustable font size
- Brightness control
- Bookmarks UI
- Table of contents navigation

### Phase 06: WiFi Features
- Download books from Project Gutenberg
- Sync reading positions to cloud
- Software updates

### Phase 07: Power Management
- Sleep mode after inactivity
- Battery monitoring
- Wake-on-button

## References

- [[INPUT_SYSTEM]] - Button input abstraction and event handling
- [[DISPLAY_SPECS]] - E-paper display technical specifications
- [[BUTTON_LAYOUT]] - GPIO button hardware configuration
- [[PHASE_02_LOG]] - Phase 2 implementation notes
- [Raspberry Pi Zero W Specs](https://www.raspberrypi.com/products/raspberry-pi-zero-w/)
- [Linux framebuffer programming](https://www.kernel.org/doc/Documentation/fb/framebuffer.txt)

## Revision History

- 2026-01-13: Initial e-reader application architecture design (Phase 03)
