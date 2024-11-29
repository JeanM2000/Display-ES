#include <Arduino.h>
#include "SmartButton.h"

/*!
    @brief Constructor of Button class
*/
SmartButton::SmartButton(uint8_t readThisPin, int pinMode, bool signalActive, bool impulsesOnHold) {
    pin = readThisPin;
    mode = pinMode;
    active = signalActive;
    hold = impulsesOnHold;
}

/*!
    @brief Call this in setup() to initialize button
*/
void SmartButton::init() {
    pinMode(pin, mode);   // Select pin mode
}


/*!
    @brief Changes time button needs to pressed to be considered held down
    @param time in ms (if =0 or no argument, setting is untouched)
    @returns Current setting
*/
uint16_t SmartButton::getsetTimeToEnterHold(uint16_t time) {
    if (time != 0)
        timeContinousPress = time;
    return timeContinousPress;
}


/*!
    @brief Changes interval with which impulses are send while button is held down
    @param time in ms (if =0 or no argument, setting is untouched)
    @returns Current setting
*/
uint16_t SmartButton::ImpulseInterval(uint16_t time) {
    if (time != 0)
        timeImpulseInterval = time;
    return timeImpulseInterval;
}


/*!
    @brief Call this in loop() to read button
    @returns impulse on each press (optional: impulses if held down)
*/
bool SmartButton::read() {
    switch (state) {
        // Wait until user presses the button
        case BUT::IDLE: {
            if (millis() >= timeAllowNextRead) {    // Button debounced?
                if (digitalRead(pin) == active) {       // Button released?
                    // Save time when buttons was pressed
                    timePressed = millis();     
                    // Give button time to bounce
                    timeAllowNextRead = millis() + TIME_DEBOUNCE;
                    state = BUT::PRESSED;
                    return true;        // Return one impulse
                }
            }
        } break;

        // User has pressed the button:
        case BUT::PRESSED: {
            if (millis() >= timeAllowNextRead) {    // Button debounced?
                if (digitalRead(pin) != active) {       // Button released?
                    // Give button time to bounce
                    timeAllowNextRead = millis() + TIME_DEBOUNCE;
                    state = BUT::IDLE;
                }
            }
            // Button not released yet? It must be held down!
            if (millis() >= timePressed + timeContinousPress) {
                state = BUT::HOLD;                       
            }
        } break;

        // User is holding down the button:
        case BUT::HOLD: {
            if (digitalRead(pin) != active) {   // Button released?
                // Give button time to bounce
                timeAllowNextRead = millis() + TIME_DEBOUNCE;
                state = BUT::IDLE;
            }
            // Should holding down the button create impulses?
            if (hold) {
                if (millis() >= timeNextImpulse) {  // Time reached for next impulse?
                    // Time for next impulse is:
                    timeNextImpulse = millis() + timeImpulseInterval;
                    return true;        // Return one impulse
                }
            }
        }
    }
    return false;   // No impulse
}


/*!
    @brief Query if button is in hold state
    @returns true if button is held down
*/
bool SmartButton::isHeldDown() {
    if (state == BUT::HOLD) {
        return true;
    } else {
        return false;
    }
}