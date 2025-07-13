#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <Arduino.h>
#include <queue>
#include "func.h"
#include <EEPROM.h>

// ------------------------------
// Initialization & Calibration
// ------------------------------
void initSensors();
void calibrateThreshold();
void resetCalibrateThreshold();

// EEPROM Storage
void saveThresholdsToEEPROM();
bool loadThresholdsFromEEPROM();
void resetEEPROM();  // Optional, only if you use this function

// ------------------------------
// Sensor Data Processing
// ------------------------------
void processSensors();
void processWhiteAngle();
int getLightDif(int currentLight, int lastLight);


// ------------------------------
// White Line Detection
// ------------------------------
bool whiteDetected();
void resetWhiteLine();
int getWhiteCountHistory();

void getEEPROMThreshold();

void getWhiteSensor(bool output[32]);

int getSensitivity();


#endif // GRAYSCALE_H
