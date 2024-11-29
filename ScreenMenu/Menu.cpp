#include <Arduino.h>
#include "ScreenMenu.h"


// Constructor of menu class
Menu::Menu(uint16_t xPos, uint16_t yPos, uint16_t Width, uint16_t Height, uint16_t Color,
                uint8_t Colums, uint8_t Rows) {
    x = xPos;
    y = yPos;
    w = Width;
    h = Height;
    c = Color;
    col = Colums;
    row = Rows;
}


void Menu::display() {
    uint16_t new_x = x;
    uint16_t new_y = y;

    // Draw Menu Background
    drawMenuBackground(x, y, w, h, c);

    for (uint8_t i=0 ; i<nextElemID ; i++) {
        // Check if end of colum was reached, jump to next column
        if ((i > 0) && (grid[i].x == 0)) {
            new_x += elems[i - 1]->getWidth();
            new_y = y;
        }
        // Draw element
        elems[i]->display(new_x, new_y);
        // Get new y coordinate to draw next element
        new_y += elems[i]->getHeight();

    }
}


void Menu::add(Element *e) {
    // static uint8_t xOnScreen = x;
    // static uint8_t yOnScreen = y;

    // Check if grid y pos is still in range; if not, menu is full
    if (nextGridPos.y >= col) {
        #ifdef ENABLE_DEBUG_MESSAGES
        Serial.println("Critical Error! Too many elements in this Menu!");
        #endif
        while(1) {}
    }

    
    elems[nextElemID] = e;  // store pointer to element with corresponding element ID

    if (nextElemID == 0) {  // Select first element
        e->select();
    }

    // Link grid position to elem ID and vice versa
    grid[nextElemID].x = nextGridPos.x;
    grid[nextElemID].y = nextGridPos.y;
    elemIDgridxy[nextGridPos.x][nextGridPos.y] = nextElemID;
    

    #ifdef ENABLE_DEBUG_MESSAGES
    static char Print_Buffer[49]; //Buffer for printing to Serial Monitor
    sprintf(Print_Buffer, "Added element with ID:%d on position x=%d, y=%d", nextElemID, nextGridPos.x, nextGridPos.y);
    Serial.println(Print_Buffer);
    #endif



    // // Update x and y position on screen
    // if (elemID > 0 && grid[elemID].x == 0) {
    //     xOnScreen = elems[elemID - 1]->getWidth();
    //     yOnScreen = y;
    // }
    // yOnScreen += elems[elemID]->getHeight();

    // Update grid x and y values
    if (nextGridPos.x < row - 1) { // jump to next row
        nextGridPos.x++;
    } else {    // jump to next column
        nextGridPos.x = 0;
        nextGridPos.y++;
    }

    // increment element ID for next element to be added
    nextElemID++;
}


// return type is not used yet, change later
boolean Menu::userInput(ACTION act) {
    switch (act) {
        case ACTION::UP: {
            if (elems[selectedElemID]->changeValueWithInput() && elems[selectedElemID]->isActive()) {
                elems[selectedElemID]->setValue(elems[selectedElemID]->getValue() + 
                ((changeValueUpDownBehaviour()) ? (-1*elems[selectedElemID]->getsetValueStep()) : (elems[selectedElemID]->getsetValueStep())));
                return false;
            }

            if (cursor.x > 0) {
                elems[elemIDgridxy[cursor.x][cursor.y]]->deselect();
                cursor.x--;
                elems[elemIDgridxy[cursor.x][cursor.y]]->select();
                selectedElemID = elemIDgridxy[cursor.x][cursor.y];
                return true;
            }
        } break;

        case ACTION::DOWN: {
            if (elems[selectedElemID]->changeValueWithInput() && elems[selectedElemID]->isActive()) {
                elems[selectedElemID]->setValue(elems[selectedElemID]->getValue() + 
                ((changeValueUpDownBehaviour()) ? (elems[selectedElemID]->getsetValueStep()) : (-1*elems[selectedElemID]->getsetValueStep())));
                return false;
            }

            if (cursor.x < (row - 1)) {
                elems[elemIDgridxy[cursor.x][cursor.y]]->deselect();
                cursor.x++;
                elems[elemIDgridxy[cursor.x][cursor.y]]->select();
                selectedElemID = elemIDgridxy[cursor.x][cursor.y];
                return true;
            }
        } break;

        case ACTION::LEFT: {
            if (elems[selectedElemID]->changeValueWithInput() && elems[selectedElemID]->isActive()) {
                elems[selectedElemID]->setValue(elems[selectedElemID]->getValue() -
                elems[selectedElemID]->getsetValueStep());
                return false;
            }

            if (cursor.y > 0) {
                elems[elemIDgridxy[cursor.x][cursor.y]]->deselect();
                cursor.y--;
                elems[elemIDgridxy[cursor.x][cursor.y]]->select();
                selectedElemID = elemIDgridxy[cursor.x][cursor.y];
                return true;
            }
        } break;

        case ACTION::RIGHT: {
            if (elems[selectedElemID]->changeValueWithInput() && elems[selectedElemID]->isActive()) {
                elems[selectedElemID]->setValue(elems[selectedElemID]->getValue() +
                elems[selectedElemID]->getsetValueStep());
                return false;
            }

            if (cursor.y < (col - 1)) {
                elems[elemIDgridxy[cursor.x][cursor.y]]->deselect();
                cursor.y++;
                elems[elemIDgridxy[cursor.x][cursor.y]]->select();
                selectedElemID = elemIDgridxy[cursor.x][cursor.y];
                return true;
            }
        } break;

        case ACTION::ENTER: {
            elems[elemIDgridxy[cursor.x][cursor.y]]->toggleActive();
        } break;
    }
    return false;
}


void Menu::toggleActive() {
    active = !active;
}

boolean Menu::isActive() {
    return active;
}

boolean Menu::justActivated() {
    if (!prevActive && active) {
        prevActive = active;
        return true;
    }
    prevActive = active;
    return false;
}

boolean Menu::justDeactivated() {
    if (prevActive && !active) {
        prevActive = active;
        return true;
    }
    prevActive = active;
    return false;
}


grid_s Menu::getCursorPos() {
    return cursor;
}


uint16_t Menu::getMenuColor() {
    return c;
}