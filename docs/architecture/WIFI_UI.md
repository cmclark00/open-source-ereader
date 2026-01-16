---
type: architecture
title: WiFi Management UI Design
created: 2026-01-16
tags:
  - architecture
  - wifi
  - ui
  - network
  - phase-06
related:
  - "[[EREADER_DESIGN]]"
  - "[[INPUT_SYSTEM]]"
  - "[[WIFI_SETUP]]"
  - "[[USER_GUIDE]]"
---

# WiFi Management UI Design

This document describes the complete user interface design for WiFi management on the e-reader. It covers network scanning, connection management, password entry with a 5-button interface, and all screen layouts for the WiFi features in Phase 06.

## Overview

The WiFi management system provides users with a way to:
- Scan for available WiFi networks
- View network details (SSID, signal strength, security type)
- Connect to networks (with password entry)
- Disconnect from networks
- View connection status
- Manage saved networks

### Design Constraints

The e-reader has unique constraints that shape the UI design:

1. **5-Button Input**: UP, DOWN, SELECT, BACK, MENU (no touchscreen, no keyboard)
2. **E-Paper Display**: 400×300 pixels, black/white, ~5 second refresh
3. **Limited Memory**: Must be efficient with resources
4. **Battery Life**: Minimize WiFi usage, provide quick connection/disconnection
5. **User Patience**: Text entry is slow with 5 buttons

### Design Philosophy

**Primary Strategy**: Pre-configure WiFi credentials via SD card config file before first boot (recommended).

**Secondary Strategy**: On-device password entry using a character selection grid (functional but slow).

This two-pronged approach balances convenience (for users with computer access) with flexibility (for users who need to configure on-device).

## Application State Machine Extensions

### WiFi States

The WiFi UI adds new states to the existing e-reader state machine:

```
                    ┌──────────────┐
                    │ MENU_LIBRARY │
                    │   or READING │
                    └──────┬───────┘
                           │
                      MENU │
                           ▼
              ┌────────────────────────┐
              │   MENU_SETTINGS        │
              │  (system settings)     │
              └─────┬──────────────────┘
                    │
        SELECT WiFi │
                    ▼
         ┌─────────────────┐
         │  WIFI_MAIN      │  (status overview)
         └────┬────────┬───┘
              │        │
     SELECT   │        │ BACK
     Scan     │        │
              ▼        │
         ┌─────────────────┐
         │  WIFI_SCAN      │  (scanning networks)
         └────┬────────────┘
              │
              ▼
         ┌─────────────────┐
         │  WIFI_LIST      │  (network list)
         └────┬────────────┘
              │
     SELECT   │
     Network  │
              ▼
         ┌─────────────────┐
         │  WIFI_CONNECT   │  (connection dialog)
         └────┬────────────┘
              │
    Need PWD? │
              ▼
         ┌─────────────────┐
         │  WIFI_PASSWORD  │  (text input)
         └────┬────────────┘
              │
              ▼
         ┌─────────────────┐
         │  WIFI_CONNECTING│  (progress)
         └────┬───┬────────┘
              │   │
      Success │   │ Failed
              │   │
              ▼   ▼
         ┌─────────────────┐
         │  WIFI_RESULT    │  (success/error)
         └─────────────────┘
```

### State Definitions

#### STATE_WIFI_MAIN

**Purpose**: Display WiFi status overview and main menu options.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ WiFi Settings                      │
├────────────────────────────────────┤
│ Status: Connected                  │
│   Network: MyHomeWiFi              │
│   Signal: ████░░░░ (50%)           │
│   IP: 192.168.1.42                 │
│                                    │
│ > Scan for Networks                │
│   Disconnect                       │
│   Saved Networks                   │
│   Back                             │
├────────────────────────────────────┤
│ ↑↓:Navigate  SELECT:Choose         │
└────────────────────────────────────┘
```

**If Disconnected**:
```
┌────────────────────────────────────┐
│ WiFi Settings                      │
├────────────────────────────────────┤
│ Status: Disconnected               │
│                                    │
│ > Scan for Networks                │
│   Saved Networks                   │
│   Back                             │
│                                    │
│                                    │
│                                    │
├────────────────────────────────────┤
│ ↑↓:Navigate  SELECT:Choose         │
└────────────────────────────────────┘
```

**Button Handling**:
- UP/DOWN: Navigate menu items
- SELECT: Activate selected option
- BACK: Return to Settings menu
- MENU: Return to main menu

#### STATE_WIFI_SCAN

**Purpose**: Show scanning progress while searching for networks.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Scanning for Networks              │
├────────────────────────────────────┤
│                                    │
│                                    │
│            ◐ Scanning...           │
│                                    │
│     Please wait, this may take     │
│         up to 10 seconds.          │
│                                    │
│                                    │
│                                    │
├────────────────────────────────────┤
│ BACK:Cancel                        │
└────────────────────────────────────┘
```

