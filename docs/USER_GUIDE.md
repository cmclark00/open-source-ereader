---
type: reference
title: E-Reader User Guide
created: 2026-01-14
tags:
  - user-guide
  - documentation
  - phase-04
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
7. [Current Limitations](#current-limitations)
8. [Tips for Best Experience](#tips-for-best-experience)
9. [Troubleshooting](#troubleshooting)
10. [Technical Specifications](#technical-specifications)

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
| **MENU** | (Reserved for future reading options) |

**Reading Tips:**
- At the first page, pressing UP has no effect
- At the last page, pressing DOWN has no effect
- Your reading position is automatically saved when you return to the library
- The status bar shows: `Book Title [Current Page/Total Pages]`

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

- **When you leave a book** (press BACK to return to library)
- Your position is saved to `/etc/ereader/bookmarks.txt`
- Bookmarks persist across reboots

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

## Current Limitations

This is Phase 4 of the project, with multi-format support! Some features are not yet implemented:

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
- ❌ No grayscale or color support
- ❌ No font size adjustment (currently 8x16 fixed font)
- ❌ No font family selection

### Navigation Limitations
- ✅ Sequential page navigation (up/down)
- ❌ No jump to page number
- ❌ No search within book
- ❌ No table of contents navigation

### Organization Limitations
- ✅ Alphabetical sorting by filename
- ❌ No folders or collections
- ❌ No metadata (author, title, description)
- ❌ No sorting options (by date, size, etc.)

### Connectivity Limitations
- ❌ No WiFi features yet (planned for Phase 6)
- ❌ No cloud sync or online library
- ❌ No over-the-air book downloads

### Other Limitations
- ❌ No dictionary lookup
- ❌ No notes or highlights
- ❌ No reading statistics
- ❌ No multiple bookmarks per book

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
- **Font**: 8×16 pixel bitmap font
- **Characters Per Line**: 47 characters
- **Lines Per Page**: 14 lines (visible area)
- **Margins**: Top: 20px, Bottom: 20px, Left: 10px, Right: 10px
- **Line Spacing**: 2 pixels
- **Status Bar**: Line 1 (title + page indicator)
- **Separator Lines**: Lines 1 (below status bar) and 16 (above hints)
- **Content Area**: Lines 2-15 (14 lines)
- **Hints Bar**: Line 17

### File Specifications

- **Books Directory**: `/books/` (root of filesystem)
- **Bookmarks File**: `/etc/ereader/bookmarks.txt`
- **Log File**: `/var/log/ereader.log`
- **Maximum Book Size**: 10 MB (10,485,760 bytes)
- **Maximum Books**: 1000 (configurable)
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

This is Phase 4 of the e-reader project. Completed phases:

- ✅ **Phase 1-2**: Hardware setup, display drivers, button input
- ✅ **Phase 3**: Text rendering, bookmarks, library management
- ✅ **Phase 4**: EPUB and PDF support with metadata extraction

Future phases will add:
- **Phase 5**: Enhanced UI, font selection, margins, reading statistics
- **Phase 6**: WiFi features, online library sync, web interface
- **Phase 7**: Final polish, documentation, release preparation

See the project roadmap in the main repository for details.

---

**Enjoy your reading!** 📖

This e-reader is an open-source project built with love for book enthusiasts and tinkerers. If you encounter issues or have suggestions, please contribute back to the project.

---

*Last updated: 2026-01-14 (Phase 04 - Multi-format support)*
