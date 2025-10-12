# Hardware Requirements Specification

This document defines the hardware requirements and constraints for the open-source e-reader.

## Design Goals

1. **Affordability**: Total BOM cost under $50 (bulk pricing)
2. **Open Source**: All components should have open drivers/documentation
3. **Power Efficiency**: 3-4 weeks battery life with typical use
4. **Reliability**: MTBF > 10,000 hours
5. **Repairability**: User-replaceable battery, modular design

## Functional Requirements

### FR1: Display
- **FR1.1**: E-ink display, 6 inches minimum
- **FR1.2**: Resolution: 800x600 minimum (167 PPI for 6")
- **FR1.3**: 16 levels of grayscale minimum
- **FR1.4**: Support for partial screen updates
- **FR1.5**: Full refresh time < 1 second
- **FR1.6**: Partial refresh time < 300ms

### FR2: Processing
- **FR2.1**: ARM-based SoC, 400MHz minimum
- **FR2.2**: RAM: 256MB minimum, 512MB recommended
- **FR2.3**: Boot time < 5 seconds from cold start
- **FR2.4**: Resume from sleep < 1 second
- **FR2.5**: Support for Linux kernel 5.x or newer

### FR3: Storage
- **FR3.1**: Internal storage: 4GB minimum for OS and apps
- **FR3.2**: User storage: Expandable via microSD (support up to 128GB)
- **FR3.3**: OS partition read-only (SquashFS or similar)
- **FR3.4**: Store at least 1000 typical books

### FR4: Power
- **FR4.1**: Battery capacity: 2000mAh minimum
- **FR4.2**: Battery life: 3 weeks with 30 min/day reading
- **FR4.3**: USB charging (Type-C preferred, Micro-USB acceptable)
- **FR4.4**: Charging time < 4 hours
- **FR4.5**: Battery level indicator (at least 4 levels)
- **FR4.6**: Auto-sleep after 2 minutes of inactivity
- **FR4.7**: Deep sleep power < 10mW

### FR5: Input
- **FR5.1**: At minimum 6 physical buttons:
  - Page forward
  - Page backward
  - Menu/Home
  - Select/OK
  - Back
  - Power
- **FR5.2**: Button debouncing in hardware or software
- **FR5.3**: Optional: Capacitive touch overlay on display

### FR6: Connectivity (Optional)
- **FR6.1**: USB data connection for file transfer
- **FR6.2**: Optional WiFi for downloading books
- **FR6.3**: No requirement for cellular connectivity

### FR7: Physical Design
- **FR7.1**: Weight < 200g
- **FR7.2**: Thickness < 10mm
- **FR7.3**: Dimensions approximately 160mm x 120mm
- **FR7.4**: Durable case (plastic or 3D printed)
- **FR7.5**: User-replaceable battery (no special tools)

## Non-Functional Requirements

### NFR1: Performance
- **NFR1.1**: Page turn latency < 500ms
- **NFR1.2**: PDF render time < 2 seconds per page
- **NFR1.3**: EPUB render time < 1 second per page
- **NFR1.4**: File browser scrolling smooth (>10 FPS)

### NFR2: Environmental
- **NFR2.1**: Operating temperature: 0°C to 40°C
- **NFR2.2**: Storage temperature: -20°C to 60°C
- **NFR2.3**: Humidity: 10% to 80% non-condensing
- **NFR2.4**: No active cooling required

### NFR3: Safety
- **NFR3.1**: Battery protection circuit (over-charge, over-discharge, short)
- **NFR3.2**: ESD protection on exposed connectors
- **NFR3.3**: No exposed high voltage
- **NFR3.4**: Meet CE/FCC requirements (for EMI)

### NFR4: Manufacturability
- **NFR4.1**: PCB: 2-layer preferred, 4-layer acceptable
- **NFR4.2**: Components: Standard SMD sizes (0603 or larger)
- **NFR4.3**: No BGA packages if possible (easier assembly)
- **NFR4.4**: Hand-solderable design (for DIY builders)

### NFR5: Sourcing
- **NFR5.1**: All components available from major distributors
- **NFR5.2**: No single-source components (if possible)
- **NFR5.3**: Components in production for >2 years expected
- **NFR5.4**: Alternative part numbers identified for critical components

## Component Selection Criteria

### Priority Order:
1. **Cost**: Must fit within budget
2. **Availability**: In stock at major distributors
3. **Documentation**: Public datasheets and specs
4. **Linux Support**: Mainline kernel support preferred
5. **Community**: Active development community
6. **Power**: Low power consumption
7. **Performance**: Adequate for requirements
8. **Size**: Compact footprint

## Minimum Viable Product (MVP) Specifications

For the first prototype, we can relax some requirements:

- **Display**: 4.2" acceptable (cheaper, easier to get)
- **RAM**: 128MB acceptable (will limit PDF capability)
- **Storage**: microSD only (no eMMC)
- **Case**: 3D printed prototype (not production-ready)
- **Buttons**: 4 buttons minimum (forward, back, menu, power)
- **Battery**: Use standard 18650 cell with holder
- **No WiFi**: Focus on core reading functionality

## Stretch Goals

Features for future revisions:

### Hardware
- Frontlight/backlight LEDs
- Audio output (for audiobooks)
- Accelerometer (for auto-rotation)
- Larger display options (7.8", 10")
- Color e-ink display
- Capacitive touch
- Wacom digitizer (for note-taking)

### Connectivity
- WiFi standard
- Bluetooth for audio
- USB-C with PD charging

### Improvements
- Aluminum case option
- Waterproofing (IP67)
- Higher resolution display
- Faster processor for complex PDFs

## Test Requirements

Each prototype must pass:

### Power Tests
- Battery life test (30 min reading/day simulation)
- Charging time measurement
- Sleep mode current draw
- Power consumption per operation

### Display Tests
- Full refresh timing
- Partial refresh timing
- Ghosting evaluation (after N refreshes)
- Viewing angle quality
- Contrast ratio measurement

### Performance Tests
- Boot time measurement
- Page turn latency
- PDF render time (various sizes)
- EPUB render time
- Memory usage profiling

### Reliability Tests
- 1000 charge/discharge cycles
- 10,000 button press cycles
- Drop test (desk height)
- Temperature cycling
- Long-term burn-in (1 week continuous)

### User Experience Tests
- Reading comfort evaluation
- Button feel and placement
- Weight and ergonomics
- Screen glare assessment

## Documentation Requirements

For each hardware revision:

1. **Schematic** (PDF and source)
2. **PCB layout** (Gerbers and source)
3. **Bill of Materials** (with part numbers)
4. **Assembly instructions** (with photos)
5. **3D case files** (STL and source)
6. **Testing procedures**
7. **Known issues list**

## Revision History

- **v0.1** (2024): Initial requirements specification
- Changes will be tracked here as design evolves

## References

- E-Ink display datasheets
- Allwinner SoC user manuals
- USB-C specification
- Li-Ion battery safety guidelines
- PCB design standards (IPC-2221)