**Button Handling**:
- BACK: Cancel scan, return to WIFI_MAIN
- All others: Ignored during scan

**Timeout**: 10 seconds max
**Transition**: Auto-transition to WIFI_LIST when scan completes

#### STATE_WIFI_LIST

**Purpose**: Display list of available networks for selection.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Available Networks        [1/2]    │
├────────────────────────────────────┤
│ > MyHomeWiFi      ████████ 🔒     │
│   GuestNetwork    ████░░░░ 🔒     │
│   CoffeeShop      ██████░░         │
│   Neighbor_2G     ██░░░░░░ 🔒     │
│   FreeWiFi        ████░░░░         │
│   Library_Public  ██████░░         │
│   Work_Secure     ████████ 🔒     │
│   OfficeNet       ██░░░░░░ 🔒     │
│   MobileHotspot   ████░░░░ 🔒     │
│   EdgeCase_VeryLongSSIDName...     │
│   NetGear-5G      ██░░░░░░ 🔒     │
│                                    │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Connect  M:Scan  │
└────────────────────────────────────┘
```

**Display Format**:
- SSID (truncated to ~22 chars if needed)
- Signal strength bar (8 chars: █ blocks, 8 levels)
- Security indicator: 🔒 for secured, blank for open

**Signal Strength Mapping**:
- 8/8 blocks: -30 to -50 dBm (Excellent)
- 7/8 blocks: -50 to -60 dBm (Very Good)
- 6/8 blocks: -60 to -67 dBm (Good)
- 5/8 blocks: -67 to -70 dBm (OK)
- 4/8 blocks: -70 to -75 dBm (Fair)
- 3/8 blocks: -75 to -80 dBm (Weak)
- 2/8 blocks: -80 to -85 dBm (Very Weak)
- 1/8 block:  -85 to -90 dBm (Minimal)

**Sorting**: Networks sorted by signal strength (strongest first)

**Pagination**: 11 networks per screen, with page indicator

**Button Handling**:
- UP: Previous network (wrap to bottom at top)
- DOWN: Next network (wrap to top at bottom)
- SELECT: Choose network, transition to WIFI_CONNECT
- BACK: Return to WIFI_MAIN
- MENU: Re-scan networks

**Edge Cases**:
- No networks found: Show "No networks found. Press MENU to scan again."
- Hidden networks: Not shown (require pre-configuration via config file)

#### STATE_WIFI_CONNECT

**Purpose**: Show network details and initiate connection.

**Screen Layout (Open Network)**:
```
┌────────────────────────────────────┐
│ Connect to Network                 │
├────────────────────────────────────┤
│ Network: CoffeeShop                │
│ Security: Open (No Password)       │
│ Signal: ██████░░ (75%)             │
│                                    │
│ This is an open network. Your     │
│ connection will not be encrypted.  │
│                                    │
│ Connect to this network?           │
│                                    │
│   > Yes, Connect                   │
│     Cancel                         │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Screen Layout (Secured Network, Saved)**:
```
┌────────────────────────────────────┐
│ Connect to Network                 │
├────────────────────────────────────┤
│ Network: MyHomeWiFi                │
│ Security: WPA2-PSK                 │
│ Signal: ████████ (95%)             │
│                                    │
│ Saved password found.              │
│                                    │
│ Connect using saved password?      │
│                                    │
│   > Yes, Connect                   │
│     Enter New Password             │
│     Cancel                         │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Screen Layout (Secured Network, No Saved Password)**:
```
┌────────────────────────────────────┐
│ Connect to Network                 │
├────────────────────────────────────┤
│ Network: GuestNetwork              │
│ Security: WPA2-PSK                 │
│ Signal: ████░░░░ (50%)             │
│                                    │
│ This network requires a password.  │
│                                    │
│ NOTE: Password entry with 5 buttons│
│ is slow. Consider pre-configuring  │
│ via SD card (see User Guide).      │
│                                    │
│   > Enter Password                 │
│     Cancel                         │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Button Handling**:
- UP/DOWN: Navigate options
- SELECT: Confirm choice
- BACK: Cancel, return to WIFI_LIST

