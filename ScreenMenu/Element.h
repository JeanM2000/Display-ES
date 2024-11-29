#pragma once
#include <Arduino.h>
#include "ScreenMenu.h"

enum class ACTION {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER
};

enum class SHOW {
    NONE,
    VALUE,
    BAR,
    BOTH
};

enum class ELEM_STATE {
    NORMAL,
    SELECTED,
    ACTIVE
};

struct grid_s {
    uint8_t x;
    uint8_t y;
};

struct ElementProperties_s {
    int16_t Width;
    int16_t Height;
    String Text;
    uint8_t TextSize;
    uint16_t Color;
    uint16_t ColorSelected;
    uint16_t ColorActive;
    uint8_t BorderOffset;
    uint8_t BorderThickness;
    uint8_t BorderRadius;
};

struct values_s {
    SHOW visiable;
    uint8_t digits;
    int16_t iVal;
    int16_t iLowLim;
    int16_t iUpLim;
    int16_t iStep = 1;
};


class Element {

public:
    // Constructor
    // Element(uint16_t Width, uint16_t Height, String Title, void (*PointerToActionFunction)() = &NoAction);
    Element(uint16_t Width, uint16_t Height, String Title);
    Element() {}

    // Default action function
    // static void NoAction() {}

    // Setting up element

    // void passRenderer(RENDER_CLASS_TYPE *Renderer);
    // void passCoordinates(uint16_t x, uint16_t y);


    // Displaying element

    void display(uint16_t x, uint16_t y);


    // Controlling the element

    void toggleSelect();
    void select();
    void deselect();
    void toggleActive();
    void activate();
    void deactivate();

    boolean isActive();
    boolean justActivated();
    boolean justDeactivated();


    // Change rendering properties (0 returns current value)

    uint8_t getsetTextSize(uint8_t size = 0);
    uint8_t getsetBorderOffset(uint8_t pixels = 0);
    uint8_t getsetBorderThickness(uint8_t pixels = 0);
    uint8_t getsetBorderRadius(uint8_t pixels = 0);
    uint16_t getsetColor(uint16_t color = 0x0);
    uint16_t getsetSelectedColor(uint16_t color = 0x0);
    uint16_t getsetActiveColor(uint16_t color = 0x0);


    // Get element properties

    uint16_t getWidth();
    uint16_t getHeight();


    // Values attached to element

    void initValue(int16_t value, int16_t lowerBound, int16_t upperBound, SHOW disp = SHOW::BAR, uint8_t digits = 0);
    void ValueShow(SHOW disp);
    void setValue(int16_t value);
    int16_t getValue();
    int16_t getsetValueStep(int16_t step = 0);
    boolean changeValueWithInput(boolean arg = false);


private:
    ElementProperties_s prop;
    values_s vals;
    boolean allowValChange;

    // void (*funcPtr)();

    boolean selected;
    boolean active, prevActive, prevDeactive;
};

