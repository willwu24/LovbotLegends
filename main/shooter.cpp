#include "shooter.h"
#include <Arduino.h>

// === Kicker State ===
int kickerStatus = 0;

const int kickStrength = 90;  // Max around 90 ms
const int kickGap = 1000;     // Minimum delay between kicks

// === Setup ===
void setupKicker() {
  pinMode(SHOOT_PIN, OUTPUT);
}

// === Kick Control ===
void kick() {
  if (kickerStatus == OFF) {
    digitalWrite(SHOOT_PIN, HIGH);
    kickerStatus = ON;
    delay(kickStrength);  // Use delay instead of timer
    digitalWrite(SHOOT_PIN, LOW);
    kickerStatus = OFF;
    delay(kickGap);  // Wait before next kick is allowed
  }
}


// === Debugging ===
void printKickerStatus() {
  Serial.print("Time: "); Serial.print(millis());
  Serial.print(" | Status: "); Serial.println(kickerStatus);
}
