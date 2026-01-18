---
type: architecture
title: System Overview and Architecture
created: 2026-01-17
tags:
  - architecture
  - system-design
  - overview
  - boot-sequence
  - phase-07
related:
  - "[[EREADER_DESIGN]]"
  - "[[INPUT_SYSTEM]]"
  - "[[FORMAT_SUPPORT]]"
  - "[[WIFI_UI]]"
  - "[[OTA_UPDATES]]"
  - "[[BUILD_GUIDE]]"
  - "[[USER_GUIDE]]"
---

# System Overview and Architecture

This document provides a comprehensive high-level overview of the open-source e-reader system, covering the complete architecture from hardware boot to user interaction. It serves as the entry point for understanding how all components fit together.

## Table of Contents

1. [System Overview](#system-overview)
2. [High-Level Architecture](#high-level-architecture)
3. [Boot Sequence](#boot-sequence)
4. [Component Architecture](#component-architecture)
5. [Data Flow Diagrams](#data-flow-diagrams)
6. [File System Layout](#file-system-layout)
7. [Design Decisions and Trade-offs](#design-decisions-and-trade-offs)
8. [Performance Characteristics](#performance-characteristics)
9. [Related Documentation](#related-documentation)

---

## System Overview

### Project Mission

The open-source e-reader is a minimal, distraction-free reading device built entirely from open-source hardware and software. It provides:

- **Fast Boot**: 10-30 seconds from power-on to reading interface
- **Long Battery Life**: Days to weeks of reading on a single charge
- **Open Platform**: All code, schematics, and documentation freely available
- **Hackable**: Easy to modify, extend, and customize
- **Privacy-Focused**: No tracking, no ads, no cloud dependencies

### System Components

```
┌─────────────────────────────────────────────────────────────┐
│                    E-Reader System                          │
├─────────────────────────────────────────────────────────────┤
│  Hardware Layer                                             │
│  - Raspberry Pi Zero W (BCM2835, 512MB RAM, WiFi)           │
│  - Waveshare 4.2" E-Paper Display (400×300, SPI)            │
│  - 5 GPIO Tactile Buttons (UP, DOWN, SELECT, BACK, MENU)    │
│  - Optional Battery System (18650 Li-Ion + Power Monitor)   │
├─────────────────────────────────────────────────────────────┤
│  Operating System Layer                                     │
│  - Custom Buildroot Linux (Kernel 6.1 LTS)                  │
│  - Minimal Init System (systemd-lite or busybox init)       │
│  - Linux Input Subsystem (gpio-keys driver)                 │
│  - SPI Bus Driver (BCM2835 SPI)                             │
├─────────────────────────────────────────────────────────────┤
│  Driver Layer                                               │
│  - E-Paper Display Driver (SPI communication, IL0398)       │
│  - Button Input Library (debouncing, event callbacks)       │
│  - Battery Monitor (INA219/MCP3008, optional)               │
│  - WiFi Manager (WPA Supplicant wrapper)                    │
├─────────────────────────────────────────────────────────────┤
│  Application Layer                                          │
│  - E-Reader Application (main process)                      │
│    - Book Manager (scanning, metadata, bookmarks)           │
│    - Format Support (TXT, EPUB, PDF)                        │
│    - Text Renderer (pagination, word wrap, fonts)           │
│    - UI System (library browser, reader, menus)             │
│    - Settings Manager (preferences, persistence)            │
│    - Search Engine (in-book text search)                    │
│    - Power Manager (auto-sleep, wake)                       │
│    - Network Features (WiFi config, downloads, OTA)         │
└─────────────────────────────────────────────────────────────┘
```

### Key Specifications

| Aspect | Specification |
|--------|---------------|
| **Processor** | ARM1176JZF-S @ 700MHz (BCM2835) |
| **Memory** | 512MB SDRAM (200-300MB available to app) |
| **Display** | 400×300 pixels, 1-bit B&W, 4.2" diagonal |
| **Storage** | MicroSD card (8GB minimum, ext4 filesystem) |
| **Input** | 5 hardware buttons (GPIO-connected) |
| **Network** | 802.11 b/g/n WiFi (BCM43438 chip) |
| **Power** | 5V 2A micro-USB (or optional battery system) |
| **Boot Time** | 10-30 seconds to reading interface |
| **Operating System** | Custom Buildroot Linux (~50MB rootfs) |

---

## High-Level Architecture

### System Architecture Diagram

```
┌───────────────────────────────────────────────────────────────────┐
│                         USER INTERACTION                          │
│               (Physical Buttons + E-Paper Display)                │
└─────────────────┬─────────────────────────┬───────────────────────┘
                  │                         │
         Button Events                 Display Updates
                  │                         │
                  ▼                         ▼
        ┌─────────────────┐      ┌──────────────────┐
        │  Input System   │      │ Display Driver   │
        │  (gpio-keys)    │      │ (SPI + IL0398)   │
        └────────┬────────┘      └────────▲─────────┘
                 │                        │
            Input Events              Framebuffer
                 │                        │
                 ▼                        │
        ┌─────────────────────────────────┴─────────────────┐
        │          E-Reader Application (main)              │
        │                                                   │
        │  ┌───────────────┐  ┌───────────────┐            │
        │  │ State Machine │  │ Event Loop    │            │
        │  │ (UI states)   │  │ (button poll) │            │
        │  └───────┬───────┘  └───────┬───────┘            │
        │          │                  │                     │
        │          ▼                  ▼                     │
        │  ┌──────────────────────────────────────────┐    │
        │  │         UI Components                    │    │
        │  │  - Library Browser                       │    │
        │  │  - Reader View                           │    │
        │  │  - Settings Menu                         │    │
        │  │  - Search UI                             │    │
        │  └──────────────┬───────────────────────────┘    │
        │                 │                                │
        │                 ▼                                │
        │  ┌──────────────────────────────────────────┐    │
        │  │       Core Services                      │    │
        │  │  ┌────────────┐  ┌───────────────┐      │    │
        │  │  │ Book Mgr   │  │ Text Renderer │      │    │
        │  │  └────────────┘  └───────────────┘      │    │
        │  │  ┌────────────┐  ┌───────────────┐      │    │
        │  │  │ Settings   │  │ Search Engine │      │    │
        │  │  └────────────┘  └───────────────┘      │    │
        │  │  ┌────────────┐  ┌───────────────┐      │    │
        │  │  │ Power Mgr  │  │ WiFi Manager  │      │    │
        │  │  └────────────┘  └───────────────┘      │    │
        │  └──────────────┬───────────────────────────┘    │
        │                 │                                │
        │                 ▼                                │
        │  ┌──────────────────────────────────────────┐    │
        │  │      Format Support Layer                │    │
        │  │  ┌──────────┐ ┌──────────┐ ┌─────────┐  │    │
        │  │  │TXT Reader│ │EPUB Reader│ │PDF Reader│ │    │
        │  │  └──────────┘ └──────────┘ └─────────┘  │    │
        │  └──────────────┬───────────────────────────┘    │
        │                 │                                │
        └─────────────────┼────────────────────────────────┘
                          │
                          ▼
              ┌───────────────────────┐
              │   File System         │
              │  /books/              │
              │  /etc/ereader/        │
              │  /var/log/            │
              └───────────────────────┘
```

### Layered Architecture

The system follows a clean layered architecture:

1. **Hardware Layer**: Physical components (Pi Zero W, display, buttons, battery)
2. **Kernel Layer**: Linux kernel with device drivers
3. **Driver Layer**: User-space drivers and hardware abstraction
4. **Service Layer**: Core services (book management, rendering, search)
5. **UI Layer**: User interface components and state management
6. **Application Layer**: Main event loop and coordination

Each layer only depends on layers below it, ensuring clean separation of concerns.

---

## Boot Sequence

### Complete Boot Flow

```
Power On (t=0s)
    │
    ▼
┌─────────────────────────────────────────────┐
│ Raspberry Pi Bootloader (GPU firmware)      │  t=0-2s
│ - Load from SD card boot partition          │
│ - Initialize hardware (RAM, clocks, GPU)    │
│ - Load kernel to memory                     │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│ Linux Kernel Boot                           │  t=2-7s
│ - Decompress kernel                         │
│ - Initialize device drivers                 │
│   - SPI bus (BCM2835 SPI)                   │
│   - GPIO (gpio-keys for buttons)            │
│   - WiFi chip (BCM43438)                    │
│   - MMC/SD card controller                  │
│ - Mount root filesystem (ext4)              │
│ - Start init process (PID 1)                │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│ Init System (systemd/busybox init)          │  t=7-9s
│ - Mount /proc, /sys, /dev                   │
│ - Start essential services:                 │
│   - udev (device management)                │
│   - syslog (logging)                        │
│   - wpa_supplicant (WiFi, if configured)    │
│ - Start e-reader application service        │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│ E-Reader Application Startup                │  t=9-12s
│ 1. Initialize logging                       │
│ 2. Load settings from /etc/ereader/         │
│ 3. Initialize display:                      │
│    - Open SPI device (/dev/spidev0.0)       │
│    - Send initialization sequence           │
│    - Clear display to white                 │
│ 4. Initialize button input:                 │
│    - Open input device (/dev/input/event0)  │
│    - Set up event polling                   │
│ 5. Initialize core services:                │
│    - Book manager (scan /books/)            │
│    - Settings manager                       │
│    - Power manager                          │
│    - Battery monitor (if available)         │
│ 6. Load bookmarks and reading positions     │
│ 7. Display splash screen (2s)               │
│ 8. Transition to Library Browser            │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│ Library Browser Ready                       │  t=12-15s
│ - Book list displayed on screen             │
│ - Buttons responsive                        │
│ - User can select book and start reading    │
└─────────────────────────────────────────────┘
                 │
                 ▼
            Ready for User
```

### Boot Performance Breakdown

| Phase | Duration | Optimization Status |
|-------|----------|-------------------|
| Bootloader | 2s | Fixed (GPU firmware) |
| Kernel boot | 5s | Optimized (minimal drivers) |
| Init system | 2s | Optimized (minimal services) |
| App startup | 3-6s | Can be improved (parallel init) |
| **Total** | **12-15s** | **Target: <10s** |

### Boot Optimization Techniques

1. **Kernel Configuration**: Only essential drivers compiled in
2. **Minimal Init**: No unnecessary services (no SSH, no cron)
3. **Fast Filesystem**: ext4 with noatime mount option
4. **Deferred WiFi**: WiFi only activated when needed
5. **Display Overlap**: Show splash while scanning books
6. **Lazy Loading**: Load book metadata on-demand

---

## Component Architecture

### Core Components

#### 1. Book Manager

**Location**: `src/ereader/books/book_manager.c/h`

**Purpose**: Centralized book library management

**Responsibilities**:
- Scan `/books/` directory for supported files (.txt, .epub, .pdf)
- Extract and cache book metadata (title, author, file size)
- Maintain reading positions (bookmarks)
- Sort and filter book lists

**Key Data Structures**:
```c
typedef struct {
    char filename[256];       // Basename of file
    char title[256];          // Display title (from metadata or filename)
    char author[256];         // Author (from EPUB/PDF metadata)
    book_format_t format;     // TXT, EPUB, or PDF
    long file_size;           // Size in bytes
    int last_page;            // Bookmark (page number)
    time_t last_read;         // Last access timestamp
} book_metadata_t;

typedef struct {
    book_metadata_t *books;   // Array of book metadata
    int count;                // Number of books
    time_t last_scan;         // When directory was last scanned
} book_list_t;
```

**Related**: [[EREADER_DESIGN]]

#### 2. Format Support

**Location**: `src/ereader/formats/`

**Purpose**: Unified interface for multiple book formats

**Architecture**: Plugin system with format-specific readers
- **TXT Reader**: Direct file read, UTF-8 text
- **EPUB Reader**: ZIP extraction, XML parsing, HTML stripping
- **PDF Reader**: External tool integration (pdftotext)

**Key Interface**:
```c
typedef struct {
    bool (*validate)(const char *filepath);
    int (*open)(const char *filepath, book_t *book);
    void (*close)(book_t *book);
    int (*get_text)(book_t *book, char **text, int *length);
    int (*get_metadata)(book_t *book, book_metadata_t *meta);
} format_interface_t;
```

**Related**: [[FORMAT_SUPPORT]]

#### 3. Text Renderer

**Location**: `src/ereader/rendering/text_renderer.c/h`

**Purpose**: Convert text to paginated display content

**Capabilities**:
- Word wrapping with hyphenation support
- Multiple font sizes (6×12, 8×16, 10×20)
- Configurable margins and line spacing
- Smart pagination (avoid breaking sentences)
- Status bar and UI chrome rendering

**Pipeline**:
```
Raw Text → Word Wrap → Pagination → Framebuffer Rendering → Display
```

**Related**: [[EREADER_DESIGN]], section "Text Layout Engine"

#### 4. Input System

**Location**: `src/ereader/ui/` (integrated with UI components)

**Purpose**: Translate button events to application actions

**Design**: Event-driven state machine
- Different button mappings per UI state
- Debouncing handled by kernel driver
- Long-press detection (future feature)

**Event Flow**:
```
Hardware Button → gpio-keys driver → /dev/input/event0 →
Application Poll → Event Handler → State Machine → UI Update
```

**Related**: [[INPUT_SYSTEM]]

#### 5. Display Driver

**Location**: `src/display-test/epd_driver.c/h` (reused by main app)

**Purpose**: Low-level e-paper display control

**Capabilities**:
- SPI communication with IL0398 controller
- Full and partial refresh modes
- Power management (sleep/wake)
- Framebuffer → Display pipeline

**Key Operations**:
- `epd_init()`: Initialize hardware and clear display
- `epd_display_frame()`: Render framebuffer to screen (~5s)
- `epd_sleep()`: Power down display to save energy

**Related**: [[DISPLAY_SPECS]]

#### 6. Settings Manager

**Location**: `src/ereader/settings/settings_manager.c/h`

**Purpose**: Persist user preferences

**Settings Storage**: `/etc/ereader/settings.conf` (INI-style key=value)

**Managed Settings**:
- Font size (small/medium/large)
- Line spacing (single/1.5/double)
- Margins (narrow/normal/wide)
- Auto-sleep timeout (5/10/15/30 min or never)
- Display mode (normal/dark - future)

**Related**: [[EREADER_DESIGN]], section "Settings System"

#### 7. Search Engine

**Location**: `src/ereader/search/search_engine.c/h`

**Purpose**: In-book text search

**Algorithm**: Linear substring search (case-sensitive or insensitive)

**Features**:
- Predefined search terms (no keyboard required)
- Context extraction (60 chars around match)
- Result pagination
- Jump-to-page functionality

**Performance**: ~500-1000 matches/second on Pi Zero W

**Related**: [[EREADER_DESIGN]], section "Search Engine"

#### 8. Power Manager

**Location**: `src/ereader/power/power_manager.c/h`

**Purpose**: Automatic power saving with idle timeout

**Features**:
- Configurable idle timeout
- Sleep warning (30s before sleep)
- Display power-down in sleep mode
- Wake on button press

**State Machine**:
```
ACTIVE → (idle timeout-30s) → WARNING → (30s) → SLEEPING
  ↑                                                 │
  └────────────(button press)──────────────────────┘
```

**Related**: [[EREADER_DESIGN]], section "Power Manager"

#### 9. Battery Monitor

**Location**: `src/ereader/power/battery_monitor.c/h`

**Purpose**: Read battery voltage and estimate remaining charge

**Supported Hardware**:
- INA219 (I2C current/voltage sensor) - preferred
- MCP3008 (SPI ADC with voltage divider)
- Dummy mode (always 100%)

**Integration**: Optional feature, gracefully disabled if hardware absent

**Related**: [[POWER_SUPPLY]], [[BOM]]

#### 10. WiFi Manager

**Location**: `src/ereader/network/wifi_manager.c/h`

**Purpose**: WiFi configuration and connection management

**Features**:
- Scan for available networks
- WPA/WPA2 authentication
- Connection status monitoring
- Configuration persistence

**UI Integration**: Interactive WiFi setup menu with SSID selection

**Related**: [[WIFI_UI]], [[WIFI_SETUP]]

#### 11. Download Manager

**Location**: `src/ereader/network/download_manager.c/h`

**Purpose**: Download books from online sources

**Supported Sources**:
- Project Gutenberg
- Direct URL downloads
- Future: Calibre integration

**Features**:
- HTTP/HTTPS support
- Progress tracking
- Automatic format detection
- Save to `/books/` directory

**Related**: [[BOOK_SOURCES]]

#### 12. OTA Update System

**Location**: `src/ereader/network/` (integrated with WiFi manager)

**Purpose**: Over-the-air software updates

**Architecture**:
- Download signed update packages
- Verify checksums
- Apply updates to SD card partitions
- Automatic rollback on failure

**Related**: [[OTA_UPDATES]]

---

## Data Flow Diagrams

### Button Press → Screen Update

This is the most common interaction flow:

```
┌─────────────┐
│ User presses│
│ DOWN button │
└──────┬──────┘
       │
       ▼
┌──────────────────────────────────────┐
│ Hardware: GPIO pin goes LOW          │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ Kernel: gpio-keys driver detects     │
│         edge, applies debouncing     │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ Kernel: Write event to /dev/input/  │
│         event0 device file           │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Event loop reads from device    │
│      using read() syscall            │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Decode input_event struct       │
│      - type = EV_KEY                 │
│      - code = KEY_DOWN               │
│      - value = 1 (press)             │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: State Machine processes event   │
│      Current state: READING          │
│      Action: Next page               │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Update reading state            │
│      - current_page++                │
│      - Calculate page offset         │
│      - Extract text for new page     │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Text Renderer                   │
│      - Apply word wrapping           │
│      - Format status bar             │
│      - Render to framebuffer         │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Display Driver                  │
│      - Send framebuffer via SPI      │
│      - Trigger display refresh       │
│      - Wait ~5 seconds               │
└──────┬───────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ Hardware: E-paper display updates    │
│           New page visible to user   │
└──────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────────┐
│ App: Save bookmark                   │
│      - Update /etc/ereader/          │
│        bookmarks.txt                 │
└──────────────────────────────────────┘
```

**Latency Breakdown**:
- Hardware to kernel: <1ms
- Kernel to app: <10ms
- State update: <1ms
- Text rendering: <100ms
- Display refresh: ~5000ms (hardware limit)
- Bookmark save: <50ms

**Total**: ~5.1 seconds from button press to visible update

### Book Opening Flow

```
User Selects Book from Library
    │
    ▼
┌─────────────────────────────────────────┐
│ Book Manager: Get metadata              │
│ - filename: "example.epub"              │
│ - format: EPUB                          │
│ - last_page: 42 (from bookmarks)        │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ Format Interface: Select EPUB reader    │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ EPUB Reader: Open file                  │
│ 1. Unzip /books/example.epub            │
│ 2. Parse META-INF/container.xml         │
│ 3. Locate content.opf                   │
│ 4. Extract spine (reading order)        │
│ 5. Load all XHTML chapters              │
│ 6. Strip HTML tags → plain text         │
│ 7. Concatenate into single text buffer  │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ Text Renderer: Paginate text            │
│ - Calculate pages (text_length / 750)   │
│ - Jump to last_page (42)                │
│ - Extract page text                     │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ Reader UI: Display page                 │
│ - Render status bar                     │
│ - Render page content                   │
│ - Render control hints                  │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ Display Driver: Update screen           │
│ - Full refresh (~5s)                    │
└─────────────────────────────────────────┘
         │
         ▼
     User Reading
```

**Opening Time Estimates**:
- Small TXT (50KB): <1s
- Medium EPUB (500KB): 2-3s
- Large PDF (5MB): 5-10s

### WiFi Connection Flow

```
User Enters WiFi Settings
    │
    ▼
┌─────────────────────────────────────────┐
│ WiFi Manager: Scan for networks         │
│ - wpa_cli scan                          │
│ - wpa_cli scan_results                  │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ WiFi UI: Display network list           │
│ - SSID, signal strength, encryption     │
└────────┬────────────────────────────────┘
         │
         ▼
    User Selects Network
         │
         ▼
┌─────────────────────────────────────────┐
│ WiFi UI: Password entry screen          │
│ (Future: on-screen keyboard)            │
│ (Current: pre-configured in config)     │
└────────┬────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│ WiFi Manager: Connect                   │
│ 1. Write wpa_supplicant.conf            │
│ 2. wpa_cli reconfigure                  │
│ 3. Wait for connection (timeout 30s)    │
│ 4. Verify IP address (DHCP)             │
└────────┬────────────────────────────────┘
         │
         ├──Success──┐
         │           │
         │           ▼
         │      ┌─────────────────────────────┐
         │      │ Time Sync: NTP query        │
         │      │ Download Manager: Available │
         │      │ OTA Updates: Check          │
         │      └─────────────────────────────┘
         │
         └──Failure──┐
                     │
                     ▼
              ┌─────────────────────────────┐
              │ WiFi UI: Error message      │
              │ - "Wrong password"          │
              │ - "Signal too weak"         │
              │ - "DHCP timeout"            │
              └─────────────────────────────┘
```

---

## File System Layout

### Complete Directory Structure

```
/ (Root filesystem, ext4, ~50MB total)
│
├── bin/                          # Essential system binaries
│   ├── sh                        # Shell (busybox)
│   ├── ls, cat, grep, etc.       # Basic utilities
│   └── ...
│
├── sbin/                         # System administration binaries
│   ├── init                      # Init process
│   └── ...
│
├── lib/                          # Shared libraries
│   ├── libc.so.6                 # C standard library
│   ├── libpthread.so.0           # Threading library
│   └── ...
│
├── usr/
│   ├── bin/
│   │   └── ereader               # Main application binary (~500KB)
│   │
│   ├── lib/                      # Application libraries
│   │   ├── libzip.so             # EPUB support
│   │   ├── libxml2.so            # XML parsing
│   │   └── libpoppler.so         # PDF support
│   │
│   └── share/
│       └── ereader/
│           ├── fonts/            # Future: additional fonts
│           └── themes/           # Future: UI themes
│
├── etc/
│   ├── ereader/                  # Application configuration
│   │   ├── settings.conf         # User settings (font, margins, etc.)
│   │   ├── bookmarks.txt         # Reading positions
│   │   └── wifi_credentials.conf # WiFi networks (encrypted)
│   │
│   ├── wpa_supplicant/           # WiFi configuration
│   │   └── wpa_supplicant.conf
│   │
│   ├── init.d/                   # Init scripts
│   │   └── S99ereader            # Auto-start e-reader app
│   │
│   └── fstab                     # Filesystem mount table
│
├── var/
│   ├── log/
│   │   ├── ereader.log           # Application log
│   │   ├── syslog                # System log
│   │   └── boot.log              # Boot messages
│   │
│   ├── cache/
│   │   └── ereader/
│   │       ├── covers/           # Future: book cover thumbnails
│   │       └── metadata/         # Cached metadata
│   │
│   └── tmp/                      # Temporary files
│       └── epub_extract/         # EPUB extraction workspace
│
├── books/                        # USER CONTENT - Books directory
│   ├── example.txt               # Plain text books
│   ├── novel.epub                # EPUB books
│   ├── manual.pdf                # PDF books
│   └── welcome.txt               # Default sample book
│
├── home/
│   └── root/                     # Root user home (mostly unused)
│
├── dev/                          # Device files (managed by udev)
│   ├── input/
│   │   └── event0                # Button input device
│   ├── spidev0.0                 # SPI device for display
│   ├── i2c-1                     # I2C bus (battery monitor)
│   ├── mmcblk0                   # SD card block device
│   └── ...
│
├── proc/                         # Process information (virtual)
├── sys/                          # Kernel/device info (virtual)
└── tmp/                          # Temporary files (tmpfs, RAM-based)
```

### Critical Paths

| Path | Purpose | Writable | Size Limit |
|------|---------|----------|------------|
| `/usr/bin/ereader` | Main application executable | No | ~500KB |
| `/books/` | User's book library | Yes | Limited by SD card |
| `/etc/ereader/` | Configuration and state | Yes | ~10KB |
| `/var/log/ereader.log` | Application log | Yes | Rotated at 1MB |
| `/var/tmp/` | Temporary files | Yes | RAM-limited |

### Persistence Strategy

**Read-Only Elements** (burned into SD image):
- Operating system (`/bin`, `/lib`, `/usr`)
- Application binary (`/usr/bin/ereader`)
- Default configuration (`/etc/` - base files)

**Writable Elements** (user data):
- Books (`/books/`)
- Settings and bookmarks (`/etc/ereader/`)
- Logs (`/var/log/`)

**Backup Recommendations**:
- Users should back up `/books/` and `/etc/ereader/` directories
- Everything else can be restored from SD card image

---

## Design Decisions and Trade-offs

### 1. Embedded Linux vs. Bare Metal

**Decision**: Use embedded Linux (Buildroot)

**Rationale**:
- **Pros**: Rich ecosystem, driver support, WiFi stack, filesystem
- **Cons**: Larger footprint, slower boot than bare metal
- **Trade-off**: Accept 10-15s boot time for 90% reduction in development effort

### 2. Full Book Loading vs. Streaming

**Decision**: Load entire book into memory (with 10MB size limit)

**Rationale**:
- **Pros**: Fast page turns, simple implementation
- **Cons**: Memory usage scales with book size
- **Trade-off**: 200MB available RAM easily handles typical books (1-5MB)
- **Future**: Implement streaming for >10MB books if needed

### 3. Bitmap Fonts vs. TrueType

**Decision**: Use embedded bitmap fonts (8×16, 6×12, 10×20)

**Rationale**:
- **Pros**: No external dependencies, fast rendering, tiny memory footprint (<10KB)
- **Cons**: Limited font sizes, no anti-aliasing
- **Trade-off**: E-paper display is 1-bit anyway, anti-aliasing not beneficial

### 4. SPI vs. Parallel Display Interface

**Decision**: Use SPI for display communication

**Rationale**:
- **Pros**: Only 4 GPIO pins needed, standard Linux SPI driver
- **Cons**: Slower than parallel (but 5s refresh hides this)
- **Trade-off**: Display refresh is hardware-limited (5s), SPI speed irrelevant

### 5. Event-Driven vs. Polling Input

**Decision**: Polling with blocking read on `/dev/input/event0`

**Rationale**:
- **Pros**: Simple main loop, no threading complexity
- **Cons**: Slight latency vs. true event-driven (negligible <10ms)
- **Trade-off**: Single-threaded simplicity worth minor latency

### 6. External Tools vs. Native PDF Parsing

**Decision**: Use `pdftotext` utility (Poppler) for PDF extraction

**Rationale**:
- **Pros**: Mature PDF parsing, handles complex PDFs
- **Cons**: Requires spawning subprocess, slower
- **Trade-off**: PDF is complex format, don't reinvent the wheel

### 7. EPUB HTML Rendering vs. Text Extraction

**Decision**: Strip HTML tags and render as plain text

**Rationale**:
- **Pros**: Simple, no HTML rendering engine needed
- **Cons**: Loses formatting (bold, italic, images)
- **Trade-off**: E-paper display limited anyway, text-only sufficient for reading

### 8. WiFi Always-On vs. On-Demand

**Decision**: WiFi disabled by default, activated only when needed

**Rationale**:
- **Pros**: Dramatically extends battery life (WiFi is power-hungry)
- **Cons**: Slight delay when user wants network features
- **Trade-off**: Days of battery life > instant network access

### 9. Systemd vs. Busybox Init

**Decision**: Use minimal init (Busybox or systemd-lite)

**Rationale**:
- **Pros**: Fast boot, tiny footprint
- **Cons**: Less sophisticated service management
- **Trade-off**: E-reader runs single application, complex init unnecessary

### 10. Pre-Built Images vs. Source-Only

**Decision**: Provide both pre-built SD card images and source code

**Rationale**:
- **Pros**: Users can get started in 5 minutes (flash image)
- **Cons**: More maintenance (build multiple images)
- **Trade-off**: Accessibility for non-technical users worth the effort

---

## Performance Characteristics

### Boot Performance

| Phase | Target | Actual | Optimization Potential |
|-------|--------|--------|------------------------|
| Bootloader | 2s | 2s | None (GPU firmware) |
| Kernel | 3s | 5s | Medium (disable unused drivers) |
| Init | 1s | 2s | Low (already minimal) |
| App startup | 2s | 3-6s | High (parallel book scanning) |
| **Total** | **8s** | **12-15s** | **Achievable** |

### Runtime Performance

| Operation | Target | Actual | Notes |
|-----------|--------|--------|-------|
| Button response | <100ms | 20-50ms | Excellent |
| Page turn (text) | <200ms | 50-150ms | Excellent |
| Page turn (display) | N/A | ~5000ms | Hardware limit |
| Book open (TXT) | <1s | 0.5-1s | Excellent |
| Book open (EPUB) | <3s | 2-3s | Good |
| Book open (PDF) | <5s | 5-10s | Acceptable |
| Library scan (100 books) | <1s | 0.5-1s | Excellent |
| Search (1000 matches) | <2s | 1-2s | Good |

### Memory Usage

| Component | Static | Dynamic | Peak |
|-----------|--------|---------|------|
| Code + Libraries | 2MB | - | 2MB |
| Framebuffer | 15KB | - | 15KB |
| Book metadata (100 books) | - | 30KB | 30KB |
| Current book text | - | Variable | 1-10MB |
| UI buffers | - | 50KB | 50KB |
| **Typical Total** | **2MB** | **2-5MB** | **4-7MB** |
| **Available** | - | - | **200-300MB** |

**Safety Margin**: 40-70× headroom for typical usage

### Power Consumption

| State | Current Draw | Battery Life (2500mAh) |
|-------|--------------|------------------------|
| Active reading (WiFi off) | ~150mA | 16 hours |
| Active with WiFi | ~350mA | 7 hours |
| Sleep mode | ~50mA | 50 hours (2 days) |
| Deep sleep (future) | ~10mA | 250 hours (10 days) |

**Assumptions**: 3.7V Li-Ion battery, no display refresh in sleep

---

## Related Documentation

### Architecture Documents
- **[[EREADER_DESIGN]]** - Detailed application architecture
- **[[INPUT_SYSTEM]]** - Button input system design
- **[[FORMAT_SUPPORT]]** - Book format abstraction layer
- **[[WIFI_UI]]** - WiFi configuration interface
- **[[OTA_UPDATES]]** - Over-the-air update system

### Hardware Documents
- **[[BOM]]** - Complete bill of materials
- **[[WIRING_GUIDE]]** - Hardware assembly instructions
- **[[DISPLAY_SPECS]]** - E-paper display specifications
- **[[BUTTON_LAYOUT]]** - GPIO button connections
- **[[POWER_SUPPLY]]** - Battery and power system
- **[[CASE_DESIGN]]** - Enclosure design options
- **[[SCHEMATIC]]** - Hardware schematics

### Build and Deployment
- **[[BUILD_GUIDE]]** - Complete build from source instructions
- **[[BUILD_PROCESS]]** - Buildroot build system details
- **[[CUSTOM_PACKAGES]]** - Creating Buildroot packages
- **[[FLASHING_GUIDE]]** - SD card flashing instructions

### User Documentation
- **[[USER_GUIDE]]** - End-user operating instructions
- **README.md** - Project overview and quick start

### Testing Documentation
- **[[TEST_PLAN]]** - Comprehensive testing strategy
- **[[BUTTON_TESTING]]** - Input system tests
- **[[FORMAT_TESTING]]** - Format support tests
- **[[PERFORMANCE]]** - Performance benchmarks
- **[[WIFI_TESTING]]** - Network feature tests

### Project Planning
- **Phase Logs** (`docs/progress/PHASE_*_LOG.md`) - Implementation history
- **Phase Planning** (`Auto Run Docs/Initiation/Phase-*.md`) - Future roadmap

---

## Glossary

| Term | Definition |
|------|------------|
| **Buildroot** | Embedded Linux build system used to create custom OS |
| **E-paper** | Electrophoretic display technology (low power, sunlight readable) |
| **EPUB** | Electronic publication format (ZIP + XHTML + XML metadata) |
| **Framebuffer** | Memory buffer representing screen pixels |
| **gpio-keys** | Linux kernel driver for GPIO-connected buttons |
| **IL0398** | E-paper display controller chip (Waveshare 4.2") |
| **OTA** | Over-The-Air (wireless software updates) |
| **SPI** | Serial Peripheral Interface (4-wire synchronous bus) |
| **WPA Supplicant** | WiFi authentication daemon (Linux) |

---

## Revision History

- **2026-01-17**: Initial system overview document created for Phase 07 release preparation
