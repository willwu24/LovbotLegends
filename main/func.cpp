#include "func.h"

double motorCorrectionKp = 0.27, motorCorrectionKi = 0.00, motorCorrectionKd = 0.02;
double motorTurningKp = 0.23, motorTurningKi = 0.00, motorTurningKd = 0.00;
double motorSetpoint, motorInput, motorCorrectionOutput, motorTurningOutput;

PID motorCorrectionPID(&motorInput, &motorCorrectionOutput, &motorSetpoint, motorCorrectionKp, motorCorrectionKi, motorCorrectionKd, DIRECT);
PID motorTurningPID(&motorInput, &motorTurningOutput, &motorSetpoint, motorTurningKp, motorTurningKi, motorTurningKd, DIRECT);

int angleThres = 35;
int turnSpeed = 7;
double motorChange = 3;
int turningMode = 1;
int motorMode = 0;
int MIN_TURNING_SPEED = 10;
int MAX_TURNING_SPEED = 40;

int currSpeed[5] = {0};
int tarSpeed[5] = {0};
unsigned long lastChanged = 0;

int attackMode;
int robotRole;
int robotMode;
bool kickoff = false;
int botNum = 1;
int targetMode = 1;

long lastLaser = 0;
bool laserSensed = false;
int laserTimer = -10000;
int laserCounter = -1;
int laserTolerance = 80;

moveType motion;

int motorSign[4][4] = {
  { 1,  1, -1, -1},
  {-1,  1,  1, -1},
  {-1, -1,  1,  1},
  { 1, -1, -1,  1}
};

void setSerialMonitor() {
  Serial.begin(9600);
}

void setMotorsDir(int dir1, int dir2, int dir3, int dir4) {
  motor_dir[1] = dir1;
  motor_dir[2] = dir2;
  motor_dir[3] = dir3;
  motor_dir[4] = dir4;
}

void setupMotors() {
  Serial.println("setup motors");
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

void setAngleThres(int angle) { angleThres = angle; }
int getAngleThres() { return angleThres; }
void setTurnSpeed(int s) { turnSpeed = s; }

int getTarget() { return motion.target; }
void setTarget(int newTarget) { motion.target = newTarget; }
int getSpeed() { return motion.speed; }
void setSpeed(int newSpeed) { motion.speed = newSpeed; }
int getDir() { return motion.direction; }
void setDir(int newDirection) { motion.direction = newDirection; }

void setTurningMode(int mode) { turningMode = mode; }
int getTurningMode() { return turningMode; }
void setMotorMode(int mode) { motorMode = mode; }
int getMotorMode() { return motorMode; }
void setMotorChange(int change) { motorChange = change; }
double getMotorChange() { return motorChange; }

void move(){
  int d = motion.direction;
  int s = motion.speed;
  int targetAngle = motion.target;

  int angleDif = getAngleDif(getTarget(), getCompass());
  motorInput = angleDif;
  motorCorrectionPID.Compute();
  motorTurningPID.Compute();

  int speed1 = 0, speed2 = 0, speed3 = 0, speed4 = 0;

  if (turningMode == MANUAL) {
    if (abs(angleDif) > angleThres) {
      int val = angleDif < 0 ? turnSpeed : -turnSpeed;
      speed1 = val; speed2 = val; speed3 = val; speed4 = val;
    }
  } else {
    if (abs(angleDif) > angleThres) {
      int val = angleDif < 0
        ? constrain(motorTurningOutput, MIN_TURNING_SPEED, MAX_TURNING_SPEED)
        : constrain(motorTurningOutput, -MAX_TURNING_SPEED, -MIN_TURNING_SPEED);
      speed1 = val; speed2 = val; speed3 = val; speed4 = val;
    }
  }

  if (motorMode == 0) {
    setMotors(speed1, speed2, speed3, speed4);
  } else {
    setMotorsGradual(speed1, speed2, speed3, speed4);
  }
}

double toRadian(int degree) { return degree * 3.1415926 / 180; }
double toDegree(double radian) { return radian * 180 / 3.14159265; }

int sinLaw(int degree1, int degree2, int speed2) {
  double rad1 = toRadian(degree1);
  double rad2 = toRadian(degree2);
  return (sin(rad1) * speed2) / sin(rad2);
}

int getAngleDif(int target, int currentValue) {
  int current = currentValue;
  if (target < 180) {
    return current > target + 180 ? -360 + current - target : current - target;
  } else {
    return current > target - 180 ? current - target : 360 - target + current;
  }
}

int getAbsAngle(int a) {
  a = abs(a) % 360;
  return a > 180 ? 360 - a : a;
}

void setMotor(int which, int motorSpeed) {
  motorSpeed *= motor_dir[which];
  motorSpeed = constrain(motorSpeed, -MAX_SPEED, MAX_SPEED);

  digitalWrite(DIR[which], motorSpeed >= 0 ? LOW : HIGH);
  analogWrite(PWM[which], motorSpeed >= 0 || motor_prob[which] == 1 ? abs(motorSpeed) * 255 / 100 : 255 + motorSpeed * 255 / 100);
}

void setMotors(int s1, int s2, int s3, int s4) {
  setMotor(1, s1);
  setMotor(2, s2);
  setMotor(3, s3);
  setMotor(4, s4);
}

void setMotorsGradual(int s1, int s2, int s3, int s4) {
  tarSpeed[1] = s1;
  tarSpeed[2] = s2;
  tarSpeed[3] = s3;
  tarSpeed[4] = s4;

  if (millis() - lastChanged > 1) {
    for (int i = 1; i <= 4; i++) {
      if (abs(tarSpeed[i] - currSpeed[i]) < motorChange) {
        currSpeed[i] = tarSpeed[i];
      } else {
        currSpeed[i] += (tarSpeed[i] - currSpeed[i]) / abs(tarSpeed[i] - currSpeed[i]) * motorChange;
      }
    }
    lastChanged = millis();
  }

  setMotors(currSpeed[1], currSpeed[2], currSpeed[3], currSpeed[4]);
}

void STOPNOW() {
  setMotors(0, 0, 0, 0);
}

bool getLaser() {
  return analogRead(LAZER_PIN) < 10;
}

bool hasBall() {
  if (getLaser() && (getEyeAngle() < 60 || getEyeAngle() > 300)) {
    laserCounter++;
    lastLaser = millis();
    return true;
  } else {
    if (millis() - lastLaser < 50) {
      return true;
    }
    laserCounter = -1;
    return false;
  }
}

long getSystemTime() { return millis(); }
void resetSystemTime() {}
void wait(float sec) { delay(sec * 1000); }

void setAttackMode(int a) { attackMode = a; }
int getAttackMode() { return attackMode; }

void setRobotRole(int a) { robotRole = a; }
int getRobotRole() { return robotRole; }

void setRobotMode(int a) { robotMode = a; }
int getRobotMode() { return robotMode; }

int getTargetMode() { return targetMode; }
void setTargetMode(int a) { targetMode = a; }

bool getKickoff() { return kickoff; }
void setKickoff(bool a) { kickoff = a; }

void setBotNum(int a) { EEPROM.write(32, a); }
int getBotNum() { return EEPROM.read(32); }
