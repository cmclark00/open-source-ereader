---
type: testing
title: E-Reader Comprehensive Test Plan
created: 2026-01-16
tags:
  - testing
  - phase-05
  - verification
  - quality-assurance
related:
  - "[[BUTTON_TESTING]]"
  - "[[FORMAT_TESTING]]"
  - "[[PERFORMANCE]]"
  - "[[TEST_RESULTS]]"
---

# E-Reader Comprehensive Test Plan

This document provides a complete test plan for the e-reader application, covering all features from Phases 1-5. Use this plan to verify functionality before releases, after major changes, or when deploying to new hardware.

## Test Overview

### Scope
This test plan covers:
- Hardware functionality (display, buttons, power)
- File format support (TXT, EPUB, PDF)
- Core reading features (navigation, pagination, rendering)
- Phase 5 features (bookmarks, settings, search, power management, battery monitoring)
- UI polish and visual feedback
- Error handling and edge cases
- Performance and memory usage

### Test Environment
- **Hardware:** Raspberry Pi Zero W with 4.2" Waveshare E-Ink Display
- **Software:** Custom Buildroot Linux with ereader application
- **Test Data:** Sample books in `/books/` directory

### Test Categories
1. **Smoke Tests** - Quick verification that basic functionality works
2. **Feature Tests** - Detailed testing of specific features
3. **Integration Tests** - Testing interactions between components
4. **Edge Case Tests** - Boundary conditions and error handling
5. **Performance Tests** - Speed, memory, and battery life
6. **Usability Tests** - Can a new user operate the device?

---

## 1. Hardware Tests

### 1.1 Display Tests

#### 1.1.1 Display Initialization
**Objective:** Verify e-paper display initializes correctly at boot

**Prerequisites:** Device powered off

**Steps:**
1. Power on the device
2. Observe display during boot sequence
3. Wait for application to load

**Expected Results:**
- [ ] Display clears to white background during initialization
- [ ] No persistent ghost images from previous session
- [ ] Initialization completes within 5 seconds
- [ ] Display shows library menu or empty library message

**Pass/Fail Criteria:**
- PASS: Display initializes cleanly with no artifacts
- FAIL: Display shows corruption, doesn't clear, or hangs

---

#### 1.1.2 Display Refresh Quality
**Objective:** Verify display refresh produces clean, readable output

**Steps:**
1. Boot to library menu
2. Navigate through several books (UP/DOWN buttons)
3. Open a book
4. Navigate through several pages
5. Observe text clarity and contrast

**Expected Results:**
- [ ] Text is sharp and readable at normal viewing distance
- [ ] No ghosting from previous content
- [ ] Black text on white background with good contrast
- [ ] No missing pixels or display artifacts
- [ ] Full display refresh occurs when needed

**Pass/Fail Criteria:**
- PASS: All text is readable without strain
- FAIL: Significant ghosting, poor contrast, or missing content

---

#### 1.1.3 Display Power Down (Sleep Mode)
**Objective:** Verify display can be powered down and restored

**Prerequisites:** Power management feature implemented

**Steps:**
1. Boot device and wait for sleep timeout (or trigger sleep manually)
2. Observe display when entering sleep mode
3. Press any button to wake device
4. Verify display restores to previous state

**Expected Results:**
- [ ] Display clears or shows sleep message before power down
- [ ] Display powers down completely (no partial refresh)
- [ ] Wake-up restores display without corruption
- [ ] Previous application state is maintained

**Pass/Fail Criteria:**
- PASS: Sleep and wake cycle works without data loss
- FAIL: Display corruption, state loss, or wake failure

---

### 1.2 Button Tests

**Note:** Detailed button testing is covered in [[BUTTON_TESTING]]. This section provides a quick verification checklist.

#### 1.2.1 Individual Button Function
**Objective:** Verify each button works independently

**Steps:**
1. Boot to library menu
2. Test each button individually:
   - **UP:** Should move selection up
   - **DOWN:** Should move selection down
   - **SELECT:** Should open selected book
   - **BACK:** Should return to menu (when in reading view)
   - **MENU:** Should open menu/settings (if implemented)

**Expected Results:**
- [ ] UP button responds correctly
- [ ] DOWN button responds correctly
- [ ] SELECT button responds correctly
- [ ] BACK button responds correctly
- [ ] MENU button responds correctly
- [ ] No button presses are missed
- [ ] No duplicate/spurious events

**Pass/Fail Criteria:**
- PASS: All buttons respond as expected
- FAIL: Any button doesn't respond or produces wrong action

---

#### 1.2.2 Simultaneous Button Presses
**Objective:** Verify buttons don't interfere with each other

**Steps:**
1. Press multiple buttons simultaneously:
   - UP + DOWN
   - SELECT + BACK
   - All 5 buttons together

**Expected Results:**
- [ ] System handles simultaneous presses gracefully
- [ ] No crashes or unexpected behavior
- [ ] Last button pressed takes precedence (or both actions occur)

**Pass/Fail Criteria:**
- PASS: System remains stable with simultaneous presses
- FAIL: Crash, hang, or corrupted state

---

#### 1.2.3 Rapid Button Presses
**Objective:** Verify button debouncing works correctly

**Steps:**
1. Rapidly press SELECT button 20 times in quick succession
2. Observe application behavior

**Expected Results:**
- [ ] Each press registers once (no double-triggers)
- [ ] No missed presses
- [ ] System remains responsive

**Pass/Fail Criteria:**
- PASS: All presses register correctly with no duplicates
- FAIL: Missed presses or duplicate actions

---

### 1.3 Power and Battery Tests

#### 1.3.1 Power Supply Verification
**Objective:** Verify device operates correctly on 5V power

**Steps:**
1. Connect 5V 2A power supply to micro-USB port
2. Boot device and verify normal operation
3. Perform reading tasks for 10 minutes

**Expected Results:**
- [ ] Device boots successfully
- [ ] No power-related shutdowns or brownouts
- [ ] Display updates work consistently
- [ ] System remains stable during extended use

**Pass/Fail Criteria:**
- PASS: Device operates normally on external power
- FAIL: Instability, shutdowns, or display issues

---

#### 1.3.2 Battery Monitoring (If Hardware Present)
**Objective:** Verify battery level detection and display

**Prerequisites:** Battery HAT or ADC battery monitoring hardware installed

