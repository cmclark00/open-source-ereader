# Operating System Architecture

This document outlines the software architecture for the open-source e-reader OS.

## Design Philosophy

- **Minimal**: Only include what's necessary for reading e-books
- **Fast boot**: Target boot time under 5 seconds
- **Power efficient**: Optimize for battery life (weeks of reading)
- **Stable**: Prioritize reliability over features
- **Hackable**: Easy for users to customize and extend

## OS Stack Overview

```
┌─────────────────────────────────────┐
│     E-Reader Application            │  User space applications
├─────────────────────────────────────┤
│   UI Framework (Qt/GTK/Custom)      │  Graphics libraries
├─────────────────────────────────────┤
│  Document Rendering (MuPDF, etc)    │  Content libraries
├─────────────────────────────────────┤
│    Display Manager / Init System    │  System services
├─────────────────────────────────────┤
│      C Library (musl/glibc)         │  System libraries
├─────────────────────────────────────┤
│        Linux Kernel (6.x)           │  Kernel space
├─────────────────────────────────────┤
│         U-Boot Bootloader           │  Firmware
└─────────────────────────────────────┘
```

## Build System Options

### Option 1: Buildroot (RECOMMENDED)
**Pros**:
- Designed for embedded systems
- Small footprint
- Fast builds
- Good documentation
- Wide hardware support

**Cons**:
- Less flexibility than Yocto
- Harder to create package manager

**Best for**: Getting started quickly, minimal systems

### Option 2: Yocto/OpenEmbedded
**Pros**:
- Industry standard
- Very flexible
- Good package management
- Scalable

**Cons**:
- Steep learning curve
- Slower builds
- More complex

**Best for**: Long-term maintainability, complex systems

### Option 3: Debian/Ubuntu Minimal
**Pros**:
- Familiar environment
- Easy package management
- Large software repository

**Cons**:
- Larger footprint (200MB+ rootfs)
- Slower boot time
- Higher resource usage

**Best for**: Development/prototyping on capable hardware

### Recommendation
Start with **Buildroot** for the production OS, use Debian on dev boards for initial development.

## Bootloader: U-Boot

- Standard bootloader for embedded Linux
- Hardware support for most ARM SoCs
- Configuration for fast boot:
  - Disable unnecessary features
  - Silent boot mode
  - Minimal delay
  - Direct kernel loading

**Target boot time contribution**: <1 second

## Linux Kernel

### Version Strategy
- Use **mainline kernel** (6.1 LTS or newer) when possible
- Minimal vendor patches
- Focus on long-term support (LTS) releases

### Key Kernel Configs
```
CONFIG_EMBEDDED=y                    # Embedded system optimizations
CONFIG_CC_OPTIMIZE_FOR_SIZE=y        # Optimize for size
CONFIG_FB=y                          # Framebuffer support
CONFIG_SPI=y                         # SPI for e-ink display
CONFIG_INPUT_EVDEV=y                 # Input event interface
CONFIG_RTC=y                         # Real-time clock
CONFIG_PM=y                          # Power management
CONFIG_CPU_FREQ=y                    # CPU frequency scaling
CONFIG_CPU_IDLE=y                    # CPU idle states
```

### Disabled Features (to reduce size/power)
- Most networking (unless WiFi model)
- Sound/audio subsystem
- GPU/DRM (not needed for e-ink)
- Bluetooth (unless required)
- USB gadget modes (unless needed)

**Target kernel size**: 2-4 MB compressed

## Init System Options

### Option 1: BusyBox init (RECOMMENDED)
- Minimal shell scripts
- Fast and simple
- ~1MB footprint

### Option 2: systemd
- More features (journaling, dependency management)
- Slower boot time
- Larger footprint (~10-15MB)
- Better for complex configurations

### Option 3: OpenRC
- Middle ground between BusyBox and systemd
- Dependency-based
- Moderate footprint

**Recommendation**: Use BusyBox init for minimal/fast boot

## Display Stack

### E-Ink Display Driver
- Kernel framebuffer driver (fbdev)
- Support for partial updates
- Waveform management
- Custom driver may be needed for specific displays

### Display Server Options

#### Option 1: Direct Framebuffer (RECOMMENDED)
**Pros**:
- Simplest approach
- Lowest overhead
- Full control of display updates
- No compositor overhead

**Cons**:
- Single application fullscreen
- Manual display management

**Best for**: Dedicated e-reader device

#### Option 2: Wayland (Minimal)
**Pros**:
- Modern graphics stack
- Better multi-app support
- Security benefits

**Cons**:
- More complex
- Higher resource usage
- Slower refresh

**Best for**: Future expandability

#### Option 3: X11 (Not Recommended)
- Too heavy for embedded
- Unnecessary features
- Higher power consumption

## C Library

### Option 1: musl libc (RECOMMENDED)
- Small footprint (~600KB)
- Fast
- Standards compliant
- Growing ecosystem

### Option 2: uClibc-ng
- Designed for embedded
- Very configurable
- Smaller than musl

### Option 3: glibc
- Standard C library
- Larger (~2MB+)
- Best compatibility

