---
type: reference
title: Complete Build Guide - Open Source E-Reader
created: 2026-01-17
tags:
  - build-guide
  - assembly
  - tutorial
  - hardware
  - software
related:
  - "[[BOM]]"
  - "[[WIRING_GUIDE]]"
  - "[[BUILD_PROCESS]]"
  - "[[FLASHING_GUIDE]]"
  - "[[USER_GUIDE]]"
---

# Complete Build Guide: Open Source E-Reader

This comprehensive guide will walk you through building your own open-source e-reader from scratch. By following these steps, you'll create a functional e-reading device using a Raspberry Pi Zero W and e-paper display.

## Table of Contents

1. [Overview](#overview)
2. [What You'll Build](#what-youll-build)
3. [Time and Skill Requirements](#time-and-skill-requirements)
4. [Part 1: Gathering Components](#part-1-gathering-components)
5. [Part 2: Software Preparation](#part-2-software-preparation)
6. [Part 3: Hardware Assembly](#part-3-hardware-assembly)
7. [Part 4: First Boot and Testing](#part-4-first-boot-and-testing)
8. [Part 5: Initial Configuration](#part-5-initial-configuration)
9. [Troubleshooting](#troubleshooting)
10. [Next Steps](#next-steps)

---

## Overview

This guide covers the complete process of building an open-source e-reader, including:
- Ordering and gathering all necessary parts
- Compiling the custom Linux system (or using pre-built images)
- Assembling the hardware (soldering, wiring)
- Flashing the SD card and booting the device
- Configuring WiFi, time zone, and adding books

**Project Cost:** $75-175 depending on optional components and tools you already have
**Build Difficulty:** Intermediate (requires basic soldering and Linux knowledge)
**Estimated Build Time:** 4-8 hours total (excluding shipping and software compilation)

---

## What You'll Build

### Hardware Specifications
- **Display:** Waveshare 4.2" e-paper (400×300 pixels, black/white)
- **Processor:** Raspberry Pi Zero W (1GHz ARM, 512MB RAM)
- **Storage:** 8GB+ microSD card
- **Input:** 5 physical buttons (UP, DOWN, SELECT, BACK, MENU)
- **Connectivity:** WiFi 802.11 b/g/n, optional battery power
- **Boot Time:** 10-30 seconds from power-on to usable

### Software Features
- **Operating System:** Custom Buildroot-based minimal Linux (kernel 6.1 LTS)
- **Formats Supported:** TXT, EPUB, PDF, HTML, Markdown
- **WiFi Features:** Configure networks, browse and download books from online sources
- **Reading Features:** Automatic pagination, bookmarks, search, font size adjustment
- **Power:** Low-power e-paper technology, automatic time synchronization

### What Makes This Special
- **Fully Open Source:** All software, schematics, and documentation freely available
- **Minimal and Fast:** Boots directly to reading interface, no bloat
- **Customizable:** Full source code access, modify as you wish
- **Educational:** Learn embedded Linux, hardware interfacing, and e-paper technology
- **Privacy-Focused:** No telemetry, no DRM, complete control over your data

---

## Time and Skill Requirements

### Time Investment

| Phase | Time Required | Can Be Done In Parallel |
|-------|---------------|-------------------------|
| Ordering parts | 15-30 min | - |
| Shipping wait | 1-4 weeks | ✓ Start software setup |
| Software build (first time) | 1-4 hours | ✓ Automated, can do other tasks |
| Soldering GPIO header | 15-30 min | - |
| Wiring display and buttons | 30-60 min | - |
| Testing and troubleshooting | 30-120 min | - |
| **Total active time** | **4-8 hours** | |

### Skills Required

**Essential:**
- Basic computer literacy (command line, file navigation)
- Ability to follow step-by-step instructions carefully
- Patience and attention to detail

**Recommended:**
- Basic soldering skills (or willingness to learn)
- Familiarity with Linux terminal
- Ability to read wiring diagrams

**Optional (helpful but not required):**
- Experience with Raspberry Pi or single-board computers
- Basic electronics knowledge (GPIO, voltage, current)
- Git and version control experience

### Tools You'll Need

See [Part 1: Gathering Components](#part-1-gathering-components) for the complete list, but you'll need:
- Soldering iron and solder
- Small screwdrivers
- Wire cutters/strippers
- Multimeter (helpful but optional)
- Computer with SD card reader

---

## Part 1: Gathering Components

### Step 1.1: Review the Bill of Materials

The complete parts list with supplier links is available in [[BOM]]. Here's a summary:

**Core Components (Required):**
- Raspberry Pi Zero W - $15
- Waveshare 4.2" E-Paper Display (Rev 2.2) - $30-40
- MicroSD Card (8GB+, Class 10) - $5-8
- 40-pin GPIO Header (if not pre-soldered on Pi) - $1-2
- Female-Female Jumper Wires (40-piece pack) - $3-5
- 5V 2A Micro-USB Power Supply - $8-10
- Tactile Push Buttons (5x) - $3-5

**Optional but Recommended:**
- USB-to-TTL Serial Adapter (for debugging) - $5-10
- MicroSD Card Reader - $5-10
- Soldering kit (if you don't have one) - $20-50

**Total Cost: $75-100** for basic build, $125-175 with enclosure and battery

### Step 1.2: Component Selection Tips

**Raspberry Pi Zero W:**
- Consider pre-soldered GPIO header version (saves time)
- Alternative: Pi Zero 2 W is faster but uses more power
- Check stock availability - these often sell out

**E-Paper Display:**
- **CRITICAL:** Get Rev 2.2 specifically (Rev 2.1 has different pinout)
- Verify it includes the driver board (not just the bare display)
- Waveshare official store or verified Amazon sellers recommended

**MicroSD Card:**
- Use reputable brands (SanDisk, Samsung, Kingston)
- Class 10 or UHS-I minimum for good performance
- 8GB is sufficient, 16GB gives room for growth

**Power Supply:**
- Official Raspberry Pi power supply strongly recommended
- Cheap power supplies cause 90% of Raspberry Pi issues
- Look for 5.1V rating (compensates for cable voltage drop)

**Buttons:**
- 12×12mm buttons are more durable and easier to press
- 6×6mm buttons are more compact for smaller builds
- Buy extras (they're cheap) in case of mistakes

### Step 1.3: Where to Order

**Recommended Suppliers:**

*United States:*
- **Adafruit** - Excellent support, great documentation
- **Waveshare Official Store** - Direct source for display (ships from China)
- **Amazon** - Fast shipping, verify seller ratings
- **DigiKey/Mouser** - Electronic components, buttons, connectors

*International:*
- **PiHut (UK)** - thepihut.com
- **Pimoroni (UK)** - pimoroni.com
- **Core Electronics (Australia)** - core-electronics.com.au

See [[BOM]] for direct links to suppliers.

### Step 1.4: Procurement Strategy

**Optimize Shipping Time:**
1. **Order display FIRST** - Takes 2-4 weeks from Waveshare China
2. **Order Pi Zero W** - Check stock at multiple suppliers
3. **Order remaining parts** - Can be Amazon Prime for fast delivery

**While Waiting for Parts:**
- Set up your build environment (see Part 2)
- Compile the software (takes 1-4 hours)
- Read through the wiring guide
- Watch soldering tutorials if you're new to it

---

## Part 2: Software Preparation

You have two options: use a pre-built image (easier) or build from source (more educational).

### Option A: Using Pre-Built Images (Recommended for Beginners)

**Note:** Pre-built images will be available on GitHub Releases after Phase 07 completion.

1. **Download the Image**
   - Visit: [GitHub Releases](https://github.com/your-repo/open-source-ereader/releases)
   - Download: `ereader-v1.0.0-sdcard.img.gz`
   - Verify checksum: `sha256sum ereader-v1.0.0-sdcard.img.gz`

2. **Extract the Image**
   ```bash
   # On Linux/Mac:
   gunzip ereader-v1.0.0-sdcard.img.gz

   # On Windows:
   # Use 7-Zip or WinRAR to extract
   ```

3. **Skip to Part 3** - Your software is ready!

### Option B: Building from Source (Developers and Tinkerers)

Building from source gives you full control and understanding of the system.

#### Step 2.1: Prepare Your Build Environment

**System Requirements:**
- Linux environment (native, WSL2, or VM)
- 8GB+ RAM recommended (4GB minimum)
- 40GB+ free disk space
- Multi-core CPU (faster builds)
- Stable internet connection

**On Ubuntu/Debian Linux:**
```bash
# Update package lists
sudo apt-get update

# Install required build tools
sudo apt-get install -y \
    build-essential \
    libncurses5-dev \
    git \
    wget \
    cpio \
    python3 \
    unzip \
    rsync \
    bc \
    file \
    patch \
    perl
```

**On Fedora/RHEL:**
```bash
sudo dnf install -y \
    gcc gcc-c++ make \
    ncurses-devel \
    git wget cpio python3 \
    unzip rsync bc file patch perl
```

**Windows Users:**
- Install WSL2 (Windows Subsystem for Linux)
- Use Ubuntu 20.04 or 22.04 from Microsoft Store
- Run all build commands inside WSL2

#### Step 2.2: Clone the Repository

```bash
# Clone the project
git clone https://github.com/your-repo/open-source-ereader.git
cd open-source-ereader

# Check out stable release (or use master for latest)
git checkout v1.0.0
```

#### Step 2.3: Run the Build

**Method 1: Automated Build Script (Recommended)**

```bash
# Run the build script
./scripts/build.sh

# This will:
# 1. Download Buildroot (if needed)
# 2. Configure the system
# 3. Download all packages
# 4. Compile toolchain, kernel, and packages
# 5. Generate the SD card image
```

**Estimated Build Time:**
- Fast machine (8-core, SSD): 30-45 minutes
- Average machine (4-core): 1-2 hours
- Slower machine (2-core): 2-4 hours

**Method 2: Manual Build**

```bash
# Download Buildroot
make download-buildroot

# Load configuration
make config

# Optional: Review configuration
make menuconfig

# Start build
make build
```

#### Step 2.4: Verify Build Output

After successful build:

```bash
# Check output files
ls -lh buildroot/output/images/

# You should see:
# - sdcard.img       (~200 MB) - THIS IS WHAT YOU NEED
# - zImage           (~6 MB)
# - rootfs.ext4      (~256 MB)
# - bcm2708-rpi-zero-w.dtb (~25 KB)
```

**Build Troubleshooting:**

If the build fails:
```bash
# Check available disk space
df -h

# View build log for errors
tail -100 buildroot/output/build/build-time.log

# Clean and retry
make clean
make build
```

See [[BUILD_PROCESS]] for detailed build documentation.

---

## Part 3: Hardware Assembly

### Step 3.1: Soldering the GPIO Header (if needed)

**Tools Needed:**
- Soldering iron (25-40W)
- Solder (60/40 or lead-free)
- Flux (optional, helps with clean joints)
- Third-hand/vise to hold boards
- Wet sponge for cleaning tip

**If Your Pi Zero W Has Pre-Soldered Headers:**
Skip to Step 3.2!

**Soldering Instructions:**

1. **Prepare the workspace**
   - Well-ventilated area
   - Good lighting
   - Heat-resistant surface

2. **Position the header**
   - Insert 40-pin header into Pi Zero W GPIO pads
   - Use tape or BluTack to hold in place
   - Ensure header is perfectly straight and flush

3. **Solder the pins**
   - Heat iron to 350°C (660°F)
   - Touch iron to pad and pin simultaneously
   - Apply solder to junction (not to iron)
   - Remove solder, then iron
   - Each joint should be shiny and cone-shaped
   - **Solder all 40 pins** (don't skip any)

4. **Inspect your work**
   - Look for cold joints (dull, grainy appearance)
   - Check for solder bridges between adjacent pins
   - Verify header is straight and secure

**Soldering Resources:**
- [Adafruit Guide to Excellent Soldering](https://learn.adafruit.com/adafruit-guide-excellent-soldering)
- Video: "How to Solder" by EEVblog

### Step 3.2: Wiring the E-Paper Display

**Safety First:**
- Ensure Raspberry Pi is UNPOWERED during all wiring
- Use correct voltage (3.3V, NOT 5V for display VCC)
- Double-check all connections before powering on

**Tools Needed:**
- Female-female jumper wires (8 wires for display)
- Color-coded wires recommended (red=power, black=ground)

**Display Connections:**

Use the following table to connect your display:

| Waveshare Display Pin | RPi Zero W Pin | BCM GPIO | Wire Color Suggestion |
|----------------------|----------------|----------|----------------------|
| VCC (Power) | Pin 1 | 3.3V | Red |
| GND (Ground) | Pin 6 | GND | Black |
| DIN (MOSI) | Pin 19 | GPIO 10 | Blue |
| CLK (SCLK) | Pin 23 | GPIO 11 | Yellow |
| CS (Chip Select) | Pin 24 | GPIO 8 | Green |
| DC (Data/Command) | Pin 22 | GPIO 25 | Orange |
| RST (Reset) | Pin 11 | GPIO 17 | Purple |
| BUSY | Pin 18 | GPIO 24 | White |

**Visual Wiring Guide:**

```
Waveshare 4.2" Display          Raspberry Pi Zero W
┌─────────────────┐            ┌─────────────────┐
│  VCC ●          │────Red────>│ Pin 1 (3.3V)    │
│  GND ●          │───Black───>│ Pin 6 (GND)     │
│  DIN ●          │───Blue────>│ Pin 19 (GPIO10) │
│  CLK ●          │──Yellow───>│ Pin 23 (GPIO11) │
│   CS ●          │───Green───>│ Pin 24 (GPIO8)  │
│   DC ●          │──Orange───>│ Pin 22 (GPIO25) │
│  RST ●          │──Purple───>│ Pin 11 (GPIO17) │
│ BUSY ●          │───White───>│ Pin 18 (GPIO24) │
└─────────────────┘            └─────────────────┘
```

**Wiring Steps:**

1. **Lay out components**
   - Place Pi Zero W on anti-static mat
   - Position display nearby
   - Organize jumper wires by color

2. **Connect power first**
   - **VCC to Pin 1 (3.3V)** - Use RED wire
   - **GND to Pin 6 (GND)** - Use BLACK wire
   - **CRITICAL:** Do NOT use Pin 2/4 (5V) - will damage display!

3. **Connect SPI signals**
   - DIN → Pin 19 (GPIO 10 MOSI)
   - CLK → Pin 23 (GPIO 11 SCLK)
   - CS → Pin 24 (GPIO 8 CE0)

4. **Connect control signals**
   - DC → Pin 22 (GPIO 25)
   - RST → Pin 11 (GPIO 17)
   - BUSY → Pin 18 (GPIO 24)

5. **Verify connections**
   - **Trace each wire** from display to Pi
   - Check against table above
   - Ensure firm connections (wiggle test)
   - Look for any crossed wires

**Common Mistakes to Avoid:**
- ❌ Using 5V instead of 3.3V for VCC (damages display)
- ❌ Swapping DIN and CLK
- ❌ Connecting to Pin numbers instead of GPIO numbers
- ❌ Loose jumper wire connections

See [[WIRING_GUIDE]] for detailed diagrams and troubleshooting.

### Step 3.3: Wiring the Buttons

**Required Components:**
- 5× tactile push buttons (6×6mm or 12×12mm)
- Jumper wires or soldered connections

**Button Pin Assignments:**

| Button Function | GPIO | Physical Pin | Connection |
|----------------|------|--------------|------------|
| UP | GPIO 5 | Pin 29 | Button → GND |
| DOWN | GPIO 6 | Pin 31 | Button → GND |
| SELECT | GPIO 13 | Pin 33 | Button → GND |
| BACK | GPIO 19 | Pin 35 | Button → GND |
| MENU | GPIO 26 | Pin 37 | Button → GND |
| Common Ground | - | Pin 34, 39 | Shared GND |

**Button Wiring Schematic:**

```
Each button configuration (active-low with internal pull-up):

     3.3V (internal pull-up ~50kΩ)
         │
         ├── GPIO Pin (5, 6, 13, 19, or 26)
         │
         ├── [Tactile Button] (normally open)
         │
         └── GND (Pin 34 or 39)

When NOT pressed: GPIO reads HIGH (1)
When pressed: GPIO reads LOW (0)
```

**Wiring Steps:**

1. **Choose button placement**
   - UP/DOWN: Easy thumb access
   - SELECT: Center or dominant thumb position
   - BACK: Near edge for easy escape
   - MENU: Less frequently used position

2. **Connect button GPIOs**
   - UP button → Pin 29 (GPIO 5)
   - DOWN button → Pin 31 (GPIO 6)
   - SELECT button → Pin 33 (GPIO 13)
   - BACK button → Pin 35 (GPIO 19)
   - MENU button → Pin 37 (GPIO 26)

3. **Connect common ground**
   - Connect other terminal of ALL buttons to common ground rail
   - Connect ground rail to Pin 34 or 39 (GND)

4. **Verify button wiring**
   - Each button should have one wire to its GPIO pin
   - All buttons should share a common ground connection

**GPIO Header Section (Bottom Right Pins):**

```
Pin 29: GPIO 5  [UP]        Pin 30: GND
Pin 31: GPIO 6  [DOWN]      Pin 32: GPIO 12 (unused)
Pin 33: GPIO 13 [SELECT]    Pin 34: GND [Common ground]
Pin 35: GPIO 19 [BACK]      Pin 36: GPIO 16 (unused)
Pin 37: GPIO 26 [MENU]      Pin 38: GPIO 20 (unused)
Pin 39: GND [Common]        Pin 40: GPIO 21 (unused)
```

**Assembly Options:**

*Option A: Breadboard (Prototyping)*
- Insert buttons into breadboard
- Use jumper wires to connect to Pi
- Easy to modify and test
- Not suitable for permanent installation

*Option B: Perfboard (Permanent)*
- Solder buttons to perfboard
- Solder wires from buttons to Pi GPIO
- More reliable, professional result
- Can be mounted in enclosure

*Option C: Custom PCB (Advanced)*
- Design PCB with KiCad/Eagle
- Order from PCB fabrication service
- Most professional and reliable
- See Phase 07 documentation

### Step 3.4: Physical Layout

**Recommended Component Arrangement:**

```
┌─────────────────────────────────┐
│         E-Paper Display         │
│          (400×300 px)           │
│                                 │
│      ┌───────────────┐          │
│      │ Reading Area  │          │
│      │               │          │
│      └───────────────┘          │
│                                 │
│  [UP]  [DOWN]         [SELECT]  │
│                                 │
│         [BACK]  [MENU]          │
│                                 │
│    [Raspberry Pi Zero W]        │
│         (underneath)            │
└─────────────────────────────────┘
```

**Tips for Clean Assembly:**
- Keep wires as short as possible (reduces interference)
- Route wires away from display ribbon cable
- Use cable ties or tape to secure wire bundles
- Label wires with masking tape if needed
- Consider enclosure design during layout

---

## Part 4: First Boot and Testing

### Step 4.1: Flash the SD Card

**Warning:** This will erase all data on the SD card!

**Using balenaEtcher (Recommended):**

1. Download and install [balenaEtcher](https://www.balena.io/etcher/)
2. Launch Etcher
3. Click "Flash from file"
4. Select `buildroot/output/images/sdcard.img` (or downloaded image)
5. Click "Select target" and choose your SD card
6. **Verify correct device** (check size to confirm)
7. Click "Flash!" and wait 2-5 minutes
8. Etcher will automatically verify the write
9. Safely eject SD card

**Using dd (Linux/Mac):**

```bash
# Insert SD card and identify device
lsblk  # On Linux
diskutil list  # On Mac

# Unmount the SD card (but don't eject)
sudo umount /dev/sdX*  # Linux (replace sdX with your device)
sudo diskutil unmountDisk /dev/diskN  # Mac (replace diskN)

# Write image (BE VERY CAREFUL WITH DEVICE NAME!)
sudo dd if=buildroot/output/images/sdcard.img of=/dev/sdX bs=4M status=progress && sync

# Wait for completion (2-5 minutes)

# Eject SD card
sudo eject /dev/sdX  # Linux
sudo diskutil eject /dev/diskN  # Mac
```

**Using Windows Tools:**

*Win32DiskImager:*
1. Download Win32DiskImager
2. Run as Administrator
3. Select image file
4. Select SD card drive letter
5. Click "Write"

*Rufus:*
1. Download Rufus
2. Select SD card device
3. Click "SELECT" and choose image
4. Click "START"

See [[FLASHING_GUIDE]] for detailed flashing instructions.

### Step 4.2: Pre-Boot Checklist

Before powering on for the first time, verify:

- [ ] SD card is fully inserted into Pi Zero W
- [ ] Display is correctly wired (3.3V, NOT 5V!)
- [ ] All 8 display connections are firm
- [ ] Buttons are wired correctly (if installed)
- [ ] No short circuits or crossed wires
- [ ] Power supply is 5V 2A micro-USB
- [ ] Power supply is NOT YET connected

**Optional: Serial Console Setup**

If you have a USB-to-TTL adapter for debugging:

```
USB-TTL Adapter    Raspberry Pi Zero W
──────────────     ──────────────────
  TX      ────────> Pin 10 (GPIO 15 RXD)
  RX      <────────  Pin 8 (GPIO 14 TXD)
  GND     ──────────  Pin 6 (GND)

DO NOT connect VCC!
```

Connect to serial console:
```bash
# Linux/Mac
screen /dev/ttyUSB0 115200

# Windows
# Use PuTTY with COM port, 115200 baud, 8N1
```

### Step 4.3: Power On and First Boot

**The Moment of Truth:**

1. **Connect power supply**
   - Plug micro-USB into Pi Zero W
   - You should see:
     - Green LED on Pi starts blinking (SD card activity)
     - Display may flicker or show static (normal)

2. **Wait for boot (10-30 seconds)**
   - Be patient - first boot can take up to 30 seconds
   - Watch for display activity
   - Serial console (if connected) will show boot messages

3. **Successful boot indicators**
   - Display shows either:
     - **Library menu** with book list (if books present)
     - **Empty library message** (if no books)
   - Green LED activity stops (system idle)
   - Serial console shows login prompt

4. **Test display**
   - You should see clear, readable text on the e-paper
   - Text should be properly aligned
   - No garbage characters or artifacts

### Step 4.4: Basic Hardware Testing

**Test 1: Display Refresh**
- Press any button
- Display should refresh (takes ~2 seconds)
- Content should update clearly

**Test 2: Button Response**
- Press UP button → Display should respond
- Press DOWN button → Display should respond
- Try each button to verify it's wired correctly

**Test 3: Serial Console (if connected)**
```bash
# Login (via serial console)
# Username: root
# Password: ereader

# Check system status
uname -a
# Should show: Linux ereader 6.1.x ... armv6l GNU/Linux

# Check display device
ls /dev/spi*
# Should show: /dev/spidev0.0

# Check button GPIOs
ls /sys/class/gpio/
# Should show gpio5, gpio6, gpio13, gpio19, gpio26
```

### Step 4.5: First Boot Troubleshooting

**Issue: Display shows nothing**

*Check:*
- Is display wired correctly? (Verify VCC = 3.3V)
- Are SPI connections correct? (MOSI, SCLK, CS)
- Is power supply adequate? (Use 2A+ supply)
- Green LED blinking? (If not, SD card not readable)

*Test:*
```bash
# Via serial console
ls /dev/spidev0.0
# If missing, SPI not enabled or wired wrong
```

**Issue: Display shows garbage/artifacts**

*Causes:*
- Incorrect DC, RST, or BUSY pin wiring
- Loose SPI connections
- Wrong display revision (need Rev 2.2)

*Solution:*
- Power off
- Re-check EVERY wire against wiring table
- Ensure tight connections
- Power on and test again

**Issue: System won't boot (green LED doesn't blink)**

*Causes:*
- SD card not fully inserted
- SD card not flashed correctly
- Corrupt image file
- Faulty SD card

*Solution:*
- Re-flash SD card
- Try different SD card
- Verify image file checksum

**Issue: Buttons don't work**

*Test via serial console:*
```bash
# Test UP button (GPIO 5)
cat /sys/class/gpio/gpio5/value
# Should show: 1 (not pressed), 0 (pressed)

# If always 0: GPIO shorted to ground
# If always 1: Button not connected or broken
# If no file: GPIO not exported (device tree issue)
```

See [Troubleshooting](#troubleshooting) section for more issues.

---

## Part 5: Initial Configuration

### Step 5.1: Adding Your First Books

**Via SD Card (Easiest):**

1. **Power off the device**
   - Unplug power supply
   - Wait 5 seconds

2. **Remove SD card**
   - Carefully remove from Pi Zero W
   - Insert into computer's SD card reader

3. **Mount the SD card**
   - Linux/Mac: Should auto-mount
   - Windows: May need ext4 driver (see below)

4. **Copy books**
   - Navigate to the rootfs partition
   - Create/navigate to `/books/` directory
   - Copy your ebook files:
     - `.txt` - Plain text
     - `.epub` - EPUB format
     - `.pdf` - PDF documents
     - `.html` - HTML documents
     - `.md` - Markdown files

5. **Safely eject SD card**
   - Unmount properly
   - Reinsert into Pi Zero W
   - Power on

**Windows ext4 Access:**
- Use [Linux Reader](https://www.diskinternals.com/linux-reader/) (free)
- Or use WSL2 to mount SD card
- Or use network transfer (see below)

**Via Network (After WiFi setup):**

```bash
# Connect via SSH (after WiFi configured)
scp mybook.epub root@ereader.local:/books/

# Or use the built-in library browser:
# MENU → Online Library → Browse & Download
```

**Recommended Test Books:**

For testing, try these free public domain books:
- **Alice's Adventures in Wonderland** - Classic, good formatting test
- **Pride and Prejudice** - Long book, tests pagination
- **A Christmas Carol** - Medium length, varied formatting
- **The Republic** by Plato - Tests complex text structures

Download from:
- [Project Gutenberg](https://www.gutenberg.org/) - EPUB/TXT
- [Standard Ebooks](https://standardebooks.org/) - High-quality EPUB
- [Internet Archive](https://archive.org/) - Multiple formats

### Step 5.2: WiFi Configuration

**Method 1: Via Serial Console**

```bash
# Login via serial console
# Username: root / Password: ereader

# Scan for networks
wpa_cli scan
sleep 2
wpa_cli scan_results

# Add your network
wpa_cli add_network
# Returns network ID (usually 0)

wpa_cli set_network 0 ssid '"YourNetworkName"'
wpa_cli set_network 0 psk '"YourPassword"'
wpa_cli enable_network 0
wpa_cli save_config

# Verify connection
wpa_cli status
# Should show: wpa_state=COMPLETED

# Test internet
ping -c 3 google.com
```

**Method 2: Via UI (Recommended)**

1. From library menu, press MENU button
2. Navigate to "Settings" → "WiFi"
3. Select "Scan Networks"
4. Choose your network from list
5. Enter password using on-screen keyboard
6. Select "Connect"
7. Wait for connection confirmation

**Method 3: Pre-configure on SD Card**

Before first boot, edit `/etc/wpa_supplicant.conf` on SD card:

```conf
ctrl_interface=/var/run/wpa_supplicant
update_config=1
country=US

network={
    ssid="YourNetworkName"
    psk="YourPassword"
    key_mgmt=WPA-PSK
}
```

### Step 5.3: Time Zone Configuration

**Via Serial Console:**

```bash
# List available timezones
ls /usr/share/zoneinfo/

# Set timezone (example: US Eastern)
ln -sf /usr/share/zoneinfo/America/New_York /etc/localtime

# Verify
date
# Should show correct time and timezone
```

**Via UI:**

1. MENU → Settings → Time & Date
2. Select your region
3. Select your city/timezone
4. Confirm selection

**Note:** With WiFi connected, time syncs automatically via NTP (chrony).

### Step 5.4: Verify All Features

**Checklist:**

- [ ] Display shows library menu clearly
- [ ] All 5 buttons respond correctly
- [ ] Can navigate book list with UP/DOWN
- [ ] Can open a book with SELECT
- [ ] Can read and turn pages
- [ ] WiFi connects successfully
- [ ] Can download a book from online library
- [ ] Bookmarks save and restore correctly
- [ ] Search function works
- [ ] Settings menu accessible
- [ ] Time displays correctly

**If everything works:** Congratulations! Your e-reader is complete! 🎉

---

## Troubleshooting

### Common Issues and Solutions

#### Display Issues

**Problem: Display shows nothing / stays blank**

*Diagnostic steps:*
1. Check power LED on Pi → If off, power supply issue
2. Check green LED → If blinking, Pi is working
3. Verify 3.3V on display VCC pin (multimeter)
4. Verify SPI wiring (MOSI, SCLK, CS)

*Solutions:*
- Re-check ALL 8 display connections
- Verify Pin 1 (3.3V), not Pin 2/4 (5V)
- Test with known-good jumper wires
- Check for loose connections

**Problem: Display shows garbled text or artifacts**

*Causes:*
- Wrong DC, RST, or BUSY wiring
- SPI timing issues (loose wires)
- Incorrect display driver

*Solutions:*
- Verify DC = Pin 22 (GPIO 25)
- Verify RST = Pin 11 (GPIO 17)
- Verify BUSY = Pin 18 (GPIO 24)
- Shorten jumper wires (reduce capacitance)

**Problem: Display refreshes very slowly or not at all**

*Causes:*
- BUSY pin not connected
- Display in wrong mode
- Power supply insufficient

*Solutions:*
- Check BUSY wire connection
- Use 2A or higher power supply
- Check for voltage drops (multimeter)

#### Button Issues

**Problem: Button doesn't respond**

*Test:*
```bash
# Via serial console
cat /sys/class/gpio/gpio5/value  # UP button
# Press button - value should change from 1 to 0
```

*Solutions:*
- Check button wiring (GPIO pin correct?)
- Verify ground connection
- Test button with multimeter (continuity mode)
- Replace button if faulty

**Problem: Button triggers wrong action**

*Cause:* Wiring to wrong GPIO pin

*Solution:*
- Check button wiring against table in Part 3.3
- Remember: Physical Pin # ≠ GPIO #
- UP = GPIO 5 = Pin 29 (not GPIO 29!)

**Problem: Multiple buttons trigger simultaneously**

*Causes:*
- Wires crossed or shorted
- Missing ground connection

*Solutions:*
- Verify each button goes to unique GPIO
- Check common ground is connected
- Look for solder bridges or wire crossings

#### Boot Issues

**Problem: Pi won't boot (no LED activity)**

*Causes:*
- SD card not inserted fully
- Image not flashed correctly
- Faulty SD card
- Insufficient power supply

*Solutions:*
- Reseat SD card firmly
- Re-flash image
- Try different SD card
- Use official Pi power supply (2A+)

**Problem: Boot loop (LED blinks, then restarts)**

*Causes:*
- Corrupt filesystem
- Power supply brownout
- Hardware short circuit

*Solutions:*
- Re-flash SD card
- Test with only display connected (no buttons)
- Check for short circuits
- Use higher amperage power supply

#### WiFi Issues

**Problem: Can't see WiFi networks**

*Check:*
```bash
# Via serial console
ip link show wlan0
# Should show wlan0 interface

ifconfig wlan0 up
wpa_cli scan
wpa_cli scan_results
```

*Solutions:*
- Ensure WiFi not disabled in config
- Check antenna on Pi Zero W (built-in)
- Move closer to router (Pi Zero W has weak WiFi)

**Problem: Connects to WiFi but no internet**

*Test:*
```bash
# Check connection
wpa_cli status
# Should show: wpa_state=COMPLETED

# Check IP address
ip addr show wlan0
# Should show IP address (e.g., 192.168.1.x)

# Test DNS
ping -c 3 8.8.8.8  # Google DNS
ping -c 3 google.com  # Test DNS resolution
```

*Solutions:*
- Check router DHCP settings
- Verify DNS configuration
- Restart wpa_supplicant
- Check router firewall settings

#### Software Issues

**Problem: Books don't appear in library**

*Check:*
- Books are in `/books/` directory (not subdirectory)
- Files have correct extensions (.txt, .epub, .pdf)
- Files not corrupted
- Filesystem mounted read-write

*Solution:*
```bash
# Via serial console
ls -lh /books/
# Should list your book files

# Check filesystem
mount | grep /dev/mmcblk0p2
# Should NOT show "ro" (read-only)
```

**Problem: Application crashes or freezes**

*Diagnostic:*
```bash
# Check application logs
dmesg | tail -50
# Look for errors or kernel panics

# Check if application is running
ps aux | grep ereader
```

*Solutions:*
- Reboot device
- Check for corrupted book files (remove and test)
- Re-flash SD card if persistent

### Getting Help

If you're still stuck:

1. **Check Documentation**
   - [[WIRING_GUIDE]] - Detailed wiring help
   - [[BUILD_PROCESS]] - Build troubleshooting
   - [[USER_GUIDE]] - Software usage help

2. **Review Logs**
   - Serial console output during boot
   - `dmesg` command for kernel messages
   - Application logs in `/var/log/`

3. **Community Support**
   - GitHub Issues: Report bugs and ask questions
   - Reddit: /r/raspberry_pi, /r/ereader
   - Raspberry Pi Forums

4. **Provide Information**
   When asking for help, include:
   - Hardware used (Pi model, display revision)
   - Software version (git commit or release)
   - Full error messages
   - Photos of wiring (if hardware issue)
   - Output of diagnostic commands

---

## Next Steps

### Recommended Enhancements

**1. Build an Enclosure**
   - See [[CASE_DESIGN]] for 3D printable designs
   - Commercial project boxes (Hammond, Serpac)
   - Custom laser-cut acrylic
   - Hand-carved wood case

**2. Add Battery Power**
   - LiPo battery (2000-3000mAh)
   - PowerBoost 1000C charging circuit
   - Power switch
   - Battery indicator
   - See [[POWER_SUPPLY]] for details

**3. Customize the Software**
   - Modify UI theme and fonts
   - Add new book formats
   - Implement custom features
   - See [[CONTRIBUTING]] and [[DEVELOPMENT]]

**4. Optimize for Your Use**
   - Adjust font sizes
   - Configure reading preferences
   - Set up automatic backups
   - Create book collections

### Learning More

**Understanding the System:**
- [[EREADER_DESIGN]] - Software architecture
- [[INPUT_SYSTEM]] - How buttons work
- [[FORMAT_SUPPORT]] - Book format handling
- [[WIFI_UI]] - WiFi and networking

**Advanced Topics:**
- Custom Buildroot packages
- Device tree overlays
- E-paper display drivers
- Power management optimization

**Contributing to the Project:**
- Report bugs on GitHub
- Submit improvements
- Share your build photos
- Help other builders

### Share Your Build!

We'd love to see what you created:
- Post photos to /r/raspberry_pi
- Share on Twitter/X with #OpenSourceEReader
- Submit to Hackaday
- Write a blog post about your experience

Tag us so we can feature your build!

---

## Appendix: Quick Reference

### GPIO Pin Summary

```
Display Connections:
├─ Pin 1  → VCC (3.3V)
├─ Pin 6  → GND
├─ Pin 11 → RST (GPIO 17)
├─ Pin 18 → BUSY (GPIO 24)
├─ Pin 19 → DIN/MOSI (GPIO 10)
├─ Pin 22 → DC (GPIO 25)
├─ Pin 23 → CLK/SCLK (GPIO 11)
└─ Pin 24 → CS (GPIO 8)

Button Connections:
├─ Pin 29 → UP (GPIO 5)
├─ Pin 31 → DOWN (GPIO 6)
├─ Pin 33 → SELECT (GPIO 13)
├─ Pin 34 → Common GND
├─ Pin 35 → BACK (GPIO 19)
├─ Pin 37 → MENU (GPIO 26)
└─ Pin 39 → Common GND
```

### Essential Commands

```bash
# Serial console login
Username: root
Password: ereader

# Check system info
uname -a
cat /proc/cpuinfo

# WiFi management
wpa_cli scan
wpa_cli scan_results
wpa_cli status

# File operations
ls /books/
df -h
mount

# Application control
ps aux | grep ereader
killall ereader-app
/usr/bin/ereader-app

# System logs
dmesg | tail -50
cat /var/log/messages

# Reboot/shutdown
reboot
poweroff
```

### Specifications at a Glance

| Component | Specification |
|-----------|---------------|
| Display | 4.2" e-paper, 400×300 px, B/W |
| CPU | ARM1176JZF-S @ 1GHz |
| RAM | 512MB LPDDR2 |
| Storage | 8GB+ microSD |
| Connectivity | WiFi 802.11 b/g/n |
| Power | 5V 2A micro-USB |
| Boot Time | 10-30 seconds |
| OS | Custom Buildroot Linux 6.1 |
| Formats | TXT, EPUB, PDF, HTML, MD |

### Build Time Summary

| Phase | Time |
|-------|------|
| Part procurement | 15-30 min + 1-4 weeks shipping |
| Software build | 1-4 hours (automated) |
| Soldering header | 15-30 min |
| Wiring display + buttons | 30-60 min |
| Flashing SD card | 5-10 min |
| Testing and config | 30-120 min |
| **Total active time** | **4-8 hours** |

---

## Conclusion

Congratulations on building your own open-source e-reader! You now have a fully customizable, privacy-respecting reading device that you built with your own hands.

This is just the beginning - you can modify the software, design custom cases, add new features, and make it truly your own.

**What You've Accomplished:**
✅ Built a complete embedded Linux device from scratch
✅ Learned hardware interfacing (SPI, GPIO)
✅ Worked with e-paper display technology
✅ Created a functional, useful device
✅ Joined the open-source hardware community

**Happy Reading!** 📚

---

*For more information, visit the [project repository](https://github.com/your-repo/open-source-ereader) or check the documentation in the `docs/` directory.*

**License:** This documentation is released under Creative Commons BY-SA 4.0
**Last Updated:** 2026-01-17
**Document Version:** 1.0.0
