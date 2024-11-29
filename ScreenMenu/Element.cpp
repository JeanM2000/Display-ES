#include <Arduino.h>
#include "ScreenMenu.h"


// Element::Element(uint16_t Width, uint16_t Height, String Title, void (*PointerToActionFunction)()) {
Element::Element(uint16_t Width, uint16_t Height, String Title) {
    prop.Width = Width;
    prop.Height = Height;
    prop.Text = Title;
    prop.TextSize = (Height - (6*DEFAULT_BORDER_OFFSET + 2*DEFAULT_BORDER_THICKNESS)) / 8;
    prop.Color = DEFAULT_COLOR;
    prop.ColorSelected = DEFAULT_SELECTED_COLOR;
    prop.ColorActive = DEFAULT_ACTIVE_COLOR;
    prop.BorderOffset = DEFAULT_BORDER_OFFSET;
    prop.BorderThickness = DEFAULT_BORDER_THICKNESS;
    prop.BorderRadius = DEFAULT_BORDER_RADIUS;

    // funcPtr = PointerToActionFunction;
}

// Passes the renderer class to the element class
// void Element::passRenderer(RENDER_CLASS_TYPE *Renderer) {
//     r = Renderer;
// }


// void Element::passCoordinates(uint16_t x, uint16_t y) {
//     cord.x = x;
//     cord.y = y;
// }

// Displays the Element on the screen
void Element::display(uint16_t x, uint16_t y) {
    if (selected && active) {
        drawActiveSelectedElement(x, y, prop, vals);
    } else if (selected) {
        drawSelectedElement(x, y, prop, vals);
    } else if (active) {
        drawActiveElement(x, y, prop, vals);
    } else {
        drawElement(x, y, prop, vals);
    }
}


// !!! Should only be called by menu class !!!
void Element::toggleSelect() {
    selected = !selected;
    mustRedraw(FLAG::SET);
}

// !!! Should only be called by menu class !!!
void Element::select() {
    selected = true;
    mustRedraw(FLAG::SET);
}

// !!! Should only be called by menu class !!!
void Element::deselect() {
    selected = false;
    mustRedraw(FLAG::SET);
}

void Element::toggleActive() {
    active = !active;
    mustRedraw(FLAG::SET);
}

void Element::activate() {
    active = true;
    // (funcPtr)();
    mustRedraw(FLAG::SET);
}

void Element::deactivate() {
    active = false;
    // (funcPtr)();
    mustRedraw(FLAG::SET);
}

boolean Element::isActive() {
    return active;
}

boolean Element::justActivated() {
    if (!prevActive && active) {
        prevActive = active;
        return true;
    }
    prevActive = active;
    return false;
}

boolean Element::justDeactivated() {
    if (prevDeactive && !active) {
        prevDeactive = active;
        return true;
    }
    prevDeactive = active;
    return false;
}


uint8_t Element::getsetTextSize(uint8_t size) {
    if (size != 0) {
        prop.TextSize = size;
        mustRedraw(FLAG::SET);
    }
    return prop.TextSize;
}


// Changes the offset from border to edge of element; if called without argument returns current offset
uint8_t Element::getsetBorderOffset(uint8_t pixels) {
    if (pixels != 0) {
        prop.BorderOffset = pixels;
        mustRedraw(FLAG::SET);
    }
    return prop.BorderOffset;
}

// Changes the thickness of element border; if called without argument returns current thickness
uint8_t Element::getsetBorderThickness(uint8_t pixels) {
    if (pixels != 0) {
        prop.BorderThickness = pixels;
        mustRedraw(FLAG::SET);
    }
    return prop.BorderOffset;
}


uint8_t Element::getsetBorderRadius(uint8_t pixels) {
    if (pixels != 0) {
        prop.BorderRadius = pixels;
        mustRedraw(FLAG::SET);
    }
    return prop.BorderRadius;
}

uint16_t Element::getsetColor(uint16_t color) {
    if (color != 0x0) {
        prop.Color = color;
        mustRedraw(FLAG::SET);
    }
    return prop.Color;
}

// Changes the color of element (border) then selected; if called without argument returns current color
uint16_t Element::getsetSelectedColor(uint16_t color) {
    if (color != 0x0) {
        prop.ColorSelected = color;
        mustRedraw(FLAG::SET);
    }
    return prop.ColorSelected;
}

// Changes the color of element (filler) then active ; if called without argument returns current color
uint16_t Element::getsetActiveColor(uint16_t color) {
    if (color != 0x0) {
        prop.ColorActive = color;
        mustRedraw(FLAG::SET);
    }
    return prop.ColorActive;
}

// Returns the Width of the element
uint16_t Element::getWidth() {
    return prop.Width;
}

// Returns the Height of the element
uint16_t Element::getHeight() {
    return prop.Height;
}

// digitCount includes minus-sign if values can be negative
void Element::initValue(int16_t value, int16_t lowerBound, int16_t upperBound, SHOW disp, uint8_t digitCount) {
    vals.iVal = value;
    vals.iLowLim = lowerBound;
    vals.iUpLim = upperBound;
    vals.visiable = disp;
    vals.digits = digitCount;
}


boolean Element::changeValueWithInput(boolean arg) {
    if (arg)
        allowValChange = true;
    return allowValChange;
}


void Element::ValueShow(SHOW disp) {
    vals.visiable = disp;
    mustRedraw(FLAG::SET);
}


void Element::setValue(int16_t value) {
    if (value < vals.iLowLim) {
        vals.iVal = vals.iLowLim;
    } else if (value > vals.iUpLim) {
        vals.iVal = vals.iUpLim;
    } else {
        vals.iVal = value;
    }
    mustRedraw(FLAG::SET);
}

int16_t Element::getValue() {
    return vals.iVal;
}

int16_t Element::getsetValueStep(int16_t step) {
    if (step != 0)
        vals.iStep = step;
    return vals.iStep;
}