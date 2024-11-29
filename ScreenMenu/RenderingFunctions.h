#pragma once
#include <Arduino.h>
#include "ScreenMenu.h"
#include "Element.h"

void drawMenuBackground(int16_t x0, int16_t y0, int16_t Width, int16_t Height, uint16_t ColorBackground);
void drawText(int16_t x0, int16_t y0, ElementProperties_s elem, String text, uint16_t color);
void drawElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals);
void drawSelectedElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals);
void drawActiveElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals);
void drawActiveSelectedElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals);