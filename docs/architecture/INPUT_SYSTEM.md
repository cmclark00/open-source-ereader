---
type: architecture
title: Input System Architecture
created: 2026-01-13
tags:
  - architecture
  - input
  - buttons
  - design
  - phase-03
related:
  - "[[BUTTON_LAYOUT]]"
  - "[[PHASE_02_LOG]]"
  - "[[Phase-03-Basic-E-Reader-Application]]"
---

# Input System Architecture

This document outlines the high-level design for the E-Reader's input handling system. It describes how button events translate into application actions, state management for different input modes, and extensibility for future input methods.

## Overview

The input system provides a clean abstraction layer between hardware input events (button presses) and application logic (page turns, menu navigation, settings). This separation allows:

- **Hardware Independence:** Swap button implementation without changing app code
- **Mode-Specific Behavior:** Same button does different things in different contexts
- **Testability:** Mock input events for automated testing
- **Future Expansion:** Add touchscreen, keyboard without rewriting app

## Architecture Layers

```
┌─────────────────────────────────────────────────┐
│          Application Logic                      │
│  (E-Reader UI, Page Rendering, Menu System)     │
└──────────────────┬──────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────┐
│      Input Abstraction Layer (Phase 3)          │
│  - Action mapping (button → action)              │
│  - State machine (reading/menu/settings modes)   │
│  - Event dispatch to handlers                    │
└──────────────────┬──────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────┐
│      Button Input Library (Phase 2)              │
│  - Read /dev/input/eventX                        │
│  - Software debouncing                           │
│  - Event callbacks                               │
└──────────────────┬──────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────┐
│         Linux Input Subsystem                    │
│  - gpio-keys driver                              │
│  - Input event interface                         │
└──────────────────┬──────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────┐
│      Hardware (GPIO Buttons)                     │
│  - 5 tactile switches                            │
│  - GPIO 5, 6, 13, 19, 26                         │
└─────────────────────────────────────────────────┘
```

## Input Modes (State Machine)

The E-Reader operates in different input modes, where the same button performs different actions depending on the current mode.

### Mode Definitions

#### 1. Reading Mode (Default)

Active when user is reading a book.

| Button | Action | Description |
|--------|--------|-------------|
| UP | Previous Page | Navigate to previous page |
| DOWN | Next Page | Navigate to next page |
| SELECT | Toggle Menu | Open/close reading menu (bookmarks, TOC, etc.) |
| BACK | Return to Library | Exit book, return to library |
| MENU | Quick Settings | Brightness, font size, etc. |

**Long Press Actions (Future):**
- UP (long): Scroll up (continuous)
- DOWN (long): Scroll down (continuous)
- SELECT (long): Add bookmark
- BACK (long): Return to home screen
- MENU (long): Full settings menu

#### 2. Menu Mode

Active when navigating lists (library, table of contents, settings).

| Button | Action | Description |
|--------|--------|-------------|
| UP | Move Up | Highlight previous menu item |
| DOWN | Move Down | Highlight next menu item |
| SELECT | Activate Item | Open book, toggle setting, etc. |
| BACK | Exit Menu | Return to previous screen |
| MENU | Context Menu | Additional options for selected item |

**Pagination:**
- UP at top of list: Go to previous page of items
- DOWN at bottom: Go to next page of items

#### 3. Settings Mode

Active when adjusting settings (font size, brightness, etc.).

| Button | Action | Description |
|--------|--------|-------------|
| UP | Increase Value | Larger font, brighter, etc. |
| DOWN | Decrease Value | Smaller font, dimmer, etc. |
| SELECT | Confirm | Save setting and exit |
| BACK | Cancel | Revert changes and exit |
| MENU | Reset to Default | Reset current setting |

#### 4. Dialog Mode

Active when user sees a dialog (confirm delete, etc.).

| Button | Action | Description |
|--------|--------|-------------|
| UP | Select Previous Option | Highlight "Yes" or "No" |
| DOWN | Select Next Option | Highlight "No" or "Yes" |
| SELECT | Confirm | Execute selected action |
| BACK | Cancel | Close dialog, no action |
| MENU | (disabled) | No action in dialogs |

### State Transitions

```
                    ┌──────────────┐
                    │   Startup    │
                    └──────┬───────┘
                           │
                           ▼
              ┌────────────────────────┐
              │    Library Mode        │
              │  (Menu Mode variant)   │
              └─────┬────────────┬─────┘
                    │            │
        SELECT book │            │ MENU
                    │            │
                    ▼            ▼
         ┌─────────────┐  ┌──────────┐
         │ Reading Mode │  │ Settings │
         └──────┬───────┘  │   Mode   │
                │          └─────┬────┘
    SELECT/MENU │                │
                │                │ BACK
                ▼                │
         ┌─────────────┐         │
         │  Menu Mode  │◄────────┘
         │  (in book)  │
         └──────┬──────┘
                │
           BACK │
                │
                ▼
          (Return to Reading)
```

