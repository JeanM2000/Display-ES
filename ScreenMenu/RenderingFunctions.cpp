#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "ScreenMenu.h"

/* 
Adjust the code in these functions to draw elements on your specific hardware. The default code
provided here is intended to draw to a canvas "GFXcanvas16" from the AdafruitGFX library. Take a look
at the README file for the code you should put in your main.cpp to use this default configuration.
*/


// Tell the compiler what screen to draw to. Usually this was already defined in main.cpp.
extern GFXcanvas16 canvas;
extern Handler MenuHandler;


// Draws the background color of the menu
void drawMenuBackground(int16_t x0, int16_t y0, int16_t Width, int16_t Height, uint16_t ColorBackground) {
    canvas.fillRect(x0, y0, Width, Height, ColorBackground);
}


void drawText(int16_t x0, int16_t y0, ElementProperties_s elem, String text, uint16_t color) {
    if (elem.Text != "") {
        canvas.setCursor(x0 + 3*elem.BorderOffset + elem.BorderThickness,
            y0 + ((elem.Height/2) - (7*elem.TextSize)/2));
        canvas.setTextColor(color);
        canvas.setTextSize(elem.TextSize);
        canvas.print(text);
    }
}


void drawValue(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals, uint16_t color) {
    int16_t newx0 = x0 + 6*elem.TextSize*elem.Text.length() + 2*elem.BorderOffset;
    if (elemVals.visiable == SHOW::VALUE || elemVals.visiable == SHOW::BOTH) {
        drawText(newx0, y0, elem, String(elemVals.iVal), color);
    }
    if (elemVals.visiable == SHOW::BAR || elemVals.visiable == SHOW::BOTH) {
        int16_t barX = newx0 + (3+2)*elem.BorderOffset + elem.BorderThickness + 6*elem.TextSize*elemVals.digits;
        int16_t barY = y0 + 3*elem.BorderOffset + elem.BorderThickness;
        int16_t barWidth = elem.Width - (3*elem.BorderOffset + elem.BorderThickness) - barX;
        int16_t barHeight = elem.Height - 2*(3*elem.BorderOffset + elem.BorderThickness);
        canvas.drawRect(barX, barY, barWidth, barHeight, color);
        canvas.fillRect(barX, barY,
            (barWidth * (elemVals.iVal + abs(elemVals.iLowLim)))/
            (elemVals.iUpLim + abs(elemVals.iLowLim)),
            barHeight, color);
    }
}


// A "normal" (not selected nor active) element is drawn without an outline and it's default color
void drawElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals) {
    // Draw the element title (7 pixels is the height of an letter of the smallest text size)
    drawText(x0, y0, elem, elem.Text, elem.Color);

    drawValue(x0, y0, elem, elemVals, elem.Color);
}


// A "selected" (cursor currently at this element's position) element is drawn with an outline and the "selected color"
void drawSelectedElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals) {
    // Draw an outline around the element in the "selected color"
    // First draw a filled in rectangle in the "selected color"
    canvas.fillRoundRect(x0 + elem.BorderOffset, y0 + elem.BorderOffset,
        elem.Width - 2*elem.BorderOffset, elem.Height - 2*elem.BorderOffset,
        elem.BorderRadius, elem.ColorSelected);
    uint8_t newOffset = elem.BorderThickness + elem.BorderOffset;
    // Then darw a filled in rectangle in the menu background color to get
    // an border around the element in the "selected color"
    canvas.fillRoundRect(x0 + newOffset, y0 + newOffset,
        elem.Width - 2*newOffset, elem.Height - 2*newOffset,
        elem.BorderRadius - elem.BorderThickness, MenuHandler.getActiveMenu().getMenuColor());

    // Draw the element title (7 pixels is the height of an letter of the smallest text size)
    drawText(x0, y0, elem, elem.Text, elem.ColorSelected);

    drawValue(x0, y0, elem, elemVals, elem.ColorSelected);
}


// An "active" (function corresponding to this element is active) element is drawn in inverse and without an outline
void drawActiveElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals) {
    // Draw the filled rectangle inside the element border (therefor calculate a new offset)
    uint8_t newOffset = elem.BorderThickness + elem.BorderOffset;
    canvas.fillRoundRect(x0 + newOffset, y0 + newOffset,
        elem.Width - 2*newOffset, elem.Height - 2*newOffset, 
        elem.BorderRadius, elem.ColorActive);

    // Draw the element title (7 pixels is the height of an letter of the smallest text size)
    drawText(x0, y0, elem, elem.Text, MenuHandler.getActiveMenu().getMenuColor());

    drawValue(x0, y0, elem, elemVals, MenuHandler.getActiveMenu().getMenuColor());
}


// If the element is active and selected, it's drawn in inverse, with the "selected color" and an outline 
void drawActiveSelectedElement(int16_t x0, int16_t y0, ElementProperties_s elem, values_s elemVals) {
    // Draw the filled rectangle with the element border
    canvas.fillRoundRect(x0 + elem.BorderOffset, y0 + elem.BorderOffset,
        elem.Width - 2*elem.BorderOffset, elem.Height - 2*elem.BorderOffset, 
        elem.BorderRadius, elem.ColorSelected);

    // Draw the element title (7 pixels is the height of an letter of the smallest text size)
    drawText(x0, y0, elem, elem.Text, MenuHandler.getActiveMenu().getMenuColor());

    drawValue(x0, y0, elem, elemVals, MenuHandler.getActiveMenu().getMenuColor());
}