# Raspberry Pi Zero W E-Reader Variant

This guide shows how to build the e-reader using a **Raspberry Pi Zero W** instead of the Orange Pi Zero LTS. The Pi Zero W is a great development platform and can serve as either a prototype or final device.

## Why Use Pi Zero W?

### Advantages
- **You already have one!** Zero cost if you're using existing hardware
- **Excellent documentation**: Raspberry Pi has the best community support
- **Easy GPIO access**: Well-documented 40-pin header
- **WiFi + Bluetooth built-in**: Useful for development and future features
- **Widely available**: Easy to source from multiple vendors
- **Great software support**: Mainline Linux kernel support

### Trade-offs vs Orange Pi Zero
- **Slower CPU**: 1GHz single-core vs 1.2GHz quad-core
  - Still fine for reading! Page turns will be 200-300ms instead of 100-150ms
  - PDF rendering slightly slower but acceptable for basic documents
- **Slightly more expensive**: $10-15 vs $15-18 (not much difference)
- **Power consumption**: Similar (~150-200mA active, ~50mA idle)

### Verdict
**Perfect for development and prototyping!** Can be used as final device if you're okay with slightly slower performance. If you're building one for yourself, the Pi Zero W is an excellent choice.

## Hardware Compatibility

### What Works Unchanged
✅ **Display connection**: E-ink displays connect via SPI, same as Orange Pi  
✅ **Power system**: 5V input, battery charging circuit identical  
✅ **Buttons**: GPIO pins work the same way  
✅ **Memory**: 512MB is sufficient for reading  
✅ **Storage**: microSD card, same as Orange Pi  

### What Changes
🔧 **GPIO pin numbers**: Different between BCM2835 and H3 (but easy to adapt)  
🔧 **SPI configuration**: Same protocol, different device tree setup  
🔧 **Buildroot config**: Use `raspberrypi0w_defconfig` instead of Orange Pi config  

## Bill of Materials (Pi Zero W Version)

| Component | Specification | Quantity | Unit Cost | Total |
|-----------|--------------|----------|-----------|-------|
| **Processor** | Raspberry Pi Zero W | 1 | $10-15* | $15.00 |
| **Display** | 6" E-ink 800x600 (SPI) | 1 | $28-30 | $30.00 |
| **Battery** | 2x 18650 cells (5000mAh) | 2 | $3-4 | $7.00 |
| **Charging** | TP4056 module + protection | 1 | $1.50 | $1.50 |
| **Buck/Boost** | MT3608 boost converter | 1 | $1.00 | $1.00 |
| **Buttons** | 8x tactile switches + caps | 1 | $2.00 | $2.00 |
| **PCB** | Custom carrier board (2-layer) | 1 | $3-5 | $4.00 |
| **Case** | 3D printed PETG | 1 | $3-5 | $4.00 |
| **Storage** | 16GB microSD card | 1 | $5.00 | $5.00 |
| **Misc** | Wires, connectors, screws | - | $2-3 | $3.00 |
| **TOTAL** | | | | **$72.50** |

\* *Or $0 if you already have one!*

**Cost vs Orange Pi version**: About $3-5 more (due to Pi Zero W price)

## Pin Mapping: Pi Zero W to E-Reader

The Pi Zero W has a 40-pin GPIO header. Here's how to connect everything:

### E-Ink Display (SPI Connection)

| Function | Pi Zero W Pin | BCM GPIO | Notes |
|----------|---------------|----------|-------|
| VCC | Pin 1 or 17 | 3.3V | Power for display logic |
| GND | Pin 6, 9, 14, 20, 25, 30, 34, 39 | GND | Ground |
| DIN (MOSI) | Pin 19 | GPIO 10 | SPI data out |
| CLK (SCLK) | Pin 23 | GPIO 11 | SPI clock |
| CS | Pin 24 | GPIO 8 | Chip select (SPI0 CE0) |
| DC | Pin 22 | GPIO 25 | Data/command select |
| RST | Pin 18 | GPIO 24 | Display reset |
| BUSY | Pin 16 | GPIO 23 | Display busy signal |

**Enable SPI**: Add to `/boot/config.txt`:
```
dtparam=spi=on
```

### Buttons (GPIO Inputs)

