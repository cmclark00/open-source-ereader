/*
 * framebuffer.c - Low-Level Framebuffer Operations Implementation
 *
 * Implements basic framebuffer operations for the e-reader application.
 * This module provides a monochrome (1-bit per pixel) framebuffer abstraction
 * for the 400×300 e-paper display.
 *
 * Memory Layout:
 * - Total buffer size: 15,000 bytes (400×300 pixels / 8 bits per byte)
 * - Byte ordering: Row-major (left-to-right, top-to-bottom)
 * - Bit ordering within bytes: MSB = leftmost pixel, LSB = rightmost pixel
 * - Color encoding: 1 = black, 0 = white (framebuffer convention)
 *
 * Example pixel layout for first 8 pixels (byte 0):
 *   Byte 0: [P0 P1 P2 P3 P4 P5 P6 P7]
 *           [b7 b6 b5 b4 b3 b2 b1 b0]
 *   P0 at (0,0), P1 at (1,0), P2 at (2,0), etc.
 *
 * Note: The display driver may use different color encoding (0=black, 1=white),
 * so conversion may be needed when copying to hardware buffer.
 *
 * Performance Considerations:
 * - fb_set_pixel: O(1) - Direct bit manipulation
 * - fb_draw_hline: O(n) - Linear in width
 * - fb_draw_rect: O(n*m) - Linear in area
 * - fb_invert_region: O(n*m) - Linear in area (read-modify-write per pixel)
 *
 * Phase 03: Basic E-Reader Application
 *
 * Copyright (C) 2024-2026 Open Source E-Reader Project Contributors
 *
 * This file is part of the Open Source E-Reader project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: E-Reader Project
 */

#include <string.h>
#include <stdlib.h>
#include "framebuffer.h"

/**
 * Initialize a framebuffer
 *
 * Sets up the framebuffer structure with display dimensions and clears
 * the buffer to white (blank screen).
 *
 * Parameters:
 *   fb - Pointer to framebuffer structure to initialize (must not be NULL)
 *
 * Side effects:
 *   - Sets fb->width and fb->height to display dimensions
 *   - Fills entire buffer with COLOR_WHITE (blank screen)
 */
void fb_init(framebuffer_t *fb) {
    if (!fb) return;

    fb->width = FB_WIDTH;
    fb->height = FB_HEIGHT;
    fb_clear(fb, COLOR_WHITE);
}

/**
 * Clear the framebuffer to a specific color
 *
 * Fills the entire framebuffer with a solid color. This is much faster
 * than calling fb_set_pixel for each pixel individually.
 *
 * Parameters:
 *   fb    - Pointer to framebuffer structure
 *   color - Color to fill (COLOR_WHITE or COLOR_BLACK)
 *
 * Performance:
 *   O(1) - Uses memset which is highly optimized on most platforms
 *   Typical time: <1ms for 15KB buffer
 */
void fb_clear(framebuffer_t *fb, uint8_t color) {
    if (!fb) return;

    /* Fill entire buffer with color using optimized memory operation */
    memset(fb->data, color, FB_BUFFER_SIZE);
}

/**
 * Set a pixel in the framebuffer
 *
 * Sets a single pixel to the specified color using bit manipulation.
 *
 * Pixel layout: Each byte contains 8 pixels (left to right)
 * Bit 7 (MSB) = leftmost pixel, Bit 0 (LSB) = rightmost pixel
 * 1 = black, 0 = white (inverse of display driver which uses 0=black, 1=white)
 *
 * Algorithm:
 * 1. Convert (x,y) coordinate to byte index using row-major ordering
 * 2. Calculate which bit within that byte represents the pixel
 * 3. Use bit masking to set or clear the bit without affecting others
 *
 * Example for pixel at (10, 5):
 *   - Byte index = (5 * 400 + 10) / 8 = 2010 / 8 = 251
 *   - Bit offset = 7 - (10 % 8) = 7 - 2 = 5
 *   - To set black: data[251] |= (1 << 5)  -> sets bit 5
 *   - To set white: data[251] &= ~(1 << 5) -> clears bit 5
 *
 * Parameters:
 *   fb    - Pointer to framebuffer structure
 *   x     - X coordinate (0 to FB_WIDTH-1)
 *   y     - Y coordinate (0 to FB_HEIGHT-1)
 *   color - COLOR_BLACK (0x00) or COLOR_WHITE (0xFF)
 *
 * Behavior:
 *   - Out-of-bounds coordinates are silently ignored (clipped)
 *   - NULL framebuffer pointer is safely handled
 *
 * Performance:
 *   O(1) - Constant time bit manipulation
 */
