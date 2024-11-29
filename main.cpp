/*
- Welche Peripherie haben wir jetzt?	Poti, LED, RGB LED(-Strip), 7-Segment, !Sensoren!, 
                                        Irgendwas zum Aktivieren bzw. Sequenz bei DMX (Logik auf anderem ESP,
                                        dieser hier gib nur frei)
- Präsentation vom DMX-Teil?
- Bitmaps
- Anforderung an das Menü?
- Datenstruktur für EPS now
- Doxygen comments in VS Code
- Wie Code in Doku einfügen? Vorschlag: Notepad++ Print to PDF
- Doku in latex, mit overleaf?
- Doku erklärt Ziel, Umsetzung (Bedienungsanleitung), Probleme, Grenzen der Implementation, Code im Anhang
*/




#include <Arduino.h>
#include <ESP32Encoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "SmartButton.h"
#include "ScreenMenu.h"

#define GET_MAC_ADDRESS
// #define SHOW_PROCESSING_TIME

#define SCK 12
#define MOSI 11
#define CS 10
#define DC 7
#define RST -1
#define LED 16

#define NEO_PIXEL_PIN 48

#define ROTARY_SWITCH_PIN 8
#define ROTARY_DT_PIN 18
#define ROTARY_CLK_PIN 17

#define BUTTON_ENTER_PIN 38
#define BUTTON_UP_PIN 39
#define BUTTON_LEFT_PIN 40
#define BUTTON_DOWN_PIN 41
#define BUTTON_RIGHT_PIN 42

#define PRINT_DEBUG_MS 1000
#define REFESH_RATE_MS 100


// Create instance of hardware specific Adafruit_ST7789 class for controlling display
Adafruit_ST7789 tft = Adafruit_ST7789(CS, DC, RST);
// Create instance of Adafruit_GFX RGB canvas to draw to (used to get rid of flickering)
// as data is immendiately displyed on tft after sending, without buffering
GFXcanvas16 canvas(320, 240);


ESP32Encoder RotaryEncoder;
SmartButton RotaryEncoderButton = SmartButton(ROTARY_SWITCH_PIN, EXTERNAL_PULLUP, LOW);

// Create instances of classes for processing input from buttons (debounce, long presses)
SmartButton buttonUP = SmartButton(BUTTON_UP_PIN, INTERNAL_PULLUP, LOW, true);
SmartButton buttonDOWN = SmartButton(BUTTON_DOWN_PIN, INTERNAL_PULLUP, LOW, true);
SmartButton buttonLEFT = SmartButton(BUTTON_LEFT_PIN, INTERNAL_PULLUP, LOW, true);
SmartButton buttonRIGHT = SmartButton(BUTTON_RIGHT_PIN, INTERNAL_PULLUP, LOW, true);
SmartButton buttonENTER = SmartButton(BUTTON_ENTER_PIN, INTERNAL_PULLUP, LOW);

// Create instances of classes for setting up menu
Handler MenuHandler = Handler();

Element ReturnToMainMenu = Element(91 , 48, "Back");

Menu MainMenu = Menu(0, 40, 320, 200, 0x2945, 1, 3);
Element Alerts = Element(320, 60, "Alerts");
Element enterRGBMenu = Element(320, 50, "Onboard RGB LED");
Element enterSettings = Element(320, 40, "Settings");

Menu RGBMenu = Menu(0, 0, 320, 240, ST77XX_BLACK, 1, 5);
Element RGBEnable = Element(320, 48, "Turn ON/OFF");
Element RGBRed = Element(320, 48, "RED:  ");
Element RGBGreen = Element(320, 48, "GREEN:");
Element RGBBlue = Element(320, 48, "BLUE: ");

Menu Settings = Menu(0, 0, 320, 240, ST77XX_BLACK, 1, 3);
Element BacklightBrightness = Element(320, 40, "Backlight:");
Element MacAddress = Element(320, 40, "MAC: 80:65:99:C7:AA:AC");

