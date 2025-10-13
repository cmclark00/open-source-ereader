# Open Source E-Reader Project - AI Context

This file contains a comprehensive summary of the project for AI assistants to quickly understand the current state and design decisions.

## Project Overview

**Goal**: Create an affordable, open-source e-ink e-reader that runs on embedded Linux.

**Target**: Trade paperback-sized device that mimics the feel of reading a physical book.

**Cost Target**: $50-75 per unit (single build), $45-55 at scale (100+ units)

**Current Status**: Phase 0 - Planning and Research (Complete)

## Key Design Decisions Made

### Display
- **Chosen**: 6" E-ink display, 800x600 resolution (167 PPI)
- **Reasoning**: Standard 167 PPI chosen over high-res 265 PPI to save $20
  - Good enough for novels (same as Kindle used for years)
  - High-res better for PDFs but not worth cost for MVP
  - Can upgrade later if needed
- **Technology**: E-Ink Pearl or Carta (standard generation)
- **Refresh Rate**: 500ms partial, 800ms full (acceptable for reading)
- **Size Rationale**: 6" matches trade paperback, industry standard

### Processor
- **Chosen**: Allwinner H3 SoC with 512MB DDR3 RAM
- **Implementation**: Using Orange Pi Zero LTS module for MVP
  - Pre-assembled module reduces complexity
  - Can design custom H3 board later for production
- **Alternative Considered**: Allwinner V3s (rejected - only 64MB RAM, too limited)

### Input System
- **Chosen**: 8 physical tactile buttons (no touchscreen)
- **Layout**: Asymmetric right-side placement for one-handed reading
  - 2 large page turn buttons (forward/back) on right edge
  - 4-way navigation (up/down/left/right)
  - 2 function buttons (home/menu) on bottom
  - 1 power button on top edge
- **Text Entry**: On-screen keyboard navigated with directional buttons
  - No physical keyboard (saves cost and complexity)
  - Good enough for library search and basic input
- **Future**: Optional Bluetooth via ESP32 module for wireless page turners

### Power System
- **Battery**: 2x 18650 cells in series (7.4V, 5000-6000mAh total)
  - Using 18650 holder for MVP (user-replaceable)
  - Can switch to Li-Po pouch for production
- **Charging**: TP4056 IC with protection (over-charge, over-discharge, short)
- **Regulation**: MT3608 boost converter (7.4V → 5V for Orange Pi)
- **Target Battery Life**: 3-4 weeks with 30 min/day reading

### Operating System
- **Build System**: Buildroot (chosen over Yocto for simplicity)
- **Kernel**: Linux 6.x LTS with mainline support
- **Bootloader**: U-Boot
- **Init**: BusyBox init (minimal, fast boot)
- **C Library**: musl libc (small footprint)
- **Target Boot Time**: <5 seconds
- **OS Footprint**: 100-150MB (SquashFS compressed)

### Software Stack
- **Document Rendering**: MuPDF (lightweight, supports EPUB/PDF/MOBI)
- **UI Framework**: Qt Embedded (good framebuffer support, rich widgets)
- **Display Driver**: Direct framebuffer (no X11/Wayland overhead)
- **File Formats**: EPUB, PDF, MOBI, TXT (priority order)