**Recommendation**: musl for production, glibc for development

## File System

### Root Filesystem
- **SquashFS** (read-only, compressed): OS partition
- **ext4** (read-write): User data and settings
- **VFAT** (optional): External SD card compatibility

### Layout
```
/boot           # Kernel, DTB (on separate partition)
/               # Root filesystem (SquashFS, compressed)
/opt/ereader    # E-reader application
/home/reader    # User documents and settings (ext4)
/media          # Mount point for external storage
```

### Partitioning (8GB eMMC example)
```
Boot partition:   4MB   (U-Boot, kernel, DTB)
Root filesystem:  128MB (SquashFS, compressed)
Data partition:   7.8GB (ext4, user documents)
```

## Core Software Components

### Document Rendering Libraries

#### Option 1: MuPDF (RECOMMENDED)
- Lightweight (~5MB)
- Fast rendering
- Supports PDF, EPUB, XPS, CBZ
- Good API

#### Option 2: Poppler
- Standard PDF library
- Larger footprint
- Good PDF support
- Limited e-book format support

#### Option 3: FBReader
- Full e-reader app
- Can use as library
- Many format support
- Larger footprint

#### Option 4: CoolReader
- Open source e-reader engine
- Good performance
- EPUB, FB2, MOBI, etc.

### UI Framework Options

#### Option 1: Qt Embedded (RECOMMENDED)
- Good framebuffer support
- Rich widget library
- QML for modern UI
- Medium footprint (~15-20MB)

#### Option 2: GTK+
- Lighter than Qt
- Good widget support
- More traditional UI
- ~10-15MB

#### Option 3: LVGL (Lightweight)
- Designed for embedded
- Very small footprint (~1MB)
- Good for simple UIs
- C-based

#### Option 4: Custom SDL/Cairo
- Maximum control
- Smaller footprint
- More development work
- Best performance

**Recommendation**: Qt for feature-rich, LVGL for ultra-minimal

## Reader Application Features

### Essential Features
- Open and read EPUB, PDF, MOBI, TXT files
- Page navigation (next, previous, jump to page)
- Bookmarks
- Reading progress tracking
- Font size adjustment
- Basic library management

### Nice-to-Have Features
- Dictionary lookup
- Annotations and highlights
- Table of contents navigation
- Search within book
- Multiple reading modes (pagination, scroll)
- Night mode (inverse colors)
- Collections/categories

## Power Management

### Strategies
1. **CPU frequency scaling**: Lower frequency during reading
2. **Suspend to RAM**: When idle for >2 minutes
3. **E-ink partial updates**: Reduce full refreshes
4. **Backlight control**: Dim or off when possible
5. **Disable unused peripherals**: WiFi, USB, etc.

### Power States
```
Active Reading:   100-200mW (partial updates every 10s)
Idle:             50-100mW (display static)
Suspend:          5-10mW (RAM retention)
Off:              <1mW
```

**Target battery life**: 3-4 weeks typical reading (30min/day)

## Software Update Strategy

### Option 1: Manual SD Card Update
- User downloads image
- Writes to SD card
- Boot from SD to update
- Simple, reliable

### Option 2: OTA Updates (if WiFi)
- Download update package
- Verify signature
- Apply in recovery mode
- More complex

### Option 3: Dual Partition
- Two root filesystem partitions
- Update inactive partition
- Swap on reboot
- Fallback capability

## Development Tools

### On-Device
- Dropbear SSH (lightweight SSH server)
- BusyBox utilities
- GDB (for debugging)

### Cross-Compilation
- ARM GCC toolchain
- Buildroot SDK
- QEMU for testing

## Storage Requirements Estimate

```
Bootloader (U-Boot):        512 KB
Kernel:                     3 MB
Root filesystem (SquashFS): 80-120 MB
  - Core system:            20 MB
  - C library:              1-2 MB
  - UI framework:           15-20 MB
  - Document libraries:     10-15 MB
  - Reader application:     5-10 MB
  - Fonts:                  10-15 MB
  - Utilities:              5-10 MB
```

**Total OS footprint**: ~100-150MB

## Memory Requirements

```
Kernel:                 15-20 MB
System services:        5-10 MB
Reader app:             30-50 MB
Document cache:         20-40 MB
Framebuffer:            1-4 MB (depends on resolution)
```

**Minimum RAM**: 128MB (tight, not recommended)
**Recommended RAM**: 256MB (comfortable for most books)
**Ideal RAM**: 512MB (smooth PDF handling)

## Next Steps

1. Set up Buildroot configuration
2. Build minimal system for target hardware
3. Port e-ink display driver
4. Test boot time and memory usage
5. Integrate document rendering library
6. Develop basic reader UI
7. Implement power management
8. Test battery life

## References

- [Buildroot User Manual](https://buildroot.org/downloads/manual/manual.html)
- [Embedded Linux Wiki](https://elinux.org/)
- [MuPDF Documentation](https://mupdf.com/docs/)
- [Linux Kernel Power Management](https://www.kernel.org/doc/html/latest/admin-guide/pm/)
