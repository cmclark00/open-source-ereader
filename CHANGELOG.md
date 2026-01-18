# Changelog

All notable changes to the Open Source E-Reader project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-01-18 - Release Candidate

### Summary
First public release of the Open Source E-Reader project. Complete feature set for a functional e-reading device with multi-format support, WiFi connectivity, and comprehensive documentation.

### Added - Features
- **Multi-format book support**: EPUB, PDF, and TXT file reading
- **E-paper display driver**: Custom SPI driver for Waveshare 4.2" e-paper (IL0398/UC8176)
- **Text rendering engine**: Word wrapping, pagination, multiple font sizes
- **Book library management**: Automatic scanning, sorting, and organization
- **Menu system**: Scrollable library, settings, WiFi configuration
- **Bookmark system**: Automatic position saving, resume on book reopening
- **WiFi connectivity**: WPA/WPA2 network support, DHCP configuration
- **Online book downloads**: Project Gutenberg browser and download manager
- **Power management**: Sleep mode, display refresh optimization
- **Battery monitoring**: Voltage reading, charge level estimation (optional INA219)
- **Settings menu**: Font size, margins, display options, power settings
- **Reading statistics**: Pages read, time tracking, progress percentage
- **Full-text search**: Search across entire library or current book
- **OTA updates**: Over-the-air system updates via WiFi (optional)

### Added - Documentation
- Complete BUILD_GUIDE.md with step-by-step assembly instructions
- Comprehensive USER_GUIDE.md covering all features and usage
- Bill of Materials (BOM.md) with verified part numbers and suppliers
- Wiring guide with GPIO pinout and connection diagrams
- Complete hardware schematics and electrical specifications
- Case design documentation with 3D printable models
- System architecture documentation (SYSTEM_OVERVIEW.md)
- Developer documentation (CONTRIBUTING.md, DEVELOPMENT.md)
- Code documentation with file headers and function comments
- Marketing materials and project descriptions
- Testing plans and procedures

### Added - System Components
- **Buildroot configuration**: Custom minimal Linux (kernel 6.1 LTS)
- **Device tree overlays**: SPI, GPIO, I2C configuration
- **Init system**: Fast boot sequence (~10 seconds)
- **Root filesystem**: Minimal overlay with e-reader application
- **Network stack**: wpa_supplicant, dhcpcd, wget, ntpd

### Technical Details
- **Boot time**: ~10 seconds from power-on to library
- **Memory usage**: 4-7 MB typical application footprint
- **Display**: 400×300 pixel black/white e-paper
- **Input**: 5 GPIO buttons (UP, DOWN, SELECT, BACK, MENU)
- **Storage**: MicroSD card (8GB minimum)
- **Power**: 5V micro-USB or optional 18650 battery
- **WiFi**: Built-in on Pi Zero W, WPA/WPA2 support

### Known Limitations
- Screen size: 4.2" (smaller than commercial 6" e-readers)
- Resolution: 400×300 (lower than modern commercial devices)
- Boot time: ~10 seconds (vs 3-5 seconds for commercial devices)
- Battery life: 1-2 weeks typical (vs 2-4 weeks for optimized commercial readers)
- Maximum book size: Limited by available RAM (~50MB for large PDFs)
- Display refresh: Full refresh only (no partial updates)
- PDF rendering: Text extraction only (no complex layout preservation)

### Build System
- Buildroot 2023.02 LTS
- Linux kernel 6.1 LTS
- GCC 12.2 toolchain
- Custom BR2_EXTERNAL structure
- Pre-built SD card images available

### Dependencies
- libzip (EPUB container handling)
- libxml2 (EPUB metadata parsing)
- Poppler (PDF text extraction)
- wpa_supplicant (WiFi authentication)
- dhcpcd (DHCP client)

---

## Development History