**Transitions**:
- "Yes, Connect" (open/saved) → STATE_WIFI_CONNECTING
- "Enter Password" → STATE_WIFI_PASSWORD
- "Cancel" → STATE_WIFI_LIST

#### STATE_WIFI_PASSWORD

**Purpose**: Text input for WiFi password using character grid.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Enter Password: MyHomeWiFi         │
├────────────────────────────────────┤
│ Pass: MyP****                      │
│                                    │
│  A B C D E F G H I J K L M N O P   │
│  Q R S T U V W X Y Z [a-z] [0-9]  │
│  > a  b  c  d  e  f  g  h  i  j   │
│    k  l  m  n  o  p  q  r  s  t   │
│    u  v  w  x  y  z  [A-Z] [SYM]  │
│    0  1  2  3  4  5  6  7  8  9   │
│  [SPACE] [BACKSPACE] [DONE]       │
│                                    │
│ Tips: UP/DOWN navigate rows        │
│       SELECT chooses character     │
├────────────────────────────────────┤
│ Mode: Lowercase                    │
└────────────────────────────────────┘
```

**Character Grid Design**:

Three modes cycle via [A-Z], [a-z], [0-9], [SYM] buttons:

**Mode 1: Uppercase Letters**
```
Row 1: A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [a-z]
Row 2: 0 1 2 3 4 5 6 7 8 9 [SYM]
Row 3: [SPACE] [BACKSPACE] [DONE]
```

**Mode 2: Lowercase Letters**
```
Row 1: a b c d e f g h i j k l m n o p q r s t u v w x y z [A-Z]
Row 2: 0 1 2 3 4 5 6 7 8 9 [SYM]
Row 3: [SPACE] [BACKSPACE] [DONE]
```

**Mode 3: Numbers (Quick Access)**
```
Row 1: 0 1 2 3 4 5 6 7 8 9 [A-Z]
Row 2: [SPACE] [BACKSPACE] [DONE]
```

**Mode 4: Symbols**
```
Row 1: ! @ # $ % ^ & * ( ) - _ = + [ ] { }
Row 2: \ | ; : ' " < > , . / ? ~ ` [A-Z]
Row 3: [SPACE] [BACKSPACE] [DONE]
```

**Navigation Model**:

Two-level navigation for efficiency:

**Level 1: Row Selection** (default)
- UP/DOWN: Move between rows
- SELECT: Enter Level 2 (character selection within row)
- BACK: Cancel password entry

**Level 2: Character Selection**
- UP/DOWN: Move along characters in current row
- SELECT: Choose character, add to password, return to Level 1
- BACK: Return to Level 1 without selecting

**Special Buttons**:
- `[A-Z]`: Switch to uppercase mode
- `[a-z]`: Switch to lowercase mode
- `[0-9]`: Switch to numbers mode
- `[SYM]`: Switch to symbols mode
- `[SPACE]`: Add space character
- `[BACKSPACE]`: Delete last character
- `[DONE]`: Finish password entry, proceed to connect

**Password Display**:
- Show first 3 characters, then asterisks (e.g., "MyP****")
- Maximum length: 63 characters (WPA2 standard)
- Minimum length: 8 characters (enforced by DONE button)

**Button Handling** (Level 1):
- UP: Previous row
- DOWN: Next row
- SELECT: Enter Level 2 character selection
- BACK: Cancel, return to WIFI_CONNECT
- MENU: Show on-screen help

**Button Handling** (Level 2):
- UP: Previous character in row
- DOWN: Next character in row
- SELECT: Choose character, add to password
- BACK: Return to Level 1
- MENU: (ignored)

**Optimization**: Cursor wraps around (e.g., from 'Z' to 'A' when pressing DOWN)

**Alternative Simpler Design** (If complexity is too high):

Single-row sequential navigation through all characters:
```
┌────────────────────────────────────┐
│ Enter Password: MyHomeWiFi         │
├────────────────────────────────────┤
│ Password: MyP****                  │
│                                    │
│        Current Character: [ a ]    │
│                                    │
│      ↑ UP: Previous Character      │
│      ↓ DOWN: Next Character        │
│      SELECT: Add to Password       │
│                                    │
│  Characters: A-Z, a-z, 0-9, !@#... │
│                                    │
│  [BACKSPACE]  [DONE]              │
├────────────────────────────────────┤
│ SELECT:Add  MENU:Backspace  BACK:Cancel
└────────────────────────────────────┘
```

This simpler version cycles through all ~90 characters sequentially. Slower but much simpler to implement.

**Recommendation**: Start with simple sequential design for Phase 06, upgrade to grid navigation in Phase 07 if user feedback demands it.

#### STATE_WIFI_CONNECTING

**Purpose**: Show connection progress.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Connecting to Network              │
├────────────────────────────────────┤
│                                    │
│   Network: MyHomeWiFi              │
│   Security: WPA2-PSK               │
│                                    │
│            ◐ Connecting...         │
│                                    │
│   This may take up to 30 seconds.  │
│                                    │
│                                    │
├────────────────────────────────────┤
│ BACK:Cancel                        │
└────────────────────────────────────┘
```