void fb_set_pixel(framebuffer_t *fb, int x, int y, uint8_t color) {
    if (!fb) return;

    /* Bounds checking - silently clip out-of-range coordinates */
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) {
        return;
    }

    /* Calculate byte offset in row-major order */
    int byte_index = (y * FB_WIDTH + x) / 8;

    /* Calculate bit position within byte (MSB = leftmost pixel) */
    int bit_offset = 7 - (x % 8);

    /* Set or clear the bit based on color using bit masking */
    if (color == COLOR_BLACK) {
        fb->data[byte_index] |= (1 << bit_offset);   /* OR sets the bit to 1 (black) */
    } else {
        fb->data[byte_index] &= ~(1 << bit_offset);  /* AND with inverted mask clears bit to 0 (white) */
    }
}

/**
 * Get a pixel value from the framebuffer
 *
 * Reads the color value of a single pixel using bit extraction.
 *
 * Parameters:
 *   fb - Pointer to framebuffer structure
 *   x  - X coordinate (0 to FB_WIDTH-1)
 *   y  - Y coordinate (0 to FB_HEIGHT-1)
 *
 * Returns:
 *   COLOR_BLACK (0x00) if pixel is black
 *   COLOR_WHITE (0xFF) if pixel is white
 *   -1 if coordinates are out of bounds or fb is NULL
 *
 * Performance:
 *   O(1) - Constant time bit extraction
 */
int fb_get_pixel(framebuffer_t *fb, int x, int y) {
    if (!fb) return -1;

    /* Bounds checking */
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) {
        return -1;
    }

    /* Calculate byte offset and bit position using same algorithm as fb_set_pixel */
    int byte_index = (y * FB_WIDTH + x) / 8;
    int bit_offset = 7 - (x % 8);

    /* Extract and return pixel value using bit masking */
    return (fb->data[byte_index] & (1 << bit_offset)) ? COLOR_BLACK : COLOR_WHITE;
}

/**
 * Draw a horizontal line
 */
void fb_draw_hline(framebuffer_t *fb, int x, int y, int width, uint8_t color) {
    if (!fb) return;

    for (int i = 0; i < width; i++) {
        fb_set_pixel(fb, x + i, y, color);
    }
}

/**
 * Draw a vertical line
 */
void fb_draw_vline(framebuffer_t *fb, int x, int y, int height, uint8_t color) {
    if (!fb) return;

    for (int i = 0; i < height; i++) {
        fb_set_pixel(fb, x, y + i, color);
    }
}

/**
 * Draw a filled rectangle
 *
 * Draws a solid rectangular region with automatic clipping to framebuffer bounds.
 * Partially off-screen rectangles are clipped to visible region.
 *
 * Clipping Algorithm:
 * 1. If left edge is negative, shift x to 0 and reduce width by the offset
 * 2. If top edge is negative, shift y to 0 and reduce height by the offset
 * 3. If right edge exceeds width, reduce width to fit
 * 4. If bottom edge exceeds height, reduce height to fit
 *
 * Example clipping for rectangle at (-10, 50, 100, 100):
 *   - Original: x=-10, y=50, width=100, height=100
 *   - After left clip: x=0, y=50, width=90, height=100 (width reduced by 10)
 *   - Result: Only visible portion (90×100) is drawn starting at (0,50)
 *
 * Parameters:
 *   fb     - Pointer to framebuffer structure
 *   x      - Top-left X coordinate (can be negative)
 *   y      - Top-left Y coordinate (can be negative)
 *   width  - Rectangle width in pixels
 *   height - Rectangle height in pixels
 *   color  - Fill color (COLOR_BLACK or COLOR_WHITE)
 *
 * Performance:
 *   O(width × height) - Draws height horizontal lines
 *   Typical 100×100 rectangle: ~10ms on Pi Zero W
 *
 * Side effects:
 *   Modifies framebuffer pixels in the specified region
 */
