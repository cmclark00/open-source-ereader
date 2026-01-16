# Phase 06: WiFi and Network Features

This phase unlocks the Raspberry Pi Zero W's WiFi capabilities, enabling book downloads, online libraries, and network management. By the end of this phase, users can download books directly to the device without removing the SD card.

## Tasks

- [x] Enable WiFi in Buildroot and kernel:
  - ✅ Updated `configs/ereader_rpi0w_defconfig` to include:
    - BR2_PACKAGE_RPI_WIFI_FIRMWARE=y (Broadcom WiFi firmware for brcmfmac43430)
    - BR2_PACKAGE_WPA_SUPPLICANT=y with CLI and passphrase support
    - BR2_PACKAGE_WIRELESS_TOOLS=y and BR2_PACKAGE_IW=y for WiFi management
    - BR2_PACKAGE_DHCPCD=y for DHCP client
    - BR2_PACKAGE_BIND=y for DNS resolution
  - ✅ Updated `configs/linux_ereader.fragment` with:
    - CONFIG_BRCMFMAC=y and CONFIG_BRCMFMAC_SDIO=y (Broadcom WiFi driver)
    - CONFIG_WIRELESS=y, CONFIG_CFG80211=y, CONFIG_MAC80211=y (wireless subsystem)
    - CONFIG_MMC=y and CONFIG_MMC_BCM2835=y (SDIO bus support)
    - Cryptographic support for WPA/WPA2 (AES, CCM, GCM, CMAC)
    - Network stack requirements (PACKET, UNIX, INET)
  - ✅ Created comprehensive `docs/hardware/WIFI_SETUP.md` documenting:
    - Hardware overview and antenna considerations
    - Firmware requirements and verification
    - Kernel driver configuration details
    - Userspace components (wpa_supplicant, wireless tools, DHCP)
    - Network interface management procedures
    - Configuration file formats and locations
    - First-time setup options (pre-configure, serial console, wpa_passphrase)
    - Troubleshooting guide for common WiFi issues
    - Performance characteristics and security considerations

- [x] Add firmware and configuration files:
  - ✅ Firmware automatically included via BR2_PACKAGE_RPI_WIFI_FIRMWARE=y in defconfig
  - ✅ Created `board/ereader/rootfs-overlay/etc/wpa_supplicant.conf.example`:
    - Comprehensive template with examples for WPA2-PSK, open networks, hidden SSIDs
    - Multiple network configuration examples with priorities
    - Detailed comments explaining all options and security modes
    - Instructions for using wpa_passphrase for encrypted PSK
  - ✅ Created `board/ereader/rootfs-overlay/etc/init.d/S40network`:
    - Automatically starts wpa_supplicant and dhcpcd on boot
    - Smart startup logic: checks for config file and skips if not configured
    - Comprehensive WiFi management: start, stop, restart, status, scan
    - Connection monitoring with timeout and detailed logging
    - Status command shows: interface state, connection info, IP address, signal strength
    - Scan command lists available networks with SSIDs and signal levels
    - Logs to /var/log/network.log for troubleshooting
  - ✅ Updated `board/ereader/post-build.sh` to make S40network executable
  - ✅ Documented first-time WiFi setup in `docs/USER_GUIDE.md`:
    - Added comprehensive WiFi Setup section with TOC entry
    - Two setup methods: pre-configure via SD card (recommended) and serial console
    - WiFi configuration file format and examples
    - Multiple network configuration with priorities
    - WiFi status checking and service management commands
    - Battery life considerations and performance characteristics
    - Extensive troubleshooting section (connection, weak signal, DHCP issues)
    - Hidden network configuration and security mode details
    - Cross-reference to hardware/WIFI_SETUP.md for technical details

- [ ] Design WiFi management UI:
  - Create `docs/architecture/WIFI_UI.md` documenting:
    - WiFi menu: scan networks, connect, disconnect, status
    - Network list display (SSID, signal strength, security)
    - Text input system for entering WiFi passwords (challenging with 5 buttons!)
    - Consider: pre-configured via SD card config file vs on-device entry
  - Plan for simple password entry (A-Z navigation, slow but functional)