void setup() {
    // Setup Serial Monitor:
    Serial.begin(115200);

    // Setup WiFi:
    WiFi.mode(WIFI_STA);

    #ifdef GET_MAC_ADDRESS
    Serial.print("My WiFi MAC Adddress is: ");
    Serial.println(WiFi.macAddress());
    #endif


    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }


    // Rotary Encoder
    RotaryEncoder.attachHalfQuad(ROTARY_DT_PIN, ROTARY_CLK_PIN);
    RotaryEncoder.clearCount();
    RotaryEncoderButton.init();


    // Setup Buttons for user input:
    buttonUP.init();
    buttonDOWN.init();
    buttonLEFT.init();
    buttonRIGHT.init();
    buttonENTER.init();

    // Pin for controlling tft backlight
    pinMode(LED, OUTPUT);
    analogWriteFrequency(100000);
    analogWrite(LED, 255);

    // Setup TFT Display:
    tft.init(240, 320);
    tft.setSPISpeed(40000000);
    tft.invertDisplay(false);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);

    // Customise element properties:
    // entry1.BorderThickness(1);
    // entry1.BorderRadius(1);


    // Setup Menu:
    MenuHandler.add(&MainMenu);
    MainMenu.add(&Alerts);
    MainMenu.add(&enterRGBMenu);
    MainMenu.add(&enterSettings);
    // MainMenu.add(&entry4);

    MenuHandler.add(&RGBMenu);
    RGBMenu.add(&ReturnToMainMenu);
    ReturnToMainMenu.getsetSelectedColor(ST77XX_WHITE);
    RGBMenu.add(&RGBEnable);
    RGBMenu.add(&RGBRed);
    RGBMenu.add(&RGBGreen);
    RGBMenu.add(&RGBBlue);
    RGBRed.getsetColor(ST77XX_RED);
    RGBRed.getsetSelectedColor(ST77XX_RED);
    RGBRed.initValue(255, 0, 255, SHOW::BOTH, 3);
    RGBRed.changeValueWithInput(true);
    RGBGreen.getsetColor(ST77XX_GREEN);
    RGBGreen.getsetSelectedColor(ST77XX_GREEN);
    RGBGreen.initValue(0, 0, 255, SHOW::BOTH, 3);
    RGBGreen.changeValueWithInput(true);
    RGBBlue.getsetColor(ST77XX_BLUE);
    RGBBlue.getsetSelectedColor(ST77XX_BLUE);
    RGBBlue.initValue(64, 0, 255, SHOW::BOTH, 3);
    RGBBlue.changeValueWithInput(true);

    MenuHandler.add(&Settings);
    Settings.add(&ReturnToMainMenu);
    Settings.add(&BacklightBrightness);
    Settings.add(&MacAddress);
    BacklightBrightness.initValue(255, 0, 255, SHOW::BAR);
    BacklightBrightness.changeValueWithInput(true);
    BacklightBrightness.getsetValueStep(5);
    // Just some Testing:
    // String test = "Hello World!";
    // tft.setCursor(0, 0);
    // tft.setTextColor(ST77XX_BLUE);
    // tft.setTextSize(3);
    // tft.println(test);
    // tft.print(test.length());
    // while(1);
}


