---
type: reference
title: Press Kit and Media Resources
created: 2026-01-17
tags:
  - marketing
  - press
  - media
related:
  - "[[DEMO_CONTENT_GUIDE]]"
  - "[[MARKETING_COPY]]"
---

# Open Source E-Reader - Press Kit

Media resources, fact sheet, and press materials for the Open Source E-Reader project.

---

## Quick Facts

**Project Name:** Open Source E-Reader
**Status:** Open source, actively developed
**License:** GPL v3 / MIT (software), CERN OHL (hardware), CC BY-SA (docs)
**Repository:** [GitHub URL]
**Website:** [Project Website URL]
**First Release:** [Date]
**Current Version:** v1.0.0

**Build Cost:** $70-160 USD
**Build Time:** 4-6 hours (hardware) + 1-2 hours (software)
**Boot Time:** ~15 seconds
**Battery Life:** 12-20 hours active reading

---

## Project Summary

### One-Paragraph Description

The Open Source E-Reader is a complete DIY e-reader platform built on Raspberry Pi Zero W and e-paper display technology. The project provides comprehensive build guides, hardware schematics, PCB designs, and complete source code for building a functional reading device for $70-160. The device boots in 15 seconds, supports TXT/EPUB/PDF formats, features WiFi connectivity for downloading books, and offers 12-20 hours battery life in portable configuration. Everything is open source, enabling users to inspect, modify, and improve the design.

### Elevator Pitch

Build your own e-reader for under $100. Open source hardware, open source software, complete control. No ads, no tracking, just reading.

---

## Technical Specifications

### Hardware

| Component | Specification |
|-----------|---------------|
| **Processor** | Raspberry Pi Zero W - BCM2835 SoC, ARM11 700MHz |
| **Memory** | 512MB LPDDR2 RAM |
| **Display** | Waveshare 4.2" E-Paper Module (Rev 2.2) |
| **Resolution** | 400×300 pixels (100 DPI) |
| **Display Type** | E-Ink / E-Paper, black and white, 1-bit |
| **Display Interface** | SPI (Serial Peripheral Interface) |
| **Refresh Rate** | ~2s full refresh, ~0.8s partial refresh |
| **Input** | 5× GPIO tactile push buttons |
| **Storage** | MicroSD card (8GB minimum, 32GB recommended) |
| **Connectivity** | WiFi 802.11 b/g/n (2.4GHz), Bluetooth 4.1 (unused) |
| **Power Input** | 5V 2A micro-USB |
| **Battery (optional)** | 18650 lithium-ion (2000-3000mAh) |
| **Power Consumption** | ~150mA active reading, <10mA standby |
| **Battery Life** | 12-20 hours active, weeks standby |
| **Dimensions** | ~115-120mm × 165-175mm × 18-22mm (depends on case) |
| **Weight** | ~150-250g (with battery and case) |

### Software

| Component | Specification |
|-----------|---------------|
| **Operating System** | Custom Buildroot Linux (minimal) |
| **Kernel** | Linux 6.1 LTS |
| **Init System** | Custom minimal init (direct to application) |
| **Application** | Custom C-based e-reader (v1.0) |
| **Boot Time** | ~15 seconds (power-on to library) |
| **Image Size** | ~50MB compressed, ~200MB uncompressed |
| **Supported Formats** | TXT, EPUB (HTML/CSS parsing), PDF (text extraction) |
| **Font Rendering** | 8×16 bitmap font (extensible to TrueType) |
| **Graphics** | Direct framebuffer access, custom rendering engine |
| **Networking** | wpa_supplicant for WiFi, wget/curl for downloads |

---

## Features List

### Core Features
- ✅ 15-second boot directly to book library
- ✅ 400×300 pixel e-paper display (readable in sunlight)
- ✅ Physical button navigation (5 buttons: UP, DOWN, SELECT, BACK, MENU)
- ✅ TXT, EPUB, and PDF format support
- ✅ Automatic text pagination and word wrapping
- ✅ Automatic bookmark saving and resumption
- ✅ WiFi connectivity and configuration
- ✅ Direct book downloading from online sources (Project Gutenberg, etc.)
- ✅ Book search and filtering
- ✅ Reading statistics tracking
- ✅ Power management and battery monitoring
- ✅ 12-20 hours battery life (portable configuration)

### Open Source Components
- ✅ Complete build guide with step-by-step instructions
- ✅ Bill of materials with supplier links and pricing
- ✅ Hardware schematics (Fritzing/KiCad source files)
- ✅ PCB designs and wiring diagrams
- ✅3D printable case designs (STL files)
- ✅ Complete source code (C, shell scripts, build configs)
- ✅ Developer documentation and API reference
- ✅ Contribution guidelines and coding standards

