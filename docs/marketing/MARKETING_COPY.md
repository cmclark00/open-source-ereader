---
type: reference
title: Marketing Copy and Messaging Templates
created: 2026-01-17
tags:
  - marketing
  - copy
  - messaging
related:
  - "[[DEMO_CONTENT_GUIDE]]"
  - "[[README]]"
---

# Marketing Copy and Messaging Templates

This document contains pre-written marketing copy, taglines, descriptions, and messaging for the Open Source E-Reader project. Use these templates for social media, documentation, presentations, and community engagement.

## Table of Contents

1. [Taglines and Slogans](#taglines-and-slogans)
2. [Project Descriptions](#project-descriptions)
3. [Feature Descriptions](#feature-descriptions)
4. [Use Case Narratives](#use-case-narratives)
5. [FAQ Content](#faq-content)
6. [Community Messaging](#community-messaging)
7. [Press Release Template](#press-release-template)

---

## Taglines and Slogans

### Primary Tagline
**"Build Your Own Reading Device"**
- Clear, action-oriented
- Emphasizes DIY nature
- Approachable for makers

### Alternative Taglines

**Short (Twitter bio, etc.):**
- "Open Source E-Reader - Pi Zero + E-Paper"
- "DIY E-Reader - $70 to Build, Free to Modify"
- "Your Library, Your Device, Your Code"
- "Distraction-Free Reading, Open Source Design"

**Medium (Website headers):**
- "Build your own e-reader with Raspberry Pi and e-paper display"
- "Open source e-reader - complete build guide and source code"
- "Take control of your reading with an open hardware e-reader"

**Long (Article headlines):**
- "How to Build Your Own E-Reader with Raspberry Pi Zero W and E-Paper Display for Under $100"
- "Open Source E-Reader Project: Complete Build Guide, Schematics, and Source Code"

---

## Project Descriptions

### Ultra-Short (280 characters - Twitter)
```
Open source e-reader built with Raspberry Pi Zero W + 4.2" e-paper display. Fast boot, physical buttons, TXT/EPUB/PDF support, WiFi downloads. Total cost: $70-160. Complete build guide + source code on GitHub. #RaspberryPi #OpenSource
```

### Short (1-2 sentences)
```
An open-source e-reader built with Raspberry Pi Zero W and e-paper display. Complete build guide, hardware schematics, and source code for creating your own distraction-free reading device for $70-160.
```

### Medium (Paragraph - README intro)
```
Build your own open-source e-reader for under $100. This project combines a Raspberry Pi Zero W with a 4.2" e-paper display to create a minimal, distraction-free reading device that boots directly to your book library in 15 seconds. Navigate with five physical buttons, read TXT/EPUB/PDF files, and download books over WiFi. Everything is open source - complete build guide, hardware schematics, PCB designs, and all source code included. Perfect for makers, students, and anyone who wants complete control over their reading device.
```

### Long (Full description - project pages)
```
The Open Source E-Reader is a complete DIY e-reader platform built on Raspberry Pi Zero W and e-paper display technology. This project provides everything you need to build your own reading device from scratch: detailed build guides, hardware schematics, bill of materials with supplier links, 3D printable case designs, and complete source code.

**Hardware:**
The design uses a Raspberry Pi Zero W single-board computer ($15), Waveshare 4.2" e-paper display module ($28), five GPIO push buttons ($2-4), and optional battery pack for portable use. Total build cost ranges from $70 for a basic USB-powered version to $160 for a premium portable build with battery and 3D-printed case.

**Software:**
The e-reader runs a custom Buildroot-based minimal Linux system that boots in approximately 15 seconds directly to your book library. The reading application is written in C and provides text rendering, pagination, automatic bookmarking, WiFi configuration, and book downloading capabilities. Supports plain text (TXT), EPUB, and PDF formats.

**Features:**
- Fast 15-second boot directly to library
- 400×300 pixel e-paper display - easy on eyes, readable in sunlight
- Physical button navigation - no touchscreen complexity
- TXT, EPUB, and PDF format support
- WiFi connectivity for downloading books
- Automatic bookmark persistence
- 12-20 hours battery life (portable configuration)
- Fully customizable and hackable

**Open Source:**
All components of this project are open source. Software is dual-licensed under GPL v3 / MIT. Hardware designs use CERN OHL. Documentation is Creative Commons BY-SA 4.0. Fork it, modify it, improve it, and share your changes with the community.

**Use Cases:**
- Distraction-free reading without ads or tracking
- Learning embedded Linux development and hardware interfacing
- Educational project for electronics and programming students
- Platform for custom e-ink display applications
- Base for accessibility devices or custom interfaces

Whether you're a maker looking for a weekend project, a student learning embedded systems, or a reader who wants complete control over your device - this project provides all the resources you need to build your own e-reader.
```

---

## Feature Descriptions

### Fast Boot Time
**Technical:** "Boots in ~15 seconds from power-on to library menu using minimal Buildroot Linux and optimized init sequence"

**User-Friendly:** "Turn it on and start reading in 15 seconds. No waiting for a bloated OS to load - just your books, instantly."

**Marketing:** "From power button to page one in just 15 seconds. This isn't a general-purpose computer pretending to be an e-reader - it's purpose-built for reading."

### E-Paper Display
**Technical:** "400×300 pixel e-paper display using Waveshare 4.2" module with IL0398 controller via SPI interface"

**User-Friendly:** "E-paper screen that's easy on your eyes and readable even in direct sunlight. No backlight glare, no eye strain - just like reading real paper."

**Marketing:** "Experience reading the way it was meant to be - on a screen that looks and feels like real paper. Readable in any light, gentle on your eyes for hours of comfortable reading."

### Physical Buttons
**Technical:** "Five GPIO-connected tactile push buttons with hardware debouncing and event-driven input system"

**User-Friendly:** "Simple physical buttons that always work. No frozen touchscreen, no accidental taps - just reliable buttons that click."

**Marketing:** "Navigate your entire library with five simple buttons. No touchscreen complexity, no gestures to remember. Just press and read."

### Format Support
**Technical:** "Native support for plain text, EPUB (HTML/CSS parsing), and PDF (text extraction via external tools)"

**User-Friendly:** "Read books in TXT, EPUB, and PDF formats. Works with books from Project Gutenberg, your personal collection, and most e-book sources."

**Marketing:** "Compatible with thousands of free books from Project Gutenberg and your personal library. TXT, EPUB, and PDF - if you can download it, you can read it."

### WiFi Downloading
**Technical:** "Integrated WiFi configuration and HTTP client for downloading books from online sources"

**User-Friendly:** "Connect to WiFi and download books directly to your device. No computer needed - just browse and download."

**Marketing:** "Your library expands instantly. Connect to WiFi, browse thousands of free books from Project Gutenberg, and download directly to your device."

### Battery Life
**Technical:** "12-20 hours active reading time with 18650 lithium-ion battery and DC-DC boost converter"

**User-Friendly:** "Read for weeks on a single charge. The e-paper display only uses power when changing pages, giving you incredible battery life."

**Marketing:** "Charge it once, read for weeks. With up to 20 hours of active reading time, your e-reader battery outlasts everything else in your bag."

### Open Source Everything
**Technical:** "Complete source code (GPL v3/MIT), hardware schematics (CERN OHL), and documentation (CC BY-SA) available on GitHub"

**User-Friendly:** "Everything is open source - you can see exactly how it works, modify it however you want, and share your improvements with others."

**Marketing:** "Your device, your code, your freedom. No black boxes, no locked-down software, no vendor lock-in. Inspect it, modify it, improve it, and make it truly yours."

### Distraction-Free
**Technical:** "Single-purpose device with no web browser, social media, or notification systems - minimal OS with reading application only"

**User-Friendly:** "No notifications, no emails, no social media. Just you and your book. Finally, an e-reader that helps you focus."

**Marketing:** "Reclaim your attention. In a world of endless notifications and distractions, this e-reader does one thing perfectly: let you read in peace."

---

## Use Case Narratives

### The Reader
```
**"Take Back Your Reading Time"**

Sarah loved reading, but her Kindle kept suggesting books, showing ads, and tracking her reading habits. She wanted something simpler - just her books, nothing else.

With the Open Source E-Reader, she built exactly that. No ads, no tracking, no recommendations algorithm. Just her library, organized her way. She downloads public domain classics from Project Gutenberg and reads for hours without distraction.

The e-paper display is easy on her eyes during her commute, and the two-week battery life means she never worries about charging. Best of all? She knows exactly how her device works and what it does with her data (nothing - it's all local).
```

### The Student
```
**"Learn by Building"**

Marcus needed a project for his embedded systems class. The Open Source E-Reader checked all the boxes: hardware interfacing, Linux kernel development, display drivers, input systems, and power management.

Following the build guide, he learned how to configure Buildroot, write device drivers, and optimize boot times. The project documentation explained every design decision, helping him understand the trade-offs between memory usage, performance, and power consumption.

When he presented his completed e-reader to the class, he could explain every component - from the SPI protocol communicating with the e-paper display to the state machine managing WiFi connections. He got an A on the project and a functional e-reader to use for years.
```

### The Maker
```
**"The Perfect Weekend Project"**

Alex wanted a hands-on project that combined hardware and software. After seeing the Open Source E-Reader on Hackaday, they ordered the parts ($85 total) and set aside a weekend.

Saturday was hardware day: soldering buttons, connecting the display, and 3D printing a custom case. Sunday was software: building the Buildroot image, flashing the SD card, and booting up for the first time.

By Sunday evening, Alex was reading "The Adventures of Sherlock Holmes" on a device they'd built from scratch. The satisfaction of seeing those first words appear on the e-paper display made the whole project worthwhile.

Now Alex is working on adding Bluetooth audio support for audiobooks - because that's the beauty of open source: you can always make it better.
```

### The Privacy Advocate
```
**"Your Books, Your Data, Your Device"**

When Amazon remotely deleted books from customers' Kindles, David decided he needed an e-reader he truly controlled. One that couldn't phone home, report reading habits, or delete his library remotely.

The Open Source E-Reader was the answer. The source code is public - no hidden telemetry, no tracking, no cloud requirements. Books stay on his device, reading data stays private, and he has complete control.

He downloads DRM-free books from independent publishers, loads his personal document collection, and reads with confidence that his data is his own. The device does exactly what he tells it to do - nothing more, nothing less.

For David, the Open Source E-Reader isn't just a gadget - it's a statement about digital rights and user autonomy.
```

---

## FAQ Content

### General Questions

**Q: How much does it cost to build?**
A: Between $70 and $160 depending on configuration. A basic USB-powered version costs about $70 (Pi Zero W $15, display $28, buttons $3, SD card $8, cables/misc $16). A portable version with battery and 3D-printed case costs $110-160. See the [Bill of Materials](../hardware/BOM.md) for detailed pricing.

**Q: How long does it take to build?**
A: 4-6 hours for hardware assembly (soldering, wiring, case assembly) and 1-2 hours for software setup (building image or flashing pre-built). First-time builders should allocate a full weekend. The software build takes 1-2 hours on a modern computer, but you can download pre-built images to skip this step.

**Q: Do I need programming experience?**
A: Not to build and use the basic device - just follow the build guide. Programming experience (C, Python, or Linux) is helpful if you want to modify the software or add features. The project is a great way to learn embedded Linux development.

**Q: Do I need soldering experience?**
A: Basic soldering skills are required for connecting buttons and wires. If you've never soldered before, practice on a simple project first. Alternatively, you can use breadboard and jumper wires for a solder-free prototype, though it won't be as compact or durable.

**Q: What books can I read on it?**
A: Any DRM-free TXT, EPUB, or PDF file. Works great with public domain books from Project Gutenberg, Gutenberg.org, Standard Ebooks, and independent publishers. Does NOT work with DRM-protected Kindle books (by design - this is an open device).

**Q: How does it compare to a Kindle or Kobo?**
A: Different use case. Commercial e-readers have better hardware integration, longer battery life, and commercial support. This e-reader has complete transparency, customizability, and no tracking or ads. It's for people who value open source, learning, and control over polish and convenience.

**Q: Can I use this as my primary e-reader?**
A: Depends on your needs. For reading public domain books and DRM-free content, absolutely. If you need access to Kindle store, library lending (Libby), or advanced features like adjustable frontlight, you'll want a commercial device. Many users build this as a secondary "focused reading" device.

### Technical Questions

**Q: What's the battery life?**
A: 12-20 hours of active reading time with an 18650 lithium-ion battery (2000-3000 mAh). E-paper displays only use power during page refreshes, so standby time is measured in weeks. Actual battery life depends on usage patterns (reading vs. downloading), refresh frequency, and WiFi usage.

**Q: How's the display quality?**
A: 400×300 pixel black-and-white e-paper display. Text is crisp and readable, similar to early Kindle models. No backlight (read in good lighting or add external light). Refresh time is ~2 seconds for a full refresh, ~0.8 seconds for partial refresh. Some ghosting may occur after many page turns (mitigated by periodic full refresh).

**Q: Can I add a touchscreen?**
A: The Waveshare display doesn't have touch capability, and adding a separate touchscreen layer would increase complexity and cost significantly. The physical button interface is intentionally simple and works well for the use case. If you want touchscreen, consider forking the project with a different display module.

**Q: Does it support [specific feature]?**
A: Check the roadmap in README.md. Current features: TXT/EPUB/PDF reading, WiFi downloading, bookmarks, basic search. Planned/possible: adjustable fonts, dictionary lookup, note-taking, frontlight, audiobook support. This is open source - you can add any feature you want!

**Q: What about PDF reflow?**
A: PDF support extracts text for reflow (works well for text-based PDFs). Scanned PDFs or complex layouts may not work perfectly. For best results, use EPUB or TXT formats. PDF rendering improvements are on the roadmap.

**Q: Can I use a different Raspberry Pi?**
A: Yes! Pi Zero 2 W works great (faster boot and better performance). Pi 3/4 will work but are overkill and use more power. The configuration targets Pi Zero W for optimal cost/performance/power balance.

**Q: What about other e-paper displays?**
A: The code is written for Waveshare 4.2" (IL0398 controller), but can be adapted to other SPI-based e-paper displays. You'll need to modify the display driver code. Larger displays (7.5", 10.3") work but require more power and are more expensive.

---

## Community Messaging

### Welcome Message (for new contributors)
```
Welcome to the Open Source E-Reader project! 👋

We're glad you're here. Whether you're building your first e-reader, fixing a bug, adding a feature, or improving documentation - your contributions are valued.

**Getting Started:**
- Read the [Build Guide](docs/BUILD_GUIDE.md) to assemble hardware
- Check [CONTRIBUTING.md](docs/CONTRIBUTING.md) for development setup
- Browse [open issues](issues) for ideas or bugs to fix
- Join discussions in [Discussions](discussions)

**Need Help?**
- Check existing issues and documentation first
- Ask questions in Discussions (not issues)
- Be respectful and patient - we're all learning

**Want to Contribute?**
- Fork the repo and create a feature branch
- Make your changes and test thoroughly
- Submit a pull request with clear description
- Respond to code review feedback

Thanks for being part of the open source community!
```

### Contributor Thank You Message
```
Thank you for contributing to the Open Source E-Reader project! 🎉

Your [pull request / issue report / documentation improvement] helps make this project better for everyone. Open source thrives because of contributors like you who take time to identify issues, suggest improvements, and share their work.

Whether this is your first contribution or your fiftieth, we appreciate your effort and expertise.

Happy making! 📚
```

### Issue Response Template
```
Thanks for opening this issue!

To help us investigate, could you provide:
- [ ] Hardware configuration (Pi model, display version, button setup)
- [ ] Software version (commit hash or release version)
- [ ] Steps to reproduce the issue
- [ ] Expected behavior vs. actual behavior
- [ ] Logs or error messages (if applicable)

Additional photos or videos showing the issue are very helpful for hardware-related problems.

We'll investigate and get back to you soon. Thanks for helping improve the project!
```

---

## Press Release Template

**FOR IMMEDIATE RELEASE**

### Open Source E-Reader Project Launches - Complete DIY Platform for Building Custom Reading Devices

**[City, State - Date]** - The Open Source E-Reader project, a comprehensive DIY platform for building custom e-reading devices, is now publicly available on GitHub. The project provides complete build guides, hardware schematics, and source code for creating a functional e-reader using Raspberry Pi and e-paper display technology.

**Project Overview**

The Open Source E-Reader enables makers, students, and reading enthusiasts to build their own reading device for $70-160. The platform combines a Raspberry Pi Zero W single-board computer with a 4.2" e-paper display, five physical buttons, and optional battery pack for portable use.

The device boots in approximately 15 seconds directly to a book library interface and supports TXT, EPUB, and PDF formats. Built-in WiFi connectivity enables users to download books directly to the device. Battery life ranges from 12-20 hours of active reading time.

**Key Features**

- **Fast Boot:** 15-second boot time directly to library using minimal Linux OS
- **E-Paper Display:** 400×300 pixel screen readable in sunlight, gentle on eyes
- **Physical Controls:** Five button navigation - no touchscreen complexity
- **Format Support:** TXT, EPUB, and PDF file formats
- **WiFi Enabled:** Direct book downloading from online sources
- **Long Battery Life:** 12-20 hours continuous reading (portable configuration)
- **Fully Open Source:** All hardware designs, schematics, and software code publicly available

**Educational and Accessibility Applications**

The project serves as an educational platform for learning embedded Linux development, hardware interfacing, display driver programming, and power management optimization. Students and hobbyists can study real-world implementations of SPI communication, GPIO input handling, framebuffer graphics, and embedded systems architecture.

The open platform also enables customization for accessibility needs, allowing developers to create specialized interfaces, add text-to-speech capabilities, or modify the display and input systems for users with different requirements.

**Open Source Licensing**

All project components are open source:
- **Software:** Dual-licensed GPL v3 / MIT
- **Hardware:** CERN Open Hardware License
- **Documentation:** Creative Commons BY-SA 4.0

Users are free to build, modify, and distribute their own versions while contributing improvements back to the community.

**Availability**

Complete project documentation, build guides, hardware schematics, bill of materials, and source code are available now on GitHub at [repository URL]. Pre-built SD card images are available for download, allowing users to skip the compilation process.

**About the Project**

The Open Source E-Reader project was created to provide a transparent, customizable alternative to commercial e-readers. The project emphasizes user control, privacy, and learning through hands-on hardware and software development.

**Contact Information**

Project Repository: [GitHub URL]
Documentation: [Docs URL]
Discussion Forum: [Discussions URL]

**Media Kit**

High-resolution photos, demo videos, and technical specifications available at: [Press Kit URL]

###

**Media Contact:**
[Name]
[Email]
[Project Website]
