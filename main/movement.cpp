#include "movement.h"

// === BALL TRACKING VARIABLES ===
int minGoToBallSpeed = 20;
int maxGoToBallSpeed = 55;
double ballClosest = 200;
double ballFarthest = 20;
double ballDist, speedRatio, distanceRatio;
double speedKp = 1.1, speedKi = 0.000, speedKd = 0.000;
double dirKp = 100.0, dirKi = 0.00, dirKd = 0.03;
PID speedPID(&ballDist, &speedRatio, &ballClosest, speedKp, speedKi, speedKd, DIRECT);
PID dirPID(&ballDist, &distanceRatio, &ballFarthest, dirKp, dirKi, dirKd, REVERSE);

// === COORDINATE FUNCTIONS ===
posInfo findCO(int front, int right, int back, int left) {
  posInfo currCO;
  currCO.posX = (left - right) / 2;
  currCO.posY = (back - front) / 2;
  return currCO;
}

// === BALL TRACKING FUNCTIONS ===
void setUpBallPID() {
  speedPID.SetOutputLimits(0, 100);
  speedPID.SetMode(AUTOMATIC);
  dirPID.SetOutputLimits(0, 100);
  dirPID.SetMode(AUTOMATIC);
}

void goToBallPID() {
  int ballDir = getEyeAngle();
  ballDist = getEyeValue();
  
  dirPID.Compute();
  speedPID.Compute();
  
  double finalSpeed = maxGoToBallSpeed * (speedRatio / 100.0);
  finalSpeed = constrain(finalSpeed, minGoToBallSpeed, maxGoToBallSpeed);
  
  double ballAngleRatio = ballDir > 180 ? (360.0 - ballDir) / 180.0 : ballDir / 180.0;
  ballAngleRatio = constrain(ballAngleRatio, 0.0, 1.0);
  
  double offset = 90 * (distanceRatio / 100.0) * ballAngleRatio;
  if (ballDir >= 180) {
    offset *= -1;
  }
  
  double finalDirection = ballDir + offset;
  setDir(finalDirection);
  setSpeed(finalSpeed);
  
  if (ballDist < 12) {
    resetBallPID();
  }
}

void resetBallPID() {
  speedPID.SetOutputLimits(0, 100);
  dirPID.SetOutputLimits(0, 100);
}
