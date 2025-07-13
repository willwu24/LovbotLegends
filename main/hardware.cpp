#include "hardware.h"

// === BUTTON VARIABLES ===
const int TOTAL = 5;
bool scanning = false;
bool prevButton = false;
int buttonState[TOTAL] = {0};
int previousButtonPressed[TOTAL] = {0};
bool longPressed[TOTAL] = {0};
unsigned long pressStart[TOTAL] = {0};

// === LED VARIABLES ===
int LEDStates[TOTAL_LED] = {0, 0, 0, 0, 0};
int LEDPins[TOTAL_LED] = {33, 34, 35, 36, 37};
long blinkTime[TOTAL_LED] = {0, 0, 0, 0, 0};

// === BUTTON FUNCTIONS ===
void setupButton() {
  for (int i = 0; i < TOTAL; ++i) {
    pinMode(BUTTON_PIN[i], INPUT_PULLUP);
  }
}

void updateButton() {
  for (int i = 0; i < TOTAL; ++i) {
    int pressedNow = !digitalRead(BUTTON_PIN[i]);
    
    if (pressedNow == 1 && previousButtonPressed[i] == 0) {
      pressStart[i] = millis();
    }
    
    if (pressedNow == 1 && (millis() - pressStart[i]) >= LONG_PRESS_MS) {
      longPressed[i] = true;
    } else if (pressedNow == 0) {
      longPressed[i] = false;
      pressStart[i] = 0;
    }
    
    if (pressedNow == 1 && previousButtonPressed[i] == 0) {
      buttonState[i] = (buttonState[i] + 1) % TOTAL_STATE;
    }
    
    previousButtonPressed[i] = pressedNow;
  }
}

bool buttonPressed(int which) { return longPressed[which]; }
bool getButtonState(int which) { return buttonState[which]; }

void clearButtonState() {
  for (int i = 0; i < TOTAL; ++i) buttonState[i] = 0;
}

void checkButton() {
  if (buttonPressed(3) && !prevButton) {
    scanning = !scanning;
  }
  if (scanning) {
    Serial.println("calibrating threshold");
    // calibrateThreshold(); // This function would be in sensors
  }
  setLEDState(3, scanning ? 1 : 0);
  prevButton = buttonPressed(3);
}

// === LED FUNCTIONS ===
void setLED() {
  long now = millis();
  
  for (int i = 0; i < TOTAL_LED; ++i) {
    switch (LEDStates[i]) {
      case OFF:
        digitalWrite(LEDPins[i], LOW);
        break;
      case ON:
        digitalWrite(LEDPins[i], HIGH);
        break;
      case BLINK:
        if (now - abs(blinkTime[i]) > BLINK_INTERVAL) {
          if (blinkTime[i] > 0) {
            blinkTime[i] = -now;
            digitalWrite(LEDPins[i], LOW);
          } else {
            blinkTime[i] = now;
            digitalWrite(LEDPins[i], HIGH);
          }
        }
        break;
    }
  }
}

void setLEDState(int which, int state) {
  if (which >= 0 && which < TOTAL_LED && state >= 0 && state <= 2) {
    LEDStates[which] = state;
  }
}

void setLEDAll(int state) {
  for (int i = 0; i < TOTAL_LED; ++i) {
    LEDStates[i] = state;
    if (state == BLINK) {
      blinkTime[i] = millis();
    }
  }
  setLED();
}

void clearLEDState() {
  setLEDAll(OFF);
}

void checkLED() {
  setLEDState(0, whiteDetected() ? ON : OFF);
  setLEDState(2, getAttackMode() == YELLOW ? ON : OFF);
}

// === SHOOTER FUNCTIONS ===
void setupKicker() {
  pinMode(SHOOT_PIN, OUTPUT);
}

void kick() {
  digitalWrite(SHOOT_PIN, HIGH);
  delay(90); // Kick strength
  digitalWrite(SHOOT_PIN, LOW);
}