| Button Function | Pi Zero W Pin | BCM GPIO | Notes |
|-----------------|---------------|----------|-------|
| Page Forward | Pin 11 | GPIO 17 | Primary page turn |
| Page Back | Pin 13 | GPIO 27 | Secondary page turn |
| Up | Pin 15 | GPIO 22 | Navigate up |
| Down | Pin 29 | GPIO 5 | Navigate down |
| Left | Pin 31 | GPIO 6 | Navigate left |
| Right | Pin 33 | GPIO 13 | Navigate right |
| Home/Menu | Pin 35 | GPIO 19 | Menu button |
| Select/OK | Pin 37 | GPIO 26 | Confirmation |

All buttons: **Active low** (button connects GPIO to GND, enable internal pull-up)

### Power Management

| Function | Pi Zero W Pin | Notes |
|----------|---------------|-------|
| 5V Input | Pin 2 or 4 | From boost converter |
| GND | Pin 6, 9, 14, etc. | Common ground |
| Battery Monitor | Pin 32 (GPIO 12) | Optional: ADC via voltage divider |

**Important**: Pi Zero W needs **5V input**, not 3.3V. Use MT3608 boost converter to go from 7.4V (2S battery) → 5V.

## Software Setup

### Option 1: Raspberry Pi OS (Quick Testing)

**Best for**: Initial testing and development

1. **Flash Raspberry Pi OS Lite**:
   ```bash
   # Download from https://www.raspberrypi.org/software/
   # Flash to SD card using Raspberry Pi Imager
   ```

2. **Enable SPI**:
   ```bash
   sudo raspi-config
   # Interface Options → SPI → Enable
   ```

3. **Install Python libraries** (for quick testing):
   ```bash
   sudo apt update
   sudo apt install python3-pip python3-pil python3-numpy
   pip3 install RPi.GPIO spidev
   ```

4. **Test display with Waveshare examples**:
   ```bash
   git clone https://github.com/waveshare/e-Paper
   cd e-Paper/RaspberryPi_JetsonNano/python/
   # Run appropriate example for your display
   python3 examples/epd_6in_test.py  # Adjust for your display model
   ```

### Option 2: Custom Buildroot Image (Production)

**Best for**: Final device with minimal OS

1. **Clone Buildroot**:
   ```bash
   git clone https://git.buildroot.net/buildroot
   cd buildroot
   git checkout 2024.02  # Latest stable
   ```

2. **Start with Pi Zero W config**:
   ```bash
   make raspberrypi0w_defconfig
   ```

3. **Customize configuration**:
   ```bash
   make menuconfig
   ```

   **Key changes**:
   - **Target options**: ARM little endian, ARM1176JZF-S
   - **Kernel**: Enable SPI support
   - **Packages**:
     - Qt5 (for UI)
     - MuPDF (for document rendering)
     - Python3 (if using Python for app)

4. **Build**:
   ```bash
   make -j$(nproc)
   # Takes 30-60 minutes on first build
   ```

5. **Flash to SD card**:
   ```bash
   sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
   sync
   ```

### Option 3: Use Our Custom Config (When Available)

Once we publish our custom Buildroot config:

```bash
git clone https://github.com/YOUR_ORG/open-source-ereader
cd open-source-ereader/os/buildroot-configs/
make BR2_EXTERNAL=. raspberrypi_zero_w_ereader_defconfig
make -j$(nproc)
```

## Device Tree Configuration

For custom kernels, you'll need to configure the device tree:

**File**: `boot/overlays/ereader-overlay.dts`

```dts
/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2835";

    fragment@0 {
        target = <&spi0>;
        __overlay__ {
            status = "okay";
            
            eink: eink@0 {
                compatible = "waveshare,eink-6in";
                reg = <0>;
                spi-max-frequency = <4000000>;
                dc-gpios = <&gpio 25 0>;
                reset-gpios = <&gpio 24 0>;
                busy-gpios = <&gpio 23 0>;
            };
        };
    };

    fragment@1 {
        target = <&gpio>;
        __overlay__ {
            button_pins: button_pins {
                brcm,pins = <17 27 22 5 6 13 19 26>;
                brcm,function = <0>; /* Input */
                brcm,pull = <2>; /* Pull-up */
            };
        };
    };
};
```

