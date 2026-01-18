# Open Source E-Reader

**Build your own Kindle-like e-reader from scratch!** This open-source project transforms a $15 Raspberry Pi Zero W and a $30 e-paper display into a fully functional e-reader that boots in seconds, reads EPUB/PDF/TXT files, connects to WiFi for downloading books, and provides a distraction-free reading experience with week-long battery life.

---

## What You'll Build

A complete e-reading device that:

- **Reads multiple formats** - EPUB, PDF, and TXT files natively
- **Boots fast** - 10-second startup from power-on to your library
- **Navigates easily** - Physical buttons (UP, DOWN, SELECT, BACK, MENU)
- **Displays beautifully** - 400×300 pixel e-paper screen, sunlight-readable
- **Remembers your place** - Automatic bookmarks and reading position
- **Connects to WiFi** - Download books from Project Gutenberg (70,000+ free titles)
- **Lasts weeks** - E-paper technology provides exceptional battery life
- **Respects privacy** - No tracking, no DRM, no cloud dependency

**Total Cost:** $50-70 • **Build Time:** 4-8 hours • **Skill Level:** Intermediate

---

## Key Features

### Reading Experience
- Multi-format support: EPUB, PDF, TXT
- Automatic pagination with word wrapping
- Smart bookmarks that save on every page turn
- Reading statistics (pages read, time, progress)
- Customizable fonts, margins, and line spacing
- Full-text search across your library

### Hardware & Performance
- 10-second boot time to library
- Week-long battery life with e-paper display
- Sunlight-readable screen (reflects light like paper)
- No eye strain during extended reading
- Physical button navigation (no touchscreen fingerprints)
- Expandable storage (microSD up to 512GB)

### Network & Privacy
- WiFi connectivity for book downloads
- Built-in Project Gutenberg browser
- Automatic time synchronization
- **No tracking or telemetry**
- **No DRM restrictions**
- **All books stored locally**

### Open Source & Customization
- 100% open source (code, hardware, docs)
- Fully hackable and customizable
- Add features, sensors, integrations
- Learn embedded Linux, drivers, C programming
- Repairable and upgradeable

---

## Hardware Requirements

### Core Components (~$60)

| Component | Description | Est. Cost |
|-----------|-------------|-----------|
| **Raspberry Pi Zero W** | 700MHz ARM CPU, 512MB RAM, WiFi | $15 |
| **Waveshare 4.2" E-Paper** | 400×300 B&W display (Rev 2.2) | $30 |
| **GPIO Buttons** | 5× tactile push buttons | $2 |
| **MicroSD Card** | 8GB minimum, Class 10 | $8 |
| **Power Supply** | 5V 2A micro-USB | $5 |

### Optional Components

- **Battery** - 18650 Li-ion + boost converter for portable use (~$20)
- **Case** - 3D printed enclosure or project box (~$10-40)
- **Tools** - Soldering iron, wire cutters, multimeter (~$50-90 if purchasing)

See the complete [Bill of Materials](docs/hardware/BOM.md) for detailed part numbers, suppliers, and alternatives.

---

## Quick Start

### Option 1: Pre-Built Image (Fastest)

