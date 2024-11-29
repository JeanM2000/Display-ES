#pragma once
#include <Arduino.h>

enum class FLAG {
    GET,
    SET,
    CLR
};

bool mustRedraw(FLAG arg);
bool changeValueUpDownBehaviour(bool arg = false);
uint16_t rgb_to_rgb565(uint8_t red, uint8_t green, uint8_t blue);