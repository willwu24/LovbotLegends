#include "offense.h"

// === State Tracking ===
int lastWhite = 0;
int wasWhite = 0;
int currWhiteSpeed = 0;

int lastWhiteAngle = 0;
int lastTarget = 0;
int firstBall = 0;

int lastHasBall = 0;
int targetSet = 0;
int lastTargetSet = 0;

int blockGoalieSide = 0;

int lastAround = 0;

static unsigned long timer = 0;

const int frontStart = 130, frontStop = 30, frontReverse = 0;
const int backStart = 160, backStop = 94, backReverse = 0;
const int leftStart = 150, leftStop = 58, leftReverse = 0;
const int rightStart = 150, rightStop = 58, rightReverse = 0;

bool turnSet = false;


// === Main Offense Control ===
void offenseMain() {
  retrieveKicker();
  setTurningMode(1);
  setAngleThres(45);

  // === White Line Detected ===

  if (whiteDetected() && firstBall != 1) {
    turnSet = false;
    setTarget(0);
    setMotorMode(0);
    resetBallPID();

    setDir(dirAngle);
    if (!getFlip()){
      setSpeed(50);
    }
    else
    {
      setSpeed(70);
    }
  }

  // === Recently Detected White Line ===
  else if ((millis() - lastWhite < 100 && !getFlip()) ||
           (millis() - lastWhite < 1000 && getFlip())) {
    Serial.println("Running White Line 50ms");

    resetBallPID();
    setDir(lastWhiteAngle);

  }

  // === Default Behavior ===
  else {
    // if(inCorner()){
    //   if (blockGoalieSide == 1){
    //     transmit(2); //left
    //     // setRobotRole(DEFENSE);
    //   }
    //   else
    //   {
    //     transmit(3);
    //     // setRobotRole(DEFENSE);
    //   }
    // }
    if(hasBall()){
      lastTargetSet = millis();
      if (millis() - lastAround > 150){
        kick();
      }
      goToBallPID();
    }
    else if (getEyeValueSmooth() < 12){
      resetBallPID();
      targetSet = 0;
      setTarget(0);
      goToCoordinate(0, 0);
    }
    else
    {
      targetSet = 0;
      lastAround = millis();
      setTarget(0);
      goToBallPID();
    }
  }
}

// === Turning Strategy ===
void grabBall() {
  while (hasBall() && getCompass() < 45 && !whiteDetected()) {
    if (getUltraLeft() > getUltraRight()) {
      Serial.print("turning left");
      setMotors(-20, -20, -30, -30);
    }
    else {
      Serial.print("turning right");
      setMotors(30, 30, 20, 20);
    }
  }
}



bool inCorner() {

  bool ballFront = getEyeAngle() < 30 || getEyeAngle() > 330;
  bool inCornerNow = ballFront && getUltraFront() < 80 && (getUltraLeft() < 80 || getUltraRight() < 80);

  if (inCornerNow) {
    if (millis() - timer >= 3000){
      if (getUltraLeft() < getUltraRight()){
        blockGoalieSide = 1;
      }
      else
      {
        blockGoalieSide = 2;
      }
      return true;
    }
  } else {
    timer = millis();  // reset if condition breaks
  }

  return false;
}
