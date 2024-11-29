#pragma once
#include <Arduino.h>
#include "ScreenMenu.h"
#include "Element.h"

class Menu {
    struct selectedElem_s {
        uint8_t ID;
        uint8_t grid_x;
        uint8_t grid_y;
    };

    public:
        Menu(uint16_t xPos, uint16_t yPos, uint16_t Width, uint16_t Height, uint16_t Color,
                uint8_t Colums, uint8_t Rows);
        Menu() {}

        void display();
        void add(Element *elem);

        boolean userInput(ACTION act);
        void toggleActive();

        boolean isActive();
        boolean justActivated();
        boolean justDeactivated();

        grid_s getCursorPos();
        uint16_t getMenuColor();
    

    private:
        Element *elems[ENTRY_COUNT];
        uint8_t nextElemID = 0;
        grid_s nextGridPos;
        grid_s grid[ENTRY_COUNT];
        uint8_t elemIDgridxy[MAX_ROW_COUNT][MAX_COLUMN_COUNT];
        uint8_t selectedElemID = 0;

        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t c;
        uint8_t row;
        uint8_t col;

        grid_s cursor;

        boolean active, prevActive;
};