---

## Use Cases

### Primary Use Cases

**Distraction-Free Reading**
- No ads, notifications, or tracking
- Single-purpose device focused on reading
- Privacy-focused (all data stays local)

**Education and Learning**
- Embedded Linux development
- Hardware interfacing (SPI, GPIO)
- Display driver programming
- Power management optimization
- Real-world systems programming in C

**Maker Projects**
- Weekend hardware project
- Soldering practice
- 3D printing and case design
- Custom firmware development

**Accessibility and Customization**
- Modify interface for specific needs
- Add custom features (text-to-speech, etc.)
- Create specialized reading devices
- Develop alternative input methods

---

## Target Audiences

### Primary Audiences

**Makers and DIY Enthusiasts (40%)**
- Want hands-on hardware projects
- Enjoy building and customizing devices
- Value understanding how things work

**Students and Educators (25%)**
- Learning embedded systems
- Studying computer engineering
- Teaching hardware/software integration

**Privacy-Conscious Readers (20%)**
- Want control over personal data
- Avoid tracking and telemetry
- Prefer open source solutions

**Open Source Advocates (15%)**
- Support open hardware movement
- Contribute to community projects
- Value software freedom

---

## Project Timeline

| Phase | Status | Description |
|-------|--------|-------------|
| **Phase 01** | ✅ Complete | Foundation and working display |
| **Phase 02** | ✅ Complete | GPIO button input system |
| **Phase 03** | ✅ Complete | Basic e-reader application |
| **Phase 04** | ✅ Complete | EPUB and PDF support |
| **Phase 05** | ✅ Complete | Polish and advanced features |
| **Phase 06** | ✅ Complete | WiFi and network features |
| **Phase 07** | 🚧 In Progress | Release preparation and launch |

---

## Cost Breakdown

### Minimum Build (~$70)
- Raspberry Pi Zero W: $15
- Waveshare 4.2" E-Paper Display: $28
- 5× Tactile Buttons: $2-3
- MicroSD Card 8GB: $8
- Cables and Misc: $15-20
- **Total: ~$70** (USB-powered, no case)

### Complete Portable Build (~$110-160)
- All minimum components: $70
- 18650 Battery + Holder: $10-15
- Battery Charger/Boost Converter: $15-25
- 3D Printed Case (material): $5-15
- OR Commercial Project Box: $8-12
- **Total: ~$110-160**

### First-Time Builder (~$200-250)
- Complete portable build: $140
- Soldering iron kit: $30-50
- Multimeter: $20-30
- Wire strippers/cutters: $10-15
- **Total: ~$200-250** (includes tools)

---

## Media Assets

### Photography

**Available Images:**
- `docs/images/hero-ereader.jpg` - Main product photo (high-resolution)
- `docs/images/components-layout.jpg` - All components before assembly
- `docs/images/wiring-detail.jpg` - Close-up of wiring connections
- `docs/images/display-closeup.jpg` - Text rendering detail
- `docs/images/ereader-front.jpg` - Front view of assembled device
- `docs/images/ereader-side.jpg` - Side profile view
- `docs/images/ereader-back.jpg` - Back view showing internals
- `docs/images/ereader-in-hand.jpg` - Scale and ergonomics
- `docs/images/comparison-size.jpg` - Size comparison with common objects
- `docs/images/comparison-display.jpg` - Display vs commercial e-reader

**Image Usage:**
- High-resolution JPEGs (3000×2000+)
- Licensed under CC BY-SA 4.0
- Attribution required: "Open Source E-Reader Project"

### Video Content

**Available Videos:**
- `docs/videos/ereader-demo-full-v1.mp4` - Complete demo (2-4 minutes)
- `docs/videos/ereader-demo-boot-v1.mp4` - Boot sequence showcase
- `docs/videos/ereader-demo-reading-v1.mp4` - Reading experience
- `docs/videos/ereader-demo-wifi-v1.mp4` - WiFi features

**Video Specifications:**
- Format: MP4 (H.264)
- Resolution: 1920×1080 (1080p)
- Frame Rate: 30fps
- Licensed under CC BY-SA 4.0

### Animated GIFs

**Available Animations:**
- `docs/images/boot-sequence.gif` - Boot from power-on to library
- `docs/images/page-turning.gif` - Page navigation demonstration
- `docs/images/menu-navigation.gif` - Library browsing
- `docs/images/settings-demo.gif` - Settings adjustment