**Steps:**
1. Boot device with battery connected
2. Check for battery indicator in status bar (if implemented)
3. Check battery level via logs: `dmesg | grep battery`
4. Test with different battery levels (100%, 50%, 20%)

**Expected Results:**
- [ ] Battery monitoring initializes correctly
- [ ] Battery percentage/voltage reported accurately
- [ ] Battery level updates periodically
- [ ] Low battery warning appears when appropriate

**Pass/Fail Criteria:**
- PASS: Battery monitoring works and reports accurate levels
- FAIL: Monitoring fails, reports wrong values, or crashes
- N/A: No battery hardware present

---

#### 1.3.3 Auto Sleep Functionality
**Objective:** Verify auto-sleep triggers after inactivity

**Prerequisites:** Power management feature implemented

**Steps:**
1. Boot device to library menu
2. Leave device idle (no button presses) for configured timeout period
3. Observe sleep warning (if implemented)
4. Verify device enters sleep mode
5. Press any button to wake
6. Verify system resumes to previous state

**Expected Results:**
- [ ] Sleep warning appears 30 seconds before timeout (if implemented)
- [ ] Device enters sleep after configured timeout (e.g., 5/10/15 minutes)
- [ ] Display powers down in sleep mode
- [ ] Any button press wakes device
- [ ] Previous state restored after wake

**Pass/Fail Criteria:**
- PASS: Sleep and wake work correctly, state preserved
- FAIL: Sleep doesn't trigger, wake fails, or state lost

---

## 2. File Format Tests

**Note:** Detailed format testing is covered in [[FORMAT_TESTING]]. This section provides integration test cases.

### 2.1 Plain Text (TXT) Tests

#### 2.1.1 Small Text File
**Objective:** Verify small text files load and display correctly

**Test File:** `welcome.txt` (3.9 KB)

**Steps:**
1. Navigate to `welcome.txt` in library
2. Verify format indicator shows `[T]`
3. Press SELECT to open
4. Navigate through all pages (UP/DOWN)
5. Verify last page indicator
6. Return to library (BACK button)

**Expected Results:**
- [ ] File loads quickly (< 1 second)
- [ ] Text displays correctly with proper wrapping
- [ ] Pagination is correct (no truncated words)
- [ ] Page counter shows correct total pages
- [ ] Navigation works smoothly
- [ ] Return to library works

**Pass/Fail Criteria:**
- PASS: All text displays correctly and navigation works
- FAIL: Text corruption, wrong pagination, or navigation failure

---

#### 2.1.2 Large Text File
**Objective:** Verify large text files are handled properly

**Test File:** `gutenberg-sample.txt` (11.5 KB) or create larger test file

**Steps:**
1. Open large text file (>100 KB if available)
2. Navigate to middle of book
3. Jump to end of book (hold DOWN or navigate manually)
4. Verify memory usage (if monitoring tools available)

**Expected Results:**
- [ ] Large file loads successfully (may take 2-3 seconds)
- [ ] All pages display correctly
- [ ] Navigation remains responsive throughout
- [ ] Memory usage is reasonable (entire file in RAM)
- [ ] No crashes or out-of-memory errors

**Pass/Fail Criteria:**
- PASS: Large files work without performance degradation
- FAIL: Crash, hang, or memory issues

---

#### 2.1.3 Text File with Special Characters
**Objective:** Verify Unicode and special character handling

**Test File:** Create test file with:
- Smart quotes: " " ' '
- Em dashes: —
- Accented characters: é à ñ ü
- Symbols: © ® ™ °
- Line breaks and paragraphs

**Steps:**
1. Open file with special characters
2. Verify all characters display correctly

**Expected Results:**
- [ ] UTF-8 characters display correctly
- [ ] Smart quotes render properly
- [ ] Accented characters are visible
- [ ] Symbols display or are replaced with reasonable fallbacks
- [ ] No crashes from unsupported characters

**Pass/Fail Criteria:**
- PASS: Most special characters display correctly or have fallbacks
- FAIL: Crashes, garbled text, or major character rendering issues

---

### 2.2 EPUB Tests

#### 2.2.1 Basic EPUB Loading
**Objective:** Verify EPUB files can be opened and read

**Test File:** `alice-in-wonderland.epub` (185 KB)

**Steps:**
1. Navigate to EPUB file in library
2. Verify format indicator shows `[E]`
3. Verify title shows metadata (not filename)
4. Press SELECT to open
5. Observe loading time
6. Navigate through several pages
7. Return to library

**Expected Results:**
- [ ] EPUB loads successfully (1-3 seconds)
- [ ] Title extracted from metadata
- [ ] HTML tags are stripped from content
- [ ] Text flows naturally across chapters
- [ ] Page navigation works smoothly
- [ ] No crashes or parsing errors

**Pass/Fail Criteria:**
- PASS: EPUB opens and displays readable text
- FAIL: Parsing failure, crashes, or unreadable content

---

#### 2.2.2 EPUB Metadata Extraction
**Objective:** Verify EPUB metadata is correctly extracted

**Test File:** EPUB with known metadata

**Steps:**
1. View EPUB in library menu
2. Verify title and author are displayed (not filename)
3. Open EPUB and check status bar for format indicator

**Expected Results:**
- [ ] Title extracted from OPF metadata
- [ ] Author displayed (if available)
- [ ] Format indicator `[E]` shows correctly
- [ ] Filename not shown when metadata is available

**Pass/Fail Criteria:**
- PASS: Metadata extracted and displayed correctly
- FAIL: Metadata missing, wrong, or causes crashes

---

#### 2.2.3 EPUB with Complex HTML
**Objective:** Verify complex HTML is handled gracefully

**Test File:** EPUB with tables, lists, nested formatting

**Steps:**
1. Open EPUB with complex HTML structure
2. Navigate through sections with tables and lists
3. Verify content is readable (even if formatting is lost)

**Expected Results:**
- [ ] EPUB opens without crashes
- [ ] Text content is extracted (formatting may be lost)
- [ ] Tables and lists appear as plain text
- [ ] Reading remains possible despite lost formatting

**Pass/Fail Criteria:**
- PASS: Content is readable even if formatting is lost
- FAIL: Crashes, parsing errors, or completely unreadable output

---

### 2.3 PDF Tests

#### 2.3.1 Basic PDF Loading
**Objective:** Verify PDF files can be opened and read

