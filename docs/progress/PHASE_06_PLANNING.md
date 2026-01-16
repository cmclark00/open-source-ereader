---
type: planning
title: Phase 06 Planning - WiFi and Network Features
created: 2026-01-16
tags:
  - phase-06
  - planning
  - wifi
  - network
related:
  - "[[PHASE_05_LOG]]"
  - "[[EREADER_DESIGN]]"
  - "[[USER_GUIDE]]"
---

# Phase 06: WiFi and Network Features - Planning Document

**Phase Start Date**: TBD
**Estimated Duration**: 3-4 weeks
**Status**: 📋 Planning
**Dependencies**: Phase 05 completion, hardware WiFi testing

---

## Overview

Phase 06 focuses on leveraging the Raspberry Pi Zero W's built-in WiFi capabilities to add network features, external input support, and remote management. This phase transforms the e-reader from a standalone device into a connected device capable of downloading books, syncing data, and accepting external input.

---

## Goals

### Primary Goals
1. **WiFi Configuration and Management**: Enable users to connect the device to WiFi networks
2. **External Keyboard Support**: Allow USB/Bluetooth keyboards for custom text input
3. **Online Book Downloads**: Download books from Project Gutenberg and other sources
4. **Web-Based Management**: Remote book management via web browser
5. **Cloud Sync**: Sync reading positions and bookmarks across devices

### Secondary Goals
1. **Over-the-Air Updates**: Software update mechanism via WiFi
2. **Network Time Sync**: Automatic time synchronization via NTP
3. **Remote Debugging**: SSH access for developers
4. **WiFi Status Display**: Show connection status in UI

---

## Features and Tasks

### 1. WiFi Configuration System

**Goal**: Enable users to connect to WiFi networks without SSH access.

**Implementation**:
- Create WiFi configuration UI (scan, select, enter password)
- Integrate with wpa_supplicant for network management
- Store WiFi credentials securely in `/etc/wpa_supplicant/wpa_supplicant.conf`
- Display WiFi status (connected, disconnected, signal strength) in UI
- Support WiFi AP mode for initial setup (phone connects to e-reader)

**Components**:
- `src/ereader/network/wifi_manager.c/h` - WiFi configuration and status
- `src/ereader/ui/wifi_menu.c/h` - WiFi configuration UI
- `src/ereader/network/wifi_scan.c/h` - Network scanning

**UI Workflow**:
```
Main Menu → Settings → WiFi Settings
  → Scan Networks
  → Select Network
  → Enter Password (via keyboard)
  → Connect
  → Show Status
```

**Tasks**:
- [ ] Implement WiFi scanning (use `wpa_cli` or direct D-Bus interface)
- [ ] Create WiFi network list UI (scrollable, signal strength indicators)
- [ ] Implement password entry screen (requires keyboard support)
- [ ] Integrate with wpa_supplicant for connection management
- [ ] Add WiFi status to status bar (icon, SSID, signal strength)
- [ ] Support WiFi AP mode for initial setup (optional, advanced)
- [ ] Handle WiFi errors gracefully (wrong password, no signal, etc.)
- [ ] Test on various WiFi networks (WPA2, WPA3, open networks)

**Estimated Effort**: 5-7 days

---

### 2. External Keyboard Support

**Goal**: Enable custom text input via USB or Bluetooth keyboards.

**Implementation**:
- Abstract keyboard input layer (unified API for physical buttons and keyboard)
- USB keyboard support via `/dev/input/event*` (already partially supported)
- Bluetooth keyboard pairing and connection
- Text input UI component (text field with cursor)
- Integration with search, WiFi password entry, URL entry

**Components**:
- `src/ereader/input/keyboard_manager.c/h` - Keyboard abstraction layer
- `src/ereader/ui/text_input.c/h` - Text input UI component
- `src/ereader/input/bluetooth_keyboard.c/h` - Bluetooth keyboard support

**Input Abstraction**:
```c
typedef enum {
    INPUT_SOURCE_BUTTONS,      // Physical GPIO buttons
    INPUT_SOURCE_USB_KEYBOARD, // USB keyboard via OTG
    INPUT_SOURCE_BT_KEYBOARD   // Bluetooth keyboard
} input_source_t;

typedef struct {
    input_source_t source;
    char character;            // For keyboard input
    button_t button;           // For button input
    bool is_special_key;       // Backspace, Enter, etc.
} input_event_t;
```

