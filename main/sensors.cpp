#include "sensors.h"

// === CAMERA VARIABLES ===
byte data[10];

// === COMPASS VARIABLES ===
Adafruit_BNO055 bno = Adafruit_BNO055(55);
int heading = 359;

// === COMPOUND EYE VARIABLES ===
const int eyeNum = 18;
int eyePortThres[eyeNum] = { 32, 64, 95, 127, 158, 190, 222, 253, 285, 317, 347, 380, 410, 442, 474, 499, 531, 562};

// === GRAYSCALE VARIABLES ===
int readPins[] = {A0, A1};
int selectorPins[] = {38, 39, 40, 41};
const int sensorSize = 32;
int sensorReadings[sensorSize];
int sensorThres[sensorSize];
bool sensorWhite[sensorSize];
bool touchWhite = false;

// === ULTRASONIC VARIABLES ===
int trigechoPin[ULTRA_NUM] = {A9, A13, A7, A8};

// === CAMERA FUNCTIONS ===
void setupCamera() {
  Serial7.begin(57600);
}

void getCameraData() {
  if (Serial7.available() >= 12) {
    byte header = Serial7.read();
    if (header == 0x99) {
      for (int i = 0; i < 10; i++) {
        data[i] = Serial7.read();
      }
      Serial7.read(); // checksum
    }
  }
}

bool homeDetected() { return getAttackMode() == 0 ? (data[2] == 1) : (data[7] == 1); }
int getHomeAngle() { return getAttackMode() == 0 ? data[0] * 2 : data[5] * 2; }
int getHomeDistance() { return getAttackMode() == 0 ? data[1] : data[6]; }
bool opposingDetected() { return getAttackMode() == 1 ? (data[2] == 1) : (data[7] == 1); }
int getOpposingAngle() { return getAttackMode() == 1 ? data[0] * 2 : data[5] * 2; }
int getOpposingDistance() { return getAttackMode() == 1 ? data[1] : data[6]; }

// === COMPASS FUNCTIONS ===
int getCompass() {
  int rawDegree = bno.getVector(Adafruit_BNO055::VECTOR_EULER).x();
  return (rawDegree - heading + 360) % 360;
}

void setupCompass() {
  if (!bno.begin()) {
    while (1);
  }
  bno.setExtCrystalUse(true);
  bno.setMode(0x0C);
}

// === COMPOUND EYE FUNCTIONS ===
void eyeSetup() {
  pinMode(EYE_PORT_PIN, INPUT);
  pinMode(EYE_VALUE_PIN, INPUT);
}

int getEyeAngle() {
  int portValue = analogRead(EYE_PORT_PIN);
  for (int i = 0; i < eyeNum - 1; i++) {
    if (portValue < (eyePortThres[i] + eyePortThres[i + 1]) / 2) {
      return i * (360.0 / eyeNum);
    }
  }
  return (eyeNum - 1) * (360.0 / eyeNum);
}

int getEyeValue() {
  return analogRead(EYE_VALUE_PIN);
}

// === GRAYSCALE FUNCTIONS ===
void initSensors() {
  for (int i = 0; i < 4; i++) pinMode(selectorPins[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(readPins[i], INPUT);
  for (int i = 0; i < 32; i++) {
    sensorThres[i] = EEPROM.read(i) * 2;
  }
}

void processSensors() {
  for (int i = 0; i < sensorSize / 2; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(selectorPins[j], (i >> j) & 1);
    }
    sensorReadings[i] = analogRead(readPins[0]);
    sensorReadings[i + 16] = analogRead(readPins[1]);
  }
  
  touchWhite = false;
  for (int i = 0; i < sensorSize; i++) {
    sensorWhite[i] = sensorReadings[i] > sensorThres[i];
    if (sensorWhite[i]) touchWhite = true;
  }
}

void processWhiteAngle() {}
void processWhiteAngleOffense() {}
bool whiteDetected() { return touchWhite; }

// === ULTRASONIC FUNCTIONS ===
void processDFUltraTrig() {}

int getUltraFront() { return analogRead(trigechoPin[0]); }
int getUltraRight() { return analogRead(trigechoPin[1]); }
int getUltraBack() { return analogRead(trigechoPin[2]); }
int getUltraLeft() { return analogRead(trigechoPin[3]); }