**Progress Stages** (if implementation supports):
1. "Authenticating..." (WPA handshake)
2. "Obtaining IP address..." (DHCP)
3. "Testing connection..." (ping gateway)

**Button Handling**:
- BACK: Cancel connection attempt
- All others: Ignored

**Timeout**: 30 seconds
**Transitions**:
- Success → STATE_WIFI_RESULT (success)
- Failure → STATE_WIFI_RESULT (error)
- Cancel → STATE_WIFI_LIST

#### STATE_WIFI_RESULT

**Purpose**: Display connection result (success or error).

**Success Screen**:
```
┌────────────────────────────────────┐
│ Connection Successful              │
├────────────────────────────────────┤
│                                    │
│   ✓ Connected to MyHomeWiFi        │
│                                    │
│   IP Address: 192.168.1.42         │
│   Signal: ████████ (95%)           │
│                                    │
│   You can now download books from  │
│   the online library.              │
│                                    │
│                                    │
├────────────────────────────────────┤
│ BACK:Continue  SELECT:Test         │
└────────────────────────────────────┘
```

**Error Screen (Wrong Password)**:
```
┌────────────────────────────────────┐
│ Connection Failed                  │
├────────────────────────────────────┤
│                                    │
│   ✗ Could not connect              │
│                                    │
│   Network: GuestNetwork            │
│   Error: Authentication failed     │
│         (incorrect password)       │
│                                    │
│   > Try Again                      │
│     Return to Network List         │
│                                    │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Error Screen (Network Unavailable)**:
```
┌────────────────────────────────────┐
│ Connection Failed                  │
├────────────────────────────────────┐
│                                    │
│   ✗ Could not connect              │
│                                    │
│   Network: MyHomeWiFi              │
│   Error: Network not found or      │
│          signal too weak           │
│                                    │
│   > Scan Again                     │
│     Return to WiFi Settings        │
│                                    │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Error Screen (DHCP Timeout)**:
```
┌────────────────────────────────────┐
│ Connection Failed                  │
├────────────────────────────────────┤
│                                    │
│   ✗ Could not connect              │
│                                    │
│   Network: CoffeeShop              │
│   Error: Could not obtain IP       │
│          address (DHCP timeout)    │
│                                    │
│   Check network settings with      │
│   network administrator.           │
│                                    │
│   > Try Again                      │
├────────────────────────────────────┤
│ SELECT:Confirm  BACK:Cancel        │
└────────────────────────────────────┘
```

