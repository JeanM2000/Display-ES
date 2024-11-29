#pragma once

#define ENABLE_DEBUG_MESSAGES

#define RENDER_CLASS_TYPE GFXcanvas16

// Given values by hardware (I think this is obsolete)
// #define SCREEN_HIGHT 240
// #define SCREEN_WIDTH 320

// Rendering options
#define DEFAULT_BORDER_OFFSET 3    // in Pixels
#define DEFAULT_BORDER_THICKNESS 2 // in Pixels
#define DEFAULT_BORDER_RADIUS 8    // in Pixels

#define DEFAULT_COLOR 0xffff            // in RGB565 format
#define DEFAULT_SELECTED_COLOR 0x35e8   // in RGB565 format
#define DEFAULT_ACTIVE_COLOR 0xffff     // in RGB565 format

// Indiviual values to avoid dynamic memory allocation (beyond the scope of this project)
#define MENU_COUNT 4        // number of ALL menus
#define ENTRY_COUNT 20      // number of ALL elements
#define MAX_ROW_COUNT 10    // number of MAXIMUM row count in a menu
#define MAX_COLUMN_COUNT 3  // number of MAXIMUM column count in a menu





// Element width for only text: TextSize*(charAmount*6-1)+2*(3*borderOffset+borderThickness)