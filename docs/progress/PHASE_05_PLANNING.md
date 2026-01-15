---
type: planning
title: Phase 05 Planning - Polish and Advanced Features
created: 2026-01-14
tags:
  - planning
  - phase-05
  - bookmarks
  - annotations
  - search
  - settings
  - power-management
  - user-experience
related:
  - "[[PHASE_04_LOG]]"
  - "[[EREADER_DESIGN]]"
  - "[[USER_GUIDE]]"
  - "[[FORMAT_SUPPORT]]"
---

# Phase 05 Planning: Polish and Advanced Features

This document outlines the design and implementation plan for Phase 5 of the e-reader project: adding advanced reading features including enhanced bookmarks, annotations, in-book search, a comprehensive settings system, and power management for improved battery life.

## Phase Goal

Transform the e-reader from a functional multi-format reader into a polished, feature-rich reading device with advanced navigation, customization options, and intelligent power management. Focus on user experience enhancements that make extended reading sessions more comfortable and productive.

## Prerequisites

Phase 05 assumes successful completion of Phase 04:
- ✓ Multi-format support (TXT, EPUB, PDF) working
- ✓ Format abstraction layer implemented
- ✓ Text rendering engine operational for all formats
- ✓ Book management with metadata extraction
- ✓ Menu and reader UI components complete
- ✓ Basic bookmark system (auto-save on exit)
- ✓ Buildroot system with required libraries (libzip, libxml2, poppler)

## Overview

Phase 05 enhances the reading experience with:

1. **Advanced Bookmarks & Annotations:** Multiple manual bookmarks per book, text annotations, bookmark management UI
2. **In-Book Search:** Find text within current book, navigate between search results
3. **Enhanced Settings System:** Font size adjustment, display settings, reading preferences, customizable button mappings
4. **Power Management:** Auto-sleep after inactivity, deep sleep mode, wake-on-button, battery optimization
5. **Reading Statistics:** Enhanced tracking with reading goals, streaks, and detailed progress reports
6. **User Experience Polish:** Improved feedback, loading indicators, error messages, help system

## Feature Set Breakdown

### 1. Advanced Bookmarks & Annotations

**Current State (Phase 4):**
- Single auto-saved bookmark per book (last reading position)
- Stored in `/etc/ereader/bookmarks.txt`
- Automatic resume on book open

**Enhancements for Phase 5:**

#### Multiple Bookmarks Per Book

**Feature Description:**
- Create up to 10 manual bookmarks per book
- Each bookmark has optional name/note
- Visual indicator on bookmarked pages
- Quick bookmark list access

**Implementation:**

```c
typedef struct {
    int page;                   // Page number of bookmark
    char name[64];              // Optional bookmark name
    char note[256];             // Optional annotation text
    time_t created;             // Timestamp of creation
    book_format_t format;       // Book format for validation
} bookmark_t;

typedef struct {
    char book_hash[64];         // Unique book identifier (filename hash)
    bookmark_t bookmarks[10];   // Array of bookmarks
    int count;                  // Number of active bookmarks
    int last_position_page;     // Auto-saved position (existing)
} book_bookmarks_t;
```

**Book Hash Generation:**
- Hash from: filename + file size + format
- Ensures bookmarks persist across file moves
- Detects book changes (avoid stale bookmarks)

**Storage Format:**
- JSON or CSV format in `/etc/ereader/bookmarks.json`
- Backward compatible with Phase 4 bookmarks.txt
- Migration script on first Phase 5 boot

**UI Features:**

1. **Bookmark Creation:**
   - Button: Long-press SELECT in reader
   - Prompt: "Add bookmark? [Name] [OK] [Cancel]"
   - Name input: Optional, max 20 characters (button input or default "Bookmark N")
   - Confirmation: "Bookmark added at page N"

2. **Bookmark List View:**
   - Button: Press MENU in reader → "Bookmarks"
   - Display: Scrollable list of bookmarks with page numbers and names
   - Actions: SELECT to jump, long-press SELECT to edit/delete
   - Sort: By page number (ascending)

3. **Bookmark Indicators:**
   - Status bar: "📑" symbol when on bookmarked page
   - Page counter: "[123/324] 📑" when current page has bookmark

4. **Bookmark Management:**
   - Edit: Change name/note
   - Delete: Remove bookmark
   - Delete all: Clear all bookmarks for current book

#### Text Annotations

**Feature Description:**
- Add text notes to bookmarks
- View annotation while reading
- Export annotations to text file

**Implementation:**

- Extend `bookmark_t` structure with `note` field (already in design above)
- UI for annotation entry: Simple text input (max 256 chars)
- Display annotation: Show below bookmark name in bookmark list
- Export: Generate `/books/annotations/[bookname]_notes.txt` with all annotations

**Annotation Display:**
```
┌─────────────────────────────┐
│  Bookmarks - Frankenstein   │
│                             │
│  > Page 42 - "Chapter 3"    │
│    "Important scene"        │
│                             │
│    Page 87 - "Bookmark 2"   │
│    "Remember this part"     │
│                             │
│    Page 156 - "Climax"      │
│    "Key turning point"      │
│                             │
│  SELECT: Jump  DEL: Remove  │
└─────────────────────────────┘
```

**Text Input Challenge:**
- No keyboard on device (Phase 5 constraint)
- Solution 1: Pre-defined annotation templates ("Important", "Review", "Quote", etc.)
- Solution 2: Annotations via SSH (edit JSON file remotely)
- Solution 3: Simple character picker (like T9) for short notes
- **Recommendation:** Start with templates, add char picker if time permits

#### Bookmark Synchronization (Stretch Goal)

**Feature Description:**
- Export bookmarks to external storage (USB/SD)
- Import bookmarks from another e-reader device

**Implementation:**
- Export: Copy `/etc/ereader/bookmarks.json` to `/books/bookmarks_export.json`
- Import: Read from `/books/bookmarks_import.json`, merge with existing
- Button: Settings → Bookmarks → Export/Import

