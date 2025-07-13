#include "Grayscale.h"

int readPins[] = {A0, A1};
int readPinsSize = 2;

int selectorPins[] = {38, 39, 40, 41};
int selectorPinsSize = 4;

const int sensorSize = 32;
int sensorReadings[sensorSize];
int minSensorThres[sensorSize];
int maxSensorThres[sensorSize];
int sensorThres[sensorSize];
bool sensorWhite[sensorSize];
int sensorWhiteHistory[sensorSize];

bool touchWhite = false;
int whiteCount = 0;
int whiteCountHistory = 0;

int calibrationTimer = 0;
const double sensitivity = 0.20;

double whiteLineKp = 5.0, whiteLineKi = 0.0, whiteLineKd = 0.0;
double whiteLineSetPoint, whiteLineInput, whiteLineOutput;
PID whiteLinePID(&whiteLineInput, &whiteLineOutput, &whiteLineSetPoint, whiteLineKp, whiteLineKi, whiteLineKd, DIRECT);

int rawAngle = 0;

void initSensors() {
  for (int i = 0; i < selectorPinsSize; i++) pinMode(selectorPins[i], OUTPUT);
  for (int i = 0; i < readPinsSize; i++) pinMode(readPins[i], INPUT);
  for (int i = 0; i < sensorSize; i++) {
    maxSensorThres[i] = 0;
    minSensorThres[i] = 999;
  }

  whiteLinePID.SetMode(AUTOMATIC);
  whiteLinePID.SetOutputLimits(-50, 50);
  whiteLineSetPoint = 0;
  getEEPROMThreshold();
}

void calibrateThreshold() {
  for (int i = 0; i < sensorSize / 2; i++) {
    for (int j = 0; j < selectorPinsSize; j++) {
      digitalWrite(selectorPins[j], (i >> j) & 1);
    }

    int reading = analogRead(readPins[0]);
    int reading16 = analogRead(readPins[1]);

    maxSensorThres[i] = max(maxSensorThres[i], reading);
    maxSensorThres[i + 16] = max(maxSensorThres[i + 16], reading16);
    minSensorThres[i] = min(minSensorThres[i], reading);
    minSensorThres[i + 16] = min(minSensorThres[i + 16], reading16);

    sensorThres[i] = minSensorThres[i] + (maxSensorThres[i] - minSensorThres[i]) * sensitivity;
    sensorThres[i + 16] = minSensorThres[i + 16] + (maxSensorThres[i + 16] - minSensorThres[i + 16]) * sensitivity;
  }
  for (int i = 0 ; i < 32 ; i++) EEPROM.write(i, sensorThres[i] / 2);
}

void getEEPROMThreshold() {
  for (int i = 0; i < 32; i++) {
    sensorThres[i] = EEPROM.read(i) * 2;
  }
}

void resetCalibrateThreshold() {
  for (int i = 0; i < sensorSize; i++) {
    sensorThres[i] = 0;
  }
}

void processSensors() {
  whiteCount = 0;
  whiteCountHistory = 0;

  for (int i = 0; i < sensorSize / 2; i++) {
    for (int j = 0; j < selectorPinsSize; j++) {
      digitalWrite(selectorPins[j], (i >> j) & 1);
    }
    sensorReadings[i] = analogRead(readPins[0]);
    sensorReadings[i + 16] = analogRead(readPins[1]);
  }

  for (int i = 0; i < sensorSize; i++) {
    sensorWhite[i] = sensorReadings[i] > sensorThres[i];
    whiteCount += sensorWhite[i];
    if (sensorWhite[i]) sensorWhiteHistory[i] = 1;
    whiteCountHistory += sensorWhiteHistory[i];
  }

  touchWhite = whiteCount > 0;
}

void processWhiteAngle() {
  rawAngle = 180;
}

void resetWhiteLine() {
  for (int i = 0; i < sensorSize; i++) {
    sensorWhiteHistory[i] = 0;
  }
  whiteCountHistory = 0;
}

void getWhiteSensor(bool output[32]) {
  for (int i = 0; i < 32; i++) {
    output[i] = sensorWhite[i];
  }
}

int getLightDif(int currentLight, int lastLight) {
  int output = currentLight - lastLight;
  return (output + 540) % 360 - 180;
}

int getSensitivity() { return sensitivity; }
bool whiteDetected() { return touchWhite; }
int getWhiteCountHistory() { return whiteCountHistory; }
