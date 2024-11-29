#pragma once
#include <Arduino.h>
#include "ScreenMenu.h"
#include "Menu.h"
#include "Util.h"

// Class for handling all menus
class Handler {

public:
    Handler();

    void add(Menu *m);
    void process();
    void changeShownMenu(Menu *showThisMenu);
    
    void userInput(ACTION act);
    void toggleActive();

    Menu getActiveMenu();


private:
    int8_t getMenuID(Menu *m);


    uint8_t menuID = 0;
    uint8_t activeMenuID = 0;
    Menu *menus[MENU_COUNT];
};