---
type: guide
title: Developer's Guide
created: 2026-01-17
tags:
  - development
  - architecture
  - extension
  - technical
related:
  - "[[CONTRIBUTING]]"
  - "[[EREADER_DESIGN]]"
  - "[[FORMAT_SUPPORT]]"
  - "[[BUILD_PROCESS]]"
---

# Developer's Guide

This guide provides detailed technical information for developers who want to extend, modify, or understand the internals of the Open Source E-Reader project.

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Development Workflow](#development-workflow)
- [Adding New Book Formats](#adding-new-book-formats)
- [Modifying UI Components](#modifying-ui-components)
- [Adding New Settings](#adding-new-settings)
- [Extending the Display System](#extending-the-display-system)
- [Power Management](#power-management)
- [Network Features](#network-features)
- [Debugging Tips and Tools](#debugging-tips-and-tools)
- [Performance Optimization](#performance-optimization)
- [Testing Strategies](#testing-strategies)

## Architecture Overview

### System Layers

The e-reader system consists of multiple layers:

```
┌─────────────────────────────────────────┐
│  Application Layer (ereader)            │  User-facing features
│  - UI, Book Management, Settings        │
├─────────────────────────────────────────┤
│  Library Layer                          │  Reusable components
│  - Format Readers, Text Renderer        │
│  - Framebuffer, Input Manager           │
├─────────────────────────────────────────┤
│  Hardware Abstraction Layer             │  Hardware interfaces
│  - Display Driver (EPD), GPIO           │
│  - SPI, I2C, Power Management           │
├─────────────────────────────────────────┤
│  Linux Kernel                           │  OS kernel
│  - Device Drivers, Filesystems          │
├─────────────────────────────────────────┤
│  Hardware                               │  Physical components
│  - Pi Zero W, E-Paper Display, Buttons  │
└─────────────────────────────────────────┘
```

### Key Components

**Application (`src/ereader/main.c`):**
- Main event loop
- State machine management
- High-level application logic

**Book Management (`src/ereader/books/`):**
- Book scanning and indexing
- Bookmark persistence
- Format detection

**Format Readers (`src/ereader/formats/`):**
- Text (.txt) reader
- EPUB reader (HTML + ZIP extraction)
- PDF reader (external tools: pdftotext, mutool)
- Format interface abstraction

**Rendering (`src/ereader/rendering/`):**
- Framebuffer operations (pixel-level drawing)
- Text rendering (bitmap fonts, word wrap, pagination)
- UI component rendering

**User Interface (`src/ereader/ui/`):**
- Library browser (book selection menu)
- Reading view (page display)
- Settings menu
- Search interface

**Power Management (`src/ereader/power/`):**
- Auto-sleep functionality
- Battery monitoring (INA219, MCP3008)

**Network (`src/ereader/network/`):**
- WiFi management
- Download manager
- Time synchronization

### Data Flow Example: Opening a Book

```
1. User presses SELECT button on book
   ↓
2. library_browser detects button event
   ↓
3. Calls book_manager_open(filename)
   ↓
4. book_manager detects format (txt/epub/pdf)
   ↓
5. Calls appropriate format reader (e.g., epub_reader_open)
   ↓
6. Format reader extracts/loads text content
   ↓
7. text_renderer paginates content
   ↓
8. Application state changes to STATE_READING
   ↓
9. reading_view_render() draws first page
   ↓
10. framebuffer sent to display driver
   ↓
11. EPD refreshes (~5 seconds)
```

## Development Workflow

### Making Changes to Existing Code

1. **Identify the component** you need to modify
2. **Read the existing code** and understand the current implementation
3. **Make your changes** following coding standards
4. **Rebuild the package:**

```bash
cd buildroot
make BR2_EXTERNAL=.. ereader-rebuild
```

5. **Test on hardware** or in development environment
6. **Commit your changes** with clear commit message

### Adding New Source Files

If you add new `.c` or `.h` files to `src/ereader/`:

1. **Create the files** in the appropriate subdirectory
2. **Update the package Makefile** (`package/ereader/ereader.mk`):

```makefile
define EREADER_BUILD_CMDS
    $(TARGET_MAKE_ENV) $(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) \
        CFLAGS="$(TARGET_CFLAGS) -I$(@D)/include" \
        LDFLAGS="$(TARGET_LDFLAGS) -lm" \
        -C $(@D) \
        ereader           # Main binary
        new_utility       # Add new binary here if needed
endef

define EREADER_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ereader $(TARGET_DIR)/usr/bin/ereader
    $(INSTALL) -D -m 0755 $(@D)/new_utility $(TARGET_DIR)/usr/bin/new_utility
endef
```

3. **Update the application Makefile** (`src/ereader/Makefile`):

```makefile
SRCS = main.c \
       books/book_manager.c \
       formats/format_interface.c \
       formats/txt_reader.c \
       your_new_file.c              # Add here

OBJS = $(SRCS:.c=.o)
```

4. **Rebuild:**

```bash
make BR2_EXTERNAL=.. ereader-rebuild
```

### Debugging During Development

**Serial Console Access:**

```bash
# Connect via USB-to-TTL serial adapter
screen /dev/ttyUSB0 115200

# Login
Login: root
Password: ereader

# Check logs
tail -f /var/log/ereader.log

# Run application manually
/usr/bin/ereader

# Kill if needed
killall ereader
```

**Adding Debug Output:**

```c
/* Use fprintf to stderr for debug messages */
#ifdef DEBUG
#define DBG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define DBG(fmt, ...) do {} while(0)
#endif

/* In your code */
DBG("Opening book: %s (size: %ld bytes)", filename, file_size);
```

## Adding New Book Formats

This section demonstrates how to add support for a new book format (e.g., MOBI).

### Step 1: Understand the Format Interface

All format readers implement the `format_interface.h` API:

```c
/* From src/ereader/formats/format_interface.h */

typedef struct format_reader {
    char format_name[32];

    /* Detect if file is this format */
    bool (*detect)(const char *filepath);

    /* Open and parse the file */
    int (*open)(const char *filepath, book_content_t *content);

    /* Close and cleanup */
    void (*close)(book_content_t *content);

    /* Get metadata (optional) */
    int (*get_metadata)(const char *filepath, book_metadata_t *metadata);
} format_reader_t;

typedef struct book_content {
    char *text;              /* Extracted text content */
    size_t text_length;      /* Length in bytes */
    char title[256];         /* Book title */
    char author[256];        /* Author name */
    /* Add more fields as needed */
} book_content_t;
```

### Step 2: Create Format Reader Files

Create `src/ereader/formats/mobi_reader.c` and `.h`:

**mobi_reader.h:**

```c
#ifndef MOBI_READER_H
#define MOBI_READER_H

#include "format_interface.h"

/* Register MOBI format reader */
void mobi_reader_register(void);

#endif /* MOBI_READER_H */
```

**mobi_reader.c:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "mobi_reader.h"
#include "format_interface.h"

/* MOBI magic bytes */
#define MOBI_MAGIC "BOOKMOBI"
#define MOBI_MAGIC_LEN 8

/**
 * Detect if file is MOBI format
 */
static bool mobi_detect(const char *filepath)
{
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        return false;
    }

    /* MOBI magic is at offset 0x3C */
    fseek(f, 0x3C, SEEK_SET);

    char magic[MOBI_MAGIC_LEN + 1] = {0};
    size_t read = fread(magic, 1, MOBI_MAGIC_LEN, f);
    fclose(f);

    if (read != MOBI_MAGIC_LEN) {
        return false;
    }

    return strncmp(magic, MOBI_MAGIC, MOBI_MAGIC_LEN) == 0;
}

/**
 * Open and parse MOBI file
 */
static int mobi_open(const char *filepath, book_content_t *content)
{
    if (!filepath || !content) {
        return -1;
    }

    /* Option 1: Use external tool (simpler) */
    /* Example using mobi2txt or similar tool */
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mobi2txt '%s' /tmp/mobi_output.txt 2>&1", filepath);

    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "mobi2txt failed: %d\n", ret);
        return -1;
    }

    /* Read extracted text */
    FILE *f = fopen("/tmp/mobi_output.txt", "r");
    if (!f) {
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    content->text = malloc(size + 1);
    if (!content->text) {
        fclose(f);
        return -1;
    }

    fread(content->text, 1, size, f);
    content->text[size] = '\0';
    content->text_length = size;

    fclose(f);

    /* Option 2: Use library (more complex, better) */
    /* If using libmobi: */
    /*
    #include <mobi.h>
    MOBIData *m = mobi_init();
    if (mobi_load_file(m, filepath) != MOBI_SUCCESS) {
        mobi_free(m);
        return -1;
    }

    MOBIRawml *rawml = mobi_init_rawml(m);
    if (mobi_parse_rawml(rawml, m) != MOBI_SUCCESS) {
        mobi_free(m);
        mobi_free_rawml(rawml);
        return -1;
    }

    // Extract text from rawml
    content->text = strdup(rawml->text);
    content->text_length = strlen(content->text);

    mobi_free_rawml(rawml);
    mobi_free(m);
    */

    /* Extract title from filename for now */
    const char *basename = strrchr(filepath, '/');
    basename = basename ? basename + 1 : filepath;
    strncpy(content->title, basename, sizeof(content->title) - 1);

    /* Remove .mobi extension */
    char *ext = strstr(content->title, ".mobi");
    if (ext) *ext = '\0';

    return 0;
}

/**
 * Close and cleanup MOBI content
 */
static void mobi_close(book_content_t *content)
{
    if (content && content->text) {
        free(content->text);
        content->text = NULL;
        content->text_length = 0;
    }
}

/**
 * Get MOBI metadata (optional)
 */
static int mobi_get_metadata(const char *filepath, book_metadata_t *metadata)
{
    /* Parse MOBI header for author, title, etc. */
    /* This is optional - return -1 if not implemented */
    return -1;
}

/* Define the format reader interface */
static format_reader_t mobi_reader = {
    .format_name = "MOBI",
    .detect = mobi_detect,
    .open = mobi_open,
    .close = mobi_close,
    .get_metadata = mobi_get_metadata
};

/**
 * Register MOBI format reader
 */
void mobi_reader_register(void)
{
    format_interface_register(&mobi_reader);
}
```

### Step 3: Register the Format Reader

In `src/ereader/formats/format_interface.c`:

```c
#include "txt_reader.h"
#include "epub_reader.h"
#include "pdf_reader.h"
#include "mobi_reader.h"  /* Add this */

void format_interface_init(void)
{
    /* Register all format readers */
    txt_reader_register();
    epub_reader_register();
    pdf_reader_register();
    mobi_reader_register();  /* Add this */
}
```

### Step 4: Add Dependencies (if needed)

If using an external library like `libmobi`:

**Update `package/ereader/ereader.mk`:**

```makefile
EREADER_DEPENDENCIES = zlib libxml2 libmobi

define EREADER_BUILD_CMDS
    $(TARGET_MAKE_ENV) $(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) \
        CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
        LDFLAGS="$(TARGET_LDFLAGS) -lz -lxml2 -lmobi -lm" \
        -C $(@D) ereader
endef
```

**Create `package/libmobi/` package** (if not in Buildroot):

See [[CUSTOM_PACKAGES]] for detailed instructions.

### Step 5: Update Makefile

In `src/ereader/Makefile`, add the new source file:

```makefile
FORMAT_SRCS = formats/format_interface.c \
              formats/txt_reader.c \
              formats/epub_reader.c \
              formats/pdf_reader.c \
              formats/mobi_reader.c        # Add this
```

### Step 6: Build and Test

```bash
# Rebuild ereader package
cd buildroot
make BR2_EXTERNAL=.. ereader-rebuild

# Flash to SD card and test with .mobi file
```

### Format Reader Best Practices

- **Error handling:** Return proper error codes, clean up on failure
- **Memory management:** Free all allocated resources in close()
- **Detection accuracy:** Make detect() reliable but fast
- **Graceful degradation:** Handle malformed files without crashing
- **Logging:** Use stderr for error messages
- **Testing:** Test with various files (valid, corrupted, edge cases)

## Modifying UI Components

### UI Architecture

The UI system consists of:

1. **Framebuffer** (`rendering/framebuffer.c`) - Low-level pixel operations
2. **Text Renderer** (`rendering/text_renderer.c`) - Text layout and rendering
3. **UI Components** (`ui/ui_components.c`) - Reusable UI elements
4. **Views** (`ui/library_browser.c`, `ui/reading_view.c`) - High-level screens

### Example: Adding a New UI Component

Let's add a progress bar component:

**In `src/ereader/ui/ui_components.h`:**

```c
/**
 * Draw a progress bar
 *
 * @param fb Framebuffer to draw on
 * @param x X position (top-left corner)
 * @param y Y position
 * @param width Width in pixels
 * @param height Height in pixels
 * @param percentage Progress (0-100)
 * @param show_text Show percentage text
 */
void ui_draw_progress_bar(framebuffer_t *fb, int x, int y,
                         int width, int height,
                         int percentage, bool show_text);
```

**In `src/ereader/ui/ui_components.c`:**

```c
void ui_draw_progress_bar(framebuffer_t *fb, int x, int y,
                         int width, int height,
                         int percentage, bool show_text)
{
    /* Clamp percentage to 0-100 */
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    /* Draw outer border */
    fb_draw_rect(fb, x, y, width, height, 1);  /* Black border */

    /* Draw inner background (white) */
    fb_fill_rect(fb, x + 1, y + 1, width - 2, height - 2, 0);

    /* Calculate filled width */
    int filled_width = ((width - 4) * percentage) / 100;

    /* Draw filled portion (black) */
    if (filled_width > 0) {
        fb_fill_rect(fb, x + 2, y + 2, filled_width, height - 4, 1);
    }

    /* Draw percentage text if requested */
    if (show_text) {
        char text[16];
        snprintf(text, sizeof(text), "%d%%", percentage);

        /* Center text in progress bar */
        int text_x = x + (width - strlen(text) * 8) / 2;
        int text_y = y + (height - 16) / 2;

        fb_draw_string(fb, text_x, text_y, text, 1);
    }
}
```

**Usage in your code:**

```c
/* Show download progress */
framebuffer_t fb;
fb_clear(&fb, 0);

ui_draw_progress_bar(&fb, 50, 140, 300, 30, download_percentage, true);

fb_draw_string_centered(&fb, 110, "Downloading book...", 1);

epd_display_frame(fb.pixels);
```

### Modifying Existing Views

Example: Add page progress indicator to reading view.

**In `src/ereader/ui/reading_view.c`:**

```c
void reading_view_render(reading_view_state_t *state, framebuffer_t *fb)
{
    /* Existing rendering code */
    fb_clear(fb, 0);

    /* Draw status bar */
    reading_view_draw_status_bar(state, fb);

    /* Draw book text */
    reading_view_draw_text(state, fb);

    /* NEW: Draw progress bar at bottom */
    int progress = (state->current_page * 100) / state->total_pages;
    ui_draw_progress_bar(fb, 10, 280, 380, 15, progress, false);

    /* Draw control hints */
    reading_view_draw_hints(state, fb);
}
```

### UI Design Guidelines

- **E-Paper Optimized:** High contrast, clear fonts, minimal gradients
- **Consistent Layout:** Status bar at top, hints at bottom
- **Readable Text:** Use 8×16 minimum font size
- **Visual Hierarchy:** Important info larger/bolder
- **Minimize Refreshes:** Batch UI updates when possible
- **Accessibility:** Clear visual feedback for all actions

## Adding New Settings

### Step 1: Define Setting in settings_manager.h

```c
/* In src/ereader/settings/settings_manager.h */

typedef enum {
    PAGE_TURN_ANIMATION_NONE = 0,
    PAGE_TURN_ANIMATION_FADE,
    PAGE_TURN_ANIMATION_SLIDE
} page_turn_animation_t;

typedef struct {
    /* Existing settings */
    font_size_t font_size;
    line_spacing_t line_spacing;
    margins_t margins;
    display_mode_t display_mode;
    int auto_sleep_minutes;

    /* NEW SETTING */
    page_turn_animation_t page_turn_animation;
} settings_t;
```

### Step 2: Add Default Value

```c
/* In src/ereader/settings/settings_manager.c */

settings_t* settings_load(void)
{
    settings_t *settings = malloc(sizeof(settings_t));
    if (!settings) {
        return NULL;
    }

    /* Set defaults */
    settings->font_size = FONT_SIZE_MEDIUM;
    settings->line_spacing = LINE_SPACING_SINGLE;
    settings->margins = MARGINS_NORMAL;
    settings->display_mode = DISPLAY_MODE_NORMAL;
    settings->auto_sleep_minutes = 15;
    settings->page_turn_animation = PAGE_TURN_ANIMATION_NONE;  /* NEW */

    /* Load from file and override defaults */
    settings_load_from_file(settings, SETTINGS_FILE_PATH);

    return settings;
}
```

### Step 3: Add Parsing and Saving Logic

```c
/* Parse from config file */
static void settings_parse_line(settings_t *settings, const char *line)
{
    char key[64], value[256];
    if (sscanf(line, "%63[^=]=%255s", key, value) != 2) {
        return;
    }

    /* Existing parsers */
    if (strcmp(key, "font_size") == 0) {
        /* ... */
    }

    /* NEW PARSER */
    else if (strcmp(key, "page_turn_animation") == 0) {
        if (strcmp(value, "none") == 0) {
            settings->page_turn_animation = PAGE_TURN_ANIMATION_NONE;
        } else if (strcmp(value, "fade") == 0) {
            settings->page_turn_animation = PAGE_TURN_ANIMATION_FADE;
        } else if (strcmp(value, "slide") == 0) {
            settings->page_turn_animation = PAGE_TURN_ANIMATION_SLIDE;
        }
    }
}

/* Save to config file */
int settings_save(const settings_t *settings)
{
    FILE *f = fopen(SETTINGS_FILE_PATH, "w");
    if (!f) {
        return -1;
    }

    fprintf(f, "# E-Reader Settings\n");
    fprintf(f, "font_size=%s\n", font_size_to_string(settings->font_size));
    /* ... existing settings ... */

    /* NEW SETTING */
    const char *anim_str = "none";
    if (settings->page_turn_animation == PAGE_TURN_ANIMATION_FADE) {
        anim_str = "fade";
    } else if (settings->page_turn_animation == PAGE_TURN_ANIMATION_SLIDE) {
        anim_str = "slide";
    }
    fprintf(f, "page_turn_animation=%s\n", anim_str);

    fclose(f);
    return 0;
}
```

### Step 4: Add to Settings Menu

```c
/* In src/ereader/ui/settings_menu.c */

typedef enum {
    SETTINGS_ITEM_FONT_SIZE = 0,
    SETTINGS_ITEM_LINE_SPACING,
    SETTINGS_ITEM_MARGINS,
    SETTINGS_ITEM_DISPLAY_MODE,
    SETTINGS_ITEM_AUTO_SLEEP,
    SETTINGS_ITEM_PAGE_ANIMATION,    /* NEW */
    SETTINGS_ITEM_COUNT
} settings_item_t;

void settings_menu_draw_items(settings_menu_state_t *state, framebuffer_t *fb)
{
    /* ... existing items ... */

    /* NEW ITEM */
    int y = 60 + (SETTINGS_ITEM_PAGE_ANIMATION * 20);
    char line[64];
    const char *anim_name = page_turn_animation_to_string(
        state->settings->page_turn_animation);
    snprintf(line, sizeof(line), "  Page Animation: %s", anim_name);

    if (state->selected_item == SETTINGS_ITEM_PAGE_ANIMATION) {
        fb_draw_string(fb, 8, y, ">", 1);  /* Selection indicator */
    }
    fb_draw_string(fb, 16, y, line, 1);
}

void settings_menu_handle_select(settings_menu_state_t *state)
{
    /* ... existing handlers ... */

    /* NEW HANDLER */
    if (state->selected_item == SETTINGS_ITEM_PAGE_ANIMATION) {
        /* Cycle through animation options */
        state->settings->page_turn_animation =
            (state->settings->page_turn_animation + 1) % 3;
        state->dirty = true;  /* Mark as modified */
    }
}
```

### Step 5: Use the Setting

```c
/* In src/ereader/ui/reading_view.c */

void reading_view_turn_page(reading_view_state_t *state, int direction)
{
    /* Update page number */
    state->current_page += direction;

    /* Apply animation based on setting */
    switch (g_app_settings->page_turn_animation) {
        case PAGE_TURN_ANIMATION_NONE:
            /* Just refresh normally */
            reading_view_render(state, &g_framebuffer);
            epd_display_frame(g_framebuffer.pixels);
            break;

        case PAGE_TURN_ANIMATION_FADE:
            /* Implement fade animation */
            reading_view_fade_animation(state, direction);
            break;

        case PAGE_TURN_ANIMATION_SLIDE:
            /* Implement slide animation */
            reading_view_slide_animation(state, direction);
            break;
    }
}
```

## Extending the Display System

### Display Driver Architecture

The display driver (`src/display-test/epd_driver.c`) handles SPI communication with the e-paper display.

Key functions:
- `epd_init()` - Initialize display hardware
- `epd_display_frame()` - Send framebuffer to display
- `epd_clear()` - Clear display to white
- `epd_sleep()` / `epd_wake()` - Power management

### Adding Partial Refresh Support

Partial refresh updates only changed regions (faster, less ghosting):

```c
/* In epd_driver.h */

/**
 * Partial refresh mode (faster, accumulates ghosting)
 *
 * @param frame_buffer Full framebuffer
 * @param x X coordinate of update region
 * @param y Y coordinate
 * @param width Width of region
 * @param height Height of region
 * @return 0 on success, negative on error
 */
int epd_partial_refresh(const uint8_t *frame_buffer,
                       int x, int y, int width, int height);
```

```c
/* In epd_driver.c */

int epd_partial_refresh(const uint8_t *frame_buffer,
                       int x, int y, int width, int height)
{
    /* Send partial refresh command sequence */
    epd_send_command(0x91);  /* Partial In */

    /* Define update window */
    epd_send_command(0x90);  /* Partial Window */
    epd_send_data(x >> 8);
    epd_send_data(x & 0xFF);
    epd_send_data((x + width - 1) >> 8);
    epd_send_data((x + width - 1) & 0xFF);
    epd_send_data(y >> 8);
    epd_send_data(y & 0xFF);
    epd_send_data((y + height - 1) >> 8);
    epd_send_data((y + height - 1) & 0xFF);
    epd_send_data(0x01);  /* PT_SCAN */

    /* Send partial image data */
    epd_send_command(0x13);  /* Write RAM */

    int bytes_per_line = width / 8;
    for (int row = y; row < y + height; row++) {
        int offset = (row * EPD_WIDTH + x) / 8;
        epd_send_data_bulk(&frame_buffer[offset], bytes_per_line);
    }

    /* Trigger refresh */
    epd_send_command(0x12);  /* Display Refresh */
    epd_wait_busy();

    /* Partial Out */
    epd_send_command(0x92);

    return 0;
}
```

### Adding Grayscale Support

For displays that support it:

```c
/* 4-level grayscale (00=white, 01=light gray, 10=dark gray, 11=black) */
typedef enum {
    GRAY_WHITE = 0,
    GRAY_LIGHT = 1,
    GRAY_DARK = 2,
    GRAY_BLACK = 3
} gray_level_t;

/* 2-bit per pixel framebuffer */
typedef struct {
    uint8_t pixels[EPD_WIDTH * EPD_HEIGHT / 4];  /* 2 bits per pixel */
} framebuffer_gray_t;

void fb_set_pixel_gray(framebuffer_gray_t *fb, int x, int y, gray_level_t level)
{
    int byte_index = (y * EPD_WIDTH + x) / 4;
    int bit_offset = ((y * EPD_WIDTH + x) % 4) * 2;

    fb->pixels[byte_index] &= ~(0x03 << bit_offset);  /* Clear 2 bits */
    fb->pixels[byte_index] |= (level << bit_offset);  /* Set new value */
}
```

## Power Management

### Implementing Custom Sleep Policies

```c
/* In src/ereader/power/power_manager.c */

/**
 * Smart sleep policy: Sleep faster when inactive, slower when reading
 */
int power_manager_update_smart(power_manager_t *pm)
{
    time_t now = time(NULL);
    int idle_seconds = now - pm->last_activity;

    /* Determine timeout based on context */
    int timeout;
    if (g_app_state == STATE_READING) {
        /* User is reading - longer timeout */
        timeout = pm->timeout_seconds;
    } else if (g_app_state == STATE_MENU_LIBRARY) {
        /* User in menu - shorter timeout */
        timeout = pm->timeout_seconds / 2;
    } else {
        /* Other states - very short timeout */
        timeout = 60;  /* 1 minute */
    }

    /* Check if should sleep */
    if (idle_seconds >= timeout - SLEEP_WARNING_SECONDS) {
        if (!pm->warning_displayed) {
            power_manager_show_warning(pm, timeout - idle_seconds);
            pm->warning_displayed = true;
            pm->state = POWER_STATE_WARNING;
        }
    }

    if (idle_seconds >= timeout) {
        return 1;  /* Should sleep now */
    }

    return 0;  /* No sleep needed */
}
```

### Battery Level Warnings

```c
/* Check battery and warn user */
void power_manager_check_battery(power_manager_t *pm)
{
    battery_status_t battery;
    battery_monitor_read(&battery);

    if (battery.level == BATTERY_LEVEL_CRITICAL) {
        /* Show critical battery warning */
        ui_show_message("CRITICAL BATTERY\nDevice will shut down soon");

        /* Auto-save bookmarks */
        book_manager_save_all_bookmarks();

        /* Enter deep sleep after short delay */
        sleep(10);
        power_manager_shutdown();
    }
    else if (battery.level == BATTERY_LEVEL_LOW) {
        /* Show low battery toast notification */
        ui_show_toast("Low Battery");
    }
}
```

## Network Features

### WiFi Connection Management

```c
/* In src/ereader/network/wifi_manager.c */

/**
 * Connect to WiFi network
 *
 * @param ssid Network SSID
 * @param password Network password (WPA2)
 * @param timeout_seconds Timeout for connection attempt
 * @return 0 on success, negative on error
 */
int wifi_manager_connect(const char *ssid, const char *password, int timeout_seconds)
{
    /* Create wpa_supplicant configuration */
    FILE *f = fopen("/tmp/wpa_supplicant.conf", "w");
    if (!f) {
        return -1;
    }

    fprintf(f, "network={\n");
    fprintf(f, "    ssid=\"%s\"\n", ssid);
    fprintf(f, "    psk=\"%s\"\n", password);
    fprintf(f, "}\n");
    fclose(f);

    /* Start wpa_supplicant */
    system("killall wpa_supplicant 2>/dev/null");
    system("wpa_supplicant -B -i wlan0 -c /tmp/wpa_supplicant.conf");

    /* Wait for connection */
    int attempts = timeout_seconds * 2;  /* Check every 500ms */
    for (int i = 0; i < attempts; i++) {
        if (wifi_manager_is_connected()) {
            /* Get IP address */
            system("udhcpc -i wlan0 -n -q");
            return 0;
        }
        usleep(500000);  /* 500ms */
    }

    return -1;  /* Timeout */
}

/**
 * Check if WiFi is connected
 */
bool wifi_manager_is_connected(void)
{
    FILE *f = popen("iw dev wlan0 link", "r");
    if (!f) {
        return false;
    }

    char line[256];
    bool connected = false;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "Connected to")) {
            connected = true;
            break;
        }
    }

    pclose(f);
    return connected;
}
```

### Downloading Books

```c
/* In src/ereader/network/download_manager.c */

/**
 * Download a book from URL
 *
 * @param url Source URL
 * @param dest_path Destination file path
 * @param progress_callback Callback for progress updates (optional)
 * @return 0 on success, negative on error
 */
int download_manager_download(const char *url, const char *dest_path,
                              void (*progress_callback)(int percentage))
{
    /* Use wget or curl */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "wget -O '%s' '%s' 2>&1",
             dest_path, url);

    FILE *pipe = popen(cmd, "r");
    if (!pipe) {
        return -1;
    }

    /* Parse wget output for progress */
    char line[256];
    while (fgets(line, sizeof(line), pipe)) {
        int percentage;
        if (sscanf(line, "%d%%", &percentage) == 1) {
            if (progress_callback) {
                progress_callback(percentage);
            }
        }
    }

    int ret = pclose(pipe);
    return (ret == 0) ? 0 : -1;
}
```

## Debugging Tips and Tools

### Serial Console Debugging

```bash
# Connect serial console
screen /dev/ttyUSB0 115200

# Enable kernel messages
dmesg -n 8

# Watch application logs in real-time
tail -f /var/log/ereader.log

# Monitor system resources
top
free -m
df -h
```

### Adding Debug Logging

```c
/* Create debug logging infrastructure */

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define LOG(level, fmt, ...) \
    do { \
        if (level >= LOG_LEVEL) { \
            fprintf(stderr, "[%s] %s:%d: " fmt "\n", \
                    log_level_string(level), __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while(0)

#define DBG(fmt, ...)   LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...)  LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)  LOG(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   LOG(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/* Usage */
DBG("Opening book: %s", filename);
INFO("Book loaded: %d pages", total_pages);
WARN("Book exceeds recommended size: %ld bytes", file_size);
ERR("Failed to allocate memory for book");
```

### Memory Leak Detection

```c
/* Simple allocation tracking */

#ifdef DEBUG_MEMORY
static int g_alloc_count = 0;
static int g_free_count = 0;

#define MALLOC(size) \
    ({ \
        void *ptr = malloc(size); \
        if (ptr) g_alloc_count++; \
        fprintf(stderr, "ALLOC %p (%zu bytes) [%d allocs, %d frees]\n", \
                ptr, (size_t)(size), g_alloc_count, g_free_count); \
        ptr; \
    })

#define FREE(ptr) \
    do { \
        if (ptr) { \
            fprintf(stderr, "FREE %p [%d allocs, %d frees]\n", \
                    ptr, g_alloc_count, ++g_free_count); \
            free(ptr); \
        } \
    } while(0)
#else
#define MALLOC(size) malloc(size)
#define FREE(ptr) free(ptr)
#endif

/* Check at program exit */
void check_memory_leaks(void)
{
    if (g_alloc_count != g_free_count) {
        fprintf(stderr, "MEMORY LEAK: %d allocations, %d frees\n",
                g_alloc_count, g_free_count);
    }
}
```

### GDB Debugging

```bash
# Build with debug symbols
make menuconfig
# Enable: Build options → build packages with debugging symbols

# Rebuild
make ereader-rebuild

# On device, run with gdb
gdbserver :1234 /usr/bin/ereader

# On host
gdb-multiarch buildroot/output/target/usr/bin/ereader
(gdb) target remote 192.168.1.100:1234
(gdb) break main
(gdb) continue
```

### Profiling Performance

```c
/* Simple timing macro */
#define TIME_FUNCTION(name, code) \
    do { \
        struct timespec start, end; \
        clock_gettime(CLOCK_MONOTONIC, &start); \
        code \
        clock_gettime(CLOCK_MONOTONIC, &end); \
        long ms = (end.tv_sec - start.tv_sec) * 1000 + \
                  (end.tv_nsec - start.tv_nsec) / 1000000; \
        fprintf(stderr, "TIMING: %s took %ld ms\n", name, ms); \
    } while(0)

/* Usage */
TIME_FUNCTION("Book loading", {
    book = load_book(filename);
});

TIME_FUNCTION("Page rendering", {
    render_page(&fb, page_num);
});
```

## Performance Optimization

### Profiling Hot Paths

```bash
# Use perf (if available)
perf record /usr/bin/ereader
perf report

# Or use time
time /usr/bin/ereader
```

### Memory Optimization Strategies

1. **Use static allocation for fixed-size buffers**
2. **Pool allocations** for frequent small allocations
3. **Lazy loading** - load data only when needed
4. **Caching** - cache frequently accessed data
5. **Memory-mapped files** for large files

### Display Refresh Optimization

```c
/* Track dirty regions to minimize refreshes */
typedef struct {
    int x, y, width, height;
    bool dirty;
} dirty_region_t;

static dirty_region_t g_dirty_regions[MAX_DIRTY_REGIONS];

void mark_dirty(int x, int y, int width, int height)
{
    /* Add to dirty regions list */
    /* Merge overlapping regions */
}

void refresh_dirty_regions(void)
{
    /* Only refresh changed areas using partial refresh */
    for (int i = 0; i < MAX_DIRTY_REGIONS; i++) {
        if (g_dirty_regions[i].dirty) {
            epd_partial_refresh(framebuffer,
                               g_dirty_regions[i].x,
                               g_dirty_regions[i].y,
                               g_dirty_regions[i].width,
                               g_dirty_regions[i].height);
            g_dirty_regions[i].dirty = false;
        }
    }
}
```

## Testing Strategies

### Unit Testing Structure

```c
/* test/test_book_manager.c */

#include <assert.h>
#include "book_manager.h"

void test_book_manager_scan(void)
{
    book_list_t *list = book_manager_scan("/test/books");
    assert(list != NULL);
    assert(list->count > 0);
    book_manager_free(list);
}

void test_book_manager_open(void)
{
    book_t *book = book_manager_open("/test/books/sample.txt");
    assert(book != NULL);
    assert(book->text_length > 0);
    book_manager_close(book);
}

int main(void)
{
    test_book_manager_scan();
    test_book_manager_open();
    printf("All tests passed!\n");
    return 0;
}
```

### Integration Testing

Create test scripts in `scripts/test/`:

```bash
#!/bin/bash
# test/integration/test_book_loading.sh

echo "Testing book loading..."

# Create test book
echo "This is a test book." > /books/test.txt

# Run ereader and check output
timeout 10 /usr/bin/ereader &
PID=$!

sleep 5

# Check if process is running
if ps -p $PID > /dev/null; then
    echo "PASS: E-reader started successfully"
    kill $PID
else
    echo "FAIL: E-reader crashed"
    exit 1
fi

rm /books/test.txt
```

### Hardware Testing Checklist

- [ ] Display initializes correctly
- [ ] All buttons respond to presses
- [ ] Books load and paginate correctly
- [ ] WiFi connects and downloads work
- [ ] Battery monitoring reads correctly
- [ ] Auto-sleep activates at timeout
- [ ] Bookmarks persist across reboots
- [ ] Settings save and load correctly
- [ ] Search finds expected results
- [ ] Long reading sessions (>1 hour) work without crashes

## Best Practices Summary

1. **Always check return values** - Handle all errors gracefully
2. **Free all allocated memory** - Use goto cleanup pattern
3. **Bounds check all arrays** - Prevent buffer overflows
4. **Use const where possible** - Prevent accidental modification
5. **Comment complex logic** - Explain WHY, not WHAT
6. **Keep functions small** - One function, one responsibility
7. **Test on hardware** - Emulation is not enough
8. **Profile before optimizing** - Don't guess, measure
9. **Document your changes** - Update docs with code
10. **Ask for help** - Community is here to support you

## Additional Resources

- [[CONTRIBUTING]] - Contribution guidelines and process
- [[EREADER_DESIGN]] - Complete architecture documentation
- [[FORMAT_SUPPORT]] - Book format implementation details
- [[BUILD_PROCESS]] - Buildroot build system guide
- [Linux Device Drivers](https://lwn.net/Kernel/LDD3/) - Kernel driver development
- [Embedded Linux Primer](https://www.embedded.com/) - Embedded systems basics
- [Buildroot User Manual](https://buildroot.org/downloads/manual/manual.html)

## Questions?

If you have development questions:
- Check existing documentation in `docs/`
- Search GitHub issues and discussions
- Ask in GitHub Discussions with `[dev]` tag
- Review source code comments

Happy developing! 🚀