### Phase 07: Release Preparation (2026-01-15 to 2026-01-18)
- Created comprehensive build guide and documentation
- Designed case/enclosure with 3D printable models
- Generated hardware schematics and wiring diagrams
- Compiled complete Bill of Materials with suppliers
- Wrote system architecture documentation
- Created developer documentation (CONTRIBUTING, DEVELOPMENT)
- Added comprehensive code comments and documentation
- Prepared marketing materials and project descriptions
- Polished README as professional project landing page

### Phase 06: WiFi and Network Features (2026-01-12 to 2026-01-15)
- Implemented WiFi connection manager with UI
- Added WPA/WPA2 network authentication
- Created Project Gutenberg book browser
- Implemented download manager for online books
- Added NTP time synchronization
- Implemented OTA update system (optional)
- Created WiFi testing and validation procedures

### Phase 05: Polish and Advanced Features (2026-01-10 to 2026-01-12)
- Added settings menu (fonts, margins, display)
- Implemented reading statistics tracking
- Created full-text search engine
- Added power management with sleep modes
- Implemented battery monitoring (optional)
- Enhanced UI with progress indicators
- Optimized display refresh patterns
- Improved error handling and user feedback

### Phase 04: EPUB and PDF Support (2026-01-08 to 2026-01-10)
- Implemented EPUB parser (libzip + libxml2)
- Added PDF text extraction (Poppler)
- Created format detection and handling
- Enhanced text renderer for HTML content
- Added metadata extraction for both formats
- Implemented format-specific pagination

### Phase 03: Basic E-Reader Application (2026-01-05 to 2026-01-08)
- Built text rendering engine with pagination
- Created book library scanner and manager
- Implemented menu system with scrolling
- Added reading view with page navigation
- Created bookmark persistence system
- Implemented boot-to-reader experience

### Phase 02: GPIO Button Input (2026-01-03 to 2026-01-05)
- Implemented GPIO button driver with linux input subsystem
- Added hardware debouncing via gpio-keys
- Created event-based input handling
- Built button test application
- Documented input system architecture

### Phase 01: Foundation and Working Display (2025-12-28 to 2026-01-03)
- Set up Buildroot build environment
- Configured Linux kernel 6.1 LTS
- Implemented e-paper display SPI driver
- Created framebuffer graphics library
- Built "Hello E-Reader" test application
- Optimized boot sequence

---

## Future Roadmap

### Version 1.1 (Planned)
- Partial display refresh support (faster page turns)
- Improved PDF rendering with layout preservation
- Grayscale display support (4-color e-paper)
- USB OTG file transfer mode
- Low battery warnings and graceful shutdown
- Additional font options
- Performance optimizations

### Version 1.2 (Planned)
- Dictionary integration (word lookup)
- Note-taking and annotations
- Highlighting and bookmarks (multiple per book)
- Enhanced search with regex support
- Library organization (categories, tags)
- Reading goals and achievements

### Version 2.0 (Future Ideas)
- Touchscreen support as alternative input
- Larger display options (5.83", 7.5", 10.3")
- Audio book playback (MP3, M4B)
- Multi-language UI and translations
- Cloud sync (privacy-respecting, optional)
- Additional formats (CBZ comics, MOBI, AZW3)
- E-ink color display support
- Frontlight/backlight support

See [docs/ROADMAP.md](docs/ROADMAP.md) for detailed future plans and community suggestions.

---

## Version History

- **1.0.0** - 2026-01-18 - First public release, all core features complete
- **0.7.0** - 2026-01-15 - Phase 06 complete, WiFi and network features
- **0.6.0** - 2026-01-12 - Phase 05 complete, settings and advanced features
- **0.5.0** - 2026-01-10 - Phase 04 complete, EPUB/PDF support
- **0.4.0** - 2026-01-08 - Phase 03 complete, basic e-reader functional
- **0.3.0** - 2026-01-05 - Phase 02 complete, button input working
- **0.2.0** - 2026-01-03 - Phase 01 complete, display driver functional
- **0.1.0** - 2025-12-28 - Project initiated, Buildroot baseline

---

*For detailed phase logs and implementation notes, see `docs/progress/PHASE_XX_LOG.md` files.*