void loop() {
    #ifdef SHOW_PROCESSING_TIME
    uint32_t timeProcessStart = micros();
    #endif
    // This takes (all if queries with alteast one being true): ~0.1ms
    // +---------------------------+
    // | READ ROTARY ENCODER INPUT |
    // +---------------------------+
    // Rotary encoder turned to the right
    if (RotaryEncoder.getCount()/2 > 0) {
        changeValueUpDownBehaviour(true);
        MenuHandler.userInput(ACTION::DOWN);
        changeValueUpDownBehaviour(true);
        RotaryEncoder.clearCount();
    }
    // Rotary encoder turned to the left
    if (RotaryEncoder.getCount()/2 < 0) {
        changeValueUpDownBehaviour(true);
        MenuHandler.userInput(ACTION::UP);
        changeValueUpDownBehaviour(true);
        RotaryEncoder.clearCount();
    }
    // Rotary encoder button pressed
    if (RotaryEncoderButton.read()) {
        MenuHandler.userInput(ACTION::ENTER);
    }



    // +------------------------------------------------------+
    // | READ ALL THE BUTTONS AND SEND ACTION TO MENU HANDLER |
    // +------------------------------------------------------+
    if (buttonUP.read())
        MenuHandler.userInput(ACTION::UP);
    if (buttonDOWN.read())
        MenuHandler.userInput(ACTION::DOWN);
    if (buttonLEFT.read())
        MenuHandler.userInput(ACTION::LEFT);
    if (buttonRIGHT.read())
        MenuHandler.userInput(ACTION::RIGHT);
    if (buttonENTER.read())
        MenuHandler.userInput(ACTION::ENTER);
    
    // +----------------------------------------+
    // | SPECIFY THE FUNCTIONALITY OF THE MENUS |
    // +----------------------------------------+
    if (ReturnToMainMenu.justActivated()) {
        MenuHandler.changeShownMenu(&MainMenu);
        ReturnToMainMenu.deactivate();
    }
    
    // MAIN MENU:
    if (MainMenu.justActivated()) {
        canvas.fillRect(0, 0, 320, 40, ST77XX_BLACK);
    }
    if (MainMenu.isActive()) {
        // Scroll Text outside the actual menu
        static uint32_t timeScrollText;
        if (millis() >= timeScrollText) {
            static int16_t xPos = 320;
            canvas.setTextWrap(false);
            canvas.setCursor(xPos, 5);
            canvas.setTextColor(0xd8e3, 0x0);
            canvas.setTextSize(4);
            canvas.print("Hauptmen");
            canvas.write(0x81);
            canvas.setTextWrap(true);
            xPos -= 2;
            if (xPos < -216) {
                xPos = 320;
            }
            timeScrollText = millis() + 100;
            mustRedraw(FLAG::SET);
        }
        // Alert the user that there are alerts (there are actually none, just a demo)
        static uint32_t timeAlertUser;
        static boolean switchAlertUser;
        if (millis() >= timeAlertUser) {
            if (switchAlertUser) {
                Alerts.getsetColor(0xd8e3);
                Alerts.getsetSelectedColor(0xd8e3);
                Alerts.getsetBorderOffset(3);
                Alerts.getsetBorderThickness(DEFAULT_BORDER_THICKNESS + 2);
            } else {
                Alerts.getsetColor(DEFAULT_COLOR);
                Alerts.getsetSelectedColor(DEFAULT_SELECTED_COLOR);
                Alerts.getsetBorderOffset(DEFAULT_BORDER_OFFSET);
                Alerts.getsetBorderThickness(DEFAULT_BORDER_THICKNESS);
            }
            timeAlertUser = millis() + 500;
            switchAlertUser = !switchAlertUser;
        }
    }
    // Make Alerts Element not activatable
    if (Alerts.justActivated()) {
        Alerts.deactivate();
    }

    // Open RGB Menu if corresponding Element gets activated
    if (enterRGBMenu.justActivated()) {
        MenuHandler.changeShownMenu(&RGBMenu);
        enterRGBMenu.deactivate();
    }

    // Open Settings menu
    if (enterSettings.justActivated()) {
        MenuHandler.changeShownMenu(&Settings);
        enterSettings.deactivate();
    }


    // RGB MENU:
    if (RGBMenu.isActive()) {
        // Adjust Color of RGB Enable Element
        static uint16_t oldColor;
        uint16_t newColor = rgb_to_rgb565(RGBRed.getValue(), RGBGreen.getValue(), RGBBlue.getValue());
        if (oldColor != newColor) {
            RGBEnable.getsetColor(newColor);
            RGBEnable.getsetSelectedColor(newColor);
            RGBEnable.getsetActiveColor(newColor);
            oldColor = newColor;
        }
    }
    // Send data to RGB LED
    if (RGBEnable.isActive()) {
        neopixelWrite(NEO_PIXEL_PIN, RGBRed.getValue()/8, RGBGreen.getValue()/8, RGBBlue.getValue()/8);
    } else {
        neopixelWrite(NEO_PIXEL_PIN, 0, 0, 0);
    }
    // Increase impulse interval then changing RGB values
    if (RGBRed.justActivated() || RGBGreen.justActivated() || RGBBlue.justActivated()) {
        buttonUP.ImpulseInterval(50);
        buttonDOWN.ImpulseInterval(50);
        buttonLEFT.ImpulseInterval(50);
        buttonRIGHT.ImpulseInterval(50);
    }
    // Increse value step if button is held down
    if (buttonUP.isHeldDown() || buttonDOWN.isHeldDown() || buttonLEFT.isHeldDown() || buttonRIGHT.isHeldDown()) {
        RGBRed.getsetValueStep(5);
        RGBGreen.getsetValueStep(5);
        RGBBlue.getsetValueStep(5);
    } else {
        RGBRed.getsetValueStep(1);
        RGBGreen.getsetValueStep(1);
        RGBBlue.getsetValueStep(1);
    }
    // Reset impulse interval then not changing RGB values
    if (RGBRed.justDeactivated() || RGBGreen.justDeactivated() || RGBBlue.justDeactivated()) {
        buttonUP.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonDOWN.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonLEFT.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonRIGHT.ImpulseInterval(TIME_DEFAULT_INTERVAL);
    }


    // SETTINGS MENU:
    if (BacklightBrightness.isActive()) {
        analogWrite(LED, BacklightBrightness.getValue());
    }
    if (BacklightBrightness.justActivated()) {
        buttonUP.ImpulseInterval(20);
        buttonDOWN.ImpulseInterval(20);
        buttonLEFT.ImpulseInterval(20);
        buttonRIGHT.ImpulseInterval(20);
    }
    if (BacklightBrightness.justDeactivated()) {
        buttonUP.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonDOWN.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonLEFT.ImpulseInterval(TIME_DEFAULT_INTERVAL);
        buttonRIGHT.ImpulseInterval(TIME_DEFAULT_INTERVAL);
    }

    if (MacAddress.justActivated()) {
        MacAddress.deactivate();
    }



    // +----------------------------+
    // | PROCESS THE MENU ON SCREEN |
    // +----------------------------+
    // This takes: ~10-12ms if canvas is redrawn
    MenuHandler.process();
    // Push canvas Bitmap to tft display every REFESH_RATE_MS milli seconds:
    // This takes: ~41ms
    // static uint32_t timeUpdate;
    // if (millis() > timeUpdate) {
    //     tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
    //     timeUpdate = millis() + REFESH_RATE_MS;
    // }
    if (mustRedraw(FLAG::GET)) {
        tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
        mustRedraw(FLAG::CLR);
    }






    // +--------------------+
    // | CODE FOR DEBUGGING |
    // +--------------------+
    while (Serial.available()) {
        char incomingChar = Serial.read();
        switch (incomingChar) {
            case 'w': {
                MainMenu.userInput(ACTION::UP);
            } break;
            case 's': {
                MainMenu.userInput(ACTION::DOWN);
            } break;
            case 'a': {
                MainMenu.userInput(ACTION::LEFT);
            } break;
            case 'd': {
                MainMenu.userInput(ACTION::RIGHT);
            } break;
            case ' ': {
                changeValueUpDownBehaviour(true);
            } break;
        }
    }

    // if (entry1.justActivated())
    //     Serial.println("Just Activated!");
    // if (entry1.justDeactivated())
    //     Serial.println("Just Deactivated!");


    // static uint32_t timePrintDebug;
    // if (millis() >= timePrintDebug) {
    //     Serial.println(redraw);
    //     timePrintDebug = millis() + PRINT_DEBUG_MS;
    // }


    // static uint8_t count;
    // if (buttonUP.read()) {
    //     count++;
    //     Serial.println(count);
    // }


    // bool prevButtonState;
    // bool ButtonState = buttonUP.read();
    // if (prevButtonState != ButtonState) {
    //     Serial.print(ButtonState);
    // }
    // prevButtonState = ButtonState;


    #ifdef SHOW_PROCESSING_TIME
    uint32_t timeProcessDuration = micros() - timeProcessStart;
    if (timeProcessDuration > 150) {
        Serial.println(timeProcessDuration);
    }
    #endif
}