**Test File:** `test-document.pdf` (13 KB)

**Steps:**
1. Navigate to PDF file in library
2. Verify format indicator shows `[P]`
3. Press SELECT to open
4. Observe loading time
5. Navigate through all pages
6. Return to library

**Expected Results:**
- [ ] PDF loads successfully (2-5 seconds)
- [ ] Text extraction works via pdftotext
- [ ] Page count is accurate
- [ ] Navigation works smoothly
- [ ] No crashes or extraction errors

**Pass/Fail Criteria:**
- PASS: PDF opens and text is readable
- FAIL: Extraction failure, crashes, or unreadable content

---

#### 2.3.2 Multi-Page PDF
**Objective:** Verify multi-page PDFs are handled correctly

**Test File:** PDF with 10+ pages

**Steps:**
1. Open multi-page PDF
2. Navigate to page 5
3. Navigate to last page
4. Jump back to first page

**Expected Results:**
- [ ] All pages extract successfully
- [ ] Page count is correct
- [ ] Navigation works across all pages
- [ ] No performance degradation on later pages

**Pass/Fail Criteria:**
- PASS: All pages accessible and readable
- FAIL: Missing pages, wrong count, or extraction failures

---

#### 2.3.3 PDF with Complex Layout
**Objective:** Verify PDFs with complex layouts extract reasonably

**Test File:** PDF with multi-column layout, tables, or sidebars

**Steps:**
1. Open complex PDF
2. Read extracted text
3. Verify content is mostly readable (order may be wrong)

**Expected Results:**
- [ ] PDF opens without crashes
- [ ] Text is extracted (order may be incorrect)
- [ ] Content is somewhat readable despite layout issues
- [ ] No major data loss

**Pass/Fail Criteria:**
- PASS: Text extracts and is partially readable
- FAIL: No text extracted or completely garbled

---

### 2.4 Mixed Library Tests

#### 2.4.1 Library with Multiple Formats
**Objective:** Verify library correctly displays mixed formats

**Prerequisites:** `/books/` contains TXT, EPUB, and PDF files

**Steps:**
1. Boot to library menu
2. Verify all formats are listed
3. Check format indicators for each file
4. Open one file of each type
5. Verify all work correctly

**Expected Results:**
- [ ] All supported formats detected and listed
- [ ] Format indicators `[T]`, `[E]`, `[P]` display correctly
- [ ] Files sorted alphabetically regardless of format
- [ ] All formats can be opened and read

**Pass/Fail Criteria:**
- PASS: All formats detected and work correctly
- FAIL: Missing files, wrong indicators, or format failures

---

## 3. Core Reading Features

### 3.1 Page Navigation

#### 3.1.1 Forward Navigation
**Objective:** Verify page forward navigation works correctly

**Steps:**
1. Open any book at first page
2. Press DOWN button repeatedly to advance pages
3. Navigate to last page
4. Verify "last page" indicator or behavior

**Expected Results:**
- [ ] Each DOWN press advances one page
- [ ] Page counter increments correctly
- [ ] Last page displays "End of book" or similar
- [ ] DOWN button has no effect at last page (or wraps to first)
- [ ] Text displays correctly on all pages

**Pass/Fail Criteria:**
- PASS: Navigation works smoothly through entire book
- FAIL: Stuck pages, wrong page numbers, or crashes

---

#### 3.1.2 Backward Navigation
**Objective:** Verify page backward navigation works correctly

**Steps:**
1. Open book and navigate to middle page
2. Press UP button repeatedly to go backward
3. Navigate to first page
4. Verify "first page" behavior

**Expected Results:**
- [ ] Each UP press goes back one page
- [ ] Page counter decrements correctly
- [ ] First page shows page 1
- [ ] UP button has no effect at first page (or wraps to last)

**Pass/Fail Criteria:**
- PASS: Backward navigation works correctly
- FAIL: Wrong pages, page count errors, or crashes

---

#### 3.1.3 Navigation at Book Boundaries
**Objective:** Verify behavior at first and last pages

**Steps:**
1. Open book at first page
2. Press UP button multiple times
3. Navigate to last page
4. Press DOWN button multiple times
5. Verify no crashes or unexpected behavior

**Expected Results:**
- [ ] UP at first page: no action or wraps to last (check spec)
- [ ] DOWN at last page: no action or wraps to first (check spec)
- [ ] No crashes from boundary navigation
- [ ] Page counter remains valid

**Pass/Fail Criteria:**
- PASS: Boundary conditions handled gracefully
- FAIL: Crashes, invalid pages, or corrupted display

---

### 3.2 Text Rendering

#### 3.2.1 Text Wrapping
**Objective:** Verify text wraps correctly at line boundaries

**Steps:**
1. Open book with long lines
2. Verify text wraps at display edge
3. Check for word breaks (should wrap at word boundaries)

**Expected Results:**
- [ ] Text wraps at display edge (400×300 pixels)
- [ ] Words are not split mid-word (except very long words)
- [ ] Line spacing is consistent
- [ ] No text overlaps or runs off screen

**Pass/Fail Criteria:**
- PASS: Text wraps naturally and is readable
- FAIL: Text overflow, mid-word breaks, or overlapping

---

#### 3.2.2 Font Rendering
**Objective:** Verify font displays clearly and consistently

**Steps:**
1. Open several books with different content
2. Verify font is readable throughout
3. Check for consistent character rendering

**Expected Results:**
- [ ] Font is clear and readable (8×16 bitmap or configured size)
- [ ] All ASCII characters display correctly
- [ ] Consistent spacing between characters and lines
- [ ] No missing or corrupted glyphs

**Pass/Fail Criteria:**
- PASS: Font renders clearly and consistently
- FAIL: Unreadable font, missing characters, or spacing issues

---

#### 3.2.3 Page Capacity
**Objective:** Verify page capacity calculations are correct

**Steps:**
1. Count characters per line on display
2. Count lines per page
3. Verify matches expected capacity:
   - Small font (6×12): ~63 chars/line, 21 lines/page
   - Medium font (8×16): ~47 chars/line, 14 lines/page
   - Large font (10×20): ~38 chars/line, 11 lines/page

**Expected Results:**
- [ ] Character count per line is approximately correct
- [ ] Line count per page is approximately correct
- [ ] Pages are filled efficiently (no large blank areas)
- [ ] Last page may have less content (expected)