**Button Handling** (Success):
- BACK: Return to WIFI_MAIN
- SELECT: Test connection (ping test, show result)

**Button Handling** (Error):
- UP/DOWN: Navigate options
- SELECT: Confirm choice
- BACK: Return to WIFI_MAIN

**Auto-Transition**: After 5 seconds, auto-return to WIFI_MAIN (user can press BACK sooner)

#### STATE_WIFI_SAVED_NETWORKS

**Purpose**: Manage saved network credentials.

**Screen Layout**:
```
┌────────────────────────────────────┐
│ Saved Networks            [1/1]    │
├────────────────────────────────────┤
│ > MyHomeWiFi              (active) │
│   WorkNetwork                      │
│   CoffeeShop_Guest                 │
│   Library_Public                   │
│                                    │
│                                    │
│                                    │
│                                    │
│                                    │
│                                    │
│                                    │
├────────────────────────────────────┤
│ SELECT:Options  BACK:Return        │
└────────────────────────────────────┘
```

**Options Menu** (when SELECT pressed on a network):
```
┌────────────────────────────────────┐
│ Network: MyHomeWiFi                │
├────────────────────────────────────┤
│                                    │
│ > Connect                          │
│   Forget Network                   │
│   View Details                     │
│   Cancel                           │
│                                    │
│                                    │
│                                    │
│                                    │
│                                    │
├────────────────────────────────────┤
│ ↑↓:Select  ENTER:Confirm           │
└────────────────────────────────────┘
```

**Button Handling**:
- UP/DOWN: Navigate saved networks
- SELECT: Open options menu
- BACK: Return to WIFI_MAIN
- MENU: (reserved)

## Status Bar Integration

The main e-reader status bar should show WiFi connection status at all times.

### Status Bar Layout (Updated)

```
┌────────────────────────────────────┐
│ Book Title      📶 ⚡ 🔋95% [12/156]│  ← Status bar
├────────────────────────────────────┤
```

**WiFi Icon States**:
- `📶` - Connected (full signal)
- `📶` with fewer bars - Connected (weak signal)
- `📵` - Disconnected
- `◐` - Connecting (animated)

**Icon Rendering** (8×16 pixel font):

Since we're using a bitmap font, WiFi icons must be rendered as text art:

**Connected (Strong Signal)**:
```
  ▄▀▀▄
 ▄▀▄▀▄
▀ ▄▀ ▀
```

**Connected (Medium Signal)**:
```
  ▄▀▀▄
 ▄   ▄
▀ ▄  ▀
```

**Connected (Weak Signal)**:
```
     ▄
    ▄
▀ ▄  ▀
```

**Disconnected**:
```
  ▄  ▄
    ✗
▀ ▄  ▀
```

**Alternative**: Use simple text indicators:
- `[WiFi:On]` / `[WiFi:Off]`
- `[✓]` / `[✗]`

## WiFi Manager Backend Interface

The UI communicates with the WiFi manager backend through a clean C API.

### Data Structures

```c
#define MAX_SSID_LENGTH 32
#define MAX_NETWORKS 50
#define MAX_PASSWORD_LENGTH 63

typedef enum {
    WIFI_SECURITY_OPEN,
    WIFI_SECURITY_WEP,
    WIFI_SECURITY_WPA_PSK,
    WIFI_SECURITY_WPA2_PSK,
    WIFI_SECURITY_WPA_WPA2_PSK
} wifi_security_t;

typedef enum {
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_ERROR
} wifi_state_t;

typedef enum {
    WIFI_ERROR_NONE,
    WIFI_ERROR_AUTH_FAILED,       // Wrong password
    WIFI_ERROR_NETWORK_NOT_FOUND, // SSID not in range
    WIFI_ERROR_DHCP_TIMEOUT,      // No IP address
    WIFI_ERROR_TIMEOUT,           // Connection timeout
    WIFI_ERROR_UNKNOWN
} wifi_error_t;

typedef struct {
    char ssid[MAX_SSID_LENGTH + 1];
    char bssid[18];               // MAC address (XX:XX:XX:XX:XX:XX)
    int signal_strength;          // dBm (-30 to -90)
    wifi_security_t security;
    int frequency;                // MHz
    bool is_saved;                // Has saved credentials
} wifi_network_t;

typedef struct {
    wifi_state_t state;
    char connected_ssid[MAX_SSID_LENGTH + 1];
    int signal_strength;          // dBm
    char ip_address[16];          // e.g., "192.168.1.42"
    wifi_error_t last_error;
    char error_message[128];
} wifi_status_t;
```

