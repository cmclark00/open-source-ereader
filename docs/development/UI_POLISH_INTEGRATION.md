---
type: guide
title: UI Polish Components Integration Guide
created: 2026-01-15
tags:
  - ui
  - integration
  - polish
  - phase-05
related:
  - "[[Phase-05-Polish-and-Advanced-Features]]"
---

# UI Polish Components Integration Guide

This guide explains how to integrate the new UI polish components into the e-reader application. These components provide loading indicators, confirmation dialogs, decorative borders, and other visual feedback elements.

## Overview

The UI polish enhancement adds three main modules:

1. **ui_components.c/h** - Reusable UI components (dialogs, progress bars, borders, etc.)
2. **loading_screen.c/h** - Loading screen for slow operations
3. Enhanced visual feedback throughout the application

## Module Files Created

```
src/ereader/ui/
├── ui_components.h        - Reusable UI component definitions
├── ui_components.c        - Reusable UI component implementations
├── loading_screen.h       - Loading screen definitions
└── loading_screen.c       - Loading screen implementation
```

## Build System Changes

The Makefile has been updated to include the new modules:

```makefile
SRC_UI := ui/menu.c ui/reader.c ui/search_ui.c ui/ui_components.c ui/loading_screen.c
```

Dependencies added:
```makefile
ui/ui_components.o: ui/ui_components.h rendering/framebuffer.h rendering/text_renderer.h
ui/loading_screen.o: ui/loading_screen.h ui/ui_components.h rendering/framebuffer.h
```

## Component Catalog

### 1. Loading Indicators

#### Spinner
```c
#include "ui/ui_components.h"

// Render animated spinner at center of screen
uint32_t frame = 0;
while (loading) {
    ui_render_spinner(fb, 200, 150, frame);
    frame++;
    // Refresh display
}
```

#### Loading Dots
```c
// Render animated "..." after text
ui_render_loading_dots(fb, x, y, frame);
```

#### Full Loading Screen
```c
#include "ui/loading_screen.h"

// Initialize
loading_screen_t screen;
loading_screen_init(&screen);

// Show loading screen
loading_screen_start(&screen, "Opening book", "my_book.pdf", false);

// Update animation (call in loop)
loading_screen_update_animation(&screen);

// Render
loading_screen_render(&screen, fb);

// Complete
loading_screen_complete(&screen);
```

### 2. Progress Bars

```c
#include "ui/ui_components.h"

// Initialize progress bar
progress_bar_t progress;
progress_bar_init(&progress, 60, 140, 280, true); // x, y, width, show_percentage

// Update progress
for (int i = 0; i <= 100; i++) {
    progress_bar_set_value(&progress, i, 100);
    progress_bar_render(&progress, fb);
    // Refresh display
}
```

### 3. Confirmation Dialogs

```c
#include "ui/ui_components.h"

// Initialize dialog
confirmation_dialog_t dialog;
confirmation_dialog_init(&dialog,
                         "Exit Book?",
                         "Your progress has been saved.\nAre you sure you want to exit?",
                         "Yes",
                         "No");

// Show dialog
confirmation_dialog_show(&dialog);

// Handle button events in main loop
dialog_result_t result = confirmation_dialog_handle_event(&dialog, &event);
if (result == DIALOG_RESULT_YES) {
    // User confirmed
} else if (result == DIALOG_RESULT_NO) {
    // User cancelled
}

// Render dialog
confirmation_dialog_render(&dialog, fb);
```

### 4. Message Boxes

```c
#include "ui/ui_components.h"

// Render a simple message box
message_box_render(fb,
                   "Information",
                   "Bookmark saved successfully!",
                   BORDER_DECORATIVE);
```

### 5. Decorative Borders

```c
#include "ui/ui_components.h"

// Draw border around a region
ui_draw_border(fb, x, y, width, height, BORDER_SINGLE);    // Single line
ui_draw_border(fb, x, y, width, height, BORDER_DOUBLE);    // Double line
ui_draw_border(fb, x, y, width, height, BORDER_DECORATIVE); // With corners

// Draw corner ornaments only
ui_draw_corner_ornaments(fb, x, y, width, height);
```

### 6. Headers and Footers

```c
#include "ui/ui_components.h"

// Render consistent header
ui_render_header(fb, "My Screen Title", "[1/5]", 0); // line_number = 0

// Render consistent footer
ui_render_footer(fb, "UP/DOWN:Navigate  SELECT:Choose", 17); // line_number = 17

// Render separator line
ui_render_separator(fb, y, true); // decorative = true
```

### 7. Toast Notifications

```c
#include "ui/ui_components.h"

// Show brief notification at bottom of screen
ui_render_toast(fb, "Bookmark saved!");
```

## Integration Examples

### Example 1: Add Loading Screen to Book Opening

**File:** `src/ereader/main.c` or wherever book loading happens