**Pass/Fail Criteria:**
- PASS: Page capacity within 10% of expected values
- FAIL: Significantly wrong capacity or wasted space

---

### 3.3 Status Bar and UI

#### 3.3.1 Status Bar Display
**Objective:** Verify status bar shows correct information

**Steps:**
1. Open a book
2. Check status bar for:
   - Format indicator ([T], [E], [P])
   - Book title
   - Current page number
   - Total page count
   - Reading progress percentage (if implemented)

**Expected Results:**
- [ ] Status bar visible at top or bottom of screen
- [ ] Format indicator displays correctly
- [ ] Book title displays (truncated if too long)
- [ ] Page numbers update as you navigate
- [ ] All information is readable

**Pass/Fail Criteria:**
- PASS: Status bar displays accurate information
- FAIL: Missing info, wrong values, or unreadable

---

#### 3.3.2 Library Menu Display
**Objective:** Verify library menu is clear and functional

**Steps:**
1. Boot to library menu
2. Verify menu elements:
   - List of books with format indicators
   - Selection highlight (> marker or inverted background)
   - Page indicator for multi-page libraries
   - Instructions or header

**Expected Results:**
- [ ] All books listed with format indicators
- [ ] Selected book clearly highlighted
- [ ] Page indicator shows if library has multiple pages
- [ ] Instructions or controls shown (optional)

**Pass/Fail Criteria:**
- PASS: Menu is clear and easy to navigate
- FAIL: Missing books, unclear selection, or confusing layout

---

#### 3.3.3 UI Responsiveness
**Objective:** Verify UI responds quickly to button presses

**Steps:**
1. Navigate through library menu
2. Open and close several books
3. Navigate pages in reading view
4. Measure approximate response time

**Expected Results:**
- [ ] Button press to screen update: < 1 second (e-paper refresh time)
- [ ] Menu navigation feels responsive
- [ ] No noticeable lag or delays
- [ ] UI remains responsive during all operations

**Pass/Fail Criteria:**
- PASS: UI feels responsive and immediate
- FAIL: Significant lag (>2 seconds) or unresponsive UI

---

## 4. Phase 5 Feature Tests

### 4.1 Bookmark System

#### 4.1.1 Auto-Save Bookmark
**Objective:** Verify reading position is auto-saved

**Steps:**
1. Open a book
2. Navigate to page 10 (or any middle page)
3. Press BACK to return to library
4. Re-open the same book
5. Verify book opens at page 10 (last position)

**Expected Results:**
- [ ] Reading position saved automatically on page turn
- [ ] Position persists after closing book
- [ ] Book reopens at last position
- [ ] Bookmark file `/etc/ereader/bookmarks.txt` is updated

**Pass/Fail Criteria:**
- PASS: Position saved and restored correctly
- FAIL: Book reopens at page 1 or wrong position

---

#### 4.1.2 Manual Bookmark Creation
**Objective:** Verify manual bookmark can be saved

**Steps:**
1. Open a book and navigate to specific page
2. Press SELECT button (or configured bookmark key)
3. Verify bookmark saved message (if displayed)
4. Close and reopen book
5. Verify bookmark is restored

**Expected Results:**
- [ ] Manual bookmark action is acknowledged
- [ ] Bookmark persists in `/etc/ereader/bookmarks.txt`
- [ ] Bookmark restored on next open
- [ ] Multiple bookmarks for different books work

**Pass/Fail Criteria:**
- PASS: Manual bookmark saves and restores
- FAIL: Bookmark not saved or not restored

---

#### 4.1.3 Multiple Book Bookmarks
**Objective:** Verify bookmarks work independently for multiple books

**Steps:**
1. Open Book A, navigate to page 5, close
2. Open Book B, navigate to page 10, close
3. Open Book C, navigate to page 3, close
4. Reopen all three books and verify positions

**Expected Results:**
- [ ] Book A reopens at page 5
- [ ] Book B reopens at page 10
- [ ] Book C reopens at page 3
- [ ] Bookmarks don't interfere with each other

**Pass/Fail Criteria:**
- PASS: All bookmarks independent and correct
- FAIL: Bookmarks overwrite each other or are lost

---

#### 4.1.4 Bookmark File Format
**Objective:** Verify bookmark storage format is correct

**Steps:**
1. Create several bookmarks
2. View `/etc/ereader/bookmarks.txt` contents
3. Verify format: `book_path,page_number,timestamp`

**Expected Results:**
- [ ] File exists at `/etc/ereader/bookmarks.txt`
- [ ] CSV format with correct fields
- [ ] One entry per book (latest position)
- [ ] File is readable and parseable

**Pass/Fail Criteria:**
- PASS: Bookmark file format is correct and consistent
- FAIL: Wrong format, corrupted data, or missing entries

---

### 4.2 Settings System

#### 4.2.1 Settings File Persistence
**Objective:** Verify settings are saved and loaded correctly

**Steps:**
1. Check `/etc/ereader/settings.conf` exists
2. View settings file contents
3. Verify default values are present
4. Reboot device
5. Verify settings persist after reboot

**Expected Results:**
- [ ] Settings file exists at `/etc/ereader/settings.conf`
- [ ] Key=value format is correct
- [ ] Default settings are present:
  - `font_size=medium`
  - `line_spacing=single`
  - `margins=normal`
  - `display_mode=normal`
  - `auto_sleep_minutes=15`
- [ ] Settings persist across reboots

**Pass/Fail Criteria:**
- PASS: Settings save, load, and persist correctly
- FAIL: Settings not saved, wrong format, or lost on reboot

---

#### 4.2.2 Settings Menu Navigation (If Implemented)
**Objective:** Verify settings menu is accessible and navigable

**Prerequisites:** Settings menu UI implemented and integrated

**Steps:**
1. From main menu, press MENU button to access settings
2. Verify settings menu appears
3. Use UP/DOWN to navigate through settings
4. Verify each setting shows current value
5. Press BACK to exit settings

**Expected Results:**
- [ ] MENU button opens settings
- [ ] Settings menu lists all 5 settings
- [ ] Current values displayed for each setting
- [ ] UP/DOWN navigation works smoothly
- [ ] BACK button exits and saves settings