### API Functions

```c
/* Initialize WiFi manager */
int wifi_manager_init(void);

/* Cleanup WiFi manager */
void wifi_manager_cleanup(void);

/* Scan for available networks */
int wifi_manager_scan(wifi_network_t *networks, int max_networks, int *count);

/* Connect to a network */
int wifi_manager_connect(const char *ssid, const char *password);

/* Disconnect from current network */
int wifi_manager_disconnect(void);

/* Get current connection status */
int wifi_manager_get_status(wifi_status_t *status);

/* Forget saved network credentials */
int wifi_manager_forget_network(const char *ssid);

/* List saved networks */
int wifi_manager_list_saved(wifi_network_t *networks, int max_networks, int *count);

/* Test connectivity (ping gateway) */
int wifi_manager_test_connection(void);
```

### Implementation Strategy

The WiFi manager will interact with system services:

1. **Scanning**: Execute `iw dev wlan0 scan` and parse output
2. **Connecting**:
   - Update `/etc/wpa_supplicant.conf` with network credentials
   - Restart wpa_supplicant: `wpa_cli reconfigure`
   - Trigger DHCP: `dhcpcd wlan0`
3. **Status**: Parse `iw dev wlan0 link` and `ip addr show wlan0`
4. **Saved Networks**: Read/write `/etc/wpa_supplicant.conf`

## Memory and Performance Considerations

### Memory Budget

**UI State Data**:
- Network list: 50 networks × ~100 bytes = 5 KB
- Status info: ~1 KB
- Password buffer: 64 bytes
- Screen framebuffer: 15 KB (shared with main app)
- **Total**: ~21 KB

**Acceptable**: Well within Pi Zero W constraints

### Performance Targets

**Network Scan**: 3-10 seconds (hardware limited)
**Connection Time**: 5-30 seconds (depending on network)
**UI Responsiveness**: < 100 ms button-to-screen update (state change, display refresh happens later)

### Optimization Strategies

1. **Scan Caching**: Cache scan results for 60 seconds, allow manual refresh
2. **Background Connection**: Connect in background, update UI when complete
3. **Minimal Refreshes**: Only refresh display when necessary (e.g., connection state changes)
4. **Lazy Loading**: Load saved networks only when viewing that screen

## User Experience Considerations

### Guiding Users to Pre-Configuration

Since on-device password entry is tedious, guide users toward pre-configuration:

**First Boot Flow**:
```
┌────────────────────────────────────┐
│ Welcome to WiFi Setup              │
├────────────────────────────────────┤
│ There are two ways to set up WiFi: │
│                                    │
│ 1. PRE-CONFIGURE (Recommended)     │
│    Edit wpa_supplicant.conf on     │
│    your computer before booting.   │
│    See User Guide for details.     │
│                                    │
│ 2. ON-DEVICE ENTRY (Slow)          │
│    Enter password using 5-button   │
│    interface (may take 5-10 min).  │
│                                    │
│ > Continue with On-Device Setup    │
│   Skip (configure later)           │
├────────────────────────────────────┤
│ SELECT:Choose  BACK:Skip           │
└────────────────────────────────────┘
```

**User Guide Section**: Prominently document pre-configuration process with examples.

### Password Entry UX Improvements

To make password entry less painful:

1. **Smart Defaults**: Start on lowercase letters (most passwords start lowercase)
2. **Recent Characters**: Show last 3 characters selected for quick repeat
3. **Common Patterns**: Quick shortcuts for common patterns (e.g., "123", "abc")
4. **Word Boundaries**: Auto-suggest space after common words (future ML feature)
5. **Show/Hide Password**: Toggle between masked and visible password

### Error Recovery

**Connection Failures**: Always provide:
- Clear error description
- Suggested fix (if applicable)
- Options: Retry, Try different network, Cancel

