#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "ScreenMenu.h"

// Constructor for Handler class
Handler::Handler() {
    static bool HandlerCreated;
    // Check if a Handler was already created, there should only be one!
    if (HandlerCreated) {
        // ### Can't print error message because serial monitor is not initialised yet! ###
        // #ifdef ENABLE_DEBUG_MESSAGES
        // Serial.println("Critical Error! Handler already created, there should only be one!");
        // #endif
        while(1) {}
    }
    HandlerCreated = true;
}


// Adds a Menu to the Menu Handler, takes pointer to a menu class as argument
void Handler::add(Menu *m) {
    // Add menu to array of pointers to menus
    menus[menuID] = m;

    if (menuID == 0) {  // Select first element
        m->toggleActive();
    }

    // Debug messages on serial monitor
    #ifdef ENABLE_DEBUG_MESSAGES
    Serial.print("Added Menu with ID: ");
    Serial.println(menuID);
    #endif

    // increment menu id for next menu to be added
    menuID++;
}


// Call this function in the main program loop!
void Handler::process() {
    if (mustRedraw(FLAG::GET)) {
        menus[activeMenuID]->display();
        #ifdef ENABLE_DEBUG_MESSAGES
        Serial.println("Just redrawn!");
        #endif
    }
}


void Handler::changeShownMenu(Menu *showThisMenu) {
    int8_t newID = getMenuID(showThisMenu);
    if (newID >= 0) {
        menus[activeMenuID]->toggleActive();
        activeMenuID = newID;
        menus[activeMenuID]->toggleActive();
        mustRedraw(FLAG::SET);
    } else {
        #ifdef ENABLE_DEBUG_MESSAGES
        Serial.println("Menu was not found in this Handler!");
        #endif
    }
}


// Noves coursor in given direction
void Handler::userInput(ACTION act) {
    menus[activeMenuID]->userInput(act);
    mustRedraw(FLAG::SET);
}


// Toggles active state of element currently selected by cursour
void Handler::toggleActive() {
    menus[activeMenuID]->toggleActive();
    mustRedraw(FLAG::SET);
}


Menu Handler::getActiveMenu() {
    return *menus[activeMenuID];
}


/**********************************************************************/
/*!
    @brief      Returns ID of menu
    @param      ofThisMenu  Pointer to menu you want the ID from
    @returns    Menu ID, -1 if menu was not found in this handler
*/
/**********************************************************************/
int8_t Handler::getMenuID(Menu *ofThisMenu) {
    // Search menu in array of pointers to menu added to this handler
    for (uint8_t i=0 ; i<MENU_COUNT ; i++) {
        if (ofThisMenu == menus[i]) {
            return i;
        }
    }
    // Error, menu was not added to this Handler
    return -1;
}