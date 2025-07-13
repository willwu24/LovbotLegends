#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <PID_v2.h>
#include <EEPROM.h>
#include <queue>
#include "core.h"

// === COMPASS CONSTANTS ===
#define RST              11    
#define BNO_ADDR         0x17  
#define PAGE_ID          0x07  
#define ACC_DATA_X_LSB   0x08  
#define OPR_MODE         0x3D  
#define ACC_CONFIG       0x08  
#define MODE_AMG         0x07  

// === COMPOUND EYE CONSTANTS ===
#define BALL_THRES      40  
#define EYE_PORT_PIN    A10 
#define EYE_VALUE_PIN   A11 

// === ULTRASONIC CONSTANTS ===
#define ULTRA_NUM 4
#define VELOCITY_TEMP(temp) ((331.5 + 0.6 * (float)(temp)) * 100 / 1000000.0)

// === CAMERA FUNCTIONS ===
void setupCamera();
void getCameraData();
bool homeDetected();
int getHomeAngle();
int getHomeDistance();
bool opposingDetected();
int getOpposingAngle();
int getOpposingDistance();

// === COMPASS FUNCTIONS ===
void setupCompass();
int getCompass();
int getCompassDif(int target, int currentValue);

// === COMPOUND EYE FUNCTIONS ===
void eyeSetup();
int getEyeAngle();
int getEyeValue();

// === GRAYSCALE FUNCTIONS ===
void initSensors();
void processSensors();
void processWhiteAngle();
void processWhiteAngleOffense();
bool whiteDetected();

// === ULTRASONIC FUNCTIONS ===
void processDFUltraTrig();
int getUltraFront();
int getUltraRight();
int getUltraBack();
int getUltraLeft();

#endif // SENSORS_H
