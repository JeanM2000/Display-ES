#include <Arduino.h>
#include "Util.h"

bool mustRedraw(FLAG arg) {
    static bool redraw = true;
    switch (arg) {
        case FLAG::GET: {
        } break;

        case FLAG::SET: {
            redraw = true;
        } break;

        case FLAG::CLR: {
            redraw = false;
        } break;
    }
    return redraw;
}


bool changeValueUpDownBehaviour(bool arg) {
    static bool invertValueUpDown;
    if (arg) {
        invertValueUpDown = !invertValueUpDown;
    }
    return invertValueUpDown;
}


uint16_t rgb_to_rgb565(uint8_t red, uint8_t green, uint8_t blue) {
    // Convert each channel to the corresponding number of bits
    uint8_t r_565 = (red >> 3) & 0x1F;    // Red channel: 5 bits (0-31)
    uint8_t g_565 = (green >> 2) & 0x3F;  // Green channel: 6 bits (0-63)
    uint8_t b_565 = (blue >> 3) & 0x1F;   // Blue channel: 5 bits (0-31)

    // Combine the channels into a single 16-bit value (RGB565 format)
    return (r_565 << 11) | (g_565 << 5) | b_565;
}