## Input Abstraction Layer Design

### Core Components

#### 1. Input Event Manager

**Responsibilities:**
- Initialize button_input library
- Register event callbacks
- Dispatch events to current mode handler
- Handle mode transitions

**API:**
```c
typedef struct input_manager input_manager_t;

/* Initialize input manager */
input_manager_t* input_manager_init(void);

/* Set current input mode */
void input_manager_set_mode(input_manager_t *mgr, input_mode_t mode);

/* Get current input mode */
input_mode_t input_manager_get_mode(input_manager_t *mgr);

/* Main event loop */
void input_manager_run(input_manager_t *mgr);

/* Cleanup */
void input_manager_cleanup(input_manager_t *mgr);
```

#### 2. Action Definitions

**Enum of all possible actions:**

```c
typedef enum {
    /* Navigation actions */
    ACTION_NEXT_PAGE,
    ACTION_PREV_PAGE,
    ACTION_SCROLL_UP,
    ACTION_SCROLL_DOWN,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,

    /* Selection actions */
    ACTION_SELECT,
    ACTION_ACTIVATE,

    /* Menu actions */
    ACTION_OPEN_MENU,
    ACTION_CLOSE_MENU,
    ACTION_TOGGLE_MENU,
    ACTION_CONTEXT_MENU,

    /* System actions */
    ACTION_GO_BACK,
    ACTION_GO_HOME,
    ACTION_OPEN_SETTINGS,

    /* Value adjustment */
    ACTION_INCREASE,
    ACTION_DECREASE,

    /* Confirmation */
    ACTION_CONFIRM,
    ACTION_CANCEL,

    ACTION_NONE
} input_action_t;
```

#### 3. Mode Handlers

**Interface for mode-specific input handling:**

```c
typedef struct {
    /* Handle button event in this mode */
    input_action_t (*handle_button)(button_t button, button_event_type_t type);

    /* Enter this mode (initialization) */
    void (*on_enter)(void);

    /* Exit this mode (cleanup) */
    void (*on_exit)(void);

    /* Mode name for debugging */
    const char *name;
} input_mode_handler_t;
```

**Example mode handler (Reading Mode):**

```c
static input_action_t reading_mode_handle_button(
    button_t button,
    button_event_type_t type)
{
    /* Only handle PRESS events in reading mode */
    if (type != BUTTON_EVENT_PRESS) {
        return ACTION_NONE;
    }

    switch (button) {
        case BUTTON_UP:     return ACTION_PREV_PAGE;
        case BUTTON_DOWN:   return ACTION_NEXT_PAGE;
        case BUTTON_SELECT: return ACTION_TOGGLE_MENU;
        case BUTTON_BACK:   return ACTION_GO_BACK;
        case BUTTON_MENU:   return ACTION_OPEN_SETTINGS;
        default:            return ACTION_NONE;
    }
}

static void reading_mode_on_enter(void)
{
    /* Hide cursor, prepare for page turns */
}

static void reading_mode_on_exit(void)
{
    /* Save reading position */
}

static input_mode_handler_t reading_mode_handler = {
    .handle_button = reading_mode_handle_button,
    .on_enter = reading_mode_on_enter,
    .on_exit = reading_mode_on_exit,
    .name = "Reading Mode"
};
```

#### 4. Action Dispatcher

**Translates actions into application calls:**

```c
typedef void (*action_handler_t)(input_action_t action, void *context);

/* Register action handler */
void input_manager_register_action_handler(
    input_manager_t *mgr,
    action_handler_t handler,
    void *context);
```

**Application action handler example:**

```c
static void app_handle_action(input_action_t action, void *context)
{
    app_state_t *app = (app_state_t *)context;

    switch (action) {
        case ACTION_NEXT_PAGE:
            ereader_turn_page(app->reader, PAGE_FORWARD);
            break;

        case ACTION_PREV_PAGE:
            ereader_turn_page(app->reader, PAGE_BACKWARD);
            break;

        case ACTION_GO_BACK:
            if (app->current_mode == MODE_READING) {
                ereader_close_book(app->reader);
                input_manager_set_mode(app->input, MODE_LIBRARY);
            }
            break;

        /* ... handle other actions ... */
    }
}
```

## Event Flow Example

**User presses DOWN button while reading:**

```
1. Hardware: GPIO 6 pulled LOW
   ↓
2. Linux: gpio-keys driver generates EV_KEY event (KEY_DOWN, value=1)
   ↓
3. Button Library: button_input_read_event() returns button_event_t
   - button = BUTTON_DOWN
   - event_type = BUTTON_EVENT_PRESS
   ↓
4. Input Manager: Receives event via callback
   - Determines current mode = MODE_READING
   - Calls reading_mode_handler.handle_button()
   ↓
5. Mode Handler: Maps button to action
   - BUTTON_DOWN in MODE_READING → ACTION_NEXT_PAGE
   ↓
6. Action Dispatcher: Calls registered action handler
   - app_handle_action(ACTION_NEXT_PAGE, app)
   ↓
7. Application: Executes action
   - ereader_turn_page(app->reader, PAGE_FORWARD)
   ↓
8. Display: E-paper updated with next page
```