### Physical Design
- **Dimensions**: 130mm x 185mm x 8mm (5.1" x 7.3" x 0.31")
- **Weight Target**: 165g (comparable to 200-page paperback)
- **Case**: Two-piece 3D printed design (snap-fit + screws)
- **Material**: PETG recommended (good balance of strength and printability)
- **Ergonomics**: Designed for comfortable one-handed reading

### PCB Design
- **Type**: 2-layer PCB for MVP (4-layer for production)
- **Size**: 120mm x 170mm
- **Assembly**: Through-hole buttons, SMD passives
- **Module-based**: Orange Pi Zero as pre-assembled module
- **Expansion**: Footprint for ESP32 (unpopulated in MVP)

## Current Development Path

### Immediate Next Steps (You Are Here)
1. **Order testing hardware** (~$27):
   - Waveshare 4.2" E-Paper display ($25-28)
   - Use existing Raspberry Pi 4B 1GB for development

2. **Set up development environment**:
   - Install Raspberry Pi OS on Pi 4
   - Test Waveshare display drivers
   - Get basic display functionality working

3. **Order final hardware from AliExpress** (~$45, 3-4 weeks):
   - Orange Pi Zero LTS: $15-18
   - 6" E-ink display (800x600): $28-30

### Phase 1: Proof of Concept (4-8 weeks)
- Get Linux booting on development board
- Integrate e-ink display driver
- Test display refresh and partial updates
- Implement basic button input
- Create minimal reading app prototype

### Phase 2: Core Software Development (8-12 weeks)
- Integrate MuPDF for document rendering
- Develop reader application UI
- Implement file browser and library
- Add bookmark and progress tracking
- Implement power management

### Phase 3: Hardware Design (8-12 weeks, parallel with Phase 2)
- Design custom PCB schematic in KiCad
- Layout PCB
- Design 3D printed case in FreeCAD
- Order PCB prototypes
- Print case prototypes

### Phase 4: Integration and Testing (6-8 weeks)
- Assemble first complete prototype
- Optimize boot time
- Test and improve battery life
- Add advanced features
- Create user documentation

## Bill of Materials Summary

### MVP Configuration (Single Unit)
| Category | Components | Cost |
|----------|-----------|------|
| Display | 6" E-ink 800x600 | $30 |
| Processor | Orange Pi Zero LTS | $18 |
| Power | 2x 18650 + charging + boost | $9 |
| Input | 8 tactile buttons + caps | $2 |
| PCB | 2-layer custom + passives | $4 |
| Case | 3D printed PETG | $4 |
| Storage | 16GB microSD | $5 |
| Misc | Wire, connectors, screws | $3 |
| **Total** | | **$75** |

### Cost Reduction Strategies
- Use 5.83" display: Save $5
- Design custom H3 board: Save $8
- Single 18650 cell: Save $3
- Bulk pricing (10+): Save $10-15

**Target for production (100+ units)**: $45-55 per unit

## Technology Choices - Reasoning

### Why Buildroot over Yocto?
- Simpler learning curve
- Faster build times
- Good enough for single-purpose device
- Excellent embedded system focus

### Why musl over glibc?
- Smaller footprint (~600KB vs 2MB+)
- Faster
- Good enough compatibility for our use case

### Why Qt over GTK/LVGL?
- Better framebuffer support
- Rich widget library
- Good documentation
- QML for modern UI (future)
- Trade-off: Larger footprint (~15-20MB) but worth it for functionality

### Why MuPDF over Poppler?
- Lighter weight (~5MB vs 10MB+)
- Faster rendering
- Good EPUB support (not just PDF)
- Clean API

### Why tactile buttons over touchscreen?
- Much cheaper (buttons ~$2, touch overlay ~$20-30)
- More reliable (no calibration issues)
- Better for one-handed use
- Page turn buttons feel more "book-like"
- Lower power consumption

### Why 18650 cells over Li-Po pouch?
- Cheaper ($3 vs $5-8)
- User-replaceable
- Safer (hard shell)
- Easier to source
- More "maker-friendly"
- Can switch to Li-Po in production

## Key Specifications

### Display Performance
- Partial refresh: Every page turn (500ms)
- Full refresh: Every 5-10 pages (800ms, clears ghosting)
- Grayscale levels: 4-16 (depends on display generation)
- Contrast ratio: 10:1 (adequate for reading)

### Power Consumption
- Active reading: 150-200mA average
- Idle (display static): 50-100mA
- Deep sleep: 5-10mA
- Battery capacity: 5000mAh (2x 2500mAh cells)
- Estimated battery life: 21-28 days (30 min/day use)

### Memory Requirements
- Kernel: 15-20MB
- System services: 5-10MB
- Reader app: 30-50MB
- Document cache: 20-40MB
- Framebuffer: 1-4MB
- **Total RAM needed**: 128MB minimum, 256MB comfortable, 512MB ideal

### Storage Layout
- Boot partition: 4MB (U-Boot, kernel, DTB)
- Root filesystem: 128MB (SquashFS, read-only)
- Data partition: Remaining space (ext4, user books and settings)

## Design Philosophy

### Core Principles
1. **Simplicity**: Only include what's necessary for reading
2. **Openness**: All hardware and software fully open-source
3. **Affordability**: Target $50 to make it accessible
4. **Repairability**: User-replaceable battery, modular design
5. **Comfort**: Ergonomics similar to reading a physical book

### What We're NOT Building
- ❌ Android tablet with e-ink (too heavy, too complex)
- ❌ Flagship e-reader competitor (not trying to beat Kindle on features)
- ❌ Note-taking device (no stylus, no handwriting)
- ❌ Web browser / email device (single-purpose reading)

### What We ARE Building
- ✅ Affordable e-reader anyone can build
- ✅ Long battery life (weeks, not days)
- ✅ Comfortable for reading novels
- ✅ Hackable and customizable
- ✅ Repairable with common tools
- ✅ Educational (learn embedded Linux)

## Known Trade-offs and Limitations

### Accepted Limitations (MVP)
- No WiFi/Bluetooth (add later via ESP32)
- No frontlight (can add LED strip later)
- No audio (can add codec later)
- No touchscreen (buttons only)
- No advanced PDF features (reflow, annotations)
- Basic UI (functional, not fancy)

### Technical Challenges
1. **E-ink driver complexity**: Partial refresh waveforms can be tricky
2. **PDF rendering**: Heavy PDFs may be slow on 1.2GHz CPU
3. **Font rendering**: Need good anti-aliasing at 167 PPI
4. **Power management**: Deep sleep states require careful driver work
5. **Build system**: Buildroot learning curve for contributors

### Future Enhancements (Post-MVP)
- Larger display options (7.8", 10")
- Color e-ink support (when affordable)
- Bluetooth page turner support
- WiFi book downloads
- Better PDF handling (faster rendering)
- Dictionary and translation
- Note-taking capability
- Audio support (audiobooks, TTS)

## File Structure

```
open-source-ereader/
├── README.md                           # Project overview
├── CLAUDE.md                           # This file (AI context)
├── docs/
│   ├── GETTING_STARTED.md             # Developer setup guide
│   ├── PROJECT_ROADMAP.md             # Development phases
│   ├── BUILD_GUIDE.md                 # Complete assembly instructions
│   ├── hardware/
│   │   ├── COMPONENT_OPTIONS.md       # All component research
│   │   ├── REQUIREMENTS.md            # Hardware requirements spec
│   │   ├── RECOMMENDED_PARTS.md       # Specific part recommendations
│   │   ├── DISPLAY_COMPARISON.md      # E-ink display analysis
│   │   ├── DISPLAY_QUALITY_ANALYSIS.md # PPI comparison
│   │   ├── INPUT_SYSTEM_DESIGN.md     # Button layout and text entry
│   │   ├── BUTTON_LAYOUT_DIAGRAM.md   # Visual button placement
│   │   ├── SCHEMATIC_DESIGN.md        # PCB electrical design
│   │   ├── CASE_DESIGN.md             # 3D case specifications
│   │   ├── FINAL_BOM.md               # Complete bill of materials
│   │   ├── SOURCING_GUIDE.md          # Where to buy components
│   │   └── CHEAP_TESTING_OPTIONS.md   # Budget alternatives
│   └── software/
│       └── OS_ARCHITECTURE.md         # Linux system design
├── hardware/                           # Hardware design files (coming)
│   ├── pcb/                           # KiCad schematics and PCB
│   ├── case/                          # 3D models (FreeCAD, STL)
│   └── bom/                           # BOM spreadsheets
├── os/                                # Operating system (coming)
│   ├── buildroot/                     # Buildroot configuration
│   ├── kernel/                        # Kernel configs and patches
│   └── rootfs/                        # Root filesystem overlay
├── software/                          # Application software (coming)
│   ├── reader-app/                    # E-book reading application
│   ├── ui/                           # User interface
│   └── drivers/                       # Display and input drivers
└── tools/                            # Build scripts (coming)
```

## Important Notes for AI Assistants

### User's Current Situation
- Has Raspberry Pi 4B 1GB, Pi Zero W, and Pi 5 8GB available
- Using Pi 4 for development (best choice for fast iteration)
- Ordered (or about to order) Waveshare 4.2" e-Paper display for testing
- Will order Orange Pi Zero LTS + 6" display from AliExpress (2-4 week delivery)
- Budget-conscious: Chose standard display over high-res to save money

### Testing Hardware
- **Development**: Raspberry Pi 4B 1GB + Waveshare 4.2" display
- **Testing**: Raspberry Pi Zero W (validate on slower hardware)
- **Production**: Orange Pi Zero LTS + 6" display

### Next Immediate Tasks
1. Receive and test 4.2" display on Pi 4
2. Set up Buildroot development environment
3. Get basic display driver working
4. Create minimal reading app prototype
5. When Orange Pi arrives: Port software to final hardware

### Key Context to Remember
- User chose **standard resolution display** (167 PPI) to save $20
- Using **module-based approach** (Orange Pi Zero LTS) for MVP simplicity
- Prioritizing **novel reading** over PDF support
- **No Bluetooth/WiFi** in MVP (can add later)
- Target is **trade paperback size and feel**
- **One-handed reading** is primary ergonomic goal

### Common Questions & Answers

**Q: Why not use Raspberry Pi Zero for final device?**
A: Too expensive for production ($15 vs $12-15 for Orange Pi in bulk), and we want to support a fully open ecosystem.

**Q: Why 167 PPI instead of 265 PPI?**
A: Saves $20, good enough for novels (same as Kindle 1-4), can upgrade later if users want.

**Q: Why no WiFi in MVP?**
A: Saves $2-3, reduces complexity, can add later via ESP32 module. Focus on core reading first.

**Q: Will this work for PDFs?**
A: Basic PDFs yes, complex technical PDFs will be slow. 167 PPI means more zooming needed. High-res display recommended for PDF-heavy users.

**Q: How long to build first working prototype?**
A: Software development: 4-8 weeks for basic reading functionality. Hardware: PCB design 2-4 weeks, assembly 1 day. Total: ~3 months for complete prototype.

**Q: Can I contribute?**
A: Yes! Once user gets basic system working, looking for help with:
- Display driver optimization
- Reading app features
- UI design
- Documentation
- Testing

## Version History

- **v0.1** (Current): Planning phase complete, all design decisions documented
- **v0.2** (Next): Working proof of concept on dev hardware
- **v1.0** (Target): Complete working prototype with custom PCB

## Last Updated

2024-01-XX - Initial planning phase documentation complete

---

**This file should be updated as the project progresses. Major design changes should be documented here for future reference.**
