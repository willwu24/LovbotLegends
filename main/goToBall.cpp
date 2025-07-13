#include "GoToBall.h"

int minGoToBallSpeed = 20;
int maxGoToBallSpeed = 55;
int speedDiff = maxGoToBallSpeed - minGoToBallSpeed;

int minDir = 0;
int maxDir = 90;
int dirDiff = maxDir - minDir;

double ballClosest, ballFarthest, ballDist, speedRatio, distanceRatio;
double speedKp = 1.1, speedKi = 0.000, speedKd = 0.000;
double dirKp = 100.0, dirKi = 0.00, dirKd = 0.03;

double PIDMinimum = 0;
double PIDMaximum = 100;

PID speedPID(&ballDist, &speedRatio, &ballClosest, speedKp, speedKi, speedKd, DIRECT);
PID dirPID(&ballDist, &distanceRatio, &ballFarthest, dirKp, dirKi, dirKd, REVERSE);

void setUpBallPID() {
    if (getBotNum == 0) {
        ballClosest = 200;
    } else {
        ballClosest = 200;
    }
    ballFarthest = 20;

    speedPID.SetOutputLimits(PIDMinimum, PIDMaximum);
    speedPID.SetMode(AUTOMATIC);
    dirPID.SetOutputLimits(PIDMinimum, PIDMaximum);
    dirPID.SetMode(AUTOMATIC);
}

void goToBallPID() {
    int ballDir = getEyeAngle();
    ballDist = getEyeValueSmooth();

    dirPID.Compute();
    speedPID.Compute();

    double finalSpeed = maxGoToBallSpeed * (speedRatio / 100.0);
    finalSpeed = constrain(finalSpeed, minGoToBallSpeed, maxGoToBallSpeed);

    double ballAngleRatio = ballDir > 180 ? (360.0 - ballDir) / 180.0 : ballDir / 180.0;
    ballAngleRatio = constrain(ballAngleRatio, 0.0, 1.0);
    double k = 4.0;
    ballAngleRatio = 1.0 - exp(-k * ballAngleRatio);

    double offset = dirDiff * (distanceRatio / 100.0) * ballAngleRatio;
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
    speedPID.SetOutputLimits(0.0, 1.0);
    speedPID.SetOutputLimits(-1.0, 0.0);
    speedPID.SetOutputLimits(PIDMinimum, PIDMaximum);

    dirPID.SetOutputLimits(0.0, 1.0);
    dirPID.SetOutputLimits(-1.0, 0.0);
    dirPID.SetOutputLimits(PIDMinimum, PIDMaximum);
}