**Tasks**:
- [ ] Create keyboard input abstraction layer
- [ ] Implement USB keyboard support (OTG, `/dev/input/event*`)
- [ ] Implement Bluetooth keyboard pairing UI
- [ ] Create text input UI component (text field, cursor, virtual keyboard optional)
- [ ] Integrate with search (custom search terms)
- [ ] Integrate with WiFi setup (password entry)
- [ ] Handle special keys (Backspace, Enter, Arrow keys, etc.)
- [ ] Test with various keyboards (layouts, special characters)

**Estimated Effort**: 4-6 days

---

### 3. Online Book Download System

**Goal**: Download books directly from online sources without removing SD card.

**Implementation**:
- Integration with Project Gutenberg API
- Book search and browse interface
- Book preview (metadata, description, cover)
- Download progress indicator
- Save to `/books/` directory

**Components**:
- `src/ereader/network/book_downloader.c/h` - HTTP download with progress
- `src/ereader/network/gutenberg_api.c/h` - Project Gutenberg integration
- `src/ereader/ui/download_menu.c/h` - Book search and download UI

**API Integration**:
- **Project Gutenberg**: https://www.gutenberg.org/ebooks/search/
  - Search by title, author, subject
  - Get book metadata (title, author, year, language)
  - Download in multiple formats (TXT, EPUB, PDF)

**UI Workflow**:
```
Main Menu → Download Books
  → Search by Title/Author
  → Browse Results (scrollable list)
  → View Book Details (metadata, description)
  → Select Format (TXT, EPUB, PDF)
  → Download (with progress bar)
  → Open Book or Return to Library
```

**Tasks**:
- [ ] Implement HTTP/HTTPS client (use libcurl or lightweight alternative)
- [ ] Integrate with Project Gutenberg API (search, metadata, download)
- [ ] Create book search UI (text input via keyboard)
- [ ] Create book preview UI (metadata display)
- [ ] Implement download with progress bar
- [ ] Handle download errors (network failure, disk full, timeout)
- [ ] Add downloaded books to library automatically
- [ ] Support resumable downloads (optional, advanced)
- [ ] Test with various book formats and sizes

**Estimated Effort**: 6-8 days

---

### 4. Web-Based Management Interface

**Goal**: Manage books and settings via web browser on phone/computer.

**Implementation**:
- Lightweight embedded web server (e.g., mongoose, libmicrohttpd)
- Web interface for book upload, management, and settings
- RESTful API for programmatic access
- Authentication for security (basic auth or token)

**Components**:
- `src/ereader/network/web_server.c/h` - Embedded HTTP server
- `src/ereader/network/web_api.c/h` - RESTful API handlers
- `web/index.html` - Web interface frontend (HTML/CSS/JS)

**Web Interface Features**:
- **Book Management**:
  - Upload books (drag-and-drop or file picker)
  - Delete books
  - View library
  - Download books from device
- **Settings**:
  - View and edit all settings
  - Apply changes remotely
- **Reading Status**:
  - View current book and reading position
  - View reading statistics (future)
- **Device Info**:
  - Battery level
  - Storage space
  - WiFi status
  - Software version

**API Endpoints**:
```
GET  /api/books              # List all books
POST /api/books              # Upload book
GET  /api/books/{id}         # Get book details
DELETE /api/books/{id}       # Delete book
GET  /api/settings           # Get all settings
PUT  /api/settings           # Update settings
GET  /api/status             # Device status (battery, WiFi, etc.)
POST /api/restart            # Restart e-reader app
```

**Security**:
- Basic authentication (username/password)
- HTTPS (optional, requires SSL certificate)
- Rate limiting to prevent abuse
- Only accessible on local network (not exposed to internet)

**Tasks**:
- [ ] Integrate embedded web server (mongoose, libmicrohttpd, or similar)
- [ ] Implement RESTful API endpoints
- [ ] Create web interface frontend (HTML/CSS/JavaScript)
- [ ] Implement book upload (multipart form data)
- [ ] Implement book download from device
- [ ] Add authentication (basic auth)
- [ ] Test on various browsers (mobile and desktop)
- [ ] Document API for developers
- [ ] Add web server toggle in settings (enable/disable)

**Estimated Effort**: 7-10 days

---

### 5. Cloud Sync (Reading Positions and Bookmarks)

**Goal**: Sync reading positions across multiple devices.

**Implementation**:
- Cloud sync service (self-hosted or third-party)
- Sync reading positions (book, page, timestamp)
- Sync bookmarks (all positions for all books)
- Sync settings (optional)
- Conflict resolution (last-write-wins or user choice)