```c
#include "ui/loading_screen.h"

// Add to app_context_t:
typedef struct {
    // ... existing fields ...
    loading_screen_t *loading_screen;
} app_context_t;

// In app_init():
ctx->loading_screen = (loading_screen_t*)malloc(sizeof(loading_screen_t));
loading_screen_init(ctx->loading_screen);

// When opening a book:
void app_open_book(app_context_t *ctx, const char *filename) {
    // Show loading screen
    loading_screen_show_opening_book(ctx->loading_screen, filename);
    loading_screen_render(ctx->loading_screen, ctx->framebuffer);
    app_refresh_display(ctx);

    // Load book (slow operation)
    book_t *book = book_load(filename);

    // Complete loading
    loading_screen_complete(ctx->loading_screen);
    loading_screen_reset(ctx->loading_screen);
}

// In app_cleanup():
if (ctx->loading_screen) {
    free(ctx->loading_screen);
}
```

### Example 2: Add Exit Confirmation Dialog to Reader

**File:** `src/ereader/ui/reader.c`

```c
#include "ui_components.h"

// Add to reader_state_t:
typedef struct {
    // ... existing fields ...
    confirmation_dialog_t *exit_dialog;
    bool exit_dialog_active;
} reader_state_t;

// In reader_create():
reader->exit_dialog = (confirmation_dialog_t*)malloc(sizeof(confirmation_dialog_t));
confirmation_dialog_init(reader->exit_dialog,
                         "Exit Book?",
                         "Progress saved.\nReturn to library?",
                         "Yes",
                         "No");
reader->exit_dialog_active = false;

// In reader_handle_event():
if (event->button == BUTTON_BACK && !reader->exit_dialog_active) {
    // Show confirmation dialog instead of immediately exiting
    confirmation_dialog_show(reader->exit_dialog);
    reader->exit_dialog_active = true;
    return READER_ACTION_REDRAW;
}

if (reader->exit_dialog_active) {
    dialog_result_t result = confirmation_dialog_handle_event(reader->exit_dialog, event);
    if (result == DIALOG_RESULT_YES) {
        reader->exit_dialog_active = false;
        return READER_ACTION_EXIT;
    } else if (result == DIALOG_RESULT_NO) {
        reader->exit_dialog_active = false;
        return READER_ACTION_REDRAW;
    }
}

// In reader_render():
// ... normal rendering ...

// Overlay dialog if active
if (reader->exit_dialog_active) {
    confirmation_dialog_render(reader->exit_dialog, fb);
}
```

### Example 3: Add Progress Bar to Library Scanning

**File:** `src/ereader/books/book_manager.c`

```c
#include "ui/ui_components.h"

// During book scanning:
int book_list_scan(book_list_t *list, const char *directory,
                   framebuffer_t *fb) {
    // Count total files first
    int total_files = count_files(directory);

    // Create progress bar
    progress_bar_t progress;
    progress_bar_init(&progress, 60, 140, 280, true);

    int processed = 0;
    // Scan files
    for (each file) {
        // Process file
        process_file(file);

        // Update progress
        processed++;
        progress_bar_set_value(&progress, processed, total_files);

        // Render progress (optional - only every 5 files to reduce updates)
        if (processed % 5 == 0 || processed == total_files) {
            fb_clear(fb, COLOR_WHITE);
            text_render_string(fb, 100, 100, "Scanning library...", COLOR_BLACK);
            progress_bar_render(&progress, fb);
            // Refresh display
        }
    }
}
```

### Example 4: Enhance Menu with Decorative Borders

**File:** `src/ereader/ui/menu.c`

```c
#include "ui_components.h"

// In menu_render_status_bar():
int menu_render_status_bar(menu_state_t *menu, framebuffer_t *fb) {
    // ... existing rendering ...

    // Add decorative separator with ornaments
    int sep_y = MARGIN_TOP + (MENU_SEPARATOR_1_LINE * LINE_HEIGHT) + (FONT_HEIGHT / 2);
    ui_render_separator(fb, sep_y, true); // decorative = true

    return MENU_SUCCESS;
}
```

### Example 5: Add Toast Notification for Bookmark Save

**File:** `src/ereader/ui/reader.c`

```c
#include "ui_components.h"

// Add to reader_state_t:
typedef struct {
    // ... existing fields ...
    bool show_toast;
    char toast_message[64];
    uint32_t toast_timer;
} reader_state_t;

// When saving bookmark manually:
int reader_save_bookmark(reader_state_t *reader) {
    // ... existing save logic ...

    // Show toast notification
    reader->show_toast = true;
    strncpy(reader->toast_message, "Bookmark saved!", sizeof(reader->toast_message));
    reader->toast_timer = 60; // Show for ~2 seconds (30 FPS)

    return READER_SUCCESS;
}

// In reader_render():
// ... normal rendering ...

// Show toast if active
if (reader->show_toast && reader->toast_timer > 0) {
    ui_render_toast(fb, reader->toast_message);
    reader->toast_timer--;
    if (reader->toast_timer == 0) {
        reader->show_toast = false;
    }
}
```