**Scan Failures**:
- Suggest checking WiFi is enabled
- Offer to retry scan
- Note: Some networks may be hidden (require pre-config)

## Accessibility and Usability

### Design Principles

1. **Clear Feedback**: Every button press should have visible feedback
2. **Escape Hatches**: BACK button always provides a way out
3. **Progress Indicators**: Show progress during long operations
4. **Error Messages**: Plain language, actionable
5. **Consistent Layout**: Same information in same places across screens

### E-Paper Specific Optimizations

1. **Minimize Refreshes**: Update only on state changes, not every button press
2. **Bold Indicators**: High contrast for signal strength, status icons
3. **Large Touch Targets**: Even though we're button-based, keep UI elements spaced
4. **Readable Fonts**: 8×16 font is clear, don't go smaller

## Implementation Checklist

### Phase 06 WiFi UI Tasks

- [ ] Implement WiFi state machine (states and transitions)
- [ ] Create `src/ereader/ui/wifi_menu.c/h` (main WiFi UI controller)
- [ ] Create `src/ereader/ui/wifi_list.c/h` (network list renderer)
- [ ] Create `src/ereader/ui/text_input.c/h` (password entry system)
- [ ] Create `src/ereader/network/wifi_manager.c/h` (backend interface)
- [ ] Design and render WiFi status icons for status bar
- [ ] Implement scan results parsing (`iw` output)
- [ ] Implement connection management (wpa_supplicant control)
- [ ] Implement saved networks management
- [ ] Add WiFi option to Settings menu
- [ ] Test all screens and transitions
- [ ] Update User Guide with WiFi setup instructions
- [ ] Create troubleshooting guide for common WiFi issues

## Future Enhancements (Phase 07+)

### Advanced Features

1. **Hidden Network Support**: Add manual SSID entry
2. **Static IP Configuration**: For networks without DHCP
3. **Proxy Settings**: For enterprise networks
4. **Connection Profiles**: Switch between home/work/travel profiles
5. **WiFi Analytics**: Show data usage, connection history
6. **QR Code Configuration**: Scan QR code from phone to configure WiFi
7. **External Keyboard**: Support USB keyboard for password entry
8. **Voice Input**: USB microphone for voice-dictated passwords (ambitious!)

### Grid Navigation Upgrade

If user feedback indicates password entry is too slow, implement the full grid navigation system with 2-level selection as described earlier.

### Predictive Text

For Phase 07, consider adding simple predictive text:
- Dictionary of common password words
- Suggest completions as user types
- Learn from successfully connected passwords (securely!)

## Testing Strategy

### Manual Testing Checklist

- [ ] Scan for networks in various signal conditions
- [ ] Connect to open network
- [ ] Connect to WPA2 network with correct password
- [ ] Attempt connection with wrong password (verify error)
- [ ] Disconnect from network
- [ ] View saved networks
- [ ] Forget saved network
- [ ] Test with 0 networks in range
- [ ] Test with 50+ networks in range (pagination)
- [ ] Test password entry with all character types
- [ ] Test BACK button at every state (escape)
- [ ] Test connection timeout scenarios
- [ ] Test battery impact (connect, stay connected, disconnect)

### Edge Cases

- **SSID with special characters**: Test with SSIDs containing spaces, quotes, unicode
- **Very long SSIDs**: Test truncation (32 char limit)
- **Duplicate SSIDs**: Multiple APs with same name (different BSSIDs)
- **Moving out of range**: Connect, then move away (auto-reconnect behavior)
- **Network disappears during scan**: Handle gracefully
- **Password length limits**: Min 8, max 63 for WPA2

## References

- [[EREADER_DESIGN]] - Main application architecture
- [[INPUT_SYSTEM]] - Button input abstraction
- [[WIFI_SETUP]] - WiFi hardware and configuration
- [[USER_GUIDE]] - User documentation
- [WPA Supplicant Configuration](https://w1.fi/cgit/hostap/plain/wpa_supplicant/wpa_supplicant.conf)
- [Linux Wireless Tools](https://hewlettpackard.github.io/wireless-tools/Tools.html)

## Revision History

- 2026-01-16: Initial WiFi UI design for Phase 06