**Compile overlay**:
```bash
dtc -@ -I dts -O dtb -o ereader-overlay.dtbo ereader-overlay.dts
sudo cp ereader-overlay.dtbo /boot/overlays/
```

**Enable in `/boot/config.txt`**:
```
dtoverlay=ereader-overlay
```

## Performance Expectations

### Pi Zero W Performance
- **Boot time**: 5-8 seconds (optimized Buildroot)
- **Page turn**: 200-300ms (simple EPUB)
- **PDF rendering**: 500ms-1s (depends on complexity)
- **Menu navigation**: Instant (<50ms)
- **Battery life**: 3-4 weeks (same as Orange Pi)

### Comparison with Orange Pi Zero
| Metric | Pi Zero W | Orange Pi Zero | Difference |
|--------|-----------|----------------|------------|
| Page turn (EPUB) | 250ms | 150ms | 100ms slower |
| PDF render | 1s | 600ms | 400ms slower |
| Menu response | 50ms | 40ms | Negligible |
| Boot time | 6s | 5s | Negligible |
| Battery life | 3-4 weeks | 3-4 weeks | Same |

**Verdict**: Slightly slower but still very usable for reading!

## PCB Design Adaptations

### Minimal Changes Required

The Pi Zero W can be used in two ways:

#### Option A: Use Pi Zero W as Module (Easiest)
- Mount Pi Zero W directly on carrier PCB using standoffs
- Connect via 40-pin header (female header on carrier PCB)
- No soldering to Pi itself (removable)
- Slightly thicker device (+3mm)

**Carrier PCB includes**:
- E-ink display connector
- Button switches
- Battery management (TP4056 + protection)
- Boost converter (7.4V → 5V)
- Pass 5V to Pi via header pins 2/4

#### Option B: Integrate Pi Zero W (Advanced)
- Desolder headers from Pi Zero W
- Solder directly to carrier PCB pads
- Slimmer design
- Requires more PCB design work

**Recommendation**: Start with Option A (module approach)

### Carrier PCB Schematic (Simplified)

```
[18650 cells] → [TP4056 charger] → [Protection] → [MT3608 boost] → [Pi Zero W 5V]
                                                                    ↓
[Buttons] → [Pull-up resistors] → [Pi Zero W GPIO]
                                    ↓
[E-ink display] ← [SPI + control GPIOs] ← [Pi Zero W]
```

**Key components on carrier board**:
1. TP4056 charging IC + USB micro/C connector
2. Battery protection IC (DW01A + FS8205A)
3. MT3608 boost converter (7.4V → 5V, 2A)
4. 8 tactile switches + 10kΩ pull-ups
5. E-ink display connector (24-pin FPC)
6. 40-pin socket for Pi Zero W

## Case Design Adaptations

### Dimensions
Pi Zero W is **65mm x 30mm x 5mm**, very similar to Orange Pi Zero.

**Internal layout**:
```
┌─────────────────────────────┐
│                             │
│    6" E-ink Display         │
│    (122mm x 91mm)           │
│                             │
├─────────────────────────────┤
│ [Pi Zero W]    [Buttons]    │ ← Right side for one-handed use
│ [Battery cells below]       │
└─────────────────────────────┘
```

**Overall dimensions**: 130mm x 185mm x 9mm (1mm thicker due to Pi Zero W module height)

### 3D Printing Notes
- Use same case design as Orange Pi version
- Adjust mounting posts for Pi Zero W position
- Add ventilation holes (Pi Zero W runs slightly warmer)
- Optional: Add access holes for USB ports (if keeping exposed)

## Development Roadmap

### Phase 1: Quick Prototype (1-2 weeks)
1. **Get display working** on Pi Zero W with Raspberry Pi OS
2. **Test buttons** with simple GPIO scripts
3. **Build minimal reader app** (Python + PyQt5)
4. **Test battery power** with USB power bank

**Deliverable**: Working prototype with all core functions

### Phase 2: Optimize Software (2-4 weeks)
1. **Switch to Buildroot** for minimal OS
2. **Optimize boot time** (target <5 seconds)
3. **Implement power management** (sleep modes)
4. **Test battery life** (aim for 3+ weeks)

**Deliverable**: Production-ready software