## Advanced Features

### Long Press Detection

**Implementation approach:**

```c
typedef struct {
    button_t button;
    struct timeval press_time;
    bool is_long_press;
} button_state_t;

/* In event handler */
if (event->event_type == BUTTON_EVENT_PRESS) {
    /* Record press time */
    button_state[button].press_time = event->timestamp;
    button_state[button].is_long_press = false;
}
else if (event->event_type == BUTTON_EVENT_RELEASE) {
    /* Calculate duration */
    long duration_ms = timeval_diff_ms(&event->timestamp,
                                       &button_state[button].press_time);

    if (duration_ms > LONG_PRESS_THRESHOLD_MS) {
        /* Trigger long-press action */
        return handle_long_press(button);
    } else {
        /* Normal press action */
        return handle_short_press(button);
    }
}
```

### Button Combinations

**Example: BACK + SELECT = Screenshot**

```c
static bool button_pressed[5] = {false};  /* Track each button state */

/* In event handler */
if (event->event_type == BUTTON_EVENT_PRESS) {
    button_pressed[button] = true;

    /* Check for combinations */
    if (button_pressed[BUTTON_BACK] && button_pressed[BUTTON_SELECT]) {
        return ACTION_TAKE_SCREENSHOT;
    }
}
else if (event->event_type == BUTTON_EVENT_RELEASE) {
    button_pressed[button] = false;
}
```

### Gesture Support (Future)

For touchscreen integration in later phases:

```c
typedef enum {
    GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT,
    GESTURE_SWIPE_UP,
    GESTURE_SWIPE_DOWN,
    GESTURE_PINCH_ZOOM_IN,
    GESTURE_PINCH_ZOOM_OUT,
    GESTURE_TAP,
    GESTURE_DOUBLE_TAP,
    GESTURE_LONG_PRESS
} gesture_t;

/* Gesture events integrate into same action system */
input_action_t gesture_to_action(gesture_t gesture, input_mode_t mode);
```

## Future Input Methods

### Touchscreen (Phase 5+)

- Same input abstraction layer
- Additional input source: touchscreen driver
- Gesture recognition maps to same actions
- Mode handlers unchanged
- Example: Swipe right → ACTION_PREV_PAGE (same as UP button)

### Physical Keyboard (USB/Bluetooth)

- USB keyboard via Linux input subsystem
- Maps standard keys to actions:
  - Arrow keys → navigation
  - Enter → select
  - Esc → back
  - Space → next page
- No application changes needed

### Remote Control (IR Receiver)

- IR receiver via LIRC or input subsystem
- Remote button presses → same button_t enum
- Transparent to application layer

## Implementation Plan (Phase 3)

### Step 1: Basic Input Manager
- Implement input_manager with mode support
- Single mode initially (reading mode)
- Direct button → action mapping

### Step 2: Mode System
- Implement mode handler interface
- Add reading mode and menu mode
- Implement mode transitions

### Step 3: Action Dispatch
- Implement action dispatcher
- Connect to e-reader application
- Test basic page navigation

### Step 4: Advanced Features
- Long press detection
- Button combinations
- Polish and optimization

## Configuration

### Customizable Button Mappings

Allow users to customize button functions (future feature):

```c
typedef struct {
    button_t button;
    input_mode_t mode;
    input_action_t action;
} button_mapping_t;

/* User configuration file: /home/ereader/.ereader/buttons.conf */
/* Format: mode,button,action */
```

Example configuration:
```
reading,up,prev_page
reading,down,next_page
reading,select,toggle_menu
menu,up,move_up
menu,down,move_down
menu,select,activate
```

## Testing Strategy

### Unit Tests

- Test each mode handler independently
- Mock button events
- Verify correct action mapping

### Integration Tests

- Test mode transitions
- Test action dispatch
- Test edge cases (rapid presses, holds)

### User Acceptance Tests

- Real hardware button testing
- Verify intuitive behavior
- Gather feedback on button mapping

## Performance Considerations

### Event Processing Latency

**Target:** < 100ms from button press to action execution

**Optimizations:**
- Minimize processing in input callback
- Use efficient mode lookup (array, not hash table)
- Avoid heap allocations in hot path

### Power Consumption

**Considerations:**
- Polling GPIO vs. interrupt-driven
- Wake-on-button-press from sleep
- Disable unused input sources

## References

- [[BUTTON_LAYOUT]] - Hardware button specifications
- [[PHASE_02_LOG]] - Phase 2 implementation details
- [Linux Input Programming](https://www.kernel.org/doc/html/latest/input/input-programming.html)
- [State Machine Design Patterns](https://en.wikipedia.org/wiki/State_pattern)

## Revision History

- 2026-01-13: Initial input system architecture design
