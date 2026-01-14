---
type: reference
title: Bill of Materials (BOM) - Open Source E-Reader
created: 2026-01-13
tags:
  - hardware
  - bom
  - parts-list
  - shopping
related:
  - "[[WIRING_GUIDE]]"
  - "[[DISPLAY_SPECS]]"
---

# Bill of Materials (BOM)

This document lists all components required to build the Open Source E-Reader. Prices are approximate and in USD as of January 2026.

## Required Components (Phase 1)

These are the minimum components needed to complete Phase 1 (Foundation and Working Display).

| Item | Description | Quantity | Est. Price | Suppliers |
|------|-------------|----------|------------|-----------|
| Raspberry Pi Zero W | BCM2835 SoC, 512MB RAM, WiFi, Bluetooth | 1 | $15 | [Adafruit](https://www.adafruit.com/product/3400), [PiShop.us](https://www.pishop.us/), [CanaKit](https://www.canakit.com/) |
| Waveshare 4.2" E-Paper Display | 400x300, B/W, Rev 2.2, SPI interface | 1 | $30-40 | [Waveshare.com](https://www.waveshare.com/4.2inch-e-paper-module.htm), [Amazon](https://amazon.com) |
| MicroSD Card | 8GB minimum, Class 10, UHS-I recommended | 1 | $5-8 | Amazon, Local electronics stores |
| 40-pin GPIO Header | 2x20 pins, 2.54mm pitch (if not pre-soldered) | 1 | $1-2 | Adafruit, Amazon, DigiKey |
| Female-Female Jumper Wires | 10cm length, assorted colors (pack of 40) | 1 pack | $3-5 | Amazon, Adafruit |
| 5V 2A Micro-USB Power Supply | High-quality PSU for stable operation | 1 | $8-10 | Official RPi Foundation PSU recommended |
| Micro-USB Cable | For power (if not included with PSU) | 1 | $3-5 | Any electronics store |

**Phase 1 Total:** ~$65-80 USD

## Optional Components (Phase 1)

| Item | Description | Quantity | Est. Price | Suppliers |
|------|-------------|----------|------------|-----------|
| USB-to-TTL Serial Adapter | For debugging via UART console | 1 | $5-10 | Amazon, Adafruit (CP2102 or FTDI-based) |
| MicroSD Card Reader | USB adapter for flashing SD cards | 1 | $5-10 | Amazon, local stores |
| Soldering Kit | Iron, solder, flux (if headers not pre-soldered) | 1 | $20-50 | Amazon, local electronics stores |

## Additional Components (Phase 2+)

These components will be needed in future phases.

### Phase 2: Button Input System

| Item | Description | Quantity | Est. Price | Suppliers |
|------|-------------|----------|------------|-----------|
| Tactile Push Buttons | 6x6mm or 12x12mm, through-hole | 5 | $3-5 | Amazon, DigiKey, Mouser |
| Breadboard (optional) | For prototyping button connections | 1 | $5-8 | Amazon, Adafruit |
| Perfboard (optional) | For permanent button mounting | 1 | $3-5 | Amazon, Adafruit |
| 0.1µF Capacitors (optional) | Ceramic capacitor for hardware debouncing | 5 | $1-2 | DigiKey, Mouser, Amazon |

**Note:** Pull-up resistors are NOT required - the Raspberry Pi's internal pull-up resistors (enabled via device tree) are used instead.

### Phase 3-4: Enclosure and Power

| Item | Description | Quantity | Est. Price | Suppliers |
|------|-------------|----------|------------|-----------|
| 3D Printed Enclosure | Custom design (STL files to be provided) | 1 | $10-20 | DIY 3D printing or online service |
| LiPo Battery | 3.7V, 2000-3000mAh (optional, for portable use) | 1 | $10-15 | Adafruit, Amazon |
| PowerBoost 1000C | LiPo to 5V converter with charging (optional) | 1 | $20 | Adafruit |
| Toggle Switch | Power on/off switch | 1 | $2-3 | Amazon, DigiKey |

## Tools Required

These tools are needed for assembly:

| Tool | Purpose | Est. Price |
|------|---------|------------|
| Soldering Iron | Soldering headers and wires | $20-50 |
| Solder (60/40 or lead-free) | Electrical connections | $5-10 |
| Wire Strippers | Stripping jumper wires (if needed) | $10-15 |
| Multimeter | Checking connections and voltage | $15-30 |
| Small Screwdrivers | Assembly (if using enclosure) | $10-15 |

## Detailed Component Specifications

### Raspberry Pi Zero W

- **Model:** Raspberry Pi Zero W v1.1
- **CPU:** BCM2835 ARM1176JZF-S @ 1GHz
- **RAM:** 512MB LPDDR2
- **WiFi:** 802.11 b/g/n 2.4GHz
- **Bluetooth:** 4.1 (BLE)
- **GPIO:** 40-pin header (may require soldering)
- **Power:** 5V via micro-USB, ~150-200mA typical
- **Alternatives:** Pi Zero 2 W (quad-core, faster, but higher power consumption)

**Purchase Notes:**
- Pre-soldered GPIO header versions available at slightly higher cost
- Consider Pi Zero 2 W for better performance (doubles the price)
- Check availability - Pi Zero W often has stock issues

### Waveshare 4.2" E-Paper Display Module (Rev 2.2)

- **Model:** Waveshare 4.2inch E-Paper Module (Rev 2.2)
- **Resolution:** 400x300 pixels
- **Display Area:** 84.8mm × 63.6mm
- **Color:** Black and White (2-level grayscale)
- **Interface:** SPI
- **Partial Refresh:** Supported
- **Full Refresh Time:** ~2 seconds
- **Operating Voltage:** 3.3V / 5V compatible
- **Operating Current:** ~26mA (refresh), <0.1mA (standby)
- **Controller:** IL0398 (or similar)

**Purchase Notes:**
- Ensure you get **Rev 2.2** (latest version)
- Avoid older revisions (Rev 2.1 has different pinout/driver)
- Some vendors sell with or without the driver board - get the full module
- Waveshare also makes a 4.2" model with red/black/white - not compatible without driver changes

**Part Numbers:**
- Waveshare SKU: 4.2inch e-Paper Module
- Amazon ASIN: B074NR1SW2 (verify Rev 2.2)

### MicroSD Card

- **Capacity:** 8GB minimum (16GB recommended for future expansion)
- **Speed Class:** Class 10 or UHS-I minimum
- **Brand:** SanDisk, Samsung, Kingston (reputable brands recommended)

**Purchase Notes:**
- Avoid very cheap/unknown brands (reliability issues)
- 32GB+ not necessary for Phase 1-2
- Application Performance Class A1 or A2 provides better random I/O

### Power Supply

- **Voltage:** 5V ± 0.25V
- **Current:** 2A minimum (2.5A recommended)
- **Connector:** Micro-USB
- **Quality:** Official Raspberry Pi power supply strongly recommended

**Purchase Notes:**
- Poor quality PSUs cause voltage drops and system instability
- E-paper refresh can draw current spikes - headroom important
- Look for "5.1V" labeled PSUs (accounts for voltage drop in cable)

### GPIO Header (if needed)

- **Type:** 2x20 pin, 2.54mm pitch (0.1")
- **Height:** Standard (not extra-tall or stackable needed)
- **Material:** Gold-plated preferred for reliability

**Purchase Notes:**
- Many Pi Zero W now come pre-soldered
- If soldering yourself, use flux for clean joints
- Consider right-angle headers if enclosure requires it

### Jumper Wires

- **Type:** Female-to-Female
- **Length:** 10cm (4") recommended
- **Quantity:** 40-piece kit provides spares
- **Color:** Assorted (for color-coding signals)

**Purchase Notes:**
- Higher quality wires have better contact
- Dupont-style connectors standard
- Consider female-to-male if not using headers

## Supplier Information

### Recommended Suppliers (United States)

- **Adafruit** - [adafruit.com](https://www.adafruit.com)
  - Excellent documentation and support
  - Ships worldwide
  - Slightly higher prices but great service

- **Waveshare Official Store** - [waveshare.com](https://www.waveshare.com)
  - Direct source for e-paper displays
  - Good technical documentation
  - Ships from China (2-4 weeks typically)

- **Amazon** - [amazon.com](https://www.amazon.com)
  - Fast shipping (Prime available)
  - Check seller ratings carefully
  - Verify part numbers (especially display revision)

- **DigiKey** - [digikey.com](https://www.digikey.com)
  - Large selection of electronic components
  - Excellent for resistors, buttons, connectors
  - Fast shipping, great for bulk orders

- **Mouser Electronics** - [mouser.com](https://www.mouser.com)
  - Similar to DigiKey
  - Large international selection

### International Suppliers

- **PiHut (UK)** - [thepihut.com](https://thepihut.com)
- **Pimoroni (UK)** - [pimoroni.com](https://www.pimoroni.com)
- **Core Electronics (Australia)** - [core-electronics.com.au](https://core-electronics.com.au)
- **BuyZero (South Africa)** - [buyzero.de](https://buyzero.de)

## Cost Summary

### Minimum Build Cost (Phase 1)

| Category | Cost |
|----------|------|
| Core Components | $65-80 |
| Optional Debug Tools | $10-20 |
| **Total (Phase 1)** | **$75-100** |

### Future Phases (Estimated)

| Phase | Additional Cost |
|-------|-----------------|
| Phase 2 (Buttons) | $10-15 |
| Phase 3-4 (Enclosure, Battery) | $40-60 |
| **Grand Total (Complete Build)** | **$125-175** |

## Procurement Tips

1. **Buy the Display First:** E-paper displays have the longest shipping time (2-4 weeks from China)
2. **Check Pi Zero Stock:** Raspberry Pi Zero W has periodic availability issues
3. **Bundle Deals:** Some sellers offer Pi Zero + accessories kits
4. **Quality Matters:** Don't skimp on power supply or SD card - causes 90% of issues
5. **Order Spares:** Consider ordering extra jumper wires, resistors (cheap insurance)

## Alternative Components

### Display Alternatives

If Waveshare 4.2" is unavailable:
- **Waveshare 2.9" E-Paper** - Smaller but cheaper ($20)
- **Waveshare 7.5" E-Paper** - Larger but more expensive ($60+)
- **Good Display EPD modules** - Professional-grade alternatives

**Note:** Different displays require different drivers and will need code modifications.

### Controller Alternatives

If Pi Zero W is unavailable:
- **Raspberry Pi Zero 2 W** - Faster (quad-core), compatible, $15-20
- **Raspberry Pi 3A+** - More powerful, $25, requires larger case
- **Raspberry Pi 4 Model B** - Overkill, but works

**Note:** Pi 4 and newer use different GPIO voltage levels (still 3.3V but different implementation).

## Detailed Button Specifications (Phase 2)

### Tactile Push Buttons

**Recommended Part Numbers:**

| Size | Part Number | Manufacturer | Description | Price (ea) | Supplier Link |
|------|-------------|--------------|-------------|------------|---------------|
| 6x6mm | B3F-1000 | Omron | 6x6x4.3mm, 100gf, through-hole | $0.30 | DigiKey, Mouser |
| 6x6mm | TL1105 | E-Switch | 6x6x5mm, 160gf, through-hole | $0.25 | DigiKey, Mouser |
| 12x12mm | B3F-4055 | Omron | 12x12x7.3mm, 260gf, through-hole | $0.50 | DigiKey, Mouser |
| 12x12mm | PTS645 | C&K | 12x12x7.3mm, 260gf, through-hole | $0.45 | DigiKey, Mouser |

**Specifications:**
- **Type:** Momentary, normally open (NO)
- **Actuation Force:** 100-260gf (gram-force)
- **Contact Resistance:** <100mΩ
- **Bounce Time:** <5ms
- **Mounting:** Through-hole (THT)
- **Lifetime:** 100,000 cycles minimum (500,000+ recommended for SELECT button)

**Purchase Tips:**
- Buy 6-10 buttons (5 needed + spares)
- 12x12mm buttons are easier to press and more durable
- 6x6mm buttons are more compact for smaller enclosures
- Get assorted packs if experimenting with different sizes

### Optional: 0.1µF Ceramic Capacitors (Hardware Debouncing)

| Value | Voltage | Package | Part Number | Price (ea) |
|-------|---------|---------|-------------|------------|
| 0.1µF (100nF) | 50V | Through-hole | RDER71H104K0P1H03B | $0.10 |
| 0.1µF (100nF) | 50V | Through-hole | K104K15X7RF5TL2 | $0.08 |

**Note:** Hardware debouncing capacitors are optional. Software debouncing (device tree + application) is usually sufficient.

## Revision History

- 2026-01-13: Updated with detailed Phase 2 button specifications
- 2026-01-13: Initial BOM created for Phase 1
