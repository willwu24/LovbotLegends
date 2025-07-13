#include "core.h"

// Global variables
int motor_dir[5];
moveType motion;
int attackMode, robotRole, robotMode, botNum = 1, targetMode = 1;
bool kickoff = false;

// Motor control variables
int angleThres = 35, turnSpeed = 7, turningMode = 1, motorMode = 0;
double motorChange = 3;
int MIN_TURNING_SPEED = 10, MAX_TURNING_SPEED = 40;
int currSpeed[5] = {0}, tarSpeed[5] = {0};
unsigned long lastChanged = 0;

// PID variables
double motorCorrectionKp = 0.27, motorCorrectionKi = 0.00, motorCorrectionKd = 0.02;
double motorTurningKp = 0.23, motorTurningKi = 0.00, motorTurningKd = 0.00;
double motorSetpoint, motorInput, motorCorrectionOutput, motorTurningOutput;

PID motorCorrectionPID(&motorInput, &motorCorrectionOutput, &motorSetpoint, motorCorrectionKp, motorCorrectionKi, motorCorrectionKd, DIRECT);
PID motorTurningPID(&motorInput, &motorTurningOutput, &motorSetpoint, motorTurningKp, motorTurningKi, motorTurningKd, DIRECT);

// Laser detection
long lastLaser = 0;
int laserCounter = -1, laserTolerance = 80;

void setSerialMonitor() { Serial.begin(9600); }

void setupVariables(){
  int new_motor_dir[5] = {0,-1,-1,-1,-1};
  for(int i = 0; i < 5; i++){
    motor_dir[i] = new_motor_dir[i];
  }
}

void setupMotors() {
  for (int i = 1; i <= 4; i++) {
    pinMode(PWM[i], OUTPUT);
    pinMode(DIR[i], OUTPUT);
  }
  motorCorrectionPID.SetMode(AUTOMATIC);
  motorCorrectionPID.SetOutputLimits(-45, 45);
  motorTurningPID.SetMode(AUTOMATIC);
  motorTurningPID.SetOutputLimits(-45, 45);
  motorSetpoint = 0;
}

// Basic getters/setters
int getTarget() { return motion.target; }
void setTarget(int newTarget) { motion.target = newTarget; }
int getSpeed() { return motion.speed; }
void setSpeed(int newSpeed) { motion.speed = newSpeed; }
int getDir() { return motion.direction; }
void setDir(int newDirection) { motion.direction = newDirection; }
int getAttackMode() { return attackMode; }
void setAttackMode(int a) { attackMode = a; }
int getRobotRole() { return robotRole; }
void setRobotRole(int a) { robotRole = a; }
int getBotNum() { return EEPROM.read(32); }
void setBotNum(int a) { EEPROM.write(32, a); }

// Math utilities
int getAngleDif(int target, int currentValue) {
  int current = currentValue;
  if (target < 180) {
    return current > target + 180 ? -360 + current - target : current - target;
  } else {
    return current > target - 180 ? current - target : 360 - target + current;
  }
}

// Motor control
void setMotor(int which, int motorSpeed) {
  motorSpeed *= motor_dir[which];
  motorSpeed = constrain(motorSpeed, -MAX_SPEED, MAX_SPEED);
  digitalWrite(DIR[which], motorSpeed >= 0 ? LOW : HIGH);
  analogWrite(PWM[which], abs(motorSpeed) * 255 / 100);
}

void setMotors(int s1, int s2, int s3, int s4) {
  setMotor(1, s1); setMotor(2, s2); setMotor(3, s3); setMotor(4, s4);
}

void move(){
  int angleDif = getAngleDif(getTarget(), getCompass());
  if (abs(angleDif) > angleThres) {
    int val = angleDif < 0 ? turnSpeed : -turnSpeed;
    setMotors(val, val, val, val);
  }
}

// Ball detection
bool getLaser() { return analogRead(LAZER_PIN) < 10; }
bool hasBall() { return getLaser() && (getEyeAngle() < 60 || getEyeAngle() > 300); }

// Utility functions
long getSystemTime() { return millis(); }
void wait(float sec) { delay(sec * 1000); }
void STOPNOW() { setMotors(0, 0, 0, 0); }