- [ ] Implement WiFi scanning and connection:
  - Create `src/ereader/network/wifi_manager.c` with functions:
    - Scan for available networks (parse `iw dev wlan0 scan` output)
    - Connect to network (write wpa_supplicant.conf, restart wpa_supplicant)
    - Check connection status (parse `iw dev wlan0 link`)
    - Disconnect from network
    - Read current network SSID and signal strength
  - Write `src/ereader/network/wifi_manager.h`
  - Handle errors: no networks found, wrong password, out of range

- [ ] Build WiFi settings UI:
  - Create `src/ereader/ui/wifi_menu.c` with:
    - Network list display (scrollable)
    - Connect dialog (password entry)
    - Status screen (connected to X, IP address, signal strength)
    - Disconnect option
  - Write `src/ereader/ui/wifi_menu.h`
  - Add WiFi option to Settings menu
  - Show WiFi status icon in status bar (connected/disconnected)

- [ ] Implement simple text input system:
  - Create `src/ereader/ui/text_input.c` for entering WiFi passwords:
    - Display alphabet grid (A-Z, 0-9, symbols)
    - Navigate with UP/DOWN/SELECT buttons
    - Backspace, space, done buttons
    - Display entered text at top
  - Write `src/ereader/ui/text_input.h`
  - Test with WiFi password entry
  - Note: This is slow but functional for occasional use

- [ ] Add online book download feature:
  - Research public domain book sources:
    - Project Gutenberg FTP/HTTP access
    - Internet Archive e-book collection
    - Other open APIs for book downloads
  - Create `docs/research/BOOK_SOURCES.md` with available sources
  - Decide on initial implementation (pre-configured bookstore vs manual URL entry)

- [ ] Implement book download manager:
  - Create `src/ereader/network/download_manager.c` with:
    - HTTP/HTTPS download function (use libcurl or wget)
    - Progress tracking (bytes downloaded / total size)
    - Save to /books/ directory with proper filename
    - Validate downloaded file (check format, not corrupt)
  - Write `src/ereader/network/download_manager.h`
  - Add libcurl or wget to Buildroot packages
  - Handle errors: network timeout, disk full, invalid file

- [ ] Create online library browser UI:
  - Create `src/ereader/ui/library_browser.c` with:
    - Browse pre-configured book catalogs (Project Gutenberg popular books)
    - Search by title or author (if API supports)
    - Display book info (title, author, size, format)
    - Download button with progress indicator
    - Navigate to "Downloads" folder after completion
  - Write `src/ereader/ui/library_browser.h`
  - Add "Library" option to main menu
  - Consider: cache catalog locally to reduce network requests

- [ ] Add time synchronization and updates:
  - Add ntpd or chrony to Buildroot for time sync via NTP
  - Create `src/ereader/network/time_sync.c` to trigger sync when WiFi connects
  - Consider OTA updates:
    - Design update mechanism (download new rootfs, apply on reboot)
    - Security considerations (signed updates)
    - Document in `docs/architecture/OTA_UPDATES.md`
  - Or: defer OTA updates to Phase 7, just do time sync in Phase 6

- [ ] Build and test WiFi features:
  - Rebuild Buildroot with WiFi support
  - Test WiFi scanning, connection, disconnection
  - Test book download from Project Gutenberg
  - Create `docs/testing/WIFI_TESTING.md` with test procedures
  - Document network performance (download speeds, latency)

- [ ] Update documentation for WiFi features:
  - Update `docs/USER_GUIDE.md` with:
    - How to set up WiFi for the first time
    - How to browse and download books
    - How to manage WiFi connections
    - Troubleshooting WiFi issues
  - Update `docs/hardware/WIFI_SETUP.md` with antenna considerations
  - Update `docs/progress/PHASE_06_LOG.md` with completion notes
  - Create `docs/progress/PHASE_07_PLANNING.md` for final polish and release prep
