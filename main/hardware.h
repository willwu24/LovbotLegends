#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include "core.h"

// === BUTTON CONSTANTS ===
#define TOTAL_BUTTONS 5
#define TOTAL_STATE 2
#define LONG_PRESS_MS 100

// === LED CONSTANTS ===
#define TOTAL_LED 5
#define BLINK_INTERVAL 300

// === SHOOTER CONSTANTS ===
#define SHOOT_PIN A12

// === BUTTON FUNCTIONS ===
void setupButton();
void updateButton();
bool buttonPressed(int which);
bool getButtonState(int which);
void clearButtonState();
void checkButton();

// === LED FUNCTIONS ===
void setLED();
void setLEDState(int which, int state);
void setLEDAll(int state);
void clearLEDState();
void checkLED();

// === SHOOTER FUNCTIONS ===
void setupKicker();
void kick();

#endif // HARDWARE_H
