---
type: reference
title: Legal and Licensing Information
created: 2026-01-18
tags:
  - legal
  - licensing
  - compliance
  - open-source
related:
  - "[[CONTRIBUTING]]"
  - "[[README]]"
---

# Legal and Licensing Information

This document provides comprehensive legal information about the Open Source E-Reader project, including license details, third-party attributions, trademark usage, and compliance guidance.

---

## Table of Contents

1. [License Overview](#license-overview)
2. [Software License (GPL v3)](#software-license-gpl-v3)
3. [Hardware License (CERN OHL)](#hardware-license-cern-ohl)
4. [Documentation License (CC BY-SA)](#documentation-license-cc-by-sa)
5. [Third-Party Software Licenses](#third-party-software-licenses)
6. [Third-Party Hardware](#third-party-hardware)
7. [Trademark Usage](#trademark-usage)
8. [Patent Considerations](#patent-considerations)
9. [Export Control](#export-control)
10. [Warranty and Liability](#warranty-and-liability)
11. [Compliance Guide](#compliance-guide)
12. [License FAQ](#license-faq)
13. [Contact Information](#contact-information)

---

## License Overview

The Open Source E-Reader project uses **multiple licenses** to cover different types of content:

| Content Type | License | Rationale |
|--------------|---------|-----------|
| **Software** (C source code) | **GPL v3** | Ensures derivative software remains open source |
| **Hardware** (schematics, PCB) | **CERN OHL-P v2** | Permissive hardware design license, freedom to manufacture |
| **Documentation** (guides, docs) | **CC BY-SA 4.0** | Free sharing with attribution, derivatives must share alike |

This multi-license approach ensures:
- Software improvements benefit the community (copyleft)
- Hardware can be manufactured commercially (permissive)
- Documentation can be freely shared and adapted (attribution required)

**Why GPL v3 for software?**
- Strong copyleft: derivatives must remain open source
- Patent protection: contributors grant patent rights
- Anti-tivoization: prevents locked-down devices
- Educational priority: ensures learning remains accessible

**Why CERN OHL-P for hardware?**
- Specifically designed for open hardware
- Permissive variant allows commercial use
- Requires sharing design files for derivatives
- Compatible with manufacturing and small-scale production

**Why CC BY-SA for documentation?**
- Standard for open educational content
- Allows remixing and translation
- Ensures improvements are shared back
- Clear attribution requirements

---

## Software License (GPL v3)

### Coverage

The GNU General Public License v3.0 (GPL v3) applies to:
- All C source code in `src/`
- Shell scripts and build scripts
- Configuration files specific to the application
- Makefiles and build definitions

### License Text

```
Copyright (C) 2024-2026 Open Source E-Reader Project Contributors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```

### What GPL v3 Means for You

**If you're using the software:**
- ✅ Free to use for any purpose (personal, commercial, educational)
- ✅ Free to study and modify the source code
- ✅ Free to distribute copies
- ✅ No royalties or licensing fees

**If you're modifying the software:**
- ⚠️ You must release your modifications under GPL v3
- ⚠️ You must provide source code to recipients
- ⚠️ You must include copyright notices and license text
- ⚠️ You cannot add additional restrictions

**If you're distributing the software:**
- ⚠️ You must include the complete source code (or offer to provide it)
- ⚠️ You must include the LICENSE file and copyright notices
- ⚠️ Recipients must receive the same GPL v3 rights you received
- ⚠️ You cannot convert it to proprietary software

**Anti-Tivoization Clause:**
GPL v3 specifically prevents "tivoization" - the practice of using GPL software in hardware that prevents users from running modified versions. If you build a device using this software, users must be able to install their own modified versions.

### Full License Text

The complete GPL v3 license text is available at:
- **Online:** https://www.gnu.org/licenses/gpl-3.0.txt
- **Local:** See `LICENSE` file in the repository root

---

## Hardware License (CERN OHL)

### Coverage

The CERN Open Hardware License v2 - Permissive (CERN-OHL-P v2) applies to:
- Electrical schematics in `hardware/schematics/`
- PCB layouts and design files
- Mechanical designs (enclosures, 3D models)
- Wiring diagrams and connection specifications
- Component placement documentation

### License Text

```
Copyright (C) 2024-2026 Open Source E-Reader Project Contributors

This hardware design is licensed under the CERN Open Hardware Licence
Version 2 - Permissive.

You may redistribute and modify this documentation and make products
using it under the terms of the CERN-OHL-P v2 (https://ohwr.org/cern_ohl_p_v2.txt).

This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY,
INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A
PARTICULAR PURPOSE. Please see the CERN-OHL-P v2 for applicable conditions.
```

### What CERN OHL-P Means for You

**If you're using the hardware designs:**
- ✅ Free to build devices for personal or commercial use
- ✅ Free to manufacture and sell devices
- ✅ Free to modify the designs
- ✅ No royalties or licensing fees

**If you're modifying the hardware designs:**
- ⚠️ You should share your improvements (encouraged, not required)
- ⚠️ You should document your changes
- ✅ You can keep modifications private if needed
- ✅ You can create proprietary derivatives (permissive variant)

**If you're distributing hardware designs:**
- ⚠️ You must include the license text
- ⚠️ You must provide attribution to the original project
- ✅ You can license your derivative designs under different terms
- ✅ You can combine with proprietary designs

**Manufacturing and Commercial Use:**
The permissive variant (CERN-OHL-P) is specifically chosen to allow:
- Commercial manufacturing and sales
- Integration into proprietary products
- Closed-source derivatives (though sharing is encouraged)
- Industrial production at any scale

### Full License Text

The complete CERN-OHL-P v2 license text is available at:
- **Online:** https://ohwr.org/cern_ohl_p_v2.txt
- **CERN OHL Homepage:** https://ohwr.org/project/cernohl/wikis/home

---

## Documentation License (CC BY-SA)

### Coverage

Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0) applies to:
- All documentation in `docs/`
- README.md and project descriptions
- Build guides, user guides, tutorials
- Architecture diagrams and illustrations
- Marketing materials and project descriptions
- Comments and inline documentation (when not part of functional code)

### License Summary

**You are free to:**
- **Share** — copy and redistribute the material in any medium or format
- **Adapt** — remix, transform, and build upon the material for any purpose, even commercially

**Under the following terms:**
- **Attribution** — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- **ShareAlike** — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
- **No additional restrictions** — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

### Attribution Requirements

When using or adapting this documentation, include:

```
Based on the Open Source E-Reader Project
https://github.com/cmclark00/open-source-ereader
Licensed under CC BY-SA 4.0
```

For substantial portions, also include:
- Link to the original source
- Statement of changes made (if modified)
- Same CC BY-SA 4.0 license for derivatives

### Full License Text

The complete CC BY-SA 4.0 license text is available at:
- **Online:** https://creativecommons.org/licenses/by-sa/4.0/legalcode
- **Summary:** https://creativecommons.org/licenses/by-sa/4.0/

---

## Third-Party Software Licenses

This project depends on numerous open-source software components. Here's a comprehensive list:

### Core System Components

#### Linux Kernel
- **License:** GNU General Public License v2 (GPL v2)
- **Usage:** Operating system kernel
- **Source:** https://www.kernel.org/
- **Notes:** Separate GPL v2 license (kernel uses v2, not v3)
- **Compliance:** Kernel source available through Buildroot

#### Buildroot
- **License:** GNU General Public License v2+ (GPL v2 or later)
- **Usage:** Build system for embedded Linux
- **Source:** https://buildroot.org/
- **Notes:** Build system itself is GPL v2+, outputs can be any license
- **Compliance:** Buildroot configuration included in repository

### Libraries and Dependencies

#### libzip (ZIP file library)
- **License:** BSD 3-Clause License
- **Usage:** EPUB file extraction (EPUBs are ZIP archives)
- **Source:** https://libzip.org/
- **Copyright:** 1999-2024 Dieter Baron and Thomas Klausner
- **License Text:**
  ```
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that conditions are met.
  See: https://libzip.org/license/
  ```

#### libxml2 (XML parser)
- **License:** MIT License
- **Usage:** EPUB metadata and content parsing
- **Source:** http://xmlsoft.org/
- **Copyright:** 1998-2012 Daniel Veillard
- **License Text:**
  ```
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software... [MIT license]
  See: http://xmlsoft.org/
  ```

#### Poppler (PDF rendering library)
- **License:** GNU General Public License v2 or v3
- **Usage:** PDF file parsing and rendering
- **Source:** https://poppler.freedesktop.org/
- **Notes:** Based on Xpdf, GPL v2/v3 dual license
- **Compatibility:** Compatible with our GPL v3 software license

#### zlib (compression library)
- **License:** zlib License (permissive)
- **Usage:** Compression/decompression for various formats
- **Source:** https://zlib.net/
- **Copyright:** 1995-2024 Jean-loup Gailly and Mark Adler

### Network Components

#### wpa_supplicant
- **License:** BSD License / GPL v2 (dual license)
- **Usage:** WiFi authentication and connection management
- **Source:** https://w1.fi/wpa_supplicant/
- **Copyright:** 2003-2024 Jouni Malinen
- **Notes:** BSD license preferred, GPL v2 available

#### curl / libcurl
- **License:** curl License (MIT/X derivative, permissive)
- **Usage:** HTTP downloads for book fetching
- **Source:** https://curl.se/
- **Copyright:** 1996-2024 Daniel Stenberg

### Firmware and Hardware Support

#### Raspberry Pi Firmware
- **License:** Proprietary Broadcom license
- **Usage:** GPU firmware, bootloader
- **Source:** https://github.com/raspberrypi/firmware
- **Notes:** Required for Pi Zero W boot, redistributable but not modifiable
- **Compliance:** Included in standard Buildroot Raspberry Pi support

#### Linux SPI Subsystem
- **License:** GPL v2 (part of Linux kernel)
- **Usage:** SPI communication with e-paper display
- **Notes:** Kernel module, GPL v2

### Development Tools (Build-time only)

These are used during compilation but not distributed:

- **GCC (GNU Compiler Collection)** - GPL v3 with GCC Runtime Library Exception
- **GNU Make** - GPL v3
- **binutils** - GPL v3
- **glibc / musl** - LGPL v2.1 (runtime library)

**Note:** Compiler and toolchain licenses do **not** affect the license of compiled binaries due to the GCC Runtime Library Exception.

### License Compatibility

All third-party licenses are compatible with GPL v3:
- **GPL v2 components:** Compatible (GPL v3 can use GPL v2 code)
- **LGPL components:** Compatible (LGPL allows linking)
- **BSD/MIT components:** Compatible (permissive licenses)
- **Proprietary firmware:** Separate from GPL code, system-level integration

### Source Code Availability

**GPL-licensed components:**
Per GPL requirements, complete source code for all GPL components is available through:
- Buildroot configuration (reproducible builds)
- Package definitions in `buildroot/package/`
- Upstream source repositories

**Obtaining sources:**
```bash
cd buildroot
make legal-info
```

This generates a `legal-info/` directory with:
- Complete source archives for all packages
- License texts for all components
- Manifest of all software included

---

## Third-Party Hardware

This project uses commercially available hardware components:

### Waveshare E-Paper Displays
- **Manufacturer:** Waveshare Electronics
- **Product:** 4.2inch e-Paper Module (Rev 2.2)
- **Documentation:** https://www.waveshare.com/wiki/4.2inch_e-Paper_Module
- **License:** Waveshare provides example code and documentation
- **Usage:** We implement our own driver based on public datasheets
- **Trademark:** "Waveshare" is a trademark of Waveshare Electronics

### Raspberry Pi
- **Manufacturer:** Raspberry Pi Foundation / Trading Ltd
- **Product:** Raspberry Pi Zero W
- **Documentation:** https://www.raspberrypi.com/documentation/
- **License:** Hardware design is proprietary, but interfaces are documented
- **Usage:** Standard Linux support through mainline kernel
- **Trademark:** "Raspberry Pi" is a trademark of Raspberry Pi Ltd

### Component Manufacturers

Hardware components (resistors, buttons, connectors, etc.) are commodity parts from various manufacturers:
- Omron (tactile switches)
- Samsung / Panasonic / LG (battery cells if used)
- Various connector and passive component manufacturers

**No proprietary designs or trade secrets are included in this project.**

---

## Trademark Usage

This project references several trademarks owned by third parties:

### Trademarks Referenced

- **"Raspberry Pi"** - Raspberry Pi Ltd
- **"Waveshare"** - Waveshare Electronics Co., Ltd
- **"Linux"** - Linus Torvalds
- **"Kindle"** - Amazon Technologies, Inc
- **"Kobo"** - Rakuten Kobo Inc

### Trademark Policy

**Our use of these trademarks is descriptive only:**
- We reference "Raspberry Pi Zero W" to identify compatible hardware
- We reference "Waveshare 4.2inch e-Paper" to specify the display model
- We reference "Kindle" and "Kobo" for comparison purposes
- We reference "Linux" to describe the operating system

**We do NOT:**
- Claim endorsement by these trademark holders
- Suggest official affiliation with these companies
- Use trademarks in product names or branding
- Imply certification or approval

### Fair Use

Our trademark usage constitutes fair use under trademark law:
- **Nominative use:** Identifying specific products/platforms
- **Comparative advertising:** Honest comparison to help consumers
- **Descriptive use:** Describing compatibility and functionality

### Project Branding

The name **"Open Source E-Reader"** is descriptive and generic, not a protected trademark. The project does not claim trademark protection.

If this project develops distinctive branding (logo, specific product name), trademark registration may be considered for:
- Preventing confusion with copycat projects
- Protecting community reputation
- Ensuring quality standards

Any such trademarks would be held by the project maintainers for the benefit of the open-source community.

---

## Patent Considerations

### Our Policy

**This project does not knowingly infringe on any patents.**

### GPL v3 Patent Protection

The GPL v3 license includes explicit patent protection:

1. **Contributor Patent License:** Each contributor grants you a patent license for their contributions
2. **Patent Retaliation:** If you sue for patent infringement, your GPL license is terminated
3. **Anti-Tivoization:** Prevents using patents to restrict hardware modifications

### Known Patent Landscape

**E-Paper Technology:**
- E Ink Corporation holds numerous patents on electrophoretic displays
- Waveshare e-paper displays are manufactured under appropriate licenses
- **Our use:** We interface with commercially available displays; we do not manufacture display technology

**PDF Technology:**
- Adobe holds patents on PDF technology but has made specifications publicly available
- **Our use:** We use the open-source Poppler library (based on Xpdf), which has been in public use for 20+ years

**EPUB Format:**
- EPUB is an open IDPF (now W3C) standard
- **Our use:** Standard implementation based on public specifications

**General Computing:**
- Many software patents cover general computing concepts
- **Risk mitigation:** We use established open-source libraries with long track records

### If You're Concerned About Patents

**For personal/educational use:**
- Patent risk is minimal
- Personal use is generally protected

**For commercial use:**
- Conduct your own patent search
- Consider consulting a patent attorney
- Many software patents are of questionable validity

**Our stance:**
We believe software and educational projects should be free from patent encumbrance. This project implements well-established, publicly documented technologies.

---

## Export Control

### Encryption Components

This project includes encryption software through:
- WiFi (WPA2/WPA3) - wpa_supplicant
- HTTPS downloads - OpenSSL/libcurl

### Export Regulations

**United States:**
- Export Administration Regulations (EAR) apply to encryption software
- **Exemption:** Open-source encryption software is generally exempt under EAR Section 742.15(b)
- **Notification:** Technically should notify BIS and NSA (though enforcement is unclear for small projects)

**Other Jurisdictions:**
- EU: Generally allows export of open-source encryption
- Most countries: No restrictions on publicly available encryption

### Our Compliance

This project:
- ✅ Makes source code publicly available
- ✅ Uses standard, widely distributed encryption (WPA, TLS)
- ✅ Does not target specific countries or regimes
- ✅ Falls under open-source exemptions in most jurisdictions

**Note:** If you're manufacturing devices commercially or exporting to restricted countries, consult legal counsel regarding export controls in your jurisdiction.

---

## Warranty and Liability

### No Warranty

**All components of this project are provided "AS IS" with absolutely NO WARRANTY.**

From the GPL v3:
```
THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
APPLICABLE LAW. THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE
THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
```

### Limitation of Liability

**Users assume all risk when building and using this project.**

- ⚠️ **Electrical hazards:** Improper wiring can damage components or cause injury
- ⚠️ **Battery hazards:** Lithium batteries can catch fire if mishandled
- ⚠️ **Data loss:** Software bugs may cause data corruption
- ⚠️ **Eye strain:** Extended reading on any display can cause eye fatigue

**The project maintainers and contributors are NOT liable for:**
- Hardware damage or failure
- Personal injury
- Data loss
- Financial losses
- Any consequential damages

### Safety Recommendations

**We strongly recommend:**
- ✅ Following all safety guidelines in the Build Guide
- ✅ Using proper safety equipment (safety glasses, ESD protection)
- ✅ Testing components before final assembly
- ✅ Using quality components from reputable suppliers
- ✅ Understanding what you're building before powering it on

**This is an educational project. Build at your own risk.**

---

## Compliance Guide

### If You're Building for Personal Use

**You automatically comply** with all licenses by:
- Using the project for your own learning/reading
- Modifying it for personal use
- No special requirements

### If You're Sharing Modified Software

**To comply with GPL v3:**

1. **Include complete source code:**
   - All C source files (modified and original)
   - Makefiles and build scripts
   - Instructions for building

2. **Include license information:**
   - Copy of LICENSE file
   - Copyright notices in source files
   - Attribution to original project

3. **Use same license:**
   - Your modifications must be GPL v3
   - Clearly mark what you changed

4. **Provide build instructions:**
   - How to compile your modified version
   - Dependencies and requirements

**Example compliance for a GitHub fork:**
```
# Modified E-Reader (My Fork)

Based on Open Source E-Reader Project
https://github.com/cmclark00/open-source-ereader

Licensed under GPL v3 (same as original)

Changes made:
- Added support for larger display
- Implemented touchscreen input
- Modified menu layout

See LICENSE for full license text.
```

### If You're Manufacturing Devices

**To comply with hardware license (CERN OHL-P):**

1. **Include attribution:**
   - Credit the original project on packaging/website
   - Link to original repository

2. **Include license text:**
   - Provide CERN-OHL-P license with documentation
   - Include in user manual or online

**Example:**
```
This device is based on the Open Source E-Reader Project
(https://github.com/cmclark00/open-source-ereader)

Hardware design licensed under CERN Open Hardware License v2 - Permissive
Software licensed under GPL v3 - source code available at [URL]
```

**For software on devices:**
Per GPL v3, you must:
- Provide source code to customers (on request or automatically)
- Allow users to install modified software on the device (anti-tivoization)
- Not add additional restrictions (no proprietary extensions that prevent modification)

### If You're Writing About the Project

**To comply with documentation license (CC BY-SA 4.0):**

1. **Give attribution:**
   ```
   Based on documentation from Open Source E-Reader Project
   https://github.com/cmclark00/open-source-ereader
   Licensed under CC BY-SA 4.0
   ```

2. **Share alike:**
   - If you create derivative documentation, use CC BY-SA 4.0
   - Make your documentation freely available

3. **Indicate changes:**
   - Note what you modified or adapted
   - Don't claim original authorship

**Example for a tutorial:**
```
# How to Build an E-Reader (Updated Guide)

This tutorial is adapted from the Open Source E-Reader Project
(https://github.com/cmclark00/open-source-ereader)

Changes: Updated for Raspberry Pi Zero 2 W, added battery testing section

Original documentation licensed under CC BY-SA 4.0
This derivative work also licensed under CC BY-SA 4.0
```

---

## License FAQ

### General Questions

**Q: Can I use this project for commercial purposes?**
A: Yes! All licenses (GPL v3, CERN OHL-P, CC BY-SA) allow commercial use.

**Q: Do I have to pay royalties?**
A: No. All licenses are royalty-free.

**Q: Can I sell devices based on this project?**
A: Yes. Hardware license (CERN OHL-P) explicitly allows commercial manufacturing. Software license (GPL v3) allows commercial use but requires providing source code.

**Q: Can I modify the design?**
A: Yes. Modifications are encouraged! Software modifications must stay GPL v3. Hardware modifications can be any license (CERN OHL-P is permissive).

### Software License Questions

**Q: Can I combine this GPL v3 code with proprietary software?**
A: Only if the proprietary software is a separate program. If you modify or link with this code, the result must be GPL v3.

**Q: Can I use this code in a closed-source commercial product?**
A: No. GPL v3 requires derivatives to remain open source.

**Q: What if I only modify the code for internal use?**
A: You don't have to release it. GPL only requires source code sharing if you *distribute* the software.

**Q: Can I dual-license my contributions?**
A: No. Contributions to GPL v3 code automatically become GPL v3. However, you retain copyright and could license the same code under different terms in a separate project.

### Hardware License Questions

**Q: Can I sell hardware without sharing my design files?**
A: Yes! CERN OHL-P is permissive. You're encouraged but not required to share modifications.

**Q: Can I make proprietary improvements to the hardware?**
A: Yes. CERN OHL-P allows proprietary derivatives.

**Q: Do I have to use CERN OHL-P for my modified hardware?**
A: No. Permissive variant allows you to choose your own license for derivatives.

### Documentation License Questions

**Q: Can I use the documentation in a book or course?**
A: Yes! CC BY-SA allows commercial use, including books and courses. Just provide attribution and use CC BY-SA for derivative works.

**Q: Can I translate the documentation?**
A: Yes! Translations are encouraged. Attribute the original, mark it as translated, and use CC BY-SA license.

**Q: Can I use screenshots in my blog post?**
A: Yes. Provide attribution and link to the project.

### Compatibility Questions

**Q: Can I use this GPL v3 software with GPL v2 libraries?**
A: Yes. GPL v3 can incorporate GPL v2 code (but not vice versa).

**Q: Can I use MIT/BSD licensed libraries with this GPL v3 project?**
A: Yes. Permissive licenses (MIT, BSD) are compatible with GPL.

**Q: Can I include Creative Commons images in GPL code?**
A: Depends on the CC license. CC BY and CC BY-SA are compatible. CC BY-NC and CC BY-ND are not.

---

## Contact Information

### Licensing Questions

For questions about licensing:
- **GitHub Issues:** https://github.com/cmclark00/open-source-ereader/issues
- **GitHub Discussions:** https://github.com/cmclark00/open-source-ereader/discussions
- **Tag:** Use `[legal]` or `[licensing]` tag in issue title

### Copyright Concerns

If you believe this project infringes your copyright:
1. Open a GitHub issue with detailed information
2. Provide evidence of copyright ownership
3. Specify which content is allegedly infringing

We will investigate all legitimate copyright concerns promptly.

### Contributing Your Work

By contributing to this project:
- You agree to license your contributions under the same licenses (GPL v3 for code, etc.)
- You certify you have the right to license your contribution
- You agree to the Developer Certificate of Origin (see CONTRIBUTING.md)

See [[CONTRIBUTING]] for detailed contribution guidelines.

---

## Summary

**This project is completely open source and free to use, modify, and distribute.**

| Activity | Requirements | Allowed? |
|----------|--------------|----------|
| Personal use | None | ✅ Yes |
| Study and learn | None | ✅ Yes |
| Modify for yourself | None | ✅ Yes |
| Share modifications | Must provide source (GPL v3) | ✅ Yes |
| Sell devices | Provide source, allow modifications | ✅ Yes |
| Commercial use | Follow license requirements | ✅ Yes |
| Create proprietary hardware | Attribution only (CERN OHL-P) | ✅ Yes |
| Create proprietary software | No - must stay GPL v3 | ❌ No |
| Remove attribution | Never allowed | ❌ No |

**Key Principles:**
1. **Software:** Must remain open source (GPL v3)
2. **Hardware:** Can be proprietary (CERN OHL-P permissive)
3. **Documentation:** Must credit and share alike (CC BY-SA)
4. **Always:** Give attribution to the original project

**We believe in open source, open hardware, and open education.**

This project exists to teach, inspire, and empower. Use it freely, learn from it deeply, and share your improvements generously.

---

**Last Updated:** 2026-01-18
**License Version:** 1.0
**Document License:** CC BY-SA 4.0

For the most current licensing information, see the LICENSE file in the repository root and visit:
https://github.com/cmclark00/open-source-ereader
