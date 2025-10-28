#!/usr/bin/env python3
"""
Framebuffer test application for E-Reader OS
Displays "Corey is awesome" in random positions on screen
"""

import sys
import time
import random
import os

def clear_console():
    """Clear the console screen"""
    sys.stdout.write('\033[2J')
    sys.stdout.write('\033[H')
    sys.stdout.flush()

def move_cursor(row, col):
    """Move cursor to specific position"""
    sys.stdout.write(f'\033[{row};{col}H')
    sys.stdout.flush()

def set_color(fg=37, bg=40, bold=False):
    """Set text color. fg=foreground, bg=background"""
    bold_code = '1;' if bold else ''
    sys.stdout.write(f'\033[{bold_code}{fg};{bg}m')
    sys.stdout.flush()

def reset_color():
    """Reset to default colors"""
    sys.stdout.write('\033[0m')
    sys.stdout.flush()

def main():
    """Main test loop"""
    # Terminal size (approximate for typical console)
    max_rows = 24
    max_cols = 80

    messages = [
        "Corey is awesome",
        "E-Reader OS Working!",
        "Framebuffer Test",
        "*** SUCCESS ***",
    ]

    colors = [
        (31, 40),  # Red on black
        (32, 40),  # Green on black
        (33, 40),  # Yellow on black
        (34, 40),  # Blue on black
        (35, 40),  # Magenta on black
        (36, 40),  # Cyan on black
        (37, 40),  # White on black
    ]

    try:
        # Redirect to main console
        console = open('/dev/console', 'w')
        old_stdout = sys.stdout
        sys.stdout = console

        clear_console()

        iteration = 0
        while True:
            # Pick random message
            message = messages[iteration % len(messages)]

            # Pick random position (leave space for message)
            row = random.randint(1, max(1, max_rows - 2))
            col = random.randint(1, max(1, max_cols - len(message)))

            # Pick random color
            fg, bg = random.choice(colors)

            # Clear screen occasionally
            if iteration % 5 == 0:
                clear_console()

            # Display message
            move_cursor(row, col)
            set_color(fg, bg, bold=True)
            sys.stdout.write(message)
            sys.stdout.flush()

            # Status line at bottom
            move_cursor(max_rows, 1)
            reset_color()
            set_color(37, 40, bold=False)
            status = f"Boot test running... iteration {iteration + 1}"
            sys.stdout.write(status.ljust(max_cols - 1))
            sys.stdout.flush()

            iteration += 1
            time.sleep(3)

    except KeyboardInterrupt:
        reset_color()
        clear_console()
        sys.stdout.write("Test stopped.\n")
    except Exception as e:
        sys.stdout = old_stdout
        print(f"Error: {e}")
    finally:
        reset_color()

if __name__ == '__main__':
    main()
