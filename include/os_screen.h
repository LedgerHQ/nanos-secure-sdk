#pragma once

#include "bolos_target.h"
#include "decorators.h"

#ifdef HAVE_BAGL
#ifdef HAVE_SE_SCREEN
// SYSCALL void screen_write_frame(unsigned char* framebuffer
// PLENGTH(BAGL_WIDTH*BAGL_HEIGHT/8));
/**
 * Initialize the screen driver and blank the screen.
 */
void screen_init(void);
/**
 * Blank the screen buffer but don't update the screen driver just yet.
 */
SYSCALL void screen_clear(void);
/**
 * Require the screen buffer to be pushed into the screen driver.
 */
SYSCALL void screen_update(void);

/**
 * Set screen brightness
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void screen_set_brightness(
    unsigned int percent);
/**
 * Require a specific zone not to be cleared/drawn by any graphic HAL to
 * implement screen overlay
 */
SYSCALL void screen_set_keepout(unsigned int x, unsigned int y,
                                unsigned int width, unsigned int height);
/**
 * Draw the given bitmap, with the given colors and position into the screen
 * buffer. Don't update the screen driver.
 */
SYSCALL void bagl_hal_draw_bitmap_within_rect(
    int x, int y, unsigned int width, unsigned int height,
    unsigned int color_count,
    const unsigned int *colors PLENGTH(color_count * 4),
    unsigned int bit_per_pixel,
    const unsigned char *bitmap PLENGTH(1 + bitmap_length_bits / 8),
    unsigned int bitmap_length_bits);
/**
 * Fill a rectangle with a given color in the screen buffer, but don't update
 * the screen driver.
 */
SYSCALL void bagl_hal_draw_rect(unsigned int color, int x, int y,
                                unsigned int width, unsigned int height);
#endif // HAVE_SE_SCREEN
#endif // HAVE_BAGL