## Best Practices

### 1. Loading Screens

- **Use for operations > 500ms**: Opening large files, scanning directories
- **Update animation regularly**: Call `loading_screen_update_animation()` in loop
- **Show progress when known**: Use `show_progress=true` for deterministic operations
- **Keep messages brief**: "Opening book", not "Opening book, please wait..."

### 2. Confirmation Dialogs

- **Use for destructive actions**: Exiting, deleting, resetting
- **Keep messages concise**: 1-2 lines maximum
- **Use clear button labels**: "Yes"/"No", "OK"/"Cancel", not just "Confirm"
- **Block other input**: Check `confirmation_dialog_is_active()` before processing other events

### 3. Progress Bars

- **Show percentage**: Set `show_percentage=true` for user clarity
- **Update wisely**: Don't refresh display on every increment (e-paper is slow)
- **Use realistic values**: Set `maximum` to actual item count, not 100

### 4. Borders and Decorations

- **Use sparingly**: Decorative borders for important dialogs only
- **Match style**: Use BORDER_SINGLE for most UI, BORDER_DECORATIVE for dialogs
- **Consider e-paper**: Avoid complex patterns that may not render well

### 5. Toast Notifications

- **Brief and transient**: 2-3 seconds maximum
- **Non-blocking**: Should not require user interaction
- **Use for feedback**: "Saved", "Copied", "Error", not for critical messages

## Performance Considerations

### E-Paper Display Refresh

- **Full refresh vs partial**: Use full refresh for dialogs, partial for progress updates
- **Minimize updates**: Batch rendering before refresh
- **Loading screens**: Limit animation updates to avoid ghosting (every 500ms is enough)

### Memory Usage

- **Stack allocation**: Small components (progress_bar_t, loading_indicator_t)
- **Heap allocation**: Dialogs and screens that persist (confirmation_dialog_t)
- **Free resources**: Always call cleanup functions

### Example Optimal Rendering:

```c
// GOOD: Batch rendering
fb_clear(fb, COLOR_WHITE);
ui_render_header(fb, "Settings", NULL, 0);
ui_render_separator(fb, y1, false);
// ... render content ...
ui_render_footer(fb, "SELECT:Save  BACK:Cancel", 17);
app_refresh_display(ctx); // Single refresh

// BAD: Multiple refreshes
ui_render_header(fb, "Settings", NULL, 0);
app_refresh_display(ctx); // Refresh
ui_render_separator(fb, y1, false);
app_refresh_display(ctx); // Refresh again
// ... very slow on e-paper
```

## Testing Checklist

- [ ] Loading screen appears during book opening
- [ ] Progress bar updates smoothly during scanning
- [ ] Confirmation dialogs block other input correctly
- [ ] Toast notifications disappear after timeout
- [ ] Borders render cleanly on e-paper display
- [ ] Spinner animation is visible (not too fast)
- [ ] Headers and footers align properly
- [ ] Message boxes center correctly on screen

## Troubleshooting

### Issue: Dialog text is cut off
**Solution:** Increase DIALOG_WIDTH in ui_components.h or use text wrapping

### Issue: Loading spinner doesn't animate
**Solution:** Ensure `loading_screen_update_animation()` is called in loop

### Issue: Progress bar doesn't show percentage
**Solution:** Set `show_percentage=true` in `progress_bar_init()`

### Issue: Toast notification stays too long
**Solution:** Reduce toast_timer value (30 = 1 second at 30 FPS)

### Issue: Borders look jagged on e-paper
**Solution:** Use BORDER_SINGLE instead of complex patterns

## Future Enhancements

Potential additions for Phase 6 or later:

1. **Animated transitions**: Fade effects between screens
2. **Custom icons**: Battery, WiFi, bookmark icons
3. **Color support**: If upgrading to color e-paper display
4. **Touch support**: If adding touchscreen overlay
5. **Themes**: Light/dark modes with different border styles

## Related Documentation

- `src/ereader/ui/ui_components.h` - Complete API reference
- `src/ereader/ui/loading_screen.h` - Loading screen API
- `docs/architecture/EREADER_DESIGN.md` - Overall architecture
- `Phase-05-Polish-and-Advanced-Features.md` - Phase 05 task list

## Summary

The UI polish components provide:

✅ Professional loading indicators for slow operations
✅ Confirmation dialogs for user safety
✅ Progress bars for transparent feedback
✅ Decorative borders for visual appeal
✅ Consistent headers and footers
✅ Toast notifications for quick feedback

Integration is straightforward and can be done incrementally without breaking existing functionality.
