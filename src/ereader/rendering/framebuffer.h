/*
 * framebuffer.h - Low-Level Framebuffer Operations
 *
 * Provides basic framebuffer operations for the e-reader application.
 * The framebuffer is a 1-bit-per-pixel buffer (400x300) for the e-paper display.
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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

/* Display Specifications */
#define FB_WIDTH        400
#define FB_HEIGHT       300
#define FB_BUFFER_SIZE  ((FB_WIDTH * FB_HEIGHT) / 8)  /* 15000 bytes, 1 bit per pixel */

/* Color Definitions (for 1-bit display) */
#define COLOR_WHITE     0xFF
#define COLOR_BLACK     0x00

/* Framebuffer Structure */
typedef struct {
    uint8_t data[FB_BUFFER_SIZE];    /* Raw framebuffer data (1-bit per pixel) */
    uint16_t width;                   /* Width in pixels (400) */
    uint16_t height;                  /* Height in pixels (300) */
} framebuffer_t;

/**
 * Initialize a framebuffer
 * @param fb: Pointer to framebuffer structure
 */
void fb_init(framebuffer_t *fb);

/**
 * Clear the framebuffer to a specific color
 * @param fb: Pointer to framebuffer structure
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void fb_clear(framebuffer_t *fb, uint8_t color);

/**
 * Set a pixel in the framebuffer
 * @param fb: Pointer to framebuffer structure
 * @param x: X coordinate (0-399)
 * @param y: Y coordinate (0-299)
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void fb_set_pixel(framebuffer_t *fb, int x, int y, uint8_t color);

/**
 * Get a pixel value from the framebuffer
 * @param fb: Pointer to framebuffer structure
 * @param x: X coordinate (0-399)
 * @param y: Y coordinate (0-299)
 * @return: COLOR_WHITE or COLOR_BLACK, or -1 if out of bounds
 */
int fb_get_pixel(framebuffer_t *fb, int x, int y);

/**
 * Draw a horizontal line
 * @param fb: Pointer to framebuffer structure
 * @param x: Starting X coordinate
 * @param y: Y coordinate
 * @param width: Line width in pixels
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void fb_draw_hline(framebuffer_t *fb, int x, int y, int width, uint8_t color);

/**
 * Draw a vertical line
 * @param fb: Pointer to framebuffer structure
 * @param x: X coordinate
 * @param y: Starting Y coordinate
 * @param height: Line height in pixels
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void fb_draw_vline(framebuffer_t *fb, int x, int y, int height, uint8_t color);

/**
 * Draw a filled rectangle
 * @param fb: Pointer to framebuffer structure
 * @param x: Top-left X coordinate
 * @param y: Top-left Y coordinate
 * @param width: Rectangle width
 * @param height: Rectangle height
 * @param color: COLOR_WHITE or COLOR_BLACK
 */
void fb_draw_rect(framebuffer_t *fb, int x, int y, int width, int height, uint8_t color);

/**
 * Invert a region of the framebuffer (used for highlighting)
 * @param fb: Pointer to framebuffer structure
 * @param x: Top-left X coordinate
 * @param y: Top-left Y coordinate
 * @param width: Region width
 * @param height: Region height
 */
void fb_invert_region(framebuffer_t *fb, int x, int y, int width, int height);

/**
 * Copy framebuffer data to external buffer (for display driver)
 * @param fb: Pointer to framebuffer structure
 * @param dest: Destination buffer (must be at least FB_BUFFER_SIZE bytes)
 */
void fb_copy_to_buffer(framebuffer_t *fb, uint8_t *dest);

#endif /* FRAMEBUFFER_H */
