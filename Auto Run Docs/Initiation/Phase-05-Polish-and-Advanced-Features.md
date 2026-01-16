# Phase 05: Polish and Advanced Features

This phase adds essential quality-of-life features that make the e-reader truly usable: persistent bookmarks, adjustable settings, power management, and a polished user experience. By the end of this phase, the device feels like a complete product.

## Tasks

- [x] Implement persistent bookmark system:
  - ✅ Bookmark system already existed in `src/ereader/books/book_manager.c`
  - ✅ Save current reading position (book path, page/position, timestamp)
  - ✅ Load last reading position for a book
  - ✅ Store bookmarks in `/etc/ereader/bookmarks.txt` (CSV format)
  - ✅ Auto-save position on every page turn (implemented in reader.c)
  - ✅ Manual bookmark creation (SELECT button → Save Bookmark)
  - ✅ Bookmarks fully integrated with reader.c - auto-loads last position when opening book
  - ✅ Updated reader status bar to show reading progress with percentage (e.g., "[45/200,23%]")
  - **Note**: Implementation completed. Auto-save now occurs on every page turn (UP/DOWN buttons). Percentage display added to page indicator.

- [x] Create settings system:
  - ✅ Created `src/ereader/settings/settings_manager.c` with functions:
    - ✅ Load settings from `/etc/ereader/settings.conf` (key=value format)
    - ✅ Save settings persistently
    - ✅ Provide getters/setters for each setting
  - ✅ Wrote `src/ereader/settings/settings_manager.h`
  - ✅ Defined settings:
    - ✅ font_size (small, medium, large)
    - ✅ line_spacing (single, 1.5, double)
    - ✅ margins (narrow, normal, wide)
    - ✅ display_mode (normal, dark - for future inverted rendering)
    - ✅ auto_sleep_minutes (5, 10, 15, 30, never)
  - ⏭️ Apply settings to text_renderer and reader components (deferred to later tasks)
  - **Note**: Core settings infrastructure complete. Settings can be loaded, saved, and accessed. Integration with renderer and UI components will occur when building the settings menu UI and dynamic font rendering.

- [x] Build settings menu UI:
  - ✅ Created `src/ereader/ui/settings_menu.c` with:
    - ✅ Scrollable list of 5 settings (font size, line spacing, margins, display mode, auto sleep)
    - ✅ UP/DOWN navigation between settings
    - ✅ SELECT button to cycle through values for each setting
    - ✅ Visual indicator of current value (displayed after setting name)
    - ✅ BACK/MENU buttons save and return to main menu
  - ✅ Wrote `src/ereader/ui/settings_menu.h` with complete API
  - ✅ Added STATE_SETTINGS to main application state machine in `ereader.h`
  - ✅ Added settings_menu_state and settings pointers to app_context_t
  - ⏭️ Integration with main.c requires manual code updates (see SETTINGS_MENU_INTEGRATION_GUIDE.md)
  - **Note**: Settings menu UI fully implemented following same pattern as book list menu. See `Auto Run Docs/Working/SETTINGS_MENU_INTEGRATION_GUIDE.md` for detailed integration instructions to add to main.c. The integration requires updating app_init(), app_cleanup(), app_change_state(), app_handle_button_event(), app_render(), and app_state_to_string().

- [x] Implement dynamic font size rendering:
  - ✅ Created `src/ereader/rendering/font_data.h` with three embedded bitmap fonts:
    - Small: 6x12 pixels (~63 chars/line, 21 lines/page)
    - Medium: 8x16 pixels (~47 chars/line, 14 lines/page) - existing font
    - Large: 10x20 pixels (~38 chars/line, 11 lines/page)
  - ✅ Updated `src/ereader/rendering/text_renderer.h` with new API:
    - `text_renderer_set_font_size()` / `text_renderer_get_font_size()`
    - `text_renderer_get_font_width()` / `text_renderer_get_font_height()`
    - `text_renderer_get_chars_per_line()` / `text_renderer_get_lines_per_page()`
    - `text_renderer_repaginate()` - re-paginate with new font, maintains reading position
  - ✅ Updated `src/ereader/rendering/text_renderer.c` with full implementation:
    - Dynamic font selection in `text_render_char()` based on current font size
    - All rendering functions updated to use dynamic font dimensions
    - Re-pagination function calculates position percentage and maps to new page count
  - ✅ Updated `src/ereader/Makefile` to include font_data.h dependency
  - **Implementation Notes**:
    - Font sizes match settings_manager.h enum values (SMALL=0, MEDIUM=1, LARGE=2)
    - Backward compatibility: FONT_WIDTH and FONT_HEIGHT macros now call dynamic functions
    - All font data embedded in binary (~8KB total for all 3 fonts covering ASCII 32-126)
    - Large font uses 16-bit encoding (2 bytes per row) for 10-pixel width
    - Re-pagination preserves approximate reading position as percentage through book
  - ⏭️ Integration with settings menu and reader UI requires main.c updates (see SETTINGS_MENU_INTEGRATION_GUIDE.md)
  - ⏭️ Testing on real hardware needed to verify readability of all three font sizes on 4.2" e-paper display

