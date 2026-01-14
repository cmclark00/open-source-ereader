/*
 * framebuffer.c - Low-Level Framebuffer Operations Implementation
 *
 * Implements basic framebuffer operations for the e-reader application.
 */

#include <string.h>
#include <stdlib.h>
#include "framebuffer.h"

/**
 * Initialize a framebuffer
 */
void fb_init(framebuffer_t *fb) {
    if (!fb) return;

    fb->width = FB_WIDTH;
    fb->height = FB_HEIGHT;
    fb_clear(fb, COLOR_WHITE);
}

/**
 * Clear the framebuffer to a specific color
 */
void fb_clear(framebuffer_t *fb, uint8_t color) {
    if (!fb) return;

    /* Fill entire buffer with color */
    memset(fb->data, color, FB_BUFFER_SIZE);
}

/**
 * Set a pixel in the framebuffer
 *
 * Pixel layout: Each byte contains 8 pixels (left to right)
 * Bit 7 (MSB) = leftmost pixel, Bit 0 (LSB) = rightmost pixel
 * 1 = black, 0 = white (inverse of display driver which uses 0=black, 1=white)
 */
void fb_set_pixel(framebuffer_t *fb, int x, int y, uint8_t color) {
    if (!fb) return;

    /* Bounds checking */
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) {
        return;
    }

    /* Calculate byte offset and bit position */
    int byte_index = (y * FB_WIDTH + x) / 8;
    int bit_offset = 7 - (x % 8);  /* MSB = leftmost pixel */

    /* Set or clear the bit based on color */
    if (color == COLOR_BLACK) {
        fb->data[byte_index] |= (1 << bit_offset);   /* Set bit (black) */
    } else {
        fb->data[byte_index] &= ~(1 << bit_offset);  /* Clear bit (white) */
    }
}

/**
 * Get a pixel value from the framebuffer
 */
int fb_get_pixel(framebuffer_t *fb, int x, int y) {
    if (!fb) return -1;

    /* Bounds checking */
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) {
        return -1;
    }

    /* Calculate byte offset and bit position */
    int byte_index = (y * FB_WIDTH + x) / 8;
    int bit_offset = 7 - (x % 8);

    /* Return pixel value */
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
 */
void fb_draw_rect(framebuffer_t *fb, int x, int y, int width, int height, uint8_t color) {
    if (!fb) return;

    /* Clip rectangle to framebuffer bounds */
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

    /* Draw rectangle line by line */
    for (int row = 0; row < height; row++) {
        fb_draw_hline(fb, x, y + row, width, color);
    }
}

/**
 * Invert a region of the framebuffer (used for highlighting)
 */
void fb_invert_region(framebuffer_t *fb, int x, int y, int width, int height) {
    if (!fb) return;

    /* Clip region to framebuffer bounds */
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

    /* Invert each pixel in the region */
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int px = x + col;
            int py = y + row;
            int current = fb_get_pixel(fb, px, py);
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
