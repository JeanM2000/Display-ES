/* 
This Button class is designed for handling button inputs on an Arduino, specifically
addressing common issues that arise with mechanical buttons, such as debouncing and
long presses. The class provides a robust mechanism for detecting button presses, 
including handling button bounces and generating impulses at regular intervals when
the button is held down. Here's a detailed explanation of its features and use:


KEY FEATURES:
    1) Flexible Pin Modes:
    The class allows different ways to handle the pin's state, including using
    internal pull-up or pull-down resistors (if supported by the hardware) or
    relying on external resistors. It is also possible to defines what voltage
    (HIGH or LOW) at the button's pin will be interpreted as a button press.
    
    2) Debouncing:
    Mechanical buttons tend to produce noisy signals when pressed or released (known
    as "bouncing"), causing multiple triggers instead of just one. The Button class
    addresses this ensuring that only one reading is considered within a certain time
    frame.

    3) Hold Detection generating Impulses:
    The class can detect if the button is being held down. If the flag impulsesOnHold
    is set (true), it generates impulses (events) periodically after a specific
    duration (default 300ms). This duration can be changed via the
    ImpulseInterval(<time in ms>)method. The duration needed for the button to be
    considered held down is configurable via the TimeToEnterHold(<time in ms>) method.
    If <time in ims> is equal to 0 the current duration value is returned, the value
    itself is not changed.


SAMPLE CODE:
    #include <Button.h>

    // Pin 2, internal pull-up, active low, generate impulses when held
    Button mybutton1(2, INTERNAL_PULLUP, LOW, true);
    // Pin 3, internal pull-down, active high, button must be released
    Button mybutton2(3, INTERNAL_PULLUP, LOW);

    void setup() {
        Serial.begin(115200);
        mybutton1.init();  // Initialize button 1
        mybutton2.init();  // Initialize button 2
    }

    void loop() {
        if (mybutton1.read()) {
            // Do something when the button is pressed or held down
            // For example, toggle an LED or increment a counter
            Serial.println("Button 1 pressed or holding...");
        }
        if (mybutton2.read()) {
            Serial.println("Button 2 pressed or holding...");
        }
    }
*/



#pragma once
#include <Arduino.h>


#define TIME_DEBOUNCE 30            // Let the button debounce for x ms
#define TIME_DEFAULT_HOLD 500       // Default time in ms after button is considered held down
#define TIME_DEFAULT_INTERVAL 500   // Default time in ms intervalls are triggered if button is held down 

// Check if board has internal pull-up resistor 
#ifdef INPUT_PULLUP
#define INTERNAL_PULLUP INPUT_PULLUP
#endif
// Check if board has internal pull-down resistor 
#ifdef INPUT_PULLDOWN
#define INTERNAL_PULLDOWN INPUT_PULLDOWN
#endif
// External pull-up and pull-down resistors require pinMode setup as input
#define EXTERNAL_PULLUP INPUT
#define EXTERNAL_PULLDOWN INPUT

// --------------------------------------------------------------------------------------
// readThisPin: Pin on Arduino
// singalActive: Is the button pressed on logic level LOW or HIGH?
// pinMode: INTERNAL_PULLUP, INPUT_PULLDOWN, EXTERNAL_PULLUP, EXTERNAL_PULLDOWN
// 
// Optional Arguments
// impulsesOnHold: Default is false: button must be released to get new impulse
//                 true: Impulse every 300 ms (default value, can be changed) if
//                       button is held down
// --------------------------------------------------------------------------------------


/// Button class for processing inputs from bouncing buttons; 
/// Optional: processing of long button presses (hold down)
class SmartButton {
    enum BUT {
        IDLE,
        PRESSED,
        HOLD
    };

    public:
        SmartButton(uint8_t readThisPin, int pinMode, bool signalActive, bool impulsesOnHold = false);
      
        void init();
        uint16_t getsetTimeToEnterHold(uint16_t time = 0);        
        uint16_t ImpulseInterval(uint16_t time = 0);
        bool read();
        bool isHeldDown();

    
    private:
        uint8_t pin;
        int mode;
        bool active;
        bool hold;
        BUT state = BUT::IDLE;
        uint16_t timeContinousPress = TIME_DEFAULT_HOLD;
        uint16_t timeImpulseInterval = TIME_DEFAULT_INTERVAL;
        uint32_t timeAllowNextRead, timePressed, timeNextImpulse;
};