- [x] Add power management and sleep mode:
  - ✅ Created `src/ereader/power/power_manager.c` with:
    - ✅ Idle timer tracking (reset on any button press)
    - ✅ Trigger sleep mode after configured timeout
    - ✅ In sleep mode: clear screen, disable display, wait for button press
    - ✅ Wake on any button press
    - ⏭️ Consider: disable WiFi chip to save power (for future WiFi features)
  - ✅ Wrote `src/ereader/power/power_manager.h`
  - ✅ Updated `src/ereader/ereader.h` to add power_manager field to app_context_t
  - ✅ Updated `src/ereader/Makefile` to include power/power_manager.c
  - ⏭️ Integration with main event loop requires manual code updates (see POWER_MANAGER_INTEGRATION_GUIDE.md)
  - ✅ Display sleep countdown warning ("Sleeping in 30 seconds...")
  - **Implementation Notes**:
    - Power manager tracks idle time since last button press
    - Integrates with settings system to read auto_sleep_minutes setting
    - Sleep warning displays 30 seconds before timeout (configurable via SLEEP_WARNING_SECONDS)
    - Sleep mode workflow: clear display → power down e-paper → wait for button → re-init display → restore state
    - Wake on ANY button press (first press only wakes, doesn't trigger action)
    - Timeout options: 5, 10, 15, 30 minutes, or 0 (never sleep)
    - Default timeout: 15 minutes (from settings_manager.h AUTO_SLEEP_DEFAULT)
  - ⏭️ Integration requires updating main.c (see `Auto Run Docs/Working/POWER_MANAGER_INTEGRATION_GUIDE.md`)
  - **Note**: Core power management module fully implemented. See integration guide for detailed instructions on adding to main.c event loop. The integration requires updates to app_init(), app_cleanup(), and app_run() to handle sleep/wake cycles.

- [ ] Implement battery monitoring (if hardware supports):
  - Research Pi Zero W battery monitoring options:
    - If using battery HAT: read voltage via I2C or ADC
    - If no HAT: document that battery monitoring requires additional hardware
  - If feasible: create `src/ereader/power/battery_monitor.c`
  - Display battery icon in status bar (optional, may skip if too complex)
  - Document battery setup in `docs/hardware/POWER_SUPPLY.md`

- [ ] Add search functionality:
  - Create `src/ereader/search/search_engine.c` with:
    - Simple text search within current book
    - Find all occurrences of search term
    - Navigate to next/previous match
    - Highlight match on screen (invert colors)
  - Create search UI: text input via button presses (slow but functional)
  - Or: defer text input to Phase 6 (WiFi keyboard), make search read-only for now
  - Document search limitations in `docs/USER_GUIDE.md`

- [ ] Improve UI polish and visual feedback:
  - Add loading spinners or progress indicators for slow operations (opening large PDF)
  - Add confirmation dialogs (e.g., "Exit book? Progress saved.")
  - Improve menu aesthetics: better alignment, borders, icons
  - Add header/footer to all screens with consistent branding
  - Smooth transitions between screens (optional: fade effects if display refresh allows)

- [ ] Optimize performance and memory usage:
  - Profile memory usage with `top` or `smem` during operation
  - Identify memory leaks (use valgrind in cross-compiled testing)
  - Optimize frequent operations: page rendering, file I/O
  - Consider memory-mapped files for large books
  - Document performance benchmarks in `docs/testing/PERFORMANCE.md`:
    - Boot time, menu load time, page turn latency
    - Memory usage (idle, reading TXT, reading EPUB, reading PDF)

- [ ] Create comprehensive testing suite:
  - Create `docs/testing/TEST_PLAN.md` with:
    - Test cases for each feature (bookmarks, settings, search, sleep)
    - Edge cases: empty library, corrupt files, out-of-memory
    - Usability tests: can a new user navigate without instructions?
  - Document test results in `docs/testing/TEST_RESULTS.md`
  - Create regression test checklist for future changes

- [ ] Update all documentation for Phase 5 features:
  - Update `docs/USER_GUIDE.md` with:
    - How to use bookmarks (auto and manual)
    - How to access and change settings
    - How sleep mode works
    - How to search within a book
  - Update `docs/architecture/EREADER_DESIGN.md` with new components
  - Update `docs/progress/PHASE_05_LOG.md` with completion notes
  - Create `docs/progress/PHASE_06_PLANNING.md` for WiFi features