void fb_draw_rect(framebuffer_t *fb, int x, int y, int width, int height, uint8_t color) {
    if (!fb) return;

    /* Clip rectangle to framebuffer bounds to prevent out-of-range access */

    /* Clip left edge: if starts off-screen to the left, adjust x and width */
    if (x < 0) {
        width += x;  /* Reduce width by the negative offset */
        x = 0;       /* Start at left edge of screen */
    }

    /* Clip top edge: if starts off-screen above, adjust y and height */
    if (y < 0) {
        height += y;  /* Reduce height by the negative offset */
        y = 0;        /* Start at top edge of screen */
    }

    /* Clip right edge: if extends beyond right side, reduce width */
    if (x + width > FB_WIDTH) {
        width = FB_WIDTH - x;
    }

    /* Clip bottom edge: if extends beyond bottom, reduce height */
    if (y + height > FB_HEIGHT) {
        height = FB_HEIGHT - y;
    }

    /* Draw rectangle line by line using optimized horizontal line function */
    for (int row = 0; row < height; row++) {
        fb_draw_hline(fb, x, y + row, width, color);
    }
}

/**
 * Invert a region of the framebuffer (used for highlighting)
 *
 * Inverts all pixels in the specified rectangular region (black<->white).
 * Commonly used for menu selection highlighting and visual feedback.
 *
 * Algorithm:
 * 1. Clip region to framebuffer bounds (same as fb_draw_rect)
 * 2. For each pixel in region: read current value, write inverted value
 * 3. Uses read-modify-write pattern which is slower than direct drawing
 *
 * Parameters:
 *   fb     - Pointer to framebuffer structure
 *   x      - Top-left X coordinate
 *   y      - Top-left Y coordinate
 *   width  - Region width in pixels
 *   height - Region height in pixels
 *
 * Performance:
 *   O(width × height) - Read and write each pixel
 *   Slower than fb_draw_rect due to read-modify-write
 *   Typical 100×100 region: ~20ms on Pi Zero W
 *
 * Use cases:
 *   - Menu item selection highlighting
 *   - Button press visual feedback
 *   - Search result highlighting in text
 *
 * Note:
 *   Consider optimizing with byte-level operations if performance
 *   becomes an issue for large regions. Current implementation prioritizes
 *   code clarity over performance.
 */
void fb_invert_region(framebuffer_t *fb, int x, int y, int width, int height) {
    if (!fb) return;

    /* Clip region to framebuffer bounds (same algorithm as fb_draw_rect) */
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if (x + width > FB_WIDTH) {
        width = FB_WIDTH - x;
    }
    if (y + height > FB_HEIGHT) {
        height = FB_HEIGHT - y;
    }

    /* Invert each pixel in the region using read-modify-write pattern */
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int px = x + col;
            int py = y + row;

            /* Read current pixel value */
            int current = fb_get_pixel(fb, px, py);

            /* Write inverted value (black->white, white->black) */
            if (current != -1) {
                fb_set_pixel(fb, px, py,
                           (current == COLOR_BLACK) ? COLOR_WHITE : COLOR_BLACK);
            }
        }
    }
}

/**
 * Copy framebuffer data to external buffer (for display driver)
 */
void fb_copy_to_buffer(framebuffer_t *fb, uint8_t *dest) {
    if (!fb || !dest) return;

    memcpy(dest, fb->data, FB_BUFFER_SIZE);
}
