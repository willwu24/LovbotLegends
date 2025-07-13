#include <Arduino.h>
#include "core.h"
#include "sensors.h"
#include "hardware.h"
#include "communication.h"
#include "movement.h"

void setup() {
  setSerialMonitor(); 
  Serial.print("Start");
  setupButton();

  while(!buttonPressed(4)) {
    setLEDState(4, 0);
    updateButton();
  }
  setLEDState(4, 1);

  Serial.println("Running basic setup...");
  setupMotors();

  Serial.println("Setting up sensors...");
  setupKicker();
  setupCamera();
  setupCompass();
  setupBluetooth();
  initSensors();
  eyeSetup();
  setUpBallPID();

  Serial.println("Finished setup.");
}

void loop() {
  player();
}
