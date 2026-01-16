---
type: reference
title: E-Reader User Guide
created: 2026-01-14
tags:
  - user-guide
  - documentation
  - phase-05
related:
  - "[[EREADER_DESIGN]]"
  - "[[DISPLAY_SPECS]]"
  - "[[BUTTON_LAYOUT]]"
  - "[[FORMAT_SUPPORT]]"
---

# E-Reader User Guide

Welcome to your open-source e-reader! This guide will help you get started with using your device, adding books, and navigating the interface.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Button Controls](#button-controls)
3. [Adding Books](#adding-books)
4. [Navigating the Library](#navigating-the-library)
5. [Reading Books](#reading-books)
6. [Bookmarks](#bookmarks)
7. [Settings](#settings)
8. [Search](#search)
9. [Power Management](#power-management)
10. [Battery Monitoring](#battery-monitoring)
11. [Current Limitations](#current-limitations)
12. [Tips for Best Experience](#tips-for-best-experience)
13. [Troubleshooting](#troubleshooting)
14. [Technical Specifications](#technical-specifications)

---

## Getting Started

### First Boot

1. **Power On**: Connect the 5V micro-USB power supply to your Raspberry Pi Zero W
2. **Wait for Boot**: The device takes approximately 10-30 seconds to boot
3. **Initial Screen**: You'll see either:
   - A **library menu** listing your books (if books are present in `/books/`)
   - An **empty library message** prompting you to add books

### What You'll See

The e-reader automatically boots directly into the reading application. There's no desktop environment or complex interface - just your books.

---

## Button Controls

Your e-reader has **5 physical buttons** for navigation. The controls change depending on whether you're in the **Library Menu** or **Reading View**.

### Library Menu Controls

When browsing your book collection:

| Button | Action |
|--------|--------|
| **UP** | Move selection up one book |
| **DOWN** | Move selection down one book |
| **SELECT** | Open the selected book |
| **BACK** | Exit application (reserved for future use) |
| **MENU** | (Reserved for future settings menu) |

**Navigation Tips:**
- Selection wraps around: pressing UP at the top moves to the bottom
- Selection wraps around: pressing DOWN at the bottom moves to the top
- The selected book is highlighted with a `>` marker and inverted background
- Page indicator shows your position: `[1/3]` means page 1 of 3 pages

### Reading View Controls

When reading a book:

| Button | Action |
|--------|--------|
| **UP** | Previous page (if not on first page) |
| **DOWN** | Next page (if not on last page) |
| **SELECT** | Save bookmark at current page |
| **BACK** | Return to library menu |
| **MENU** | Open search function |

**Reading Tips:**
- At the first page, pressing UP has no effect
- At the last page, pressing DOWN has no effect
- Your reading position is automatically saved on every page turn
- The status bar shows: `Book Title [Current Page/Total Pages, Percentage%]`
- Press MENU to search within the current book

---

## Adding Books

### Supported Formats

Currently, the e-reader supports:
- **Text files** (`.txt` extension)
- **Encoding**: UTF-8 or ASCII
- **Maximum file size**: 10 MB per book
- **Line endings**: Unix (LF), Windows (CRLF), or Mac (CR)

### How to Add Books

#### Method 1: Via SD Card (Recommended)

1. **Power off** your e-reader
2. **Remove the microSD card** from the Raspberry Pi Zero W
3. **Insert the SD card** into your computer using a card reader
4. **Mount the SD card** (may happen automatically)
5. **Navigate to the `/books/` directory** on the root partition (ext4)
   - On Linux: Usually auto-mounted at `/media/username/rootfs/books/`
   - On macOS: May require ext4 driver (e.g., extFS for Mac)
   - On Windows: Requires ext4 driver (e.g., Linux File Systems for Windows, Ext2Fsd)
6. **Copy your `.txt` files** into the `/books/` directory
7. **Safely eject** the SD card
8. **Reinsert** the SD card into your Raspberry Pi Zero W
9. **Power on** - your new books will appear in the library

#### Method 2: Via SSH (for Advanced Users)

If you've enabled SSH access:

```bash
# Connect via SSH (default password: ereader)
ssh root@ereader.local

# Copy books to the device
scp mybook.txt root@ereader.local:/books/

# Or mount via SSHFS and drag-and-drop
sshfs root@ereader.local:/books ~/ereader-books
```

### Where to Find Free Books

Many public domain books are available in plain text format:

- **Project Gutenberg**: https://www.gutenberg.org/
  - Look for "Plain Text UTF-8" downloads
- **Standard Ebooks**: https://standardebooks.org/
  - Download EPUB, then extract text (future EPUB support planned)
- **Internet Archive**: https://archive.org/
  - Many texts available in plain text format

### Organizing Your Books

- Books are listed **alphabetically by filename**
- Use descriptive filenames: `Frankenstein - Mary Shelley.txt`
- Hidden files (starting with `.`) are ignored
- Subdirectories are not currently supported (future feature)
- Keep filenames under 50 characters for best display

---

## Navigating the Library

### Library Screen Layout

```
┌────────────────────────────────────────┐
│ E-Reader Library              [1/2]    │  ← Status Bar
├────────────────────────────────────────┤
│ > Frankenstein.txt                     │  ← Selected book (highlighted)
│   Pride and Prejudice.txt              │
│   The Great Gatsby.txt                 │
│   Ulysses.txt                          │
│   War and Peace.txt                    │
│   ...                                  │
│                                        │
├────────────────────────────────────────┤
│ ↑↓:Navigate  SELECT:Open  BACK:Exit   │  ← Button Hints
└────────────────────────────────────────┘
```

### Scrolling Through Books

- The library shows **14 books per screen**
- Use **UP/DOWN buttons** to navigate
- When you reach the bottom, selection wraps to the top
- When you reach the top, selection wraps to the bottom
- The page indicator `[X/Y]` shows which screen you're on

### Empty Library

If no books are found, you'll see:

```
┌────────────────────────────────────────┐
│          No books found.               │
│                                        │
│   Copy books (.txt/.epub/.pdf) to /books/           │
│                                        │
│         BACK: Rescan library           │
└────────────────────────────────────────┘
```

Press **BACK** to rescan the `/books/` directory after adding books.

---

## Reading Books

### Reading Screen Layout

```
┌────────────────────────────────────────┐
│ Frankenstein.txt              [5/42]   │  ← Status Bar
├────────────────────────────────────────┤
│ Letter 2                               │
│                                        │
│ To Mrs. Saville, England.              │
│                                        │
│ Archangel, 28th March, 17—.           │
│                                        │
│ How slowly the time passes here,       │
│ encompassed as I am by frost and       │
│ snow! Yet a second step is taken       │
│ towards my enterprise. I have hired    │
│ a vessel and am occupied in            │
│ collecting my sailors; those whom I    │
│ have already engaged appear to be      │
│                                        │  (14 lines of text)
├────────────────────────────────────────┤
│ ↑↓:Page  SELECT:Bookmark  BACK:Menu   │  ← Button Hints
└────────────────────────────────────────┘
```

### Reading Experience

- Each page displays **14 lines of text**
- Each line can hold approximately **47 characters**
- Text is automatically wrapped at word boundaries
- Long words (>47 characters) are broken with hyphens
- Page numbers are **1-based** (first page is page 1, not 0)

### Page Navigation

- **UP button**: Go to previous page
- **DOWN button**: Go to next page
- At the **first page**: UP button has no effect
- At the **last page**: DOWN button has no effect
- Page indicator shows: `[Current/Total]`

### Returning to Library

- Press **BACK button** to return to the library menu
- Your reading position is **automatically saved** as a bookmark
- Next time you open the book, you'll resume from where you left off

---

## Bookmarks

### Automatic Bookmarks

Your e-reader automatically saves your reading position:

- **On every page turn** (UP or DOWN button while reading)
- **When you leave a book** (press BACK to return to library)
- Your position is saved to `/etc/ereader/bookmarks.txt`
- Bookmarks persist across reboots
- The status bar shows your reading progress as a percentage

### Manual Bookmarks

You can manually save your current position:

1. While reading, press the **SELECT button**
2. The bookmark is immediately saved
3. This overwrites any previous bookmark for the current book

### Bookmark File Format

Bookmarks are stored in a simple CSV format at `/etc/ereader/bookmarks.txt`:

```
# E-Reader Bookmarks
# filename,page,timestamp
Frankenstein.txt,42,1736835600
Pride and Prejudice.txt,156,1736839200
```

- One bookmark per book (most recent position)
- Page numbers are **0-based** in the file (but displayed as 1-based)
- Timestamp is Unix epoch time

### Resuming Reading

1. Open a book from the library menu
2. The e-reader automatically loads your bookmark
3. If no bookmark exists, the book opens at page 1
4. The page indicator shows your current position

---

## Settings

Your e-reader includes a comprehensive settings system that allows you to customize your reading experience.

### Accessing Settings

1. From the **Library Menu**, press the **MENU** button
2. Navigate through settings using **UP/DOWN** buttons
3. Press **SELECT** to cycle through available values for each setting
4. Press **BACK** or **MENU** to save changes and return to library

### Available Settings

#### Font Size

Control the size of text displayed on screen:

- **Small** (6×12 pixels): ~63 characters per line, 21 lines per page
  - Best for: Maximizing text on screen, reading dense content
- **Medium** (8×16 pixels): ~47 characters per line, 14 lines per page (default)
  - Best for: Balanced readability and screen usage
- **Large** (10×20 pixels): ~38 characters per line, 11 lines per page
  - Best for: Improved readability, reduced eye strain

**Note:** Changing font size will re-paginate your book, but your approximate reading position (as a percentage) is preserved.

#### Line Spacing

Adjust the vertical space between lines:

- **Single**: No extra space between lines (default)
- **1.5**: 50% more space between lines
- **Double**: Double spacing between lines

**Note:** Larger line spacing reduces the number of lines per page but can improve readability.

#### Margins

Control the white space around the text:

- **Narrow**: Minimal margins, maximum text area
- **Normal**: Balanced margins (default)
- **Wide**: Larger margins, more comfortable reading

#### Display Mode

Choose between display rendering modes:

- **Normal**: Standard black text on white background (default)
- **Dark**: Inverted colors - white text on black background (future feature)

**Note:** Dark mode is currently reserved for future implementation.

#### Auto Sleep

Set the idle timeout before the device enters sleep mode:

- **5 minutes**: Sleep after 5 minutes of inactivity
- **10 minutes**: Sleep after 10 minutes of inactivity
- **15 minutes**: Sleep after 15 minutes of inactivity (default)
- **30 minutes**: Sleep after 30 minutes of inactivity
- **Never**: Disable auto-sleep

**Note:** A warning message appears 30 seconds before sleeping. The device wakes on any button press.

### Settings File

Settings are stored in `/etc/ereader/settings.conf` in a simple key=value format:

```
font_size=medium
line_spacing=single
margins=normal
display_mode=normal
auto_sleep_minutes=15
```

You can manually edit this file via SSH if needed, but changes require restarting the application.

---

## Search

Search for text within the currently open book.

### Accessing Search

1. While reading a book, press the **MENU** button
2. The search interface will appear

### Using Search

#### Selecting a Search Term

1. Use **UP/DOWN** buttons to browse predefined search terms:
   - "the"
   - "chapter"
   - "and"
   - "said"
   - "was"
2. Press **SELECT** to search for the highlighted term

#### Search Options

- **MENU** button: Toggle case sensitivity (case-sensitive ↔ case-insensitive)
- Current mode is displayed on screen

#### Viewing Results

After searching:
- The number of results is displayed (e.g., "Result 5 of 127")
- A preview shows ~60 characters of context around the match
- Use **UP/DOWN** to navigate between results
- Press **SELECT** to jump to the page containing that result
- Press **BACK** to exit search and return to reading

### Search Features

- **Fast Search**: Linear search algorithm optimized for text files
- **Context Preview**: See surrounding text for each match
- **Result Navigation**: Browse through all matches with wraparound
- **Case Sensitivity**: Toggle between case-sensitive and case-insensitive search
- **Result Counter**: Always know your position in the results (e.g., "Result 5 of 127")
- **Maximum Results**: Up to 1000 matches per search

### Search Limitations

- **Predefined Terms Only**: Cannot enter custom search text (requires external keyboard)
  - Custom text input planned for Phase 6 (WiFi keyboard support)
- **No Visual Highlighting**: Matches are not highlighted on the book page (requires rendering changes)
- **No Search History**: Previous searches are not saved
- **Memory Intensive**: Case-insensitive search creates a temporary lowercase copy of the book
- **Basic Context Display**: Context may truncate mid-word at 60 characters

### Search Tips

1. **Choose Case Sensitivity Wisely**: Case-sensitive is faster, case-insensitive finds more matches
2. **Common Terms**: Searching for "the" or "and" may produce hundreds of results
3. **Specific Terms**: Search for "chapter" to quickly navigate chapter breaks
4. **Result Navigation**: Use UP/DOWN to browse, SELECT to jump to that page

---

## Power Management

Your e-reader includes intelligent power management to conserve battery life.

### Auto-Sleep Mode

The device automatically enters sleep mode after a period of inactivity:

1. **Idle Timer**: Tracks time since last button press
2. **Sleep Warning**: 30 seconds before sleep, displays "Sleeping in 30 seconds..."
3. **Sleep Mode**: Clears display, powers down e-paper, waits for button press
4. **Wake on Button**: Any button press wakes the device and restores state

### Configuring Auto-Sleep

1. Open **Settings** from the Library Menu (MENU button)
2. Navigate to **Auto Sleep** setting
3. Select timeout: 5, 10, 15, 30 minutes, or Never
4. Default: 15 minutes

### Manual Sleep

Currently, manual sleep is not implemented. The device sleeps automatically based on the configured timeout.

### What Happens During Sleep

1. **Display Cleared**: Screen is cleared to white
2. **E-Paper Powered Down**: Display controller enters low-power mode
3. **CPU Active**: Processor remains on, waiting for button press
4. **Reading Position Saved**: Your bookmark is preserved

### Waking from Sleep

1. **Press Any Button**: The first button press wakes the device
2. **Display Re-Initialized**: E-paper controller is powered up
3. **State Restored**: Returns to the screen you were viewing before sleep
4. **Button Action Ignored**: The wake button press does not trigger an action

### Power Consumption

- **Active Reading**: ~200-400 mW (display refresh uses most power)
- **Idle (Display Static)**: ~100-150 mW
- **Sleep Mode**: ~50-80 mW (CPU active, display off)

**Note:** E-paper displays only consume power during refresh, so static pages use very little energy.

### Sleep Tips

1. **Set Appropriate Timeout**: Balance convenience with battery life
2. **Use Sleep for Long Pauses**: Let the device sleep during breaks
3. **Wake Quickly**: Device wakes instantly, no boot delay
4. **Disable for Reading Sessions**: Set to "Never" for long reading sessions without interruption

---

## Battery Monitoring

If your device includes battery hardware (INA219 HAT or MCP3008 ADC), battery monitoring is available.

### Supported Hardware

#### INA219 Current/Voltage Sensor (I2C)
- **Interface**: I2C bus
- **I2C Address**: 0x40 (default) or 0x41
- **Voltage Range**: 0-26V
- **Current Measurement**: Yes (with shunt resistor)
- **Recommended**: Best accuracy and features

#### MCP3008 ADC (SPI)
- **Interface**: SPI bus
- **Channels**: 8-channel 10-bit ADC
- **Voltage Range**: 0-3.3V (requires voltage divider for higher voltages)
- **Current Measurement**: No (voltage only)
- **Cost**: Very affordable

### Auto-Detection

The e-reader automatically detects battery hardware at startup:

1. **Checks for INA219** on I2C bus (address 0x40, then 0x41)
2. **Checks for MCP3008** on SPI bus (CS=GPIO 8)
3. **Falls back to Dummy Mode** if no hardware detected

### Battery Display

If battery hardware is detected:
- **Status Bar**: Battery icon or percentage may be shown (integration pending)
- **Voltage Reading**: Available via logs or debug interface

### Battery Levels

The system categorizes battery voltage into levels:

- **Full**: > 4.0V (100-90%)
- **Good**: 3.8-4.0V (90-60%)
- **Medium**: 3.6-3.8V (60-30%)
- **Low**: 3.4-3.6V (30-10%)
- **Critical**: < 3.4V (< 10%)

**Note:** These values assume a single-cell Li-Ion/LiPo battery (3.7V nominal). Adjust thresholds in settings for different battery chemistries.

### No Battery Hardware

If no battery hardware is detected:
- **Dummy Mode**: Always reports 100% charge
- **No Warnings**: No low battery warnings
- **USB Power Assumed**: Assumes device is USB-powered

### Battery Safety

1. **Use Protection Circuits**: Always use batteries with built-in protection (PCB/PCM)
2. **Monitor Voltage**: Check battery voltage regularly via logs
3. **Avoid Deep Discharge**: Recharge when voltage drops below 3.5V
4. **Temperature Limits**: Avoid charging/discharging outside 0-45°C
5. **Proper Charger**: Use a Li-Ion/LiPo compatible charging circuit

### Setup Instructions

See `docs/hardware/POWER_SUPPLY.md` for detailed instructions on:
- Wiring INA219 or MCP3008 to Raspberry Pi
- Enabling I2C/SPI interfaces
- Configuring voltage dividers
- Testing battery monitoring

---

## Current Limitations

This is Phase 5 of the project, with enhanced features! Some features are not yet implemented:

### File Format Limitations
- ✅ Supports: Plain text files (`.txt`)
- ✅ Supports: EPUB 2.0/3.0 and PDF (with text extraction)
- ❌ Images are not displayed in any format (text-only rendering)

### Size Limitations
- ✅ Books up to 10 MB are supported
- ❌ Larger books will be rejected with an error message
- For reference: War and Peace (plain text) is ~3.2 MB

### Display Limitations
- ✅ Black and white display (e-paper technology)
- ✅ Three font sizes available (small, medium, large)
- ❌ No grayscale or color support
- ❌ No font family selection (bitmap fonts only)
- ❌ Dark mode not yet implemented (planned)

### Navigation Limitations
- ✅ Sequential page navigation (up/down)
- ✅ Search within book (predefined terms only)
- ❌ No jump to page number
- ❌ No custom text input for search (planned for Phase 6 with WiFi keyboard)
- ❌ No table of contents navigation
- ❌ No visual highlighting of search results on page

### Organization Limitations
- ✅ Alphabetical sorting by filename
- ❌ No folders or collections
- ❌ No metadata (author, title, description)
- ❌ No sorting options (by date, size, etc.)

### Settings Limitations
- ✅ Font size, line spacing, margins adjustable
- ✅ Auto-sleep configurable
- ✅ Settings persist across reboots
- ❌ No display brightness control (e-paper has no backlight)
- ❌ Dark mode not yet implemented

### Power Management Limitations
- ✅ Auto-sleep with configurable timeout
- ✅ Sleep warning before timeout
- ✅ Wake on any button press
- ❌ No manual sleep button
- ❌ No deep sleep mode (CPU remains active)

### Battery Monitoring Limitations
- ✅ Supports INA219 and MCP3008 battery sensors
- ✅ Auto-detection of battery hardware
- ❌ Battery status not yet shown in status bar (integration pending)
- ❌ No low battery warnings yet
- ❌ No battery charge estimation (time remaining)

### Connectivity Limitations
- ❌ No WiFi features yet (planned for Phase 6)
- ❌ No cloud sync or online library
- ❌ No over-the-air book downloads
- ❌ No external keyboard support for custom search

### Other Limitations
- ❌ No dictionary lookup
- ❌ No notes or highlights
- ❌ No reading statistics (time spent, pages read, etc.)
- ❌ No multiple bookmarks per book (only one position saved)
- ❌ No search history or saved queries

These features are planned for future phases! See the project roadmap for details.

---

## Tips for Best Experience

### Optimizing Your Reading

1. **Convert books to plain text**
   - Use Calibre to convert EPUB/PDF to TXT
   - Remove excessive blank lines for better pagination
   - Use UTF-8 encoding for international characters

2. **Format your text files**
   - Use Unix line endings (LF) for smallest file size
   - Avoid tabs (use spaces for indentation)
   - Keep lines under 47 characters for optimal display (or let the device wrap)

3. **Naming conventions**
   - Use descriptive filenames: `Author - Title.txt`
   - Avoid special characters that may not display correctly
   - Keep names under 47 characters to prevent truncation in menu

### Battery Life Tips

1. **Minimize page turns** - E-paper only uses power when refreshing
2. **Power off when not in use** - Unplug the device to save battery
3. **Use a battery pack** - Connect a USB battery pack for portable use
4. **Optimize refresh rate** - Future feature (not yet implemented)

### Display Care

1. **Avoid direct sunlight** - E-paper displays work better in shade
2. **Avoid extreme temperatures** - Keep between 0°C and 40°C
3. **Protect the screen** - Use a cover or case to prevent scratches
4. **Power cycle if frozen** - Unplug and replug if display is stuck

### Text Encoding

- **Best**: UTF-8 encoded text files
- **OK**: ASCII text files
- **Avoid**: Non-UTF-8 encodings (ISO-8859-1, Windows-1252, etc.)

To convert a text file to UTF-8:

```bash
# On Linux/Mac:
iconv -f ISO-8859-1 -t UTF-8 input.txt -o output.txt

# Or use a text editor like VS Code, Notepad++, etc.
```

---

## Troubleshooting

### No Books Showing in Library

**Problem**: Library shows "No books found" even though you added books.

**Solutions**:
1. Verify books are in `/books/` directory (not a subdirectory)
2. Ensure filenames end with `.txt`, `.epub`, or `.pdf` (case-insensitive)
3. Check that files are not hidden (don't start with `.`)
4. Press **BACK** button in empty library screen to rescan
5. Reboot the device if files are still not detected

### Book Won't Open

**Problem**: Selecting a book shows an error or returns to menu.

**Solutions**:
1. **File too large**: Books over 10 MB are rejected
   - Check file size: Right-click → Properties (Windows) or `ls -lh` (Linux)
   - Solution: Split large books into volumes, or convert to more compact format
2. **Corrupt file**: File may be damaged
   - Try opening the file on your computer first
   - Re-copy the file from the original source
3. **Out of memory**: Raspberry Pi Zero W has limited RAM (512 MB)
   - Close and reopen the application
   - Try a smaller book first to test
   - Reboot the device

### Text Displays Incorrectly

**Problem**: Strange characters, boxes, or gibberish instead of readable text.

**Solutions**:
1. **Wrong encoding**: File is not UTF-8 or ASCII
   - Convert file to UTF-8 using `iconv` or a text editor
   - Save as UTF-8 when creating/editing text files
2. **Binary file**: You may have copied a non-text file
   - Verify file contents on your computer
   - Ensure you're copying `.txt` files, not `.doc`, `.docx`, `.rtf`, etc.

### Display Shows Ghosting or Artifacts

**Problem**: Previous pages are faintly visible (ghosting), or screen has artifacts.

**Solutions**:
1. **Normal behavior**: E-paper displays may show slight ghosting
2. **Power cycle**: Unplug and replug the device to force a full refresh
3. **Wait**: Ghosting typically clears after a few page turns
4. Future update will add manual full-refresh option

### Device Won't Boot

**Problem**: Display stays blank, no text appears after 60 seconds.

**Solutions**:
1. **Check power**: Ensure 5V 2A power supply is connected
2. **Check SD card**: Remove and reinsert the SD card
3. **Check wiring**: Verify display is correctly connected (see WIRING_GUIDE.md)
4. **Serial console**: Connect USB-to-TTL adapter to see boot messages
   - Connect to ttyAMA0 at 115200 baud
   - Login: `root` / Password: `ereader`
5. **Reflash SD card**: SD card may be corrupted
   - Re-flash the SD card image
   - See `docs/deployment/FLASHING_GUIDE.md`

### Slow Performance

**Problem**: Buttons are unresponsive, pages take long to turn.

**Solutions**:
1. **E-paper refresh**: Page refreshes take 2-6 seconds (normal)
2. **Large book**: Books near 10 MB may be slower to load
3. **SD card speed**: Use a Class 10 or UHS-1 SD card for better performance
4. **Corrupted SD card**: Try reflashing the SD card image

### Lost Bookmarks

**Problem**: Bookmarks are not saved or disappear.

**Solutions**:
1. **Proper shutdown**: Always press BACK to exit reading view (saves bookmark)
2. **Bookmark file corruption**: Check `/etc/ereader/bookmarks.txt`
   - If corrupted, delete the file and it will be recreated
3. **Filesystem full**: Check available space: `df -h`
   - Delete old books or logs if needed
4. **Improper shutdown**: Unplugging power while reading may lose bookmark
   - Always exit to menu before powering off

### Buttons Not Working

**Problem**: Physical buttons have no effect.

**Solutions**:
1. **Check wiring**: Verify GPIO connections (see WIRING_GUIDE.md)
2. **Serial console**: Check if button events are being detected
   - View logs: `tail -f /var/log/ereader.log`
3. **Button hardware**: Test buttons with a multimeter
4. **Restart**: Reboot the device

---

## Technical Specifications

### Hardware

- **Platform**: Raspberry Pi Zero W
- **CPU**: BCM2835 SoC, ARM1176JZF-S @ 700 MHz (single-core)
- **RAM**: 512 MB LPDDR2
- **Display**: Waveshare 4.2" E-Paper Display (Rev 2.2)
  - Resolution: 400×300 pixels
  - Colors: Black and White (1-bit)
  - Interface: SPI
  - Controller: IL0398 (or compatible)
- **Storage**: MicroSD card (8 GB minimum, ext4 filesystem)
- **Power**: 5V 2A via micro-USB

### Software

- **Operating System**: Custom Buildroot Linux (kernel 6.1 LTS)
- **Application**: Custom C-based e-reader (version 0.3.0)
- **Init System**: SysV init with automatic startup
- **Libraries**: Direct framebuffer rendering (no X11, no GUI)

### Display Specifications

- **Screen Size**: 4.2 inches (diagonal)
- **Resolution**: 400×300 pixels
- **Font Sizes**:
  - Small: 6×12 pixels (~63 chars/line, 21 lines/page)
  - Medium: 8×16 pixels (~47 chars/line, 14 lines/page) - default
  - Large: 10×20 pixels (~38 chars/line, 11 lines/page)
- **Margins**: Adjustable (narrow, normal, wide)
- **Line Spacing**: Adjustable (single, 1.5, double)
- **Status Bar**: Line 1 (title + page indicator + percentage)
- **Separator Lines**: Lines 1 (below status bar) and 16 (above hints)
- **Content Area**: Lines 2-15 (varies with font size and line spacing)
- **Hints Bar**: Line 17

### File Specifications

- **Books Directory**: `/books/` (root of filesystem)
- **Bookmarks File**: `/etc/ereader/bookmarks.txt`
- **Settings File**: `/etc/ereader/settings.conf`
- **Log File**: `/var/log/ereader.log`
- **Maximum Book Size**: 10 MB (10,485,760 bytes)
- **Maximum Books**: 1000 (configurable)
- **Maximum Search Results**: 1000 per search
- **Supported Encodings**: UTF-8, ASCII
- **Line Endings**: LF, CRLF, CR (auto-detected)

### Performance

- **Boot Time**: ~10-30 seconds (from power-on to library menu)
- **Button Response**: < 100 milliseconds
- **Page Refresh**: 2-6 seconds (e-paper refresh limitation)
- **Book Load Time**: < 1 second for books up to 1 MB
- **Memory Usage**: ~50-100 MB for application + loaded book

### Button Mappings

| Button | GPIO Pin | Pull-Up/Down | Active State |
|--------|----------|--------------|--------------|
| UP     | GPIO 17  | Pull-Up      | Active Low   |
| DOWN   | GPIO 27  | Pull-Up      | Active Low   |
| SELECT | GPIO 22  | Pull-Up      | Active Low   |
| BACK   | GPIO 23  | Pull-Up      | Active Low   |
| MENU   | GPIO 24  | Pull-Up      | Active Low   |

See `[[BUTTON_LAYOUT]]` for complete wiring information.
  - "[[FORMAT_SUPPORT]]"

---

## Getting Help

### Documentation

- **Hardware Setup**: `docs/hardware/WIRING_GUIDE.md`
- **Architecture**: `docs/architecture/EREADER_DESIGN.md`
- **Building from Source**: `docs/buildroot/BUILD_PROCESS.md`
- **Progress Logs**: `docs/progress/PHASE_03_LOG.md` (coming soon)

### Debugging

- **Serial Console**: Connect USB-to-TTL to GPIO 14/15 at 115200 baud
- **Application Logs**: `/var/log/ereader.log`
- **System Logs**: `dmesg` (kernel messages)
- **Process Status**: `ps aux | grep ereader`

### Community

- **Project Repository**: (Add GitHub link here)
- **Issue Tracker**: (Add GitHub Issues link here)
- **Discussions**: (Add GitHub Discussions link here)

---

## What's Next?

This is Phase 5 of the e-reader project. Completed phases:

- ✅ **Phase 1-2**: Hardware setup, display drivers, button input
- ✅ **Phase 3**: Text rendering, bookmarks, library management
- ✅ **Phase 4**: EPUB and PDF support with metadata extraction
- ✅ **Phase 5**: Settings system, font sizes, search, power management, battery monitoring

Future phases will add:
- **Phase 6**: WiFi features, external keyboard, custom search, online library sync
- **Phase 7**: Final polish, optimization, release preparation

See the project roadmap in the main repository for details.

---

**Enjoy your reading!** 📖

This e-reader is an open-source project built with love for book enthusiasts and tinkerers. If you encounter issues or have suggestions, please contribute back to the project.

---

*Last updated: 2026-01-16 (Phase 05 - Polish and Advanced Features)*