**Pass/Fail Criteria:**
- PASS: Settings menu accessible and navigable
- FAIL: Menu doesn't open, navigation broken, or exit doesn't work

---

#### 4.2.3 Font Size Setting (If Implemented)
**Objective:** Verify font size can be changed

**Prerequisites:** Dynamic font rendering and settings menu integrated

**Steps:**
1. Open settings menu
2. Navigate to "Font Size" setting
3. Press SELECT to cycle through values: small → medium → large
4. Exit settings and open a book
5. Verify font size has changed
6. Verify pagination adjusts to new font size
7. Verify reading position is preserved (percentage-based)

**Expected Results:**
- [ ] Font size cycles through 3 values
- [ ] Small font: ~63 chars/line, 21 lines/page
- [ ] Medium font: ~47 chars/line, 14 lines/page
- [ ] Large font: ~38 chars/line, 11 lines/page
- [ ] Book re-paginates with new font
- [ ] Reading position preserved approximately
- [ ] New font size persists after reboot

**Pass/Fail Criteria:**
- PASS: Font size changes work and persist
- FAIL: Font doesn't change, pagination breaks, or position lost

---

#### 4.2.4 Other Settings (Line Spacing, Margins, etc.)
**Objective:** Verify other settings can be changed

**Prerequisites:** Settings integrated with rendering engine

**Steps:**
1. Test each setting individually:
   - Line spacing: single → 1.5 → double
   - Margins: narrow → normal → wide
   - Display mode: normal → dark (if implemented)
   - Auto sleep: 5 → 10 → 15 → 30 → never
2. Verify each setting takes effect
3. Verify settings persist

**Expected Results:**
- [ ] Line spacing changes affect text layout
- [ ] Margins change page capacity
- [ ] Display mode inverts colors (if dark mode implemented)
- [ ] Auto sleep timeout changes behavior
- [ ] All settings save and persist

**Pass/Fail Criteria:**
- PASS: All settings change behavior as expected
- FAIL: Settings don't take effect or don't persist

---

### 4.3 Search Functionality

#### 4.3.1 Basic Text Search (If Implemented)
**Objective:** Verify search finds text within current book

**Prerequisites:** Search feature implemented and integrated

**Steps:**
1. Open a book with known content
2. Activate search (MENU button from reading view)
3. Select a search term from predefined list (e.g., "the")
4. Verify search results are displayed
5. Navigate through results with UP/DOWN
6. Press SELECT to jump to result
7. Verify page contains search term

**Expected Results:**
- [ ] Search activates from reading view
- [ ] Predefined search terms are selectable
- [ ] Search finds all occurrences
- [ ] Result count displayed (e.g., "Result 5 of 127")
- [ ] Navigation through results works
- [ ] Jump to page works correctly
- [ ] Context preview shows surrounding text

**Pass/Fail Criteria:**
- PASS: Search finds results and navigation works
- FAIL: Search fails, wrong results, or navigation broken

---

#### 4.3.2 Case-Sensitive Search
**Objective:** Verify case-sensitive search option

**Steps:**
1. Activate search on a book
2. Select a search term (e.g., "The")
3. Toggle case sensitivity (MENU button during search)
4. Compare result counts:
   - Case-insensitive: should match "The", "the", "THE"
   - Case-sensitive: should match only "The"

**Expected Results:**
- [ ] Case sensitivity can be toggled
- [ ] Case-insensitive finds more matches
- [ ] Case-sensitive finds exact matches only
- [ ] Toggle indication displayed

**Pass/Fail Criteria:**
- PASS: Case sensitivity works as expected
- FAIL: No difference in results or toggle doesn't work

---

#### 4.3.3 Search Performance
**Objective:** Verify search completes in reasonable time

**Steps:**
1. Open large book (>1 MB)
2. Search for common term (e.g., "the")
3. Measure search time
4. Verify search doesn't freeze UI

**Expected Results:**
- [ ] Search completes within 5 seconds for 1 MB book
- [ ] Search completes within 30 seconds for 10 MB book
- [ ] Progress indicator shown during search (optional)
- [ ] UI remains responsive or shows "searching" message
- [ ] No crashes or timeouts

**Pass/Fail Criteria:**
- PASS: Search completes in reasonable time
- FAIL: Search takes >1 minute or hangs UI

---

#### 4.3.4 Search Edge Cases
**Objective:** Verify search handles edge cases

**Steps:**
1. Search for term that doesn't exist
2. Search for very common term (thousands of results)
3. Search at beginning and end of book
4. Search for special characters (if supported)

**Expected Results:**
- [ ] "No results" message shown when term not found
- [ ] Large result sets (1000+) are handled or capped
- [ ] Results at book boundaries work correctly
- [ ] Special characters handled gracefully (may not search)

**Pass/Fail Criteria:**
- PASS: Edge cases handled without crashes
- FAIL: Crashes, hangs, or undefined behavior

---

### 4.4 Power Management

#### 4.4.1 Idle Timer
**Objective:** Verify idle timer resets on button press