### Phase 3: Custom Hardware (4-6 weeks)
1. **Design carrier PCB** in KiCad
2. **Order PCB prototype** from JLCPCB/PCBWay
3. **Design 3D case** for Pi Zero W + carrier board
4. **Assemble first complete unit**

**Deliverable**: Complete custom e-reader

## Testing Checklist

Before finalizing design:

- [ ] Display refresh working (partial and full)
- [ ] All 8 buttons responsive
- [ ] EPUB rendering correct
- [ ] PDF rendering acceptable (basic documents)
- [ ] Boot time under 10 seconds
- [ ] Battery lasts 2+ weeks with daily use
- [ ] Power management (sleep after idle)
- [ ] Case comfortable to hold
- [ ] All components fit in case
- [ ] No thermal issues (Pi stays cool)

## Quick Start Guide (Pi Zero W You Already Have)

### What You Need
- Raspberry Pi Zero W (you have this!)
- 4.2" Waveshare e-Paper display (~$25)
- 8 tactile switches (~$2)
- Breadboard and jumper wires (~$5)
- USB power bank (for testing)

**Total additional cost**: ~$32

### Steps
1. **Flash Raspberry Pi OS Lite to SD card**
2. **Enable SPI**: `sudo raspi-config` → Interface Options → SPI
3. **Connect display** using pinout table above
4. **Test with Waveshare examples**:
   ```bash
   git clone https://github.com/waveshare/e-Paper
   cd e-Paper/RaspberryPi_JetsonNano/python/examples/
   python3 epd_4in2_test.py  # Shows test patterns
   ```
5. **Connect buttons** to GPIO pins (with breadboard)
6. **Write simple reading app** or use existing (FBReader, etc.)

### Next Steps
- Get 6" display when ready (~$30)
- Design carrier PCB (~$5)
- 3D print case (~$4)
- Add battery system (~$10)

**Total project cost**: ~$81 (or ~$66 if you have Pi Zero W)

## FAQ

**Q: Is Pi Zero W fast enough for reading?**  
A: Yes! Page turns are 200-300ms, which feels instant. PDFs are slower but acceptable.

**Q: Should I upgrade to Pi Zero 2 W?**  
A: Only if you read complex PDFs often. For novels/EPUB, Pi Zero W is fine.

**Q: Can I use WiFi for downloading books?**  
A: Yes! Pi Zero W has built-in WiFi. You can add a book download feature.

**Q: Battery life with WiFi on?**  
A: 1-2 weeks with occasional WiFi use. Turn off WiFi for 3-4 week battery life.

**Q: Can this run KOReader or other reading apps?**  
A: Yes! KOReader has framebuffer support and works on Pi Zero W.

**Q: Should I use this for production or switch to Orange Pi?**  
A: Pi Zero W is great for 1-10 units. For 100+ units, Orange Pi is cheaper.

## Resources

### Pi Zero W Documentation
- [Raspberry Pi Zero W Pinout](https://pinout.xyz/)
- [BCM2835 Datasheet](https://www.raspberrypi.org/documentation/)
- [Raspberry Pi OS Documentation](https://www.raspberrypi.org/documentation/)

### E-Ink Display
- [Waveshare E-Paper Wiki](https://www.waveshare.com/wiki/E-Paper)
- [E-Ink Developer Resources](https://www.e-ink.com/)

### Software
- [Buildroot Documentation](https://buildroot.org/docs.html)
- [KOReader (Open Source E-Reader App)](https://github.com/koreader/koreader)
- [MuPDF Documentation](https://mupdf.com/docs/)

### Community
- [Raspberry Pi Forums](https://www.raspberrypi.org/forums/)
- [r/raspberry_pi](https://www.reddit.com/r/raspberry_pi/)
- [Project GitHub Discussions](https://github.com/YOUR_ORG/open-source-ereader/discussions)

## Conclusion

The **Raspberry Pi Zero W is an excellent platform for this e-reader project**, especially for:
- **Development and prototyping**
- **Personal builds (1-10 units)**
- **Learning embedded Linux**
- **Testing software before custom hardware**

It's slightly more expensive and slower than Orange Pi Zero, but the trade-off is worth it for better documentation and community support.

**Next steps**: Order a Waveshare display and start testing! 🚀