**Components**:
- `src/ereader/network/sync_client.c/h` - Cloud sync client
- `src/ereader/ui/sync_menu.c/h` - Sync status and manual sync UI

**Sync Service Options**:
1. **Self-Hosted** (Recommended for privacy):
   - Simple HTTP server with authentication
   - JSON-based sync protocol
   - User runs on personal server or Raspberry Pi
2. **Third-Party** (Easier but less private):
   - Dropbox, Google Drive, or similar
   - OAuth authentication
   - Requires cloud account

**Sync Protocol**:
```json
{
  "device_id": "ereader-001",
  "last_sync": 1736835600,
  "bookmarks": [
    {
      "book": "Frankenstein.txt",
      "page": 42,
      "percentage": 23,
      "timestamp": 1736835600
    }
  ],
  "settings": {
    "font_size": "medium",
    "line_spacing": "single"
  }
}
```

**Sync Workflow**:
1. **Auto-Sync**: Sync on WiFi connect (configurable)
2. **Manual Sync**: Sync button in settings
3. **Conflict Resolution**: If both devices have changes, use most recent timestamp

**Tasks**:
- [ ] Design sync protocol (JSON-based, RESTful)
- [ ] Implement sync client (HTTP POST/GET)
- [ ] Create sync service reference implementation (Python/Node.js)
- [ ] Add sync settings (enable/disable, auto-sync, manual)
- [ ] Implement conflict resolution (last-write-wins)
- [ ] Add sync status to UI (last sync time, sync in progress)
- [ ] Test with multiple devices
- [ ] Document sync protocol for third-party implementations
- [ ] Support OAuth for third-party services (optional, advanced)

**Estimated Effort**: 5-7 days

---

### 6. Over-the-Air Software Updates

**Goal**: Update e-reader software via WiFi without SD card removal.

**Implementation**:
- Check for updates on remote server
- Download update package (compressed image or delta)
- Verify integrity (checksum, signature)
- Apply update (A/B partitioning or in-place update)
- Reboot to new version

**Components**:
- `src/ereader/network/update_manager.c/h` - Update check and download
- `src/ereader/ui/update_menu.c/h` - Update UI (check, download, apply)

**Update Process**:
1. **Check for Updates**: Query update server for latest version
2. **Download**: Download update package with progress bar
3. **Verify**: Check SHA256 hash and signature (optional)
4. **Apply**: Extract and install (requires root access)
5. **Reboot**: Restart device to new version
6. **Rollback**: If update fails, revert to previous version (A/B partitioning)

**Update Server**:
- Simple HTTP server with version manifest
- Manifest format:
  ```json
  {
    "version": "0.6.0",
    "release_date": "2026-02-15",
    "download_url": "https://example.com/ereader-0.6.0.img.gz",
    "sha256": "abc123...",
    "changelog": "Phase 06: WiFi and network features"
  }
  ```

**Security**:
- HTTPS for downloads
- SHA256 checksum verification
- Optional: GPG signature verification
- Rollback mechanism if update fails

**Tasks**:
- [ ] Implement update check (query remote server)
- [ ] Implement update download with progress
- [ ] Implement checksum verification
- [ ] Create update application script (install new version)
- [ ] Add update UI (check, download, apply)
- [ ] Implement A/B partitioning for rollback (advanced)
- [ ] Test update process on device
- [ ] Create update server reference implementation
- [ ] Document update process for developers

**Estimated Effort**: 6-8 days

---

### 7. Network Time Synchronization

**Goal**: Automatically set device time via NTP.

**Implementation**:
- NTP client to sync time on boot and WiFi connect
- Configure timezone
- Display correct time in UI (optional)

**Components**:
- Integration with system NTP client (`ntpd`, `chronyd`, or `busybox ntpd`)
- Settings for NTP server (default: pool.ntp.org)

**Tasks**:
- [ ] Configure NTP client in Buildroot
- [ ] Enable auto-sync on boot and WiFi connect
- [ ] Add timezone configuration to settings
- [ ] Test time sync on various networks
- [ ] Handle time sync failures gracefully

**Estimated Effort**: 1-2 days

---

### 8. Additional UI Enhancements

**Goal**: Improve UI to display network-related information.

**UI Elements**:
- **WiFi Icon**: Show connection status and signal strength in status bar
- **Download Progress**: Full-screen progress for book downloads
- **Sync Status**: Indicator for cloud sync in progress
- **Network Error Messages**: Clear error messages for network failures