**GIF Specifications:**
- Resolution: 640×480 to 800×600
- Frame Rate: 10-15fps
- File Size: <5MB each
- Optimized for web embedding

---

## Quotes and Testimonials

### Project Creator Quote
```
"I wanted to build an e-reader that I truly owned - no ads, no tracking, no remote deletion of books. The Open Source E-Reader project is the result: a device that does exactly what you tell it to do, and nothing more. Everything is transparent, modifiable, and under your control."

- [Creator Name], Project Lead
```

### Sample User Testimonials

**Note:** Collect actual testimonials after launch. Template examples:

```
"I built this as a weekend project and learned more about embedded Linux than I did in a semester of classes. Plus, I have a functional e-reader I can hack on forever."
- Alex, Computer Science Student
```

```
"No more ads on my lockscreen, no more Amazon tracking what I read. Just my books and me. It's refreshing to own a device that respects my privacy."
- Sarah, Privacy Advocate
```

```
"The build guide is incredibly thorough. I've never soldered before, but I followed the instructions and had a working e-reader in an afternoon. Proud of what I built."
- Marcus, First-Time Builder
```

---

## Comparison Chart

### Open Source E-Reader vs. Commercial E-Readers

| Feature | Open Source E-Reader | Kindle Paperwhite | Kobo Clara |
|---------|---------------------|-------------------|------------|
| **Price** | $70-160 (DIY) | $140 | $130 |
| **Display** | 4.2" 400×300 | 6.8" 1264×1648 | 6" 1072×1448 |
| **Formats** | TXT, EPUB, PDF | AZW, MOBI (KFX) | EPUB, PDF, MOBI |
| **DRM Support** | No (by design) | Yes (Amazon) | Yes (Adobe) |
| **Ads** | None | Optional | None |
| **Tracking** | None | Yes | Minimal |
| **Open Source** | Yes (all) | No | Partial |
| **Customizable** | Fully | No | Limited |
| **WiFi** | Yes | Yes | Yes |
| **Battery Life** | 12-20 hours | Weeks | Weeks |
| **Backlight** | No (add-on) | Yes | Yes |
| **Waterproof** | No | IPX8 | IPX8 |
| **Build Required** | Yes (4-6 hours) | No | No |
| **Learning Value** | High | None | None |

**Summary:** Commercial e-readers have superior hardware integration, battery life, and polish. The Open Source E-Reader offers transparency, customization, privacy, and educational value. Different tools for different needs.

---

## Key Messages

### For General Media
- "Build your own e-reader for under $100 with complete open source plans"
- "Distraction-free reading device with no ads, tracking, or DRM"
- "Educational platform for learning embedded Linux and hardware development"

### For Tech Media
- "Complete embedded Linux system boots to reading app in 15 seconds"
- "Direct framebuffer rendering on e-paper display via SPI interface"
- "Minimal Buildroot system optimized for single-purpose device"

### For Maker Media
- "Weekend project combines hardware assembly, soldering, and 3D printing"
- "All schematics, PCB designs, and source code available on GitHub"
- "Perfect project for learning embedded systems and custom firmware"

### For Privacy/FOSS Media
- "Open source alternative to commercial e-readers with no tracking"
- "All code auditable - no black boxes, no telemetry, no vendor lock-in"
- "GPL/MIT licensed software, CERN OHL hardware, CC BY-SA documentation"

---

## Contact Information

**Project Repository:** [GitHub URL]
**Project Website:** [Website URL]
**Documentation:** [Docs URL]
**Community Forum:** [Discussions URL]

**Media Inquiries:**
Email: [Contact Email]
Response Time: 24-48 hours

**Social Media:**
Twitter/X: [@handle]
Reddit: r/[subreddit]
Hackaday.io: [Project Page]
Hackster.io: [Project Page]

---

## Frequently Asked Questions (Press)

**Q: Why build your own e-reader instead of buying a Kindle?**
A: Different use cases. If you want the best reading experience with integrated lighting, waterproofing, and commercial support, buy a Kindle or Kobo. If you want to learn embedded systems, value privacy and transparency, or want complete customization control, build this. Many users have both - a commercial e-reader for daily use and this for technical projects or focused reading.

**Q: Is this a realistic alternative to commercial devices?**
A: For technical users and DIY enthusiasts, yes. For mainstream consumers, no. This project targets makers, students, and privacy-conscious readers who value open source and are willing to assemble hardware. It's not trying to replace commercial e-readers - it's providing an open alternative for those who want one.