**Steps:**
1. Boot device and note current time
2. Wait 1 minute
3. Press any button
4. Verify idle timer resets (sleep doesn't trigger immediately)
5. Wait for sleep timeout without pressing buttons

**Expected Results:**
- [ ] Idle timer tracks time since last button press
- [ ] Button press resets timer
- [ ] Sleep triggers after timeout (e.g., 5/10/15 minutes)
- [ ] Timer works consistently

**Pass/Fail Criteria:**
- PASS: Idle timer works and resets correctly
- FAIL: Sleep triggers too early/late or doesn't reset

---

#### 4.4.2 Sleep Warning
**Objective:** Verify sleep warning displays before timeout

**Prerequisites:** Sleep warning feature implemented

**Steps:**
1. Boot device and wait until 30 seconds before sleep timeout
2. Observe display for sleep warning
3. Press button to cancel sleep
4. Verify device doesn't sleep

**Expected Results:**
- [ ] Warning appears 30 seconds before sleep
- [ ] Warning message is clear (e.g., "Sleeping in 30 seconds...")
- [ ] Button press during warning cancels sleep
- [ ] Idle timer resets after button press

**Pass/Fail Criteria:**
- PASS: Warning appears and sleep can be cancelled
- FAIL: No warning, wrong timing, or can't cancel

---

#### 4.4.3 Sleep Mode Entry
**Objective:** Verify device enters sleep mode correctly

**Steps:**
1. Wait for full sleep timeout (or trigger sleep manually)
2. Observe sleep entry process
3. Verify display state
4. Verify system behavior

**Expected Results:**
- [ ] Display clears or shows sleep message
- [ ] E-paper display powers down
- [ ] System enters low-power mode (if implemented)
- [ ] Application state is preserved

**Pass/Fail Criteria:**
- PASS: Sleep mode entered cleanly
- FAIL: Display corruption, state loss, or system hang

---

#### 4.4.4 Wake from Sleep
**Objective:** Verify device wakes from sleep correctly

**Steps:**
1. Enter sleep mode
2. Press any button to wake
3. Observe display and system behavior
4. Verify previous state is restored

**Expected Results:**
- [ ] Any button press wakes device
- [ ] Display re-initializes correctly
- [ ] Previous application state restored:
  - If in library: library menu restored
  - If reading: book and page restored
- [ ] No crashes or corruption

**Pass/Fail Criteria:**
- PASS: Wake works and state restored
- FAIL: Wake fails, state lost, or crashes

---

#### 4.4.5 Sleep/Wake Cycle Stress Test
**Objective:** Verify sleep/wake cycle is reliable

**Steps:**
1. Enter and exit sleep mode 10 times
2. Verify each cycle works correctly
3. Monitor for memory leaks or degradation

**Expected Results:**
- [ ] All 10 sleep/wake cycles complete successfully
- [ ] No performance degradation over cycles
- [ ] No memory leaks (check with `free -m` if available)
- [ ] System remains stable

**Pass/Fail Criteria:**
- PASS: All cycles work reliably
- FAIL: Any cycle fails or system becomes unstable

---

### 4.5 Battery Monitoring (Hardware Dependent)

#### 4.5.1 Battery Detection
**Objective:** Verify battery monitoring hardware is detected

**Prerequisites:** Battery HAT or ADC hardware installed

**Steps:**
1. Boot device with battery connected
2. Check logs: `dmesg | grep battery` or `dmesg | grep i2c`
3. Verify battery monitor initialized

**Expected Results:**
- [ ] Battery monitoring hardware detected
- [ ] Initialization successful (INA219, MCP3008, or similar)
- [ ] No errors in logs
- [ ] Battery level can be read

**Pass/Fail Criteria:**
- PASS: Battery hardware detected and initialized
- FAIL: Hardware not detected or initialization errors
- N/A: No battery hardware present

---

#### 4.5.2 Battery Level Reading
**Objective:** Verify battery level is read correctly

**Steps:**
1. Read battery level via application or logs
2. Compare with external multimeter reading (if available)
3. Verify reading is within acceptable range

**Expected Results:**
- [ ] Battery voltage/percentage reported
- [ ] Reading is within 10% of actual value
- [ ] Reading updates periodically
- [ ] No crashes or errors

**Pass/Fail Criteria:**
- PASS: Battery level reading is accurate
- FAIL: Wrong readings, doesn't update, or crashes
- N/A: No battery hardware present

---

#### 4.5.3 Battery Status Display (If Implemented)
**Objective:** Verify battery status shows in UI

**Steps:**
1. Check status bar for battery indicator
2. Verify battery level is displayed (icon or percentage)
3. Test with different battery levels

**Expected Results:**
- [ ] Battery indicator visible in status bar
- [ ] Indicator updates as battery discharges
- [ ] Low battery warning when below threshold
- [ ] Indicator doesn't interfere with other UI elements

**Pass/Fail Criteria:**
- PASS: Battery status displayed accurately
- FAIL: Missing indicator, wrong values, or UI issues
- N/A: Feature not yet implemented

---

### 4.6 UI Polish and Visual Feedback

#### 4.6.1 Loading Indicators (If Implemented)
**Objective:** Verify loading indicators appear during slow operations

**Prerequisites:** Loading screen and UI components integrated

**Steps:**
1. Open large EPUB or PDF file
2. Observe loading indicator during file parsing
3. Verify indicator disappears when loading complete
4. Try other slow operations (search, pagination)

**Expected Results:**
- [ ] Loading indicator appears for slow operations
- [ ] Spinner or progress bar animation visible
- [ ] Loading message is clear (e.g., "Opening book...")
- [ ] Indicator disappears when operation complete
- [ ] User knows system is working, not frozen

**Pass/Fail Criteria:**
- PASS: Loading indicators appear and work correctly
- FAIL: No indicators, confusing feedback, or indicators don't clear
- N/A: Feature not yet implemented

---

#### 4.6.2 Confirmation Dialogs (If Implemented)
**Objective:** Verify confirmation dialogs prevent accidental actions

**Steps:**
1. Trigger action that requires confirmation (e.g., delete bookmark, exit app)
2. Verify confirmation dialog appears
3. Test SELECT (Yes) and BACK (No) responses
4. Verify correct action is taken

**Expected Results:**
- [ ] Confirmation dialog appears for destructive/important actions
- [ ] Dialog shows clear question (e.g., "Exit application?")
- [ ] SELECT confirms action
- [ ] BACK cancels action
- [ ] Action only occurs after confirmation

**Pass/Fail Criteria:**
- PASS: Confirmations work and prevent accidents
- FAIL: No confirmation, wrong action, or dialog doesn't work
- N/A: Feature not yet implemented

---

#### 4.6.3 Visual Borders and Styling (If Implemented)
**Objective:** Verify decorative borders enhance readability

**Steps:**
1. Navigate through menus and dialogs
2. Observe border styles (single, double, decorative)
3. Verify borders are clean and don't corrupt display

**Expected Results:**
- [ ] Borders render cleanly without artifacts
- [ ] Borders enhance visual hierarchy
- [ ] Borders don't overlap text or other UI elements
- [ ] Border style is consistent throughout UI

**Pass/Fail Criteria:**
- PASS: Borders look good and enhance usability
- FAIL: Corrupted borders, overlaps, or inconsistent styling
- N/A: Feature not yet implemented

---

#### 4.6.4 Error Messages
**Objective:** Verify error messages are clear and helpful

**Steps:**
1. Trigger various error conditions:
   - Open corrupt file
   - Trigger out-of-memory condition (if possible)
   - Disconnect display (hardware test)
   - Invalid button sequence
2. Observe error messages

**Expected Results:**
- [ ] Error messages are displayed (not silent failures)
- [ ] Messages are clear and describe the problem
- [ ] User can recover from errors (return to menu)
- [ ] No cryptic error codes or technical jargon

**Pass/Fail Criteria:**
- PASS: Error messages are helpful and recoverable
- FAIL: Silent failures, cryptic errors, or system hangs

---

## 5. Edge Cases and Error Handling

### 5.1 Empty Library

#### 5.1.1 Empty Books Directory
**Objective:** Verify graceful handling of empty library

**Steps:**
1. Remove all books from `/books/` directory
2. Boot device
3. Observe behavior

**Expected Results:**
- [ ] Device boots successfully
- [ ] "Empty library" or "No books found" message displayed
- [ ] Instructions to add books shown
- [ ] No crashes or errors
- [ ] Device remains usable (can add books later)

**Pass/Fail Criteria:**
- PASS: Empty library handled gracefully with clear message
- FAIL: Crash, blank screen, or confusing behavior

---

### 5.2 File System Errors

#### 5.2.1 Read-Only File System
**Objective:** Verify behavior when filesystem is read-only

**Steps:**
1. Remount filesystem as read-only: `mount -o remount,ro /`
2. Open a book (bookmarks can't be saved)
3. Try to save bookmark or change settings
4. Observe error handling

**Expected Results:**
- [ ] Read operations work normally
- [ ] Write operations fail gracefully
- [ ] Error message displayed for save failures
- [ ] System remains stable
- [ ] No crashes or corruption

**Pass/Fail Criteria:**
- PASS: Read-only mode handled gracefully
- FAIL: Crashes, corrupted data, or confusing behavior

---

#### 5.2.2 Disk Full
**Objective:** Verify behavior when disk is full

**Steps:**
1. Fill filesystem to capacity
2. Try to save bookmarks or settings
3. Observe error handling

**Expected Results:**
- [ ] Disk full error detected
- [ ] Error message displayed to user
- [ ] System remains functional (read operations still work)
- [ ] No crashes or data corruption

**Pass/Fail Criteria:**
- PASS: Disk full handled without crashes
- FAIL: Corruption, crashes, or silent failures

---

### 5.3 Corrupt or Invalid Files

#### 5.3.1 Corrupt EPUB
**Objective:** Verify handling of corrupt EPUB files

**Steps:**
1. Create corrupt EPUB (invalid ZIP, missing OPF, etc.)
2. Try to open corrupt EPUB from library
3. Observe error handling

**Expected Results:**
- [ ] Corrupt file detected during opening
- [ ] Error message displayed: "Failed to open EPUB" or similar
- [ ] Return to library menu (no crash)
- [ ] Other books remain accessible

**Pass/Fail Criteria:**
- PASS: Corrupt file handled gracefully with error message
- FAIL: Crash, hang, or silent failure

---

#### 5.3.2 Corrupt PDF
**Objective:** Verify handling of corrupt PDF files

**Steps:**
1. Create corrupt PDF (invalid header, truncated file, etc.)
2. Try to open corrupt PDF from library
3. Observe error handling

**Expected Results:**
- [ ] Corrupt file detected by pdfinfo/pdftotext
- [ ] Error message displayed
- [ ] Return to library menu
- [ ] System remains stable

**Pass/Fail Criteria:**
- PASS: Corrupt PDF handled without crashes
- FAIL: Crash or hang

---

#### 5.3.3 Wrong File Extension
**Objective:** Verify handling of files with wrong extensions

**Steps:**
1. Rename TXT file to `.epub`
2. Try to open misnamed file
3. Verify error handling

**Expected Results:**
- [ ] Parser detects file is not valid EPUB
- [ ] Error message displayed
- [ ] Return to library
- [ ] No crashes

**Pass/Fail Criteria:**
- PASS: Wrong extension handled gracefully
- FAIL: Crash, hang, or corrupted display

---

### 5.4 Memory and Performance Edge Cases

#### 5.4.1 Very Large File
**Objective:** Verify behavior with file at size limit

**Steps:**
1. Create text file at or near 10 MB limit
2. Try to open file
3. Navigate through book
4. Monitor memory usage

**Expected Results:**
- [ ] File loads successfully (may take 5-10 seconds)
- [ ] Pagination completes
- [ ] Navigation works (may be slower)
- [ ] Memory usage is high but acceptable
- [ ] No crashes or out-of-memory errors

**Pass/Fail Criteria:**
- PASS: Large file works within acceptable performance
- FAIL: Crash, out-of-memory, or hang

---

#### 5.4.2 Memory Leak Test
**Objective:** Verify no memory leaks during extended use

**Steps:**
1. Note initial memory usage: `free -m`
2. Open and close 10 different books
3. Navigate through many pages
4. Check memory usage again
5. Compare initial and final memory

**Expected Results:**
- [ ] Memory usage increases during operation (expected)
- [ ] Memory is released when books are closed
- [ ] Final memory usage close to initial (within 10%)
- [ ] No continuous growth indicating leak

**Pass/Fail Criteria:**
- PASS: No significant memory leak detected
- FAIL: Memory usage continuously grows

---

## 6. Usability Tests

### 6.1 First-Time User Experience

#### 6.1.1 Can User Navigate Without Instructions?
**Objective:** Verify interface is intuitive for new users

**Test Method:** Give device to new user with no instructions

**Observation Checklist:**
- [ ] User figures out how to navigate library menu (UP/DOWN)
- [ ] User figures out how to open a book (SELECT)
- [ ] User figures out how to navigate pages (UP/DOWN)
- [ ] User figures out how to return to library (BACK)
- [ ] User completes tasks without frustration

**Expected Results:**
- [ ] Most users can navigate within 1-2 minutes
- [ ] Button functions are discoverable
- [ ] UI provides adequate feedback

**Pass/Fail Criteria:**
- PASS: New user can perform basic tasks without help
- FAIL: User is confused or gives up

---

#### 6.1.2 Reading Comfort
**Objective:** Verify reading is comfortable for extended periods

**Test Method:** Read book for 30+ minutes

**Observation Checklist:**
- [ ] Text is readable without eye strain
- [ ] Font size is comfortable
- [ ] Line spacing is adequate
- [ ] Page turns are smooth enough
- [ ] No headaches or discomfort

**Expected Results:**
- [ ] Reading is comfortable for 30+ minutes
- [ ] E-paper display reduces eye strain vs LCD
- [ ] Page turn speed is acceptable

**Pass/Fail Criteria:**
- PASS: Reading is comfortable for extended periods
- FAIL: Significant discomfort or eye strain

---

#### 6.1.3 Button Ergonomics
**Objective:** Verify buttons are comfortable to use

**Test Method:** Use device for 30+ minutes

**Observation Checklist:**
- [ ] Buttons are easy to locate without looking
- [ ] Button press force is comfortable
- [ ] No hand fatigue from button use
- [ ] Can hold device and press buttons comfortably

**Expected Results:**
- [ ] Buttons are comfortable for extended use
- [ ] Tactile feedback is adequate
- [ ] No hand cramping or discomfort

**Pass/Fail Criteria:**
- PASS: Buttons are comfortable and easy to use
- FAIL: Awkward button placement or uncomfortable use

---

## 7. Performance Tests

**Note:** Detailed performance testing is covered in [[PERFORMANCE]]. This section provides key verification tests.

### 7.1 Boot Time

**Objective:** Verify boot time meets expectations

**Steps:**
1. Power off device
2. Power on and start timer
3. Stop timer when library menu appears
4. Measure boot time

**Expected Results:**
- [ ] Boot time < 30 seconds
- [ ] Boot is consistent across multiple tests

**Pass/Fail Criteria:**
- PASS: Boot time ≤ 30 seconds
- FAIL: Boot time > 45 seconds

---

### 7.2 Page Turn Latency

**Objective:** Verify page turns are responsive

**Steps:**
1. Open any book
2. Press DOWN button and start timer
3. Stop timer when new page is fully displayed
4. Measure latency
5. Repeat 5 times and average

**Expected Results:**
- [ ] Page turn latency < 1 second (e-paper refresh limit)
- [ ] Latency is consistent

**Pass/Fail Criteria:**
- PASS: Average latency ≤ 1 second
- FAIL: Average latency > 2 seconds

---

### 7.3 Book Load Time

**Objective:** Verify books load in acceptable time

**Test Cases:**
1. Small TXT (3.9 KB): < 1 second
2. Large TXT (1 MB): < 3 seconds
3. EPUB (185 KB): < 3 seconds
4. PDF (13 KB): < 5 seconds

**Steps:**
1. From library, select book and start timer
2. Stop timer when first page displays
3. Measure load time

**Expected Results:**
- [ ] All formats load within expected times
- [ ] Load time increases proportionally with file size

**Pass/Fail Criteria:**
- PASS: All test cases within expected times
- FAIL: Any format significantly exceeds expected time

---

### 7.4 Memory Usage

**Objective:** Verify memory usage is reasonable

**Steps:**
1. Boot device and check baseline: `free -m`
2. Open various books and check memory
3. Verify no out-of-memory conditions

**Expected Results:**
- [ ] Baseline (idle): ~150 KB
- [ ] Reading TXT (1 MB): ~1.2 MB
- [ ] Reading EPUB (185 KB): ~2-3 MB
- [ ] Reading PDF (10 MB): ~10-15 MB
- [ ] No out-of-memory errors

**Pass/Fail Criteria:**
- PASS: Memory usage within expected ranges
- FAIL: Excessive memory use or out-of-memory errors

---

## 8. Regression Test Checklist

Use this checklist after any significant changes to verify core functionality still works.

### Quick Smoke Test (5 minutes)
- [ ] Device boots to library
- [ ] Can navigate library menu (UP/DOWN)
- [ ] Can open TXT file
- [ ] Can navigate pages (UP/DOWN)
- [ ] Can return to library (BACK)
- [ ] Can open EPUB file
- [ ] Can open PDF file

### Core Functionality (15 minutes)
- [ ] All buttons work correctly
- [ ] All file formats load
- [ ] Page navigation works in all formats
- [ ] Bookmarks save and restore
- [ ] Display rendering is correct
- [ ] No crashes in normal use

### Feature Verification (30 minutes)
- [ ] Settings system works
- [ ] Search functionality works (if implemented)
- [ ] Power management works (if implemented)
- [ ] Battery monitoring works (if hardware present)
- [ ] UI polish components work (if implemented)

### Edge Cases (15 minutes)
- [ ] Empty library handled
- [ ] Corrupt files handled
- [ ] Boundary conditions work
- [ ] Error messages are clear

---

## Test Execution Guidelines

### Before Testing
1. Flash latest SD card image
2. Ensure test books are present in `/books/`
3. Fully charge battery (if testing battery features)
4. Have serial console or SSH access ready for debugging

### During Testing
1. Execute tests in order (hardware → formats → features → edge cases)
2. Document all failures with:
   - Test case ID
   - Expected result
   - Actual result
   - Steps to reproduce
   - Error messages or logs
3. Take photos/videos of display for bug reports
4. Monitor system logs: `dmesg`, `/var/log/messages`

### After Testing
1. Document results in [[TEST_RESULTS]]
2. File issues for any failures
3. Update this test plan if gaps are found
4. Create regression test checklist for critical bugs

---

## Test Result Documentation

For each test session, create entry in [[TEST_RESULTS]] with:

```
Date: YYYY-MM-DD
Tester: [Name]
Build: [Git commit hash]
Hardware: [Device model]

Summary:
- Tests Passed: X / Y
- Tests Failed: Z
- Tests Skipped: W (with reasons)

Critical Issues:
- [List any critical failures]

Notes:
- [Any observations or recommendations]
```

---

## Success Criteria

### Phase 5 Completion Requirements
To consider Phase 5 complete, the following must pass:
- [ ] All hardware tests pass
- [ ] All file format tests pass
- [ ] Core reading features work reliably
- [ ] Bookmark system works correctly
- [ ] Settings system implemented and functional
- [ ] Search works (if implemented) or deferred with documentation
- [ ] Power management works correctly
- [ ] No critical bugs or crashes
- [ ] Performance meets acceptable standards
- [ ] Usability is good enough for daily reading

### Ready for Production
To consider device ready for daily use:
- [ ] All Phase 5 tests pass
- [ ] No data loss bugs
- [ ] Battery life is acceptable
- [ ] User experience is polished
- [ ] Documentation is complete
- [ ] Known limitations are documented

---

## Revision History

- 2026-01-16: Initial comprehensive test plan created for Phase 5
