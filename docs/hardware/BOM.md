---
type: reference
title: Bill of Materials (BOM) - Open Source E-Reader
created: 2026-01-13
updated: 2026-01-17
tags:
  - hardware
  - bom
  - parts-list
  - shopping
  - phase-07
related:
  - "[[WIRING_GUIDE]]"
  - "[[DISPLAY_SPECS]]"
  - "[[SCHEMATIC]]"
  - "[[CASE_DESIGN]]"
  - "[[POWER_SUPPLY]]"
---

# Bill of Materials (BOM) - Complete Parts List

This document provides the complete, verified Bill of Materials for building the Open Source E-Reader. All components include manufacturer part numbers, supplier links, quantities, and estimated costs in USD (January 2026 pricing).

## Quick Reference

- **Minimum Viable Build:** $73-88 (core electronics only, breadboard prototype)
- **Complete Portable Build:** $140-180 (with battery, case, all features)
- **Premium Build:** $190-240 (best components, 3D printed case, UPS HAT)

---

## Table of Contents

1. [Core Components (Required)](#core-components-required)
2. [Button Input Components](#button-input-components)
3. [Power Supply Options](#power-supply-options)
4. [Enclosure Options](#enclosure-options)
5. [Optional Development Tools](#optional-development-tools)
6. [Assembly Tools Required](#assembly-tools-required)
7. [Detailed Component Specifications](#detailed-component-specifications)
8. [Supplier Information](#supplier-information)
9. [Cost Summary](#cost-summary)
10. [Alternative Components](#alternative-components)
11. [Procurement Strategy](#procurement-strategy)

---

## Core Components (Required)

These components form the minimum working e-reader (all phases).

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Primary Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-------------------|
| **Raspberry Pi Zero W** | RPi Zero W v1.1 | BCM2835 SoC, 512MB RAM, WiFi 802.11n, BT 4.1, 40-pin GPIO | 1 | $15.00 | $15.00 | [Adafruit #3400](https://www.adafruit.com/product/3400), [PiShop.us](https://www.pishop.us/product/raspberry-pi-zero-w/), [CanaKit](https://www.canakit.com/raspberry-pi-zero-wireless.html) |
| **Waveshare E-Paper Display** | Waveshare 4.2" EPD Module Rev 2.2 (SKU: 13504) | 400×300px B/W, SPI interface, IL0398 controller, partial refresh | 1 | $34.99 | $34.99 | [Waveshare.com](https://www.waveshare.com/4.2inch-e-paper-module.htm), [Amazon ASIN B074NR1SW2](https://www.amazon.com/dp/B074NR1SW2) |
| **MicroSD Card** | SanDisk Ultra 16GB (SDSQUAR-016G-GN6MA) | Class 10, UHS-I U1, A1 rated, 16GB | 1 | $6.99 | $6.99 | [Amazon](https://www.amazon.com/SanDisk-Ultra-UHS-I-Memory-Adapter/dp/B073K14CVB), Local stores |
| **GPIO Header** (if not pre-soldered) | Sullins PRPC040SAAN-RC | 2×20 pins, 2.54mm pitch, male header, through-hole | 1 | $1.50 | $1.50 | [DigiKey #S1011EC-40-ND](https://www.digikey.com/en/products/detail/sullins-connector-solutions/PRPC040SAAN-RC/2775214), [Adafruit #2822](https://www.adafruit.com/product/2822) |
| **Female-Female Jumper Wires** | Various | 10cm (4") length, 40-piece kit, Dupont connectors | 1 pk | $4.99 | $4.99 | [Amazon](https://www.amazon.com/EDGELEC-Breadboard-Optional-Assorted-Multicolored/dp/B07GD2BWPY), [Adafruit #266](https://www.adafruit.com/product/266) |
| **Tactile Push Buttons** | Omron B3F-4055 (12×12mm) | 12×12×7.3mm, 260gf, momentary NO, through-hole | 5 | $0.52 | $2.60 | [DigiKey #SW400-ND](https://www.digikey.com/en/products/detail/omron-electronics-inc-emc-div/B3F-4055/377651), [Mouser #653-B3F-4055](https://www.mouser.com/ProductDetail/653-B3F-4055) |

**Core Components Subtotal:** $66.07

**Note:** Raspberry Pi Zero WH (with pre-soldered header) costs ~$5 more ($20 vs $15) but eliminates GPIO header purchase and soldering.

---

## Button Input Components

### Recommended Button Option 1: Omron B3F Series (Professional Quality)

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **12×12mm Buttons (recommended)** | Omron B3F-4055 | 12×12×7.3mm, 260gf actuation, 500k cycle life | 5 | $0.52 | $2.60 | [DigiKey #SW400-ND](https://www.digikey.com/en/products/detail/omron-electronics-inc-emc-div/B3F-4055/377651), [Mouser](https://www.mouser.com/ProductDetail/653-B3F-4055) |
| **Button Caps (optional)** | Omron B32 Series | Tactile caps for 12×12mm buttons, various colors | 5 | $0.25 | $1.25 | [DigiKey](https://www.digikey.com/en/products/filter/switch-hardware/221), [Mouser](https://www.mouser.com/c/electromechanical/switches/switch-hardware/) |

**Total:** $2.60 (buttons only) or $3.85 (with caps)

### Recommended Button Option 2: E-Switch TL1105 (Budget-Friendly)

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **6×6mm Buttons (compact)** | E-Switch TL1105SPF160Q | 6×6×5mm, 160gf actuation, through-hole | 5 | $0.28 | $1.40 | [DigiKey #EG4621-ND](https://www.digikey.com/en/products/detail/e-switch/TL1105SPF160Q/3027634), [Mouser](https://www.mouser.com/ProductDetail/612-TL1105SPF160Q) |

**Total:** $1.40

### Optional: Hardware Debounce Capacitors

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **0.1µF Ceramic Capacitors** | Vishay K104K15X7RF5TL2 | 100nF, 50V, through-hole radial | 5 | $0.12 | $0.60 | [DigiKey #BC1084CT-ND](https://www.digikey.com/en/products/detail/vishay-bc-components/K104K15X7RF5TL2/286538), [Mouser](https://www.mouser.com/ProductDetail/594-K104K15X7RF5TL2) |

**Note:** Software debouncing is implemented in device tree and application code, making hardware capacitors optional.

### Prototyping/Mounting Options

| Item | Description | Qty | Unit Price | Total | Suppliers |
|------|-------------|-----|------------|-------|-----------|
| **Breadboard (400 tie-points)** | Half-size solderless breadboard for prototyping | 1 | $5.99 | $5.99 | [Amazon](https://www.amazon.com/EL-CP-003-Breadboard-Solderless-Distribution-Connecting/dp/B01EV6LJ7G), [Adafruit #64](https://www.adafruit.com/product/64) |
| **Perfboard (5×7cm)** | Universal prototype PCB for permanent mounting | 1 | $3.49 | $3.49 | [Amazon](https://www.amazon.com/ELEGOO-Prototype-Soldering-Compatible-Arduino/dp/B072Z7Y19F), [Adafruit #1609](https://www.adafruit.com/product/1609) |

---

## Power Supply Options

### Option 1: USB Wall Power (Development/Desktop Use)

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **Official RPi PSU** | Raspberry Pi PSU (T6716DV) | 5.1V 2.5A regulated, micro-USB, UL listed | 1 | $7.99 | $7.99 | [Adafruit #1995](https://www.adafruit.com/product/1995), [Amazon](https://www.amazon.com/CanaKit-Raspberry-Supply-Adapter-Listed/dp/B00MARDJZ4), [CanaKit](https://www.canakit.com/raspberry-pi-adapter-power-supply-2-5a-micro-usb.html) |

**Total:** $7.99 (no battery monitoring)

### Option 2: USB Power Bank (Portable, No Monitoring)

| Item | Description | Qty | Unit Price | Total | Suppliers |
|------|-------------|-----|------------|-------|-----------|
| **USB Power Bank (10,000mAh)** | Anker PowerCore 10000, dual-port, low-current mode | 1 | $21.99 | $21.99 | [Amazon](https://www.amazon.com/Anker-PowerCore-Ultra-Compact-High-speed-Technology/dp/B0194WDVHI) |
| **Short Micro-USB Cable** | 6-inch right-angle micro-USB cable | 1 | $4.99 | $4.99 | [Amazon](https://www.amazon.com/Cable-Matters-2-Pack-Right-Micro/dp/B00SVSNEW6) |

**Total:** $26.98 (~12-20 hours runtime, no monitoring)

### Option 3: UPS HAT with I2C Battery Monitoring ⭐ **Recommended for Portable Build**

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **Waveshare UPS HAT (C)** | Waveshare SKU #18305 | 5V UPS HAT, INA219 I2C monitor, Li-Po charger, boost converter | 1 | $25.99 | $25.99 | [Waveshare](https://www.waveshare.com/ups-hat-c.htm), [Amazon](https://www.amazon.com/Waveshare-UPS-HAT-Uninterruptible-Protection/dp/B093SYMRQG), [ThePiHut](https://thepihut.com/products/uninterruptible-power-supply-ups-hat-for-raspberry-pi-zero) |
| **18650 Li-Ion Batteries** | Samsung INR18650-35E | 3.7V 3500mAh, 8A discharge, protected | 2 | $7.99 | $15.98 | [Amazon](https://www.amazon.com/Samsung-INR18650-35E-Rechargeable-Batteries-Flat/dp/B07KQGFH2Y), [18650BatteryStore](https://www.18650batterystore.com/) |

**Total:** $41.97 (~6-10 hours runtime with I2C monitoring)

**Alternative UPS HATs:**

| Item | Manufacturer Part Number | Description | Unit Price | Suppliers |
|------|--------------------------|-------------|------------|-----------|
| **WatangTech UPS HAT** | WatangTech Zero UPS HAT | Built-in 1000mAh battery, INA219 monitor | $29.99 | [Amazon](https://www.amazon.com/WatangTech-Zero-UPS-HAT-Uninterruptible/dp/B0FS1F7CSQ) |
| **DFRobot UPS HAT** | DFRobot DFR0528 | Solar charging support, I2C monitoring | $24.99 | [DFRobot](https://www.dfrobot.com/) |

### Option 4: DIY Battery Solution (Advanced Users)

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **LiPo Battery** | Adafruit #328 | 3.7V 2500mAh LiPo with JST connector | 1 | $14.95 | $14.95 | [Adafruit #328](https://www.adafruit.com/product/328) |
| **PowerBoost 1000C** | Adafruit #2465 | 3.7V→5V boost converter, 1A output, load sharing | 1 | $19.95 | $19.95 | [Adafruit #2465](https://www.adafruit.com/product/2465) |
| **INA219 Breakout** (for monitoring) | Adafruit #904 | High-side I2C current/voltage/power monitor | 1 | $9.95 | $9.95 | [Adafruit #904](https://www.adafruit.com/product/904) |
| **JST PH 2-pin Connectors** | JST PH series | Battery connector (if not included) | 2 | $0.75 | $1.50 | [Adafruit #261](https://www.adafruit.com/product/261), [DigiKey](https://www.digikey.com/en/products/filter/rectangular-connectors-headers-male-pins/314) |
| **On/Off Switch** | C&K OS102011MA1QN1 | SPDT slide switch, 6-pin, through-hole | 1 | $0.85 | $0.85 | [DigiKey #CKN9559-ND](https://www.digikey.com/en/products/detail/c-k/OS102011MA1QN1/1981430) |

**Total:** $47.20 (most flexible, requires soldering and integration)

---

## Enclosure Options

### Option 1: 3D Printed Custom Case ⭐ **Recommended for Best Fit**

| Item | Description | Qty | Est. Cost | Service Options |
|------|-------------|-----|-----------|-----------------|
| **3D Printed Case** | Custom STL design (115×170×20mm), PLA/PETG/ABS | 1 set | $15-30 | DIY (filament cost) or [Craftcloud](https://craftcloud3d.com/), [Shapeways](https://www.shapeways.com/), [3D Hubs](https://www.hubs.com/) |
| **M2.5 Screws** | M2.5×6mm pan head screws (case assembly) | 8 | $2.99 | [Amazon](https://www.amazon.com/M2-5-Screws-Assortment-Kit/dp/B07VNDFYNQ) |
| **M2 Screws** | M2×6mm pan head screws (Pi Zero mounting) | 4 | included | Usually included in screw kit |
| **Rubber Feet** | Self-adhesive 8mm rubber feet | 4 | $3.99 | [Amazon](https://www.amazon.com/Adhesive-Furniture-Dampening-Transparent-Hemispheres/dp/B07VNSXY84) |

**Total:** $21.98 - $36.98

**3D Printing Notes:**
- STL files to be provided in `hardware/case-design/` directory
- Print settings: 0.2mm layer height, 20% infill, no supports needed
- Material: PLA (easiest), PETG (durable), ABS (strong, requires heated bed)
- Estimated print time: 6-8 hours for full case
- Filament required: ~100g (~$2-3 in material cost)

### Option 2: Commercial Project Box (Quick Assembly)

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **Hammond ABS Enclosure** | Hammond 1591XXCBK | 120×170×55mm black ABS enclosure | 1 | $12.99 | $12.99 | [DigiKey #HM142-ND](https://www.digikey.com/en/products/detail/hammond-manufacturing/1591XXCBK/280036), [Mouser](https://www.mouser.com/ProductDetail/546-1591XXCBK), [Amazon](https://www.amazon.com/Hammond-1591XXCBK-ABS-Project-Black/dp/B0002BSRIO) |
| **Drill Bits** (if not owned) | Step drill bit set for display/button holes | 1 | $12.99 | [Amazon](https://www.amazon.com/Neiko-10193A-Titanium-Step-Drill/dp/B000BQMKEM) |

**Total:** $12.99 (+ drilling tools if needed)

**Modification Requirements:**
- Cut display opening: 85×64mm rectangular cutout
- Drill 5× 12mm holes for buttons
- Drill micro-USB power port hole
- See [[CASE_DESIGN]] for detailed instructions

### Option 3: Laser-Cut Acrylic Layered Case

| Item | Description | Qty | Est. Cost | Service Options |
|------|-------------|-----|-----------|-----------------|
| **Laser-Cut Acrylic Layers** | 3mm clear acrylic, 5 layers (DXF design provided) | 1 set | $20-35 | [Ponoko](https://www.ponoko.com/), [SendCutSend](https://sendcutsend.com/), Local makerspaces |
| **M2.5 Standoffs** | M2.5×10mm brass standoffs (hex, male-female) | 8 | $4.99 | [Amazon](https://www.amazon.com/Litorange-Standoff-Stainless-Assortment-Male-Female/dp/B07D7828LC) |
| **M2.5 Screws/Nuts** | M2.5×6mm screws and nuts | 16 | $2.99 | [Amazon](https://www.amazon.com/M2-5-Screws-Assortment-Kit/dp/B07VNDFYNQ) |

**Total:** $27.98 - $42.98

### Option 4: DIY Cardboard/Wood Prototype (Free/Minimal Cost)

| Item | Description | Cost |
|------|-------------|------|
| **Cardboard or 3mm plywood** | Craft materials, hand-cut with utility knife/saw | $0-5 |
| **Hot glue or wood glue** | Assembly adhesive | $3-5 |

**Total:** $3-10 (functional but not polished)

---

## Optional Development Tools

| Item | Manufacturer Part Number | Description | Qty | Unit Price | Total | Suppliers |
|------|--------------------------|-------------|-----|------------|-------|-----------|
| **USB-to-TTL Serial Adapter** | Adafruit #954 (CP2102) | 3.3V/5V USB-UART converter for debugging | 1 | $9.95 | $9.95 | [Adafruit #954](https://www.adafruit.com/product/954), [Amazon](https://www.amazon.com/HiLetgo-CP2102-Converter-Adapter-Downloader/dp/B00LODGRV8) |
| **MicroSD Card Reader** | Various | USB 3.0 multi-card reader | 1 | $7.99 | $7.99 | [Amazon](https://www.amazon.com/Anker-Portable-Reader-RS-MMC-Micro/dp/B006T9B6R2), Local stores |
| **Multimeter** | UNI-T UT33D | Digital multimeter (voltage, current, continuity) | 1 | $18.99 | $18.99 | [Amazon](https://www.amazon.com/AstroAI-Digital-Multimeter-Voltage-Tester/dp/B01ISAMUA6) |

**Total:** $36.93

---

## Assembly Tools Required

### Basic Tools (Minimum Required)

| Tool | Description | Est. Price | Suppliers |
|------|-------------|------------|-----------|
| **Soldering Iron** | 60W adjustable temp (300-450°C), fine tip | $25-40 | [Amazon](https://www.amazon.com/X-Tronic-3020-XTS-Digital-Display-Soldering/dp/B01DGZFSNE), [Adafruit #180](https://www.adafruit.com/product/180) |
| **Solder** | 60/40 rosin-core or lead-free (0.8mm diameter) | $5-10 | [Amazon](https://www.amazon.com/MAIYUM-63-37-Solder-Electrical-Soldering/dp/B075WB98FJ), Hardware stores |
| **Flush Cutters** | Precision wire cutters for trimming leads | $6-12 | [Amazon](https://www.amazon.com/Hakko-CHP-170-Micro-Cutter/dp/B00FZPDG1K), [Adafruit #152](https://www.adafruit.com/product/152) |
| **Wire Strippers** | 22-30 AWG wire strippers/crimpers | $10-15 | [Amazon](https://www.amazon.com/VISE-GRIP-Multi-Tool-Stripper-Cutter-2078309/dp/B000JNNWQ2) |
| **Small Screwdriver Set** | Phillips #0/#1 and flathead precision drivers | $8-15 | [Amazon](https://www.amazon.com/TEKTON-26-Piece-Precision-Screwdriver-29106/dp/B08D3TNXDM), Hardware stores |

**Tool Subtotal:** $54-92 (one-time purchase, reusable)

### Optional Advanced Tools

| Tool | Description | Est. Price | Suppliers |
|------|-------------|------------|-----------|
| **Helping Hands** | Third-hand soldering jig with magnifier | $12-20 | [Amazon](https://www.amazon.com/QuadHands-Helping-Hands-Third-Soldering/dp/B00GIKVP5K) |
| **Desoldering Pump** | Solder sucker for fixing mistakes | $5-8 | [Amazon](https://www.amazon.com/Soldapullt-DS017-Desoldering-Tool/dp/B000NQKPF0) |
| **Drill + Bits** | Variable speed drill with step bits (for case mods) | $30-60 | [Amazon](https://www.amazon.com/BLACK-DECKER-20V-Cordless-Drill/dp/B006V6YAPI), Hardware stores |
| **Digital Calipers** | Precision measurement (for case design) | $15-25 | [Amazon](https://www.amazon.com/Neiko-01407A-Electronic-Digital-Stainless/dp/B000GSLKIW) |

---

## Detailed Component Specifications

### Raspberry Pi Zero W

- **Full Part Number:** Raspberry Pi Zero W v1.1
- **Manufacturer:** Raspberry Pi Foundation
- **CPU:** Broadcom BCM2835 SoC, ARM1176JZF-S @ 1GHz (single-core)
- **RAM:** 512MB LPDDR2
- **Wireless:**
  - WiFi: 802.11 b/g/n 2.4GHz (Cypress CYW43438)
  - Bluetooth: 4.1 BLE (Broadcom BCM43438)
- **GPIO:** 40-pin header (may require soldering)
- **Power Consumption:**
  - Idle: ~120mA (0.6W @ 5V)
  - Active (e-reader app): ~150-200mA (0.75-1.0W)
  - Peak (display refresh): ~275mA (1.4W)
- **Dimensions:** 65mm × 30mm × 5mm
- **Operating Temp:** 0°C to 50°C

**Purchase Notes:**
- **Pre-soldered Header:** Raspberry Pi Zero WH costs ~$20 (eliminates soldering)
- **Availability:** Check stock at multiple suppliers (often limited)
- **Alternatives:** Pi Zero 2 W (4× ARM Cortex-A53 cores @ 1GHz, $15-20, higher power draw)

### Waveshare 4.2" E-Paper Display Module (Rev 2.2)

- **Full Part Number:** Waveshare 4.2inch E-Paper Module (Rev 2.2)
- **Waveshare SKU:** 13504
- **Amazon ASIN:** B074NR1SW2
- **Resolution:** 400×300 pixels (landscape orientation)
- **Display Area:** 84.8mm × 63.6mm (active area)
- **Pixel Pitch:** 0.212mm × 0.212mm
- **Color Depth:** 1-bit (black/white)
- **Controller IC:** IL0398 (or WF0420T16A driver board)
- **Interface:** 4-wire SPI (MOSI, SCLK, CS, DC, RST, BUSY)
- **Refresh Time:**
  - Full refresh: ~2 seconds
  - Partial refresh: ~300ms
- **Power:**
  - Operating voltage: 3.3V or 5V
  - Refresh current: ~26mA @ 3.3V
  - Standby current: <0.1mA (bistable, no power needed to retain image)
- **Viewing Angle:** >170° (paper-like)
- **Dimensions:** 103.0mm × 78.5mm (PCB)
- **Operating Temp:** 0°C to 50°C
- **Storage Temp:** -25°C to 70°C

**CRITICAL Purchase Notes:**
- ⚠️ **Must be Rev 2.2** (latest revision) - Rev 2.1 has different pinout and incompatible driver
- ⚠️ Ensure full module with driver board is included (not just bare panel)
- ⚠️ Waveshare makes multiple 4.2" variants (B/W, B/W/Red, B/W/Yellow) - only B/W is directly compatible
- ⚠️ Verify Amazon ASIN B074NR1SW2 or check seller specifications carefully

### MicroSD Card

- **Recommended:** SanDisk Ultra 16GB (SDSQUAR-016G-GN6MA)
- **Alternative:** Samsung EVO Select 16GB (MB-ME16GA/AM)
- **Capacity:** 16GB (8GB minimum, 32GB+ unnecessary for e-reader)
- **Speed Class:**
  - Class 10 (minimum 10MB/s write)
  - UHS-I U1 (minimum 10MB/s)
  - A1 rated (random I/O: 1500 IOPS read, 500 IOPS write)
- **Filesystem:** ext4 (root), FAT32 (boot partition)
- **Expected Lifespan:** 10,000+ write cycles (suitable for e-reader use)

**Purchase Notes:**
- ⚠️ Avoid ultra-cheap/unknown brands (high failure rates)
- ⚠️ A1/A2 Application Performance Class improves boot time
- ⚠️ UHS-II/III cards offer no benefit (Pi Zero limited to 25MB/s SD clock)
- ✅ Buy from reputable sellers (counterfeit SD cards common on Amazon)

### Tactile Push Buttons - Omron B3F-4055 (Recommended)

- **Manufacturer:** Omron Electronics
- **Part Number:** B3F-4055
- **DigiKey P/N:** SW400-ND
- **Mouser P/N:** 653-B3F-4055
- **Size:** 12×12×7.3mm (button footprint × height)
- **Actuation Force:** 260gf ± 70gf (gram-force)
- **Contact Resistance:** 100mΩ maximum
- **Insulation Resistance:** 100MΩ minimum @ 500VDC
- **Bounce Time:** 5ms maximum
- **Operating Life:** 500,000 cycles minimum
- **Operating Temp:** -25°C to 70°C
- **Terminals:** Through-hole, 4-pin (2 pairs of NO contacts)
- **Mounting:** PCB or perfboard, 2.54mm grid compatible

**Why B3F-4055:**
- ✅ Large 12×12mm size (easy to press, user-friendly)
- ✅ Professional-grade reliability (500k+ cycles)
- ✅ Comfortable 260gf actuation force (not too light, not too stiff)
- ✅ Industry-standard part (widely available)
- ✅ Compatible with optional colored caps (Omron B32 series)

**Alternative:** Omron B3F-1000 (6×6mm, more compact for smaller enclosures, $0.30 ea)

### GPIO Header - Sullins PRPC040SAAN-RC

- **Manufacturer:** Sullins Connector Solutions
- **Part Number:** PRPC040SAAN-RC
- **DigiKey P/N:** S1011EC-40-ND
- **Type:** 2×20 position, male header
- **Pitch:** 2.54mm (0.1")
- **Contact Material:** Phosphor bronze, tin-plated
- **Insulator:** Black nylon, 94V-0 rated
- **Pin Length:** 11.5mm (below insulator for soldering)
- **Current Rating:** 3A per contact
- **Operating Temp:** -55°C to 105°C

**Note:** Only required if Pi Zero W does not have pre-soldered header (check before purchasing).

### Jumper Wires - Dupont Female-Female

- **Type:** Female-to-female Dupont connectors
- **Wire Gauge:** 26 AWG (0.14mm²)
- **Length:** 10cm (4") recommended (20cm and 30cm also common)
- **Connector Pitch:** 2.54mm (compatible with 0.1" headers)
- **Color:** Assorted (for signal identification)
- **Current Rating:** ~1A continuous per wire
- **Voltage Rating:** 300V

**Purchase Notes:**
- 40-piece kit provides spares and color variety
- Higher-quality wires (thicker 24 AWG) available for power connections
- Pre-crimped connectors save time vs. DIY crimping

### Power Supply - Official Raspberry Pi 5.1V 2.5A PSU

- **Manufacturer:** Raspberry Pi Foundation
- **Part Number:** T6716DV (US version)
- **Output:** 5.1V DC, 2.5A (12.75W maximum)
- **Input:** 100-240V AC, 50/60Hz (universal)
- **Connector:** Micro-USB Type B
- **Cable Length:** 1.5m (5 feet)
- **Efficiency:** 79% minimum (Level VI)
- **Certifications:** UL, CE, FCC
- **Features:**
  - Over-current protection
  - Over-temperature protection
  - Short-circuit protection
  - 18-month warranty

**Why 5.1V:** Compensates for voltage drop in micro-USB cable (ensures stable 5.0V at Pi Zero)

**Alternative PSUs:**
- CanaKit 5V 2.5A (similar specs, UL listed, $7.99)
- Anker PowerPort (if using USB-A cable, verify 2A+ output)

### Waveshare UPS HAT (C) - Battery Power Solution

- **Manufacturer:** Waveshare
- **Part Number:** Waveshare SKU #18305
- **Features:**
  - Li-Po/Li-Ion battery charging and boost conversion
  - INA219 I2C power monitor (voltage, current, power, battery %)
  - Battery protection: over-charge, over-discharge, over-current, short-circuit
  - Path management (simultaneous charge + power output)
  - 5V regulated output via boost converter
- **I2C Address:** 0x42 (INA219 monitor)
- **Battery Support:**
  - Single 18650 Li-Ion (3.7V nominal)
  - Dual 18650 (parallel, 7000mAh typical)
  - Charging: 1A max via micro-USB input
- **Runtime (estimated):**
  - Single 3500mAh 18650: ~6-7 hours (e-reader use)
  - Dual 3500mAh 18650: ~12-14 hours
- **Dimensions:** 65mm × 30mm (Pi Zero footprint)
- **Mounting:** Stacks on Pi Zero GPIO header

**Recommended Batteries:**
- Samsung INR18650-35E (3500mAh, 8A discharge, protected)
- LG INR18650-MJ1 (3500mAh, 10A discharge)
- Sony VTC6 (3000mAh, 15A discharge, high reliability)

⚠️ **Battery Safety:** Only use protected Li-Ion cells. Never use damaged batteries. Follow proper Li-Ion charging/storage procedures.

---

## Supplier Information

### Recommended Suppliers (United States)

#### **Adafruit Industries** - [adafruit.com](https://www.adafruit.com)
- **Best For:** Raspberry Pi boards, breakout boards, batteries, cables
- **Pros:**
  - Excellent documentation and tutorials
  - High-quality curated products
  - Great customer support
  - Fast US shipping (usually ships same/next day)
  - International shipping available
- **Cons:** Slightly higher prices than bulk suppliers
- **Shipping:** Free over $100 (US), USPS/UPS options
- **Returns:** 30-day satisfaction guarantee (unused items)

#### **Waveshare Official Store** - [waveshare.com](https://www.waveshare.com)
- **Best For:** E-paper displays (direct from manufacturer)
- **Pros:**
  - Authoritative source for display modules
  - Comprehensive technical documentation
  - Latest hardware revisions
- **Cons:**
  - Ships from China (2-4 weeks typical)
  - Higher shipping costs for small orders
- **Shipping:** DHL, FedEx, China Post options
- **Returns:** 30-day return policy (shipping costs not refunded)

#### **Amazon** - [amazon.com](https://www.amazon.com)
- **Best For:** Fast delivery, general components, power banks
- **Pros:**
  - Free Prime shipping (1-2 days)
  - Easy returns
  - Wide selection
- **Cons:**
  - Variable quality (check seller ratings)
  - Counterfeit risk (especially SD cards, batteries)
  - Prices fluctuate
- **Shipping:** Prime free 2-day, standard 5-7 days
- **Returns:** 30 days, usually free return shipping

#### **DigiKey Electronics** - [digikey.com](https://www.digikey.com)
- **Best For:** Buttons, connectors, precision components
- **Pros:**
  - Massive inventory (10M+ parts)
  - Same-day shipping (orders by 8pm ET)
  - Accurate technical specs and parametric search
  - No minimum order (but shipping costs apply)
- **Cons:**
  - Overwhelming selection (requires knowing exact part numbers)
  - Higher shipping costs for small orders
- **Shipping:** FedEx, UPS, USPS; free over $50 (US)
- **Returns:** 30 days (unopened/unused)

#### **Mouser Electronics** - [mouser.com](https://www.mouser.com)
- **Best For:** Electronic components, buttons, connectors (DigiKey alternative)
- **Pros:**
  - Large selection (similar to DigiKey)
  - International shipping to 223 countries
  - Good technical support
- **Cons:** Similar to DigiKey (shipping costs for small orders)
- **Shipping:** Free over $50 (US), DHL/FedEx for international
- **Returns:** 30 days

#### **SparkFun Electronics** - [sparkfun.com](https://www.sparkfun.com)
- **Best For:** Hobbyist-friendly breakout boards, kits
- **Pros:**
  - Educational focus with tutorials
  - Open-source hardware designs
  - Good for beginners
- **Cons:** Smaller selection than Adafruit
- **Shipping:** Free over $50 (US)
- **Returns:** 30-day return policy

### International Suppliers

#### **The Pi Hut (United Kingdom)** - [thepihut.com](https://thepihut.com)
- Raspberry Pi official reseller for UK/EU
- Free UK shipping over £30
- Ships worldwide

#### **Pimoroni (United Kingdom)** - [pimoroni.com](https://www.pimoroni.com)
- Raspberry Pi accessories and creative electronics
- Free UK shipping over £50
- Beautiful packaging and documentation

#### **Core Electronics (Australia)** - [core-electronics.com.au](https://core-electronics.com.au)
- Official Raspberry Pi distributor for Australia/NZ
- Free AU shipping over $75
- Local warranty support

#### **BuyZero / Pi3g (Germany)** - [buyzero.de](https://buyzero.de)
- Raspberry Pi Zero specialist (name origin)
- Fast EU shipping
- German and English support

---

## Cost Summary

### Build Configuration 1: Minimum Viable E-Reader (Breadboard Prototype)

| Component Category | Cost |
|--------------------|------|
| Core electronics (Pi Zero W, display, SD card, wires) | $66.07 |
| Buttons (Omron B3F-4055 × 5) | $2.60 |
| USB wall power supply | $7.99 |
| Breadboard (prototyping) | $5.99 |
| **Subtotal (before tools)** | **$82.65** |
| Soldering tools (if not owned) | $54.00 |
| **Total (first-time build)** | **$136.65** |
| **Total (if tools owned)** | **$82.65** |

**This build provides:** Working e-reader, not portable, exposed electronics on breadboard, USB wall powered

---

### Build Configuration 2: Complete Portable E-Reader (3D Printed Case + UPS HAT)

| Component Category | Cost |
|--------------------|------|
| Core electronics | $66.07 |
| Buttons (Omron B3F-4055 × 5) | $2.60 |
| Waveshare UPS HAT (C) + 2× 18650 batteries | $41.97 |
| 3D printed case (DIY filament cost) | $15.00 |
| Screws, standoffs, rubber feet | $6.98 |
| Perfboard (for button mounting) | $3.49 |
| **Subtotal (before tools)** | **$136.11** |
| Soldering tools (if not owned) | $54.00 |
| **Total (first-time build)** | **$190.11** |
| **Total (if tools owned)** | **$136.11** |

**This build provides:** Fully portable e-reader, 6-10 hour battery, I2C battery monitoring, professional appearance

---

### Build Configuration 3: Budget Portable Build (Power Bank + Commercial Case)

| Component Category | Cost |
|--------------------|------|
| Core electronics | $66.07 |
| Buttons (E-Switch TL1105, budget option) | $1.40 |
| USB power bank (10,000mAh) + cable | $26.98 |
| Hammond ABS enclosure | $12.99 |
| Perfboard | $3.49 |
| **Subtotal (before tools)** | **$110.93** |
| Soldering tools (if not owned) | $54.00 |
| Drill + step bits (for case modification) | $25.00 |
| **Total (first-time build)** | **$189.93** |
| **Total (if tools owned)** | **$110.93** |

**This build provides:** Portable e-reader, 12-20 hour runtime, no battery monitoring, requires case drilling

---

### Build Configuration 4: Premium Build (UPS HAT + Laser-Cut Acrylic Case)

| Component Category | Cost |
|--------------------|------|
| Core electronics | $66.07 |
| Buttons (Omron B3F-4055 × 5 with colored caps) | $3.85 |
| Waveshare UPS HAT (C) + 2× 18650 batteries | $41.97 |
| Laser-cut acrylic case (5 layers) | $35.00 |
| Standoffs, screws, hardware | $7.98 |
| Perfboard | $3.49 |
| **Subtotal (before tools)** | **$158.36** |
| Soldering tools (if not owned) | $54.00 |
| **Total (first-time build)** | **$212.36** |
| **Total (if tools owned)** | **$158.36** |

**This build provides:** Premium portable e-reader, transparent acrylic aesthetic, I2C monitoring, 6-10 hour battery

---

### Optional Add-Ons (Any Configuration)

| Item | Cost | Purpose |
|------|------|---------|
| USB-to-TTL serial adapter | $9.95 | Development debugging |
| MicroSD card reader | $7.99 | Flashing images |
| Multimeter | $18.99 | Testing connections |
| Hardware debounce capacitors (5×) | $0.60 | Optional button debouncing |
| Button caps (Omron B32 series) | $1.25 | Tactile feel improvement |

---

## Alternative Components

### If Raspberry Pi Zero W is Unavailable

| Alternative | Specs | Price | Compatibility Notes |
|-------------|-------|-------|---------------------|
| **Raspberry Pi Zero 2 W** | BCM2710A1 (4× Cortex-A53 @ 1GHz), 512MB RAM | $15-20 | ✅ Pin-compatible, faster, higher power draw (~200-300mA active) |
| **Raspberry Pi 3A+** | BCM2837B0 (4× Cortex-A53 @ 1.4GHz), 512MB RAM | $25 | ✅ Compatible GPIO, requires larger case, more power |
| **Orange Pi Zero LTS** | Allwinner H2+ quad-core, 256/512MB RAM | $15-20 | ⚠️ Requires software porting (different GPIO library, device tree) |

**Recommendation:** Pi Zero 2 W is best upgrade (same form factor, 4× performance boost)

### If Waveshare 4.2" Display is Unavailable

| Alternative | Specs | Price | Compatibility Notes |
|-------------|-------|-------|---------------------|
| **Waveshare 2.9" E-Paper** | 296×128, B/W, SPI | $20 | ⚠️ Requires driver changes, smaller screen (less readable) |
| **Waveshare 7.5" E-Paper** | 800×480, B/W, SPI | $60-80 | ⚠️ Requires driver changes, larger case, slower refresh, higher cost |
| **Good Display GDEW042T2** | 400×300, B/W, SPI, IL0398 | $30-40 | ✅ Should be compatible (same controller), verify pinout |
| **Pervasive Displays 4.2"** | 400×300, B/W, SPI | $35-50 | ⚠️ Different controller, requires driver rewrite |

**⚠️ CRITICAL:** Display changes require software driver modifications. Only Good Display GDEW042T2 with IL0398 controller may work directly.

### If Omron Buttons are Out of Stock

| Alternative | Part Number | Price (ea) | Compatibility |
|-------------|-------------|------------|---------------|
| **E-Switch TL1105SPF160Q** (6×6mm) | TL1105SPF160Q | $0.28 | ✅ Direct replacement (more compact) |
| **C&K PTS645** (12×12mm) | PTS645SM43SMTR92LFS | $0.45 | ✅ Direct replacement (similar specs) |
| **Generic tactile switches** | Various (Amazon kits) | $0.10-0.20 | ⚠️ Variable quality, check datasheet |

---

## Procurement Strategy

### Order Timeline (for fastest build)

**Week 1-2 (Order Immediately):**
1. **Waveshare 4.2" E-Paper Display** from Waveshare.com or Amazon
   - Longest shipping time (2-4 weeks from China if ordering from Waveshare direct)
   - **Alternative:** Order from Amazon (faster, verify Rev 2.2)
2. **Raspberry Pi Zero W** from Adafruit/Amazon/PiShop
   - Check stock at multiple suppliers (often limited availability)
   - Consider Pi Zero WH (pre-soldered header) to save time

**Week 2-3 (After confirming Pi Zero availability):**
3. **UPS HAT or Power Supply** (depends on desired build)
   - Waveshare UPS HAT: 1-2 weeks shipping
   - USB power bank: 1-2 days (Amazon Prime)
   - Wall PSU: 1-2 days (Amazon/Adafruit)
4. **18650 Batteries** (if using UPS HAT)
   - Amazon: 1-2 days
   - ⚠️ Verify genuine cells (counterfeits common)

**Week 3-4 (Accessories and case):**
5. **Buttons, wires, SD card, hardware** from DigiKey/Mouser/Amazon
   - DigiKey/Mouser: next-day shipping
   - Amazon: 1-2 days
6. **Enclosure** (longest lead time varies by option)
   - 3D printed (DIY): 6-8 hours print time
   - 3D printed (service): 1-2 weeks
   - Commercial box: 1-2 days (Amazon/DigiKey)
   - Laser-cut acrylic: 1-2 weeks (Ponoko/SendCutSend)

**Week 4+ (After receiving all parts):**
7. **Soldering tools** (if not owned)
   - Amazon: 1-2 days

### Money-Saving Tips

1. **Bundle Purchases:** Combine orders from same supplier to save on shipping
   - DigiKey/Mouser: Free shipping over $50 (US)
   - Adafruit: Free shipping over $100 (US)
   - Amazon: Use Prime for free 2-day

2. **Check for Kits:** Some suppliers offer Raspberry Pi starter kits
   - Often includes Pi Zero, power supply, SD card, case
   - Compare kit price vs. individual components

3. **Buy in Bulk:** If building multiple units
   - Buttons: Buy 50-100 pack ($0.10-0.15 ea vs. $0.30-0.50)
   - Resistors/caps: Buy assortment kits
   - Jumper wires: 120-piece kits (~$10)

4. **3D Printing:**
   - **Cheapest:** Print yourself ($2-3 filament cost)
   - **Budget:** Local makerspace ($5-10)
   - **Moderate:** Online service ($15-30)

5. **Alternative Power:**
   - Power bank ($22) costs half of UPS HAT ($42) but no monitoring
   - Wall PSU ($8) is cheapest for desk use

6. **Verify Stock Before Ordering:**
   - Pi Zero W has periodic shortages
   - Have backup suppliers ready

### Quality vs. Cost Decisions

**Splurge On (worth the cost):**
- ✅ **E-paper display:** Buy genuine Waveshare Rev 2.2 (compatibility critical)
- ✅ **Power supply:** Official RPi PSU or quality UPS HAT (stability critical)
- ✅ **MicroSD card:** Reputable brand (SanDisk, Samsung) - failures waste hours
- ✅ **Buttons:** Omron/C&K/E-Switch (reliability, tactile feel)

**Save Money On (budget options acceptable):**
- 💰 **Jumper wires:** Generic Amazon kits work fine
- 💰 **Enclosure:** DIY cardboard/3D print vs. laser-cut acrylic
- 💰 **Perfboard:** Any universal PCB works
- 💰 **Screws/hardware:** Generic metric screw sets

**Don't Cheap Out:**
- ❌ **18650 batteries:** Fake/damaged cells are fire hazards
- ❌ **Soldering iron:** Poor temp control damages components
- ❌ **SD cards from unknown sellers:** High counterfeit rate on Amazon

---

## Notes and Considerations

### Lead Times
- **Fastest build:** 1 week (Amazon Prime + pre-owned tools)
- **Typical build:** 2-3 weeks (Waveshare display shipping)
- **International build:** 3-4 weeks (shipping from China/US)

### Total Cost Breakdown by Builder Experience

| Builder Type | Minimum Cost | Typical Cost | Premium Cost |
|--------------|--------------|--------------|--------------|
| **Has tools, breadboard proto** | $73 | $110 | $158 |
| **Has tools, portable build** | $136 | $158 | $212 |
| **First-time builder (needs tools)** | $137 | $190 | $266 |

### Battery Runtime Estimates

| Power Configuration | Capacity | Estimated Runtime |
|---------------------|----------|-------------------|
| Wall PSU (5V 2.5A) | Unlimited | N/A (desktop use) |
| USB Power Bank (10,000mAh) | 50Wh | 12-20 hours |
| Single 18650 (3500mAh) | 13Wh | 6-7 hours |
| Dual 18650 (7000mAh) | 26Wh | 12-14 hours |

**Runtime Assumptions:**
- Average power draw: 150mA @ 5V = 0.75W
- Screen refreshes: ~2-3 per minute (reading)
- WiFi off during reading
- Actual runtime varies with usage patterns

### Regulatory and Safety Notes

- ⚠️ **Li-Ion Battery Safety:** Only use protected cells, proper chargers, avoid damaged batteries
- ⚠️ **Power Supply:** Use UL/CE certified PSUs to avoid fire hazards
- ⚠️ **ESD Precautions:** Handle Pi Zero and display with anti-static precautions
- ⚠️ **Soldering:** Use in well-ventilated area, avoid lead solder fumes

---

## Revision History

- **2026-01-17:** Phase 07 update - Complete rewrite with verified part numbers, supplier links, enclosure options, power configurations, and detailed cost breakdowns
- **2026-01-13:** Phase 02 update - Added button specifications and part numbers
- **2026-01-13:** Initial BOM created for Phase 01

---

## Related Documentation

- **[[WIRING_GUIDE]]** - Pin connections and assembly instructions
- **[[SCHEMATIC]]** - Electrical schematics and circuit diagrams
- **[[CASE_DESIGN]]** - Enclosure design and assembly guide
- **[[POWER_SUPPLY]]** - Battery and UPS HAT configuration
- **[[BUILD_GUIDE]]** - Complete step-by-step build instructions
- **[[DISPLAY_SPECS]]** - E-paper display technical specifications
- **[[BUTTON_LAYOUT]]** - GPIO button assignments and wiring

---

**Last Updated:** 2026-01-17
**BOM Version:** 2.0 (Phase 07 Release)
**Status:** Complete and verified
