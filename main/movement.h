#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include <PID_v2.h>
#include "core.h"
#include "sensors.h"

// === COORDINATE FUNCTIONS ===
posInfo findCO(int front, int right, int back, int left);

// === BALL TRACKING FUNCTIONS ===
void setUpBallPID();
void goToBallPID();
void resetBallPID();

#endif // MOVEMENT_H