**Use Case:**
- Backup bookmarks before system update
- Share bookmarks between multiple e-reader devices
- Restore bookmarks after system reset

---

### 2. In-Book Search

**Feature Description:**
- Search for text within currently open book
- Display search results with context snippets
- Navigate between results (next/previous)
- Case-insensitive search with optional case-sensitive mode

**Implementation:**

#### Search Architecture

```c
typedef struct {
    int page;               // Page where match found
    int char_offset;        // Character offset in page text
    char context[120];      // Text snippet with match (60 chars before + 60 after)
    int match_start;        // Start position of match in context
    int match_length;       // Length of matched text
} search_result_t;

typedef struct {
    search_result_t *results;   // Array of results
    int count;                  // Number of results
    int capacity;               // Allocated capacity
    int current_index;          // Currently selected result (for navigation)
    char query[64];             // Search query string
    bool case_sensitive;        // Search mode
} search_state_t;
```

#### Search Algorithm

1. **Text Search:**
   - Algorithm: Boyer-Moore or simple strstr() for small books
   - Scan entire book text (all formats already extracted to text)
   - Limit results: First 100 matches (prevent overwhelming UI)
   - Performance: <2 seconds for typical book (1-2 MB text)

2. **Context Extraction:**
   - Extract 60 chars before and 60 chars after match
   - Handle edge cases: Start/end of text, line breaks
   - Highlight match in context (use markers or brackets)