**Tasks**:
- [ ] Add WiFi icon to status bar (connected, signal strength)
- [ ] Create download progress screen (with cancel option)
- [ ] Add sync indicator (spinning icon when syncing)
- [ ] Enhance error messages for network issues
- [ ] Test on e-paper display (clear visibility)

**Estimated Effort**: 2-3 days

---

## Technical Considerations

### WiFi Power Management
- **Challenge**: WiFi consumes significant power on Pi Zero W
- **Solution**: Disable WiFi when not in use (configurable in settings)
- **Implementation**: Use `rfkill` or `ifconfig` to enable/disable WiFi

### Network Security
- **Challenge**: E-reader will be on user's WiFi network
- **Solution**:
  - Use WPA2/WPA3 for WiFi
  - Implement authentication for web interface
  - Use HTTPS for sensitive data (book downloads, sync)
  - Rate limiting to prevent abuse

### Performance Impact
- **Challenge**: Network operations are slow on Pi Zero W (single-core, 700 MHz)
- **Solution**:
  - Asynchronous downloads (don't block UI)
  - Progress indicators for all network operations
  - Timeout handling (fail gracefully if network slow)
  - Cache frequently accessed data

### Storage Constraints
- **Challenge**: SD card space limited (8 GB typical)
- **Solution**:
  - Display available storage before download
  - Implement "delete after reading" option (auto-cleanup)
  - Warn user if storage low (<100 MB free)
  - Support streaming for large books (future)

### Dependency Management
- **New Dependencies**:
  - `libcurl` or `wget` - HTTP/HTTPS client
  - `wpa_supplicant` - WiFi management (already in Buildroot)
  - `mongoose` or `libmicrohttpd` - Embedded web server
  - `jansson` or `cJSON` - JSON parsing
  - `openssl` or `mbedtls` - HTTPS and authentication

---

## Integration with Phase 05 Features

### Custom Search Terms (via Keyboard)
- Replace predefined search terms with text input field
- Use external keyboard for entry
- Store recent searches (history)

### WiFi Password Entry (via Keyboard)
- Text input field for WPA password
- Mask password characters (show dots or asterisks)
- Option to show password (toggle)

### Settings via Web Interface
- Mirror all settings from Settings Menu to web interface
- Allow remote configuration
- Apply settings immediately (no reboot required)

---

## Phase 06 Deliverables

### Source Code
1. **WiFi Management**:
   - `src/ereader/network/wifi_manager.c/h`
   - `src/ereader/ui/wifi_menu.c/h`
2. **Keyboard Support**:
   - `src/ereader/input/keyboard_manager.c/h`
   - `src/ereader/ui/text_input.c/h`
3. **Book Downloads**:
   - `src/ereader/network/book_downloader.c/h`
   - `src/ereader/network/gutenberg_api.c/h`
   - `src/ereader/ui/download_menu.c/h`
4. **Web Interface**:
   - `src/ereader/network/web_server.c/h`
   - `web/index.html`, `web/style.css`, `web/app.js`
5. **Cloud Sync**:
   - `src/ereader/network/sync_client.c/h`
   - `src/ereader/ui/sync_menu.c/h`
6. **OTA Updates**:
   - `src/ereader/network/update_manager.c/h`
   - `src/ereader/ui/update_menu.c/h`

### Documentation
1. **User Guide Updates**:
   - WiFi setup instructions
   - Using external keyboard
   - Downloading books
   - Web interface guide
   - Cloud sync setup
2. **Developer Documentation**:
   - WiFi API reference
   - Web server API documentation
   - Sync protocol specification
   - Update mechanism guide
3. **Integration Guides**:
   - WiFi integration with main.c
   - Keyboard integration
   - Web server setup

### Testing
1. **WiFi Testing**: Various networks, security types, signal strengths
2. **Keyboard Testing**: USB and Bluetooth keyboards, various layouts
3. **Download Testing**: Large files, network failures, timeout handling
4. **Web Interface Testing**: Multiple browsers, mobile and desktop
5. **Sync Testing**: Multiple devices, conflict resolution
6. **Security Testing**: Authentication, rate limiting, input validation

---

## Success Metrics

### Functional Requirements
- [ ] WiFi connection works on at least 90% of common networks (WPA2/WPA3)
- [ ] External keyboard allows full text input (letters, numbers, symbols)
- [ ] Can download books from Project Gutenberg in <30 seconds (1MB book)
- [ ] Web interface accessible from any device on local network
- [ ] Cloud sync completes in <10 seconds (typical bookmark set)
- [ ] OTA updates complete successfully without bricking device

### User Experience
- [ ] WiFi setup takes <2 minutes for first-time user
- [ ] Keyboard feels responsive (<100ms input latency)
- [ ] Download progress is visible and accurate
- [ ] Web interface is intuitive (no instructions needed)
- [ ] Sync happens automatically and transparently

### Performance
- [ ] WiFi scan completes in <10 seconds
- [ ] WiFi connection established in <30 seconds
- [ ] Book download at 100+ KB/s (depends on network)
- [ ] Web interface responsive (<500ms page load)
- [ ] Sync doesn't block UI (asynchronous)

---

## Risk Assessment

### High Risk
1. **WiFi Driver Instability**: Pi Zero W WiFi can be unreliable
   - **Mitigation**: Test extensively, implement reconnect logic
2. **Network Performance**: Slow downloads on 2.4 GHz WiFi
   - **Mitigation**: Progress indicators, asynchronous operations
3. **Security Vulnerabilities**: Web server and cloud sync are attack surfaces
   - **Mitigation**: Authentication, rate limiting, input validation

### Medium Risk
1. **Bluetooth Keyboard Pairing**: Pairing can be complex for users
   - **Mitigation**: Clear pairing instructions, fallback to USB
2. **Storage Space**: Users may run out of space with downloads
   - **Mitigation**: Storage warnings, auto-cleanup options
3. **Update Failures**: OTA updates could brick device
   - **Mitigation**: A/B partitioning, rollback mechanism

### Low Risk
1. **API Changes**: Project Gutenberg API may change
   - **Mitigation**: Version API requests, graceful degradation
2. **Time Sync Failures**: NTP may be blocked on some networks
   - **Mitigation**: Fallback to manual time setting

---

## Timeline Estimate

### Week 1: WiFi and Keyboard Foundation
- Days 1-3: WiFi manager and UI
- Days 4-5: Keyboard abstraction and USB support
- Days 6-7: Text input UI component

### Week 2: Downloads and Web Server
- Days 8-10: Book downloader and Gutenberg API
- Days 11-12: Download UI
- Days 13-14: Embedded web server setup

### Week 3: Web Interface and Sync
- Days 15-17: Web interface (frontend and API)
- Days 18-20: Cloud sync client
- Day 21: Sync UI

### Week 4: Updates and Polish
- Days 22-24: OTA update system
- Days 25-26: UI polish and status indicators
- Days 27-28: Integration testing and bug fixes

**Total Estimated Time**: 4 weeks (28 days)

---

## Dependencies and Prerequisites

### Hardware
- Raspberry Pi Zero W (WiFi capability)
- USB OTG cable (for USB keyboard)
- Bluetooth keyboard (for Bluetooth testing)
- Test WiFi networks (WPA2, WPA3)

### Software
- Phase 05 integration complete (settings, search, power management)
- Buildroot updates (add libcurl, web server, JSON library)
- SSH access for debugging
- Test book collection for downloads

### Testing Environment
- Multiple WiFi networks (home, public, enterprise)
- Various keyboards (USB, Bluetooth, different layouts)
- Multiple browsers (Chrome, Firefox, Safari, mobile)
- Multiple e-reader devices (for sync testing)

---

## Next Steps

1. **Review Phase 05 Integration**: Ensure all Phase 05 features are integrated into main.c before starting Phase 06
2. **Hardware Acquisition**: Obtain USB OTG cable and Bluetooth keyboard for testing
3. **Buildroot Configuration**: Add required dependencies (libcurl, web server, JSON library)
4. **Prototype WiFi UI**: Create mockups for WiFi configuration screens
5. **Research APIs**: Investigate Project Gutenberg API and cloud sync options
6. **Security Planning**: Define authentication and security requirements

---

## Open Questions

1. **Web Server Choice**: Mongoose vs. libmicrohttpd vs. custom lightweight server?
2. **Sync Service**: Self-hosted vs. third-party (Dropbox, etc.)?
3. **Update Mechanism**: A/B partitioning (complex) vs. in-place update (simpler)?
4. **Bluetooth Stack**: Use Bluez (heavy) or lighter alternative?
5. **HTTPS Support**: Required or optional (adds complexity and certificate management)?

---

*Planning document created: 2026-01-16*
*Phase 06 start date: TBD (after Phase 05 hardware integration testing)*
