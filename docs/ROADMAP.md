---
type: reference
title: Project Roadmap - Open Source E-Reader
created: 2026-01-18
tags:
  - roadmap
  - planning
  - features
  - future
related:
  - "[[CHANGELOG]]"
  - "[[README]]"
  - "[[CONTRIBUTING]]"
---

# Project Roadmap

This document outlines the planned future development of the Open Source E-Reader project. Priorities and timelines are community-driven and subject to change based on user feedback and contributions.

---

## Table of Contents

1. [Version 1.0 - Current Release](#version-10---current-release)
2. [Version 1.1 - Near-Term Improvements](#version-11---near-term-improvements)
3. [Version 1.2 - Enhanced Reading Experience](#version-12---enhanced-reading-experience)
4. [Version 2.0 - Major Feature Expansion](#version-20---major-feature-expansion)
5. [Long-Term Vision](#long-term-vision)
6. [Community Requests](#community-requests)
7. [How to Contribute](#how-to-contribute)

---

## Version 1.0 - Current Release

**Status:** Released (2026-01-18)

### Completed Features
- ✅ Multi-format support (EPUB, PDF, TXT)
- ✅ E-paper display driver (SPI)
- ✅ GPIO button input with debouncing
- ✅ Text rendering with pagination
- ✅ Book library management
- ✅ Bookmark persistence
- ✅ WiFi connectivity (WPA/WPA2)
- ✅ Online book downloads (Project Gutenberg)
- ✅ Power management and sleep modes
- ✅ Battery monitoring (optional)
- ✅ Settings menu (fonts, margins, display)
- ✅ Reading statistics
- ✅ Full-text search
- ✅ OTA updates (optional)
- ✅ Complete documentation

### Known Issues
- No partial display refresh (all page turns are full refresh)
- PDF rendering is text-only (complex layouts not preserved)
- Search can be slow on large books
- Boot time could be further optimized
- No low battery warnings yet

---

## Version 1.1 - Near-Term Improvements

**Target:** Q2 2026
**Focus:** Performance, stability, user experience refinement

### Planned Features

#### Display Enhancements
- **Partial refresh support** - Faster page turns using partial e-paper updates
  - Reduce page turn time from ~2s to ~0.5s
  - Every 5-10 pages, perform full refresh to clear ghosting
  - Configurable in settings (full vs partial refresh)
- **Grayscale support** - 4-color display modes for better graphics
  - Support 2-bit grayscale e-paper displays
  - Enhanced PDF rendering with images
  - Better cover art display

#### Power & Battery
- **Low battery warnings** - Alert before shutdown
  - Configurable warning thresholds (20%, 10%, 5%)
  - Visual indicator in status bar
  - Graceful shutdown on critical battery
- **Improved sleep modes** - Deeper sleep states
  - Wake on button press optimization
  - Display-only refresh without full wake
  - Scheduled wake for statistics sync

#### PDF Improvements
- **Better PDF rendering** - Layout preservation
  - Render PDFs as images for complex layouts
  - Zoom and pan support for large pages
  - Configurable rendering quality vs performance
- **PDF table of contents** - Chapter navigation
  - Extract PDF bookmarks/outline
  - Navigate by section
  - Thumbnail previews (optional)

#### User Interface
- **Font selection** - Multiple font options
  - Include 3-5 bitmap fonts (serif, sans-serif, mono)
  - Optional TrueType font support (if space permits)
  - Per-book font preferences
- **UI themes** - Light and dark modes
  - Inverted display (white on black)
  - Configurable contrast levels
  - Button to toggle quickly

#### Performance
- **Boot time optimization** - Target <8 seconds
  - Parallel initialization of services
  - Lazy loading of network stack
  - Display "splash screen" earlier in boot
- **Search performance** - Faster indexing
  - Pre-index books on add
  - Cached search results
  - Progressive search (show results as found)

#### Quality of Life
- **USB file transfer** - USB OTG mass storage mode
  - Mount `/books/` over USB without removing SD card
  - Automatic unmount on disconnect
  - Compatible with Windows/Mac/Linux
- **Recently read list** - Quick access to recent books
  - Show last 5-10 opened books
  - Sort library by "recently opened"
  - Configurable in settings

### Estimated Effort
- **Small tasks** (1-2 days): Low battery warnings, recently read list, UI themes
- **Medium tasks** (3-5 days): USB file transfer, font selection, partial refresh
- **Large tasks** (1-2 weeks): PDF improvements, boot optimization, grayscale support

---

## Version 1.2 - Enhanced Reading Experience

**Target:** Q3-Q4 2026
**Focus:** Advanced reading features, library management, customization

### Planned Features

#### Reading Tools
- **Dictionary integration** - Word lookup and definitions
  - Offline dictionary (WordNet or similar)
  - Long-press word to define
  - Vocabulary building (save unknown words)
- **Note-taking** - Text annotations and highlights
  - Highlight text passages
  - Add notes to pages
  - Export highlights and notes
- **Multiple bookmarks** - More than one bookmark per book
  - Named bookmarks (Chapter 5, Important Quote)
  - Quick jump to bookmarks
  - Bookmark management menu

#### Search Enhancements
- **Advanced search** - Regex and filter support
  - Regular expression patterns
  - Search within specific books or genres
  - Search history
- **Cross-book search** - Find text across entire library
  - Full library text index
  - Show results grouped by book
  - Preview context around matches

#### Library Management
- **Collections and categories** - Organize books
  - User-defined collections (Fiction, Non-Fiction, To Read)
  - Auto-categorize by metadata
  - Filter library by collection
- **Metadata editing** - Correct book information
  - Edit title, author, year
  - Add cover images (if storage permits)
  - Tag books with keywords
- **Sorting options** - More library sort modes
  - Sort by author, date added, file size
  - Reverse sort order
  - Custom sort preferences

#### Reading Experience
- **Reading goals** - Track reading progress
  - Daily/weekly page goals
  - Reading streaks
  - Achievement badges (read 100 pages, finish 10 books)
- **Reading timer** - Session time tracking
  - Show session duration
  - Estimate time to finish chapter/book
  - Reading speed calculation (WPM)

#### Customization
- **Page layout options** - More control over display
  - Margins (top, bottom, left, right individually)
  - Line spacing (single, 1.5, double)
  - Paragraph indentation
  - Justification (left, center, justified)
- **Button remapping** - Customize button functions
  - Assign custom actions to each button
  - Long-press vs short-press actions
  - Profiles for different use cases

### Estimated Effort
- **Small tasks** (1-2 days): Sorting options, reading timer, metadata editing
- **Medium tasks** (3-5 days): Multiple bookmarks, collections, page layout options
- **Large tasks** (1-2 weeks): Dictionary integration, note-taking, advanced search

---

## Version 2.0 - Major Feature Expansion

**Target:** 2027
**Focus:** Hardware expansion, new input methods, multimedia

### Planned Features

#### Hardware Support
- **Touchscreen support** - Alternative input method
  - Resistive or capacitive touch driver
  - Touch gestures (swipe, tap, pinch)
  - Hybrid button + touch mode
- **Larger displays** - Support for bigger screens
  - 5.83" (648×480)
  - 7.5" (800×480)
  - 10.3" (1872×1404)
  - Auto-detect display size and adjust UI
- **Color e-paper** - ACeP or Kaleido support
  - 7-color e-paper displays
  - Enhanced cover art and graphics
  - Configurable color vs B&W modes
- **Frontlight/backlight** - Reading in the dark
  - PWM-controlled LED frontlight
  - Adjustable brightness
  - Warm light vs cool light options

#### Audio Features
- **Audio book playback** - Listen to books
  - MP3, M4A, M4B support
  - Playback controls (play, pause, skip)
  - Bookmark audio position
  - Speed adjustment (0.75x to 2x)
- **Text-to-speech** - Read books aloud
  - eSpeak or similar TTS engine
  - Adjustable voice and speed
  - Read while displaying text

#### Additional Formats
- **Comic books** - CBZ/CBR support
  - Image extraction and display
  - Panel-by-panel navigation
  - Zoom and pan controls
- **MOBI/AZW3** - Additional e-book formats
  - Amazon Kindle formats (if legally possible)
  - Fallback to Calibre conversion
- **Markdown** - Technical documentation
  - Render markdown files with formatting
  - Syntax highlighting for code blocks
  - Table of contents from headers

#### Cloud & Sync (Optional, Privacy-First)
- **Cloud sync** - Sync bookmarks and positions
  - Self-hosted sync server (Nextcloud, etc.)
  - No third-party cloud dependencies
  - End-to-end encryption
  - Opt-in only, fully disable by default
- **Calibre integration** - Library management
  - Sync with Calibre library
  - Metadata sync
  - Wireless transfer

#### Multi-Language Support
- **UI translations** - Internationalization
  - Support for 10+ languages
  - Language selection in settings
  - Community-contributed translations
- **Right-to-left text** - Arabic, Hebrew support
  - RTL text rendering
  - RTL UI layout
  - Bidirectional text support

### Estimated Effort
- **Medium tasks** (1 week): Additional formats, UI translations
- **Large tasks** (2-4 weeks): Touchscreen support, larger displays, audio playback
- **Very large tasks** (1-2 months): Color e-paper, cloud sync, TTS integration

---

## Long-Term Vision

### Platform Expansion
- **Raspberry Pi 4/5 support** - More powerful hardware
  - Faster boot and processing
  - Support for larger, high-res displays
  - Simultaneous WiFi and Bluetooth
- **Alternative SBCs** - ODROID, Orange Pi, etc.
  - Broader hardware compatibility
  - Cost and availability options
- **Custom PCB** - Integrated board design
  - Single board with Pi Compute Module
  - Integrated display driver and buttons
  - Professional form factor

### Advanced Features
- **Web browser** - Lightweight article reading
  - Read web articles offline
  - Save pages for later
  - Reader mode for clean text
- **RSS reader** - Subscribe to blogs and news
  - Offline article sync
  - Read feeds like a book
- **Email client** - Simple text email
  - Read emails on e-paper (easier on eyes)
  - Compose and send basic emails
  - IMAP/POP3 support

### Developer Tools
- **Plugin system** - Extend functionality
  - Loadable modules for features
  - Community-contributed plugins
  - Safe sandboxing
- **Scripting support** - Lua or Python scripts
  - Automate tasks
  - Custom reading modes
  - Data export and analysis

### Education & Accessibility
- **Educational features** - Learning tools
  - Flash cards and quiz mode
  - Language learning tools
  - Math formula rendering
- **Accessibility** - Support for users with disabilities
  - Screen reader integration
  - High contrast modes
  - Adjustable UI scaling
  - Voice control (optional)

---

## Community Requests

Features requested by users and contributors. Vote on features by commenting on GitHub issues.

### High Priority (Many Requests)
- Partial display refresh (planned for 1.1)
- Better PDF rendering (planned for 1.1)
- Dictionary integration (planned for 1.2)
- Note-taking and highlights (planned for 1.2)
- USB file transfer (planned for 1.1)

### Medium Priority (Some Interest)
- Calibre integration
- Comic book support (CBZ)
- Touchscreen option
- MOBI format support
- Reading in bed mode (rotate display 180°)

### Experimental (Under Consideration)
- Bluetooth keyboard support
- Export reading data (CSV, JSON)
- Integration with Goodreads/LibraryThing
- Built-in book metadata editor
- Support for scanned book PDFs (OCR)

---

## How to Contribute

Want to help shape the future of this project?

### Suggest Features
1. Check [GitHub Issues](https://github.com/[username]/open-source-ereader/issues) for existing requests
2. If not found, create a new issue with the "Feature Request" label
3. Describe the feature, use case, and proposed implementation
4. Engage with community discussion

### Implement Features
1. Review this roadmap and pick a feature
2. Check if someone is already working on it (comment on issue)
3. Follow the [Contributing Guide](CONTRIBUTING.md) for development setup
4. Submit a pull request when ready
5. We'll review and merge!

### Vote on Priorities
- Comment on feature request issues to show interest
- React with 👍 to features you want
- Share your use cases and requirements

### Sponsor Development
- Sponsor the project on GitHub Sponsors (if configured)
- Fund specific features via bounties
- Donate hardware for testing (larger displays, touch screens)

---

## Release Philosophy

### Guiding Principles
- **Stability first** - New features should not break existing functionality
- **Privacy by default** - All network features are opt-in
- **Open source forever** - No proprietary components or services
- **Community-driven** - User feedback shapes priorities
- **Educational value** - Code should be readable and well-documented

### Release Cycle
- **Major versions (X.0)** - Breaking changes, major new features
- **Minor versions (X.Y)** - New features, backwards-compatible
- **Patch versions (X.Y.Z)** - Bug fixes, small improvements

Expected cadence:
- Patch releases: As needed (bug fixes)
- Minor releases: 2-3 times per year
- Major releases: Once every 1-2 years

---

## Notes

This roadmap is a living document and will evolve based on:
- Community feedback and feature requests
- Contributor availability and skills
- Hardware ecosystem changes (new displays, SBCs)
- Upstream project updates (Buildroot, kernel, libraries)

Last updated: 2026-01-18

**Questions or suggestions?** Open an issue or discussion on GitHub!