**Q: What's the environmental impact?**
A: Building your own device uses standard components and avoids proprietary assemblies, making repair and upgrades easier. The modular design means you can replace individual components rather than the entire device. However, commercial e-readers benefit from economies of scale in manufacturing. Environmental impact depends on usage patterns and longevity.

**Q: Can you make money selling these?**
A: This is an open source project - anyone can build and sell assembled devices under the terms of the licenses (GPL, MIT, CERN OHL). The project maintainers don't sell pre-assembled units. Some community members may offer assembly services or kits - that's encouraged under the open source model.

**Q: What's the long-term vision?**
A: Create a sustainable, community-driven open hardware e-reader platform. Continue improving software, adding features, documenting build processes, and supporting builders. Enable others to fork the design for specialized use cases (education, accessibility, research, art installations, etc.).

---

## Sample Headlines

**General Interest:**
- "Build Your Own E-Reader for Under $100 with This Open Source Project"
- "DIY E-Reader Project Provides Complete Plans for Privacy-Focused Reading Device"
- "Open Source E-Reader Combines Raspberry Pi and E-Paper Display for $70"

**Tech Focus:**
- "Open Source E-Reader Boots Custom Linux to Reading App in 15 Seconds"
- "Raspberry Pi Zero E-Reader Project Releases Complete Hardware and Software Design"
- "Minimal Buildroot Linux Powers Open Source E-Reader Platform"

**Maker Focus:**
- "Weekend Project: Build Your Own E-Reader with Raspberry Pi and E-Paper"
- "Open Source E-Reader Project Includes 3D Printable Case and Build Guide"
- "Learn Embedded Linux Development While Building a Functional E-Reader"

**Privacy Focus:**
- "Open Source E-Reader Offers Ad-Free, Tracking-Free Alternative to Kindle"
- "Build an E-Reader That Respects Your Privacy with Open Source Plans"
- "DIY E-Reader Project Puts Users in Control of Their Reading Data"

---

## Boilerplate Text

### Short Boilerplate (50 words)
```
The Open Source E-Reader is a DIY e-reader platform built on Raspberry Pi Zero W and e-paper display technology. The project provides complete build guides, hardware schematics, and source code for building a functional reading device for $70-160. Everything is open source under GPL/MIT and CERN OHL licenses.
```

### Long Boilerplate (100 words)
```
The Open Source E-Reader is a comprehensive DIY platform for building custom e-reading devices. Combining a Raspberry Pi Zero W with a 4.2" e-paper display, the design creates a minimal, distraction-free reading device that boots in 15 seconds and supports TXT, EPUB, and PDF formats. The project includes detailed build guides, hardware schematics, bill of materials, 3D printable case designs, and complete source code. Total build cost ranges from $70 to $160 depending on configuration. All components are open source: GPL v3/MIT licensed software, CERN OHL hardware designs, and Creative Commons documentation. The project serves makers, students, and privacy-conscious readers seeking alternatives to commercial e-readers.
```

---

## License Information

**Software:**
- Dual-licensed under GNU GPL v3 and MIT License
- Contributors may choose which license to use
- Source code available on GitHub

**Hardware:**
- CERN Open Hardware License v2 (Permissive)
- Schematics, PCB designs, and enclosure designs
- Source files available in repository

**Documentation:**
- Creative Commons Attribution-ShareAlike 4.0 (CC BY-SA 4.0)
- Build guides, tutorials, and written materials
- Photos and videos under same license

**Attribution Required:**
```
Open Source E-Reader Project
Licensed under [applicable license]
https://github.com/[username]/open-source-ereader
```

---

## Download Press Kit

**Complete press kit ZIP includes:**
- This press kit document (PDF and Markdown)
- High-resolution photos (JPEGs, 3000×2000+)
- Demo videos (MP4, 1080p)
- Animated GIFs
- Project logo (PNG, SVG)
- Fact sheet (PDF)
- Technical specifications (PDF)

**Download:** `docs/marketing/press-kit.zip` (available after media assets created)

---

## Updates and News

**Latest Release:** v1.0.0 - [Date]
**Recent Updates:**
- [Date]: Public launch and GitHub release
- [Date]: Complete Phase 07 documentation
- [Date]: WiFi and network features added
- [Date]: EPUB and PDF support implemented

**Upcoming:**
- Community build videos and guides
- Additional case design variations
- Feature additions based on user feedback
- Buildroot version updates

**Subscribe for Updates:**
- GitHub: Watch repository for releases
- RSS: [Blog/News Feed URL]
- Email: [Newsletter signup]

---

*This press kit was last updated: 2026-01-17*

*For the most current information, visit the project repository on GitHub.*