1. **Download** the [latest release image](https://github.com/cmclark00/open-source-ereader/releases)
2. **Flash** to microSD card using [balenaEtcher](https://www.balena.io/etcher/)
3. **Wire** hardware following the [Wiring Guide](docs/hardware/WIRING_GUIDE.md)
4. **Add books** to the `/books/` directory on the SD card
5. **Boot** and start reading!

**Time required:** 1-2 hours

### Option 2: Build from Source (Full Experience)

```bash
# 1. Clone repository
git clone https://github.com/cmclark00/open-source-ereader.git
cd open-source-ereader

# 2. Configure Buildroot
cd buildroot
make ereader_rpi0w_defconfig

# 3. Build system (1-2 hours first time)
make

# 4. Flash to SD card
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
sync
```

**Time required:** 4-8 hours (including compilation)

See the [Build Guide](docs/BUILD_GUIDE.md) for detailed step-by-step instructions.

---

## Documentation

### Getting Started
- **[Build Guide](docs/BUILD_GUIDE.md)** - Complete assembly instructions from ordering parts to first boot
- **[User Guide](docs/USER_GUIDE.md)** - How to use your e-reader, button controls, adding books
- **[Flashing Guide](docs/deployment/FLASHING_GUIDE.md)** - Flash SD card images on Windows, macOS, Linux

### Hardware
- **[Bill of Materials](docs/hardware/BOM.md)** - Complete parts list with suppliers and pricing
- **[Wiring Guide](docs/hardware/WIRING_GUIDE.md)** - Pin connections for display and buttons
- **[Schematics](docs/hardware/SCHEMATIC.md)** - Complete electrical schematics and diagrams
- **[Case Design](docs/hardware/CASE_DESIGN.md)** - 3D printable case and enclosure options
- **[Display Specs](docs/hardware/DISPLAY_SPECS.md)** - E-paper display technical details
- **[Power Supply](docs/hardware/POWER_SUPPLY.md)** - Battery options and power management

### Software & Development
- **[System Overview](docs/architecture/SYSTEM_OVERVIEW.md)** - Complete architecture documentation
- **[Build Process](docs/buildroot/BUILD_PROCESS.md)** - Building from source with Buildroot
- **[Contributing Guide](docs/CONTRIBUTING.md)** - How to contribute code and documentation
- **[Development Guide](docs/DEVELOPMENT.md)** - Add features, modify UI, extend functionality
- **[Testing](docs/testing/TEST_PLAN.md)** - Complete test plan and quality assurance

---

## Project Status

**Current Version:** 1.0.0 - Release Candidate
**Status:** All 7 development phases complete, ready for public release

### What's Working
- ✅ Complete Buildroot Linux system (boots in ~10 seconds)
- ✅ E-paper display driver with full SPI communication
- ✅ GPIO button input with debouncing
- ✅ EPUB, PDF, and TXT format support
- ✅ Book library management with sorting and searching
- ✅ Text rendering with word wrapping and pagination
- ✅ Menu system and reading interface
- ✅ Bookmark persistence across reboots
- ✅ WiFi connectivity and configuration
- ✅ Online book downloads (Project Gutenberg integration)
- ✅ Power management and battery monitoring
- ✅ Settings menu (fonts, margins, display options)
- ✅ Reading statistics and progress tracking
- ✅ Full-text search engine

### Known Limitations
- Screen size is smaller than commercial e-readers (4.2" vs 6")
- Display resolution is 400×300 (lower than modern Kindles)
- Boot time is ~10 seconds (vs 3-5 seconds for commercial devices)
- Battery life is 1-2 weeks (vs 2-4 weeks for optimized commercial devices)

See [CHANGELOG.md](CHANGELOG.md) for version history and roadmap.

---

## Development Phases

The project was built incrementally through 7 phases:

1. **Phase 01: Foundation** ✅ - Buildroot Linux, e-paper display driver, basic graphics
2. **Phase 02: Input System** ✅ - GPIO buttons, event handling, debouncing
3. **Phase 03: Basic Reader** ✅ - Text rendering, library, menus, bookmarks
4. **Phase 04: Advanced Formats** ✅ - EPUB and PDF parsing and rendering
5. **Phase 05: Polish** ✅ - Settings, search, statistics, UI improvements
6. **Phase 06: WiFi** ✅ - Network connectivity, downloads, OTA updates
7. **Phase 07: Release** ✅ - Documentation, testing, packaging, launch

Each phase is fully documented in `docs/progress/` with detailed logs and learnings.

---

## Why Build This?

### Learn by Doing
This project is a comprehensive tutorial in embedded systems:
- **Embedded Linux** - Kernel configuration, device trees, minimal filesystems
- **Hardware interfacing** - SPI communication, GPIO handling, display drivers
- **C programming** - Text rendering, pagination, state machines, memory management
- **File formats** - EPUB structure, PDF parsing, XML processing
- **System design** - Boot optimization, power management, architecture

### Own Your Technology
Unlike commercial e-readers:
- **No vendor lock-in** - Use any e-book source, no ecosystem dependency
- **Privacy-respecting** - No tracking, telemetry, or reading habit monitoring
- **DRM-free** - Read any file you own without restrictions
- **Fully transparent** - See exactly how every component works
- **Customizable** - Modify the UI, add features, integrate with other systems
- **Repairable** - Fix and upgrade yourself, no proprietary parts

### Build Something Unique
Create a device you can't buy in stores:
- Customize the reading experience to your exact preferences
- Add features commercial devices don't offer
- Integrate with home automation or other projects
- Learn valuable skills while building a practical tool
- Join a community of builders and hackers

---

## Comparison: DIY vs Commercial E-Readers

| Feature | This Project | Amazon Kindle | Kobo |
|---------|--------------|---------------|------|
| **Cost** | ~$60 (DIY) | $100-$300 | $100-$250 |
| **Screen** | 4.2" (400×300) | 6"-7" | 6"-10" |
| **Formats** | EPUB, PDF, TXT | KFX, MOBI, AZW3 | EPUB, PDF, MOBI |
| **DRM** | None | Required | Optional |
| **Open Source** | 100% | No | No |
| **Privacy** | No tracking | Extensive tracking | Some tracking |
| **Customizable** | Fully | Very limited | Limited |
| **Repairable** | Yes | Difficult | Difficult |
| **Battery** | 1-2 weeks | 2-4 weeks | 2-4 weeks |
| **Boot Time** | ~10 seconds | ~3-5 seconds | ~3-5 seconds |
| **Learning Value** | High | None | None |

**Choose this project if you value:** Transparency, customization, privacy, learning, control
**Choose commercial if you prioritize:** Convenience, larger screen, longer battery, faster boot

---

## License

This project is released under open-source licenses:

- **Software** - [GPL v3](LICENSE) (or specify your chosen license)
- **Hardware** - CERN Open Hardware License (or specify)
- **Documentation** - Creative Commons BY-SA 4.0 (or specify)

See [docs/LEGAL.md](docs/LEGAL.md) for complete licensing information and third-party attributions.

---

## Contributing

This is an open-source educational project. Contributions are welcome!

**Ways to contribute:**
- 🐛 Report bugs or issues
- 💡 Suggest new features
- 📝 Improve documentation
- 🔧 Submit pull requests
- 🌍 Translate documentation
- 📸 Share photos of your build
- 🎓 Create tutorials or guides

See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for guidelines and development setup.

---

## Community & Support

**Questions or issues?**
- 📖 Check the [User Guide](docs/USER_GUIDE.md) and [Build Guide](docs/BUILD_GUIDE.md)
- 🔧 Review [Troubleshooting](docs/BUILD_GUIDE.md#troubleshooting) section
- 🐛 Search or file [GitHub Issues](https://github.com/cmclark00/open-source-ereader/issues)
- 💬 Join discussions on [GitHub Discussions](https://github.com/cmclark00/open-source-ereader/discussions)

**Share your build:**
- Post on [Reddit /r/raspberry_pi](https://reddit.com/r/raspberry_pi)
- Share on [Hackaday.io](https://hackaday.io/)
- Tag on Twitter/X with #OpenSourceEReader

---

## Roadmap

**Version 1.0** (Current)
- All core reading features
- Multi-format support
- WiFi connectivity
- Complete documentation

**Future Ideas** (Community-driven)
- Touchscreen support (alternative input method)
- Larger display options (5.83", 7.5")
- Audio book playback
- Dictionary integration
- Note-taking and annotations
- Cloud sync (optional, privacy-respecting)
- Multi-language UI
- Additional e-book formats (CBZ, MOBI)

See [docs/ROADMAP.md](docs/ROADMAP.md) for detailed future plans.

---

## Acknowledgments

This project builds on excellent work from:

- **Waveshare** - E-paper display modules and comprehensive documentation
- **Buildroot Project** - Embedded Linux build system
- **Raspberry Pi Foundation** - Affordable, capable hardware platform
- **Open Source Community** - Libraries and tools (libzip, libxml2, Poppler, wpa_supplicant)

Special thanks to all contributors, testers, and early builders!

---

## Get Started Today

Ready to build your own e-reader?

1. 📋 Review the [Bill of Materials](docs/hardware/BOM.md) and order parts (~$60)
2. 📥 Download the [pre-built image](https://github.com/cmclark00/open-source-ereader/releases) or build from source
3. 🔧 Follow the [Build Guide](docs/BUILD_GUIDE.md) for step-by-step assembly
4. 📚 Add books and start reading on a device you built and truly own!

**Questions?** Open an [issue](https://github.com/cmclark00/open-source-ereader/issues) or check the [documentation](docs/).

---

**Join the community of builders who want to understand and control their technology!**

*Built with ❤️ by the open-source community*