3. **Result Storage:**
   - Store results in search_state_t during search
   - Cache results (don't re-search on navigation)
   - Clear results when: Book closed, new search initiated

#### Search UI

**State: READING_SEARCH**

1. **Search Input Screen:**
   - Simple character picker or pre-defined search terms
   - Alternative: Search via configuration file (SSH-based)
   - Button: MENU in reader → "Search"

2. **Search Results Screen:**
   ```
   ┌─────────────────────────────┐
   │  Search: "monster"          │
   │  Found 15 results           │
   │                             │
   │  > Page 23: "...the monster │
   │    appeared from the..."    │
   │                             │
   │    Page 45: "...creature or │
   │    monster that could..."   │
   │                             │
   │    Page 67: "...his monster │
   │    was a reflection..."     │
   │                             │
   │  SELECT: Jump  BACK: Cancel │
   └─────────────────────────────┘
   ```

3. **In-Page Search Navigation:**
   - After jumping to result: Display highlighted match on page
   - Button: UP/DOWN for next/previous result (while in search mode)
   - Exit search mode: Press BACK (return to normal reading)

4. **Search Feedback:**
   - While searching: Display "Searching..." with progress indicator
   - No results: Display "No matches found for 'query'"
   - Results found: Display "Found N results"

#### Search Input Methods

**Option 1: Pre-defined Search Terms (Phase 5.1)**
- Common terms: "chapter", "said", "the", etc.
- User can select from list (fastest implementation)
- Limited flexibility but usable

**Option 2: SSH-Based Search (Phase 5.1)**
- User writes query to `/tmp/search_query.txt` via SSH
- Application monitors file, triggers search when modified
- Full flexibility, requires SSH access

**Option 3: Character Picker (Phase 5.2 - Stretch Goal)**
- Similar to T9 text input
- Buttons to navigate alphabet grid
- Slow but functional for short queries

**Recommendation:** Start with Option 2 (SSH-based), add Option 1 (pre-defined) as quick access, defer Option 3

---

### 3. Enhanced Settings System

**Current State (Phase 4):**
- Minimal settings infrastructure
- No persistent user preferences
- Fixed font size (8×16)
- Fixed display settings

**Enhancements for Phase 5:**

#### Settings Architecture

```c
typedef struct {
    // Display settings
    int font_size;              // 0=8x16, 1=12x24, 2=16x32 (custom sizes)
    int line_spacing;           // 0=compact, 1=normal, 2=relaxed
    bool inverted_display;      // Black text on white vs white on black
    int refresh_mode;           // 0=always full, 1=partial with auto-full, 2=always partial
    int auto_full_refresh;      // Number of partial refreshes before auto-full (5-20)

    // Reading settings
    bool show_page_numbers;     // Display page numbers in status bar
    bool show_clock;            // Display current time in status bar
    bool chapter_breaks;        // Prefer page breaks at chapter boundaries
    int sleep_timeout;          // Minutes of inactivity before auto-sleep (0=disabled, 5-60)

    // Navigation settings
    bool button_sounds;         // Enable button click sounds (if buzzer added)
    bool wrap_navigation;       // Wrap around at start/end of lists

    // Advanced settings
    bool debug_mode;            // Show debug info in status bar
    int max_bookmarks;          // Max bookmarks per book (5-20)
    bool auto_backup;           // Auto-backup bookmarks on shutdown
} settings_t;

// Settings persistence
int settings_load(settings_t *settings);
int settings_save(const settings_t *settings);
void settings_reset_to_defaults(settings_t *settings);
```

#### Settings Storage

**File Format:** INI-style configuration
**Location:** `/etc/ereader/settings.conf`

**Example settings.conf:**
```ini
[Display]
font_size=0
line_spacing=1
inverted_display=false
refresh_mode=1
auto_full_refresh=10

[Reading]
show_page_numbers=true
show_clock=true
chapter_breaks=true
sleep_timeout=15

[Navigation]
button_sounds=false
wrap_navigation=true

[Advanced]
debug_mode=false
max_bookmarks=10
auto_backup=true
```

**Parsing Library:** Mini INI parser (lightweight, ~200 lines of C)
- Alternative: Use existing library (inih - MIT licensed, ~500 lines)
- Write functions: settings_load(), settings_save()

#### Settings Menu UI

**Menu Structure:**
```
Settings
├── Display
│   ├── Font Size (8×16 / 12×24 / 16×32)
│   ├── Line Spacing (Compact / Normal / Relaxed)
│   ├── Inverted Display (On / Off)
│   ├── Refresh Mode (Full / Auto / Partial)
│   └── Auto Full Refresh (5 / 10 / 15 / 20 partials)
├── Reading
│   ├── Show Page Numbers (On / Off)
│   ├── Show Clock (On / Off)
│   ├── Chapter Break Pagination (On / Off)
│   └── Sleep Timeout (Off / 5m / 10m / 15m / 30m / 60m)
├── Navigation
│   ├── Button Sounds (On / Off)
│   └── Wrap Navigation (On / Off)
├── Bookmarks
│   ├── Max Bookmarks per Book (5 / 10 / 15 / 20)
│   ├── Export Bookmarks
│   ├── Import Bookmarks
│   └── Clear All Bookmarks
├── System
│   ├── System Information
│   ├── Storage Information
│   ├── Reset Settings to Defaults
│   └── Reboot / Shutdown
└── About
    ├── Version Information
    ├── License Information
    └── Help
```

**Settings Interaction:**
- Navigate with UP/DOWN
- SELECT to toggle or open submenu
- Multi-choice settings: Use UP/DOWN to cycle through options
- Immediate apply: Changes take effect instantly
- Auto-save: Settings saved on exit or after 5 seconds of inactivity

#### Font Size Support

**Challenge:** Current font is 8×16 bitmap font

**Implementation Options:**

**Option 1: Multiple Bitmap Fonts (Phase 5.1)**
- Create/source 12×24 and 16×32 bitmap fonts
- Load appropriate font on settings change
- Update text_renderer to handle variable font sizes
- Memory impact: ~30 KB per font

**Option 2: TrueType Font Rendering (Phase 5.2 - Stretch Goal)**
- Use FreeType library (BR2_PACKAGE_FREETYPE=y)
- Render variable font sizes (8-32 pt)
- Better quality, scalable
- Memory impact: ~500 KB (FreeType), runtime font cache ~1 MB
- Performance: Slower rendering (may need caching)

**Recommendation:** Option 1 for Phase 5.1, Option 2 for Phase 5.2 or Phase 6

**Font Sizes:**
- 8×16 (default, ~50 chars/line, ~18 lines)
- 12×24 (medium, ~33 chars/line, ~12 lines)
- 16×32 (large, ~25 chars/line, ~9 lines)

**Impact on Pagination:**
- Larger fonts → fewer pages → need re-pagination
- Re-calculate page breaks when font size changes
- Save font size with bookmark (avoid position drift)

#### Inverted Display Mode

**Feature Description:**
- Swap black/white pixels for different aesthetic
- Some users prefer white text on black background
- Useful in low-light conditions (less eye strain)

**Implementation:**
- Invert framebuffer before sending to display
- Toggle in settings: Display → Inverted Display
- Immediate visual feedback (full refresh)

**Code:**
```c
void framebuffer_invert(framebuffer_t *fb) {
    for (int i = 0; i < fb->size; i++) {
        fb->buffer[i] = ~fb->buffer[i];  // Bitwise NOT
    }
}
```

---

### 4. Power Management & Sleep Mode

**Goal:** Extend battery life from ~1-2 days to 1-2 weeks with aggressive power management

**Current Power Consumption Estimate:**
- Active reading: ~500 mW (CPU active, display refreshing)
- Idle (no input): ~300 mW (CPU idle, display static)
- **Target:** <50 mW in deep sleep

#### Auto-Sleep After Inactivity

**Feature Description:**
- Detect user inactivity (no button presses)
- After timeout (configurable: 5-60 minutes), enter sleep mode
- Display "Sleeping..." message on screen
- Wake on any button press

**Implementation:**

```c
typedef struct {
    time_t last_activity;       // Timestamp of last button press
    bool is_sleeping;           // Current sleep state
    int sleep_timeout_seconds;  // Timeout from settings (converted to seconds)
    int sleep_page_backup;      // Page number when entering sleep
} power_state_t;

// Update last activity on button press
void power_update_activity(power_state_t *power) {
    power->last_activity = time(NULL);
}

// Check if should enter sleep
bool power_should_sleep(const power_state_t *power) {
    if (power->sleep_timeout_seconds == 0) return false;  // Sleep disabled
    time_t now = time(NULL);
    int elapsed = now - power->last_activity;
    return elapsed >= power->sleep_timeout_seconds;
}
```

**Sleep Entry Sequence:**
1. Detect inactivity timeout reached
2. Save current reading position (auto-bookmark)
3. Clear display or show "Sleeping..." message
4. Put display controller in deep sleep mode
5. Enter CPU low-power mode (wait for interrupt)

**Wake Sequence:**
1. Button press generates GPIO interrupt
2. Wake CPU from low-power mode
3. Wake display controller
4. Restore reading position
5. Full display refresh
6. Resume normal operation

**Display Sleep Mode:**
- E-paper displays retain image without power (bistable)
- Put controller (UC8176) in deep sleep: Send `DEEP_SLEEP` command
- Power consumption: <1 mW in deep sleep
- Wake time: ~100-200ms

**CPU Low-Power Mode:**
- Use Linux power management: `cpufreq` governor
- Set to "powersave" mode during inactivity
- Wake on GPIO interrupt (button press)
- Alternative: Use `PM_SUSPEND_MEM` for deeper sleep (may be too aggressive)

#### Deep Sleep Mode (Advanced)

**Feature Description:**
- More aggressive power saving than auto-sleep
- Entered manually or after extended inactivity (e.g., 24 hours)
- Requires longer wake time (3-5 seconds)

**Implementation:**
- Similar to auto-sleep but with full system suspend
- Use Linux suspend-to-RAM: `/sys/power/state`
- Wake on GPIO interrupt (RTC alarm or button press)
- Restore complete application state on wake

**Power Consumption Target:**
- Deep sleep: <10 mW
- Battery life estimate: 2-4 weeks on 2000 mAh battery

**Trade-offs:**
- Longer wake time (3-5s vs <1s for auto-sleep)
- More complex state restoration
- Risk of state loss (needs robust save/restore)

**Recommendation:** Implement auto-sleep for Phase 5.1, defer deep sleep to Phase 5.2

#### Power Optimization Techniques

1. **Reduce Display Refreshes:**
   - Use partial refresh aggressively
   - Skip redundant refreshes (no content change)
   - Defer non-critical updates (clock in status bar)

2. **CPU Frequency Scaling:**
   - Use "ondemand" or "conservative" cpufreq governor
   - Reduce clock speed during reading (no heavy processing)
   - Boost to max speed only during page turns and searches

3. **Disable Unused Peripherals:**
   - WiFi: Disable by default (enable on-demand in Phase 6)
   - Bluetooth: Disable (not used in Phase 5)
   - HDMI: Disable (no HDMI on Pi Zero W, but ensure disabled in kernel)

4. **Optimize Software:**
   - Minimize background processes (no cron, no unnecessary daemons)
   - Reduce kernel wake-ups (tune timers, hrtimers)
   - Use NOHZ kernel (tickless idle)

**Expected Power Savings:**
- Current: ~500 mW average
- After optimization: ~200 mW average (reading), ~50 mW (auto-sleep)
- Battery life improvement: ~2x to 5x

---

### 5. Reading Statistics Enhancements

**Current State (Phase 4):**
- No reading statistics tracked

**Enhancements for Phase 5:**

#### Statistics Tracking

```c
typedef struct {
    // Per-book statistics
    char book_hash[64];         // Book identifier
    time_t first_opened;        // When book was first opened
    time_t last_opened;         // Most recent open
    int total_reading_seconds;  // Total time spent reading this book
    int pages_read;             // Highest page number reached
    int total_pages;            // Total pages in book
    int sessions_count;         // Number of reading sessions

    // Session statistics
    time_t session_start;       // Current session start time
    int session_start_page;     // Page at session start
    int session_pages_read;     // Pages read this session
} book_stats_t;

typedef struct {
    // Global statistics
    int total_books_opened;     // Total number of unique books opened
    int total_books_finished;   // Books read to 95%+ completion
    int total_reading_seconds;  // Lifetime reading time
    int total_pages_read;       // Lifetime pages turned
    int current_streak_days;    // Consecutive days with reading activity
    int longest_streak_days;    // Longest reading streak ever
    time_t last_read_date;      // Date of last reading activity
} global_stats_t;
```

#### Statistics Storage

**File Format:** JSON (easier for complex nested data)
**Location:** `/etc/ereader/stats.json`

**Example stats.json:**
```json
{
  "global": {
    "total_books_opened": 15,
    "total_books_finished": 3,
    "total_reading_seconds": 72000,
    "total_pages_read": 3420,
    "current_streak_days": 7,
    "longest_streak_days": 14,
    "last_read_date": 1705187200
  },
  "books": [
    {
      "book_hash": "abc123...",
      "first_opened": 1705100000,
      "last_opened": 1705186800,
      "total_reading_seconds": 7200,
      "pages_read": 156,
      "total_pages": 324,
      "sessions_count": 5
    }
  ]
}
```

**JSON Library:** cJSON (MIT licensed, lightweight, ~2000 lines)
- BR2_PACKAGE_CJSON=y (already in Buildroot)
- Functions: Parse, generate, modify JSON
- Memory overhead: ~100 KB at runtime

#### Statistics Display

**Global Statistics Screen:**

```
┌─────────────────────────────┐
│  Reading Statistics         │
│                             │
│  Books Opened: 15           │
│  Books Finished: 3          │
│                             │
│  Total Reading Time: 20h    │
│  Total Pages Read: 3,420    │
│                             │
│  Current Streak: 7 days 🔥  │
│  Longest Streak: 14 days    │
│                             │
│  Last Read: Today, 2:30 PM  │
│                             │
│  BACK: Return               │
└─────────────────────────────┘
```

**Per-Book Statistics (in Book Info screen):**

```
┌─────────────────────────────┐
│  Book Information           │
│                             │
│  Title: Frankenstein        │
│  Author: Mary Shelley       │
│  Progress: 48% (156/324)    │
│                             │
│  Time Spent: 2h 15m         │
│  Sessions: 5                │
│  Avg. Speed: 69 pages/hr    │
│  Est. Remaining: 2h 30m     │
│                             │
│  First Opened: Jan 10, 2026 │
│  Last Opened: Today, 2:30pm │
│                             │
│  SELECT: Open  BACK: Cancel │
└─────────────────────────────┘
```

**In-Reader Statistics (Quick View):**

- Button: Long-press UP in reader
- Display: Mini stats overlay on current page
- Content: Progress %, time spent, est. remaining
- Auto-dismiss after 5 seconds or button press

#### Reading Goals (Stretch Goal)

**Feature Description:**
- Set daily/weekly reading goals
- Track progress toward goals
- Visual indicators when goal achieved

**Goal Types:**
- Daily: "Read 30 minutes per day"
- Weekly: "Read 3 hours per week"
- Book completion: "Finish 1 book per month"

**Implementation:**
- Add goals to settings.conf
- Track progress in stats.json
- Display progress in statistics screen
- Notification: "Goal achieved! 🎉" when reached

**Recommendation:** Defer to Phase 5.2 (nice-to-have, not core functionality)

---

### 6. User Experience Polish

#### Loading Indicators

**Current State:** No feedback during slow operations

**Enhancements:**

1. **"Loading..." Messages:**
   - Display during: Book opening, format parsing, search
   - Location: Center of screen or status bar
   - Animation: Simple text or spinner (text-based: /-\|)

2. **Progress Bars:**
   - For long operations (>2 seconds): Search, library scan
   - ASCII progress bar: `[####------] 40%`
   - Update every 10% or 1 second

**Implementation:**
```c
// Display loading message
void ui_show_loading(const char *message) {
    framebuffer_clear(&fb);
    text_draw_centered(&fb, 150, message);  // Center of 400×300 screen
    epd_update_full();
}

// Update progress
void ui_update_progress(int percent, const char *message) {
    char progress[64];
    snprintf(progress, sizeof(progress), "%s [%3d%%]", message, percent);
    // Draw progress bar and message
}
```

#### Error Message Improvements

**Current State:** Generic errors, minimal context

**Enhancements:**

1. **Detailed Error Messages:**
   - Old: "Failed to open book"
   - New: "Failed to open book: Invalid EPUB format (missing container.xml)"

2. **Error Codes:**
   - Display error code for debugging
   - Example: "Error E042: PDF decryption not supported"

3. **Recovery Suggestions:**
   - Error: "Book file corrupted"
   - Suggestion: "Try re-copying the file from your computer"

4. **Error Logging:**
   - Log all errors to `/var/log/ereader.log`
   - Include timestamp, error code, context
   - Accessible via Settings → System → View Logs

**Error Display:**
```
┌─────────────────────────────┐
│  Error                      │
│                             │
│  Failed to open book.       │
│                             │
│  Reason: Invalid EPUB       │
│  Missing container.xml      │
│                             │
│  Suggestion: Check if file  │
│  is corrupted. Try          │
│  re-downloading or          │
│  re-copying the file.       │
│                             │
│  Error Code: E201           │
│                             │
│  BACK: Return to Library    │
└─────────────────────────────┘
```

#### Help System

**Feature Description:**
- Context-sensitive help screens
- Button mapping reference
- FAQ and troubleshooting

**Implementation:**

1. **Help Button:**
   - Location: Settings → Help
   - Or: Long-press MENU anywhere

2. **Help Topics:**
   - Button Controls (context-specific)
   - Supported File Formats
   - Adding Books
   - Using Bookmarks
   - Search Guide
   - Power Management
   - Troubleshooting

3. **Button Reference Card:**
   ```
   ┌─────────────────────────────┐
   │  Button Controls - Reading  │
   │                             │
   │  UP: Previous page          │
   │  DOWN: Next page            │
   │  SELECT: Bookmark page      │
   │    (long): View stats       │
   │  BACK: Return to library    │
   │    (long): Full refresh     │
   │  MENU: Table of contents    │
   │    (long): Settings         │
   │                             │
   │  BACK: Return               │
   └─────────────────────────────┘
   ```

4. **In-App Tooltips:**
   - Show brief help on first use of feature
   - Example: First bookmark creation → "Bookmark saved! Access via MENU → Bookmarks"
   - Disable after first few uses

---

## Architecture Changes

### Module Structure Updates

**New Modules for Phase 5:**
```
src/ereader/
├── bookmarks/              # NEW: Enhanced bookmark system
│   ├── bookmark_manager.c/h  # Bookmark CRUD operations
│   └── annotation.c/h        # Annotation handling
├── search/                 # NEW: In-book search
│   └── text_search.c/h       # Search algorithm and UI
├── settings/               # NEW: Comprehensive settings system
│   ├── settings.c/h          # Settings load/save/apply
│   └── settings_ui.c/h       # Settings menu UI
├── power/                  # NEW: Power management
│   └── power_manager.c/h     # Sleep mode, power optimization
├── stats/                  # NEW: Reading statistics
│   ├── stats_tracker.c/h     # Statistics tracking
│   └── stats_ui.c/h          # Statistics display
├── fonts/                  # NEW: Multiple font support
│   ├── font_8x16.c/h         # Small font (existing)
│   ├── font_12x24.c/h        # Medium font
│   └── font_16x32.c/h        # Large font
├── help/                   # NEW: Help system
│   └── help_ui.c/h           # Help screens
├── formats/                # EXISTING: No changes
├── rendering/              # EXISTING: Minor updates
│   └── text_renderer.c/h     # Update for variable font sizes
├── books/                  # EXISTING: Minor updates
├── ui/                     # EXISTING: Updates for new features
│   ├── menu.c/h              # Enhanced menus
│   └── reader.c/h            # Enhanced reader
└── main.c, ereader.h         # Updates for new states/features
```

### State Machine Extensions

**New States:**
```
Existing states (Phase 4):
- STARTUP
- MENU_LIBRARY
- MENU_SETTINGS
- MENU_BOOK_INFO
- READING
- READING_TOC
- EMPTY
- ERROR
- SHUTDOWN

New states (Phase 5):
- MENU_BOOKMARKS        # NEW: Bookmark management
- MENU_HELP             # NEW: Help screens
- READING_SEARCH        # NEW: Search within book
- READING_STATS         # NEW: Statistics overlay
- SLEEPING              # NEW: Auto-sleep mode
- SETTINGS_ADVANCED     # NEW: Nested settings menus
```

**State Transitions:**
```
READING ←→ MENU_BOOKMARKS
READING ←→ READING_SEARCH
READING ←→ READING_STATS
ANY_STATE → SLEEPING (on timeout)
SLEEPING → (previous state) (on wake)
MENU_SETTINGS ←→ SETTINGS_ADVANCED (nested menus)
MENU_SETTINGS ←→ MENU_HELP
```

### Memory Management Strategy

**Phase 5 Memory Budget:**
- Application base: ~2 MB
- Loaded book text: ~2 MB (unchanged)
- Font cache (3 fonts): ~90 KB
- Settings data: ~10 KB
- Statistics data: ~50 KB
- Bookmark data: ~20 KB
- Search results cache: ~100 KB
- **Total:** ~5 MB (well within 512 MB system)

**Memory Optimization:**
- Lazy load fonts (load on demand, unload unused)
- Limit search results (max 100 matches)
- Compress statistics on disk (JSON → gzip)
- Clear search cache when book closed

---

## Implementation Plan

### Phase 5 Task Breakdown

**Priority Levels:**
- P1: Core functionality, must-have
- P2: Important enhancements, should-have
- P3: Nice-to-have, polish

**Phase 5.1: Core Features (Priority 1)**

**1. Enhanced Bookmarks (P1)**
1. Design bookmark data structures (bookmark_t, book_bookmarks_t)
2. Implement bookmark_manager.c (create, read, update, delete)
3. Create bookmark storage (JSON format)
4. Migrate existing bookmarks.txt to bookmarks.json
5. Implement bookmark list UI
6. Add bookmark creation (long-press SELECT)
7. Add bookmark navigation (jump to bookmark)
8. Add bookmark deletion
9. Test with multiple books and bookmarks

**2. Settings System (P1)**
10. Implement settings.c (load, save, defaults)
11. Create settings.conf INI format
12. Implement INI parser (use inih library)
13. Add settings data structure (settings_t)
14. Create settings menu UI hierarchy
15. Implement settings application (apply changes)
16. Add Display settings (font size, line spacing, etc.)
17. Add Reading settings (page numbers, clock, etc.)
18. Add System settings (reset, info, etc.)
19. Test settings persistence and application

**3. Power Management (P1)**
20. Implement power_manager.c (activity tracking, sleep detection)
21. Add auto-sleep timeout setting
22. Implement sleep entry sequence
23. Implement wake sequence
24. Put display in deep sleep mode
25. Optimize CPU power consumption (cpufreq)
26. Test sleep/wake cycle
27. Measure power consumption (before/after)

**Phase 5.2: Enhanced Features (Priority 2)**

**4. In-Book Search (P2)**
28. Implement text_search.c (search algorithm)
29. Create search result data structures
30. Implement SSH-based search input
31. Create search results UI
32. Implement result navigation (next/prev)
33. Add search highlighting in reader
34. Test search performance and accuracy

**5. Multiple Font Sizes (P2)**
35. Source/create 12×24 bitmap font
36. Source/create 16×32 bitmap font
37. Update text_renderer.c for variable font sizes
38. Implement font switching in settings
39. Handle re-pagination on font change
40. Test rendering with all font sizes

**6. Reading Statistics (P2)**
41. Implement stats_tracker.c (tracking logic)
42. Create statistics data structures
43. Implement JSON storage (use cJSON)
44. Track reading time and pages
45. Calculate reading speed and estimates
46. Create statistics UI screens
47. Add statistics to book info screen
48. Track reading streaks
49. Test accuracy of statistics

**Phase 5.3: Polish & Stretch Goals (Priority 3)**

**7. User Experience Enhancements (P3)**
50. Implement loading indicators (messages, progress bars)
51. Improve error messages (detailed, with suggestions)
52. Add error logging to /var/log/ereader.log
53. Create help system (help_ui.c)
54. Add context-sensitive help screens
55. Implement button reference cards
56. Add first-use tooltips
57. Test UX improvements with real usage

**8. Advanced Features (P3 - Stretch Goals)**
58. Implement text annotations (note field in bookmarks)
59. Add annotation templates
60. Implement bookmark export/import
61. Add reading goals (daily, weekly)
62. Create goal progress tracking UI
63. Add inverted display mode
64. Implement character picker for text input
65. Test advanced features

**Phase 5.4: Integration & Documentation (Priority 1)**

**9. Integration & Testing (P1)**
66. Integrate all Phase 5 modules
67. Update main.c with new states
68. Update ereader.h with new data structures
69. Update Makefile with new source files
70. Comprehensive testing (all features, all formats)
71. Performance testing (memory, battery, responsiveness)
72. Regression testing (Phase 4 features still work)
73. Bug fixes and optimization

**10. Documentation (P1)**
74. Create PHASE_05_LOG.md
75. Update USER_GUIDE.md with Phase 5 features
76. Update EREADER_DESIGN.md with Phase 5 architecture
77. Document power management strategy
78. Document bookmark and search systems
79. Create troubleshooting guide
80. Update README.md with Phase 5 capabilities

---

## Technical Challenges and Solutions

### Challenge 1: Multiple Font Sizes with Bitmap Fonts

**Problem:** Creating/sourcing multiple bitmap fonts, handling re-pagination

**Solution:**
- Source from existing bitmap font libraries (GNU Unifont, Terminus, etc.)
- Convert TrueType fonts to bitmaps using FontForge
- Store fonts as C arrays (existing approach)
- Re-calculate pagination when font changes (cache per font size)

**Alternative:** Use FreeType for scalable fonts (deferred to Phase 5.2 or Phase 6)

### Challenge 2: Text Input for Search and Annotations

**Problem:** No keyboard, button-based input is slow

**Solution (Phase 5.1):**
- SSH-based input: User writes to file, app monitors
- Pre-defined search terms: Common words, quick selection
- Annotation templates: "Important", "Review", "Quote", etc.

**Solution (Phase 5.2):**
- Implement simple character picker (T9-style)
- Use UP/DOWN/LEFT/RIGHT for grid navigation
- SELECT to choose character
- Slow but functional for short texts

**Future (Phase 6):**
- External keyboard support (USB HID)
- On-screen keyboard (if touch support added)

### Challenge 3: Power Measurement and Optimization

**Problem:** Difficult to measure actual power consumption without hardware setup

**Solution:**
- Use USB power meter for measurement
- Test with typical usage patterns
- Optimize based on kernel timers and CPU usage (top, powertop)
- Compare with/without optimizations
- Target: Increase battery life by 2-5x

**Estimation:**
- Baseline: ~500 mW average → ~4-8 hours on 2000 mAh battery
- After optimization: ~200 mW → ~10-20 hours
- With auto-sleep: ~50 mW average → 40-80 hours (1.5-3 days)

### Challenge 4: JSON Parsing for Statistics and Bookmarks

**Problem:** JSON parsing adds dependency, memory overhead

**Solution:**
- Use cJSON library (lightweight, MIT licensed)
- Already in Buildroot (BR2_PACKAGE_CJSON=y)
- Memory overhead: ~100 KB
- Alternative: Use CSV or INI for simpler data (but less flexible)

**Recommendation:** Use JSON for structured data (statistics, bookmarks), INI for flat data (settings)

### Challenge 5: Bookmark Migration from Phase 4

**Problem:** Existing bookmarks.txt format incompatible with new bookmarks.json

**Solution:**
- Implement migration script on first Phase 5 boot
- Read bookmarks.txt (format: filename,page)
- Convert to bookmarks.json with default values
- Backup bookmarks.txt to bookmarks.txt.bak
- Log migration in /var/log/ereader.log

**Migration Code:**
```c
int migrate_bookmarks_phase4_to_phase5(void) {
    // Read old bookmarks.txt
    // Parse each line: filename,page
    // Create bookmark_t entry for each
    // Save to new bookmarks.json
    // Rename old file to .bak
}
```

### Challenge 6: State Preservation Across Sleep/Wake

**Problem:** Need to save and restore reader state for sleep mode

**Solution:**
- Save minimal state on sleep entry:
  - Current book filename/hash
  - Current page number
  - Reading mode (normal, TOC, search, etc.)
- Restore on wake:
  - Reopen book if needed
  - Navigate to saved page
  - Restore previous mode
- Use temporary state file: `/tmp/ereader_sleep_state.json`

**State Data:**
```json
{
  "book_hash": "abc123...",
  "page": 156,
  "mode": "READING",
  "timestamp": 1705187200
}
```

---

## Success Criteria

Phase 05 is successful when:

✓ **Enhanced Bookmarks:**
- Create multiple bookmarks per book (up to 10)
- View bookmark list with page numbers and names
- Jump to any bookmark from list
- Delete individual bookmarks
- All bookmarks persist across reboots

✓ **In-Book Search:**
- Search for text within current book (SSH-based input minimum)
- Display search results with context
- Navigate to search results
- Case-insensitive search working
- Search completes in <3 seconds for typical book

✓ **Settings System:**
- Comprehensive settings menu with all planned options
- Settings persist across reboots
- Settings changes apply immediately
- Reset to defaults working
- At least 2 font sizes available (8×16 + one larger)

✓ **Power Management:**
- Auto-sleep after configurable timeout (5-60 minutes)
- Wake on button press
- Sleep message displayed before sleep
- Display enters low-power mode in sleep
- Measurable power savings (2x improvement minimum)

✓ **Reading Statistics:**
- Track reading time per book
- Track pages read and progress percentage
- Display statistics in book info screen
- Calculate reading speed and time estimates
- Track reading streaks (consecutive days)
- All statistics persist across reboots

✓ **User Experience:**
- Loading indicators for slow operations
- Improved error messages with suggestions
- Help system accessible from menus
- Button reference cards available
- No regressions from Phase 4

✓ **Performance:**
- Memory usage: <50 MB total
- Boot time: <40 seconds (no degradation)
- Settings apply instantly (<500ms)
- Search completes in <3 seconds
- No impact on reading responsiveness

✓ **Documentation:**
- USER_GUIDE.md updated with all Phase 5 features
- EREADER_DESIGN.md updated with architecture changes
- PHASE_05_LOG.md documents implementation and results
- Help system provides adequate in-app documentation

---

## Testing Plan

### Unit Testing

**Bookmark Manager:**
- Create bookmark: Add bookmark at specific page
- List bookmarks: Retrieve all bookmarks for book
- Delete bookmark: Remove specific bookmark
- Update bookmark: Change name/note
- Bookmark limits: Enforce max 10 bookmarks per book
- Duplicate handling: Allow multiple bookmarks on same page

**Search System:**
- Case-insensitive search: Find matches regardless of case
- Result extraction: Extract context around match
- Result limits: Limit to 100 results
- Edge cases: Search at start/end of book
- No matches: Handle query with no results
- Special characters: Handle quotes, apostrophes in search

**Settings System:**
- Load settings: Read from settings.conf
- Save settings: Write to settings.conf
- Apply settings: Changes take effect immediately
- Defaults: Reset to default values
- Invalid values: Handle corrupted settings file gracefully

**Power Manager:**
- Activity tracking: Update timestamp on button press
- Sleep detection: Trigger sleep after timeout
- Wake detection: Wake on button press
- State preservation: Save and restore reader state

**Statistics Tracker:**
- Time tracking: Accurate reading time measurement
- Page tracking: Track highest page reached
- Streak calculation: Consecutive days with reading
- Speed calculation: Pages per hour calculation
- Persistence: Statistics saved and loaded correctly

### Integration Testing

**Complete User Flows:**

1. **Bookmark Workflow:**
   - Boot → Open book → Read to page 50 → Create bookmark "Important"
   - Read to page 100 → Create bookmark "Review"
   - Press MENU → Bookmarks → See 2 bookmarks
   - Select first bookmark → Jump to page 50
   - Return to library → Reopen book → Bookmarks still present

2. **Search Workflow:**
   - Boot → Open book → Press MENU → Search
   - Enter query via SSH (/tmp/search_query.txt)
   - See search results with context
   - Select result → Jump to page
   - Navigate next result → Jump to next match
   - Exit search → Return to normal reading

3. **Settings Workflow:**
   - Boot → Settings → Display → Font Size → Change to 12×24
   - Return to library → Open book → Verify larger font
   - Return to settings → Display → Inverted Display → Enable
   - Return to book → Verify inverted colors
   - Settings → Reset to Defaults → Verify reset

4. **Power Management Workflow:**
   - Boot → Open book → Read for 1 minute
   - Wait for sleep timeout (set to 2 minutes for testing)
   - Verify "Sleeping..." message appears
   - Press any button → Verify wake
   - Verify returns to same page

5. **Statistics Workflow:**
   - Boot → Open new book → Read 20 pages
   - Long-press UP → See mini stats overlay
   - Return to library → Long-press SELECT on book
   - See book info with statistics (time, progress, etc.)
   - Settings → Statistics → See global stats
   - Verify reading streak incremented

### Performance Testing

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Bookmark creation | <500ms | Stopwatch from button press to confirmation |
| Search (1 MB book) | <2 seconds | Time from query entry to results display |
| Font size change | <2 seconds | Time from selection to re-rendered page |
| Settings save | <100ms | Code timing (time() before/after) |
| Sleep entry | <2 seconds | Time from timeout to sleep message |
| Wake from sleep | <1 second | Time from button press to display update |
| Memory usage (all features) | <50 MB | free command during active use |
| Statistics calculation | <100ms | Code timing for speed/remaining calc |

### Regression Testing

**Phase 4 Features (Must Still Work):**
- Multi-format support (TXT, EPUB, PDF)
- Format detection and loading
- Text rendering and pagination
- Page navigation (UP/DOWN)
- Library browsing
- Basic auto-bookmark (last position)
- Format metadata display

**Phase 3 Features (Must Still Work):**
- TXT file reading
- Word wrapping
- Page breaks
- Status bar display
- Menu navigation
- Book scanning

### Power Consumption Testing

**Measurement Setup:**
- USB power meter between power supply and Pi Zero W
- Measure current draw in various states
- Calculate battery life estimates

**Test Cases:**

| State | Target Current | Measurement |
|-------|----------------|-------------|
| Active reading (no refresh) | <200 mA | Measure with book open, no input |
| Page turn (full refresh) | <300 mA peak | Measure during display update |
| Page turn (partial refresh) | <250 mA peak | Measure during partial update |
| Auto-sleep mode | <10 mA | Measure after sleep timeout |
| Deep sleep mode (stretch) | <5 mA | Measure in deep sleep |

**Battery Life Calculation:**
- Battery capacity: 2000 mAh (typical for portable power bank)
- Target: 20+ hours active reading, 1+ week with sleep mode

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Bitmap fonts hard to source | Medium | Medium | Use existing GNU Unifont or generate from TrueType |
| Power optimization ineffective | Medium | Medium | Test early, adjust strategy, defer deep sleep if needed |
| JSON library too large | Low | Low | cJSON is lightweight (~50 KB), acceptable overhead |
| Text input too cumbersome | High | Medium | Start with SSH-based, add templates, defer char picker |
| Statistics calculation overhead | Low | Medium | Optimize algorithm, update only on page change |
| Memory exhaustion (multiple fonts) | Low | High | Lazy load fonts, unload unused, limit cache |
| Sleep state corruption | Low | High | Robust save/restore, validate state on wake |
| Settings file corruption | Medium | Medium | Validate on load, use defaults if invalid, backup |
| Search too slow on large books | Medium | Medium | Optimize algorithm (Boyer-Moore), limit results, cache |
| User confusion with new features | Medium | Low | Comprehensive help system, tooltips, good UX design |

---

## Future Enhancements (Post-Phase 5)

### Phase 6: Network and Connectivity
- WiFi configuration UI (WPA supplicant control)
- Web-based book management interface
- OPDS catalog support (Calibre integration)
- Download books from Project Gutenberg
- Cloud bookmark sync
- Firmware updates over network

### Phase 7: Advanced Reading Features
- TrueType font rendering (FreeType)
- CSS rendering for EPUB (subset support)
- Image display in books (grayscale dithering)
- Annotations with highlighting
- Dictionary lookup (word definitions)
- Translation support (multi-language)

### Phase 8: Accessibility and Input
- External keyboard support (USB HID)
- Text-to-speech (espeak, festival)
- Touch screen support (if hardware upgraded)
- Audio book support (MP3 playback)
- Screen reader mode
- High contrast themes

### Phase 9: Advanced Formats
- MOBI format support
- AZW3 (Kindle) support with DRM removal
- Comic book formats (CBZ, CBR)
- Markdown rendering
- HTML e-books
- RTF and DOC conversion

### Phase 10: Hardware Enhancements
- Battery level indicator (with ADC or I2C fuel gauge)
- Real-time clock (RTC module for accurate timekeeping)
- Haptic feedback (vibration motor for button presses)
- Audio feedback (piezo buzzer for clicks)
- RGB status LED (for notifications)
- Larger display option (6" or 7.5" e-paper)

---

## Related Documentation

- `[[PHASE_04_LOG]]` - Phase 4 completion summary (prerequisite)
- `[[EREADER_DESIGN]]` - Application architecture (will be updated)
- `[[USER_GUIDE]]` - User documentation (will be updated)
- `[[FORMAT_SUPPORT]]` - Format abstraction layer
- `[[BUILD_PROCESS]]` - Buildroot build instructions
- `[[CUSTOM_PACKAGES]]` - Adding libraries to Buildroot

---

## References

### Power Management
- [Raspberry Pi Power Management](https://www.raspberrypi.com/documentation/computers/configuration.html#power-management)
- [Linux PM Subsystem](https://www.kernel.org/doc/html/latest/admin-guide/pm/)
- [E-Paper Display Power Consumption](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module#Power_Consumption)

### Font Rendering
- [GNU Unifont](https://unifoundry.com/unifont/) - Bitmap Unicode font
- [Terminus Font](https://terminus-font.sourceforge.net/) - Clean bitmap font
- [FreeType Library](https://www.freetype.org/) - TrueType font rendering

### Libraries
- [cJSON Documentation](https://github.com/DaveGamble/cJSON) - JSON parsing library
- [inih Documentation](https://github.com/benhoyt/inih) - INI file parser
- [Boyer-Moore Algorithm](https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm) - Fast string search

### User Experience
- [E-Reader UX Best Practices](https://www.nngroup.com/articles/e-readers/)
- [Minimalist UI Design](https://www.interaction-design.org/literature/article/minimalism-in-interactive-design)

---

**Document Version**: 1.0
**Created**: 2026-01-14
**Status**: Planning - Ready for Phase 5 development
**Estimated Effort**: ~100-120 hours total
- Phase 5.1 (Core): ~50 hours
- Phase 5.2 (Enhanced): ~40 hours
- Phase 5.3 (Polish): ~20 hours
- Integration & Documentation: ~10 hours

**Recommendation**: Proceed with phased approach
- 5.1: Bookmarks + Settings + Power Management (must-have features)
- 5.2: Search + Fonts + Statistics (important enhancements)
- 5.3: UX Polish + Stretch Goals (nice-to-have features)
