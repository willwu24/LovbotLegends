#include "player.h"


/*
  NOTE:
  - Blocking is removed
  - No parking flag implemented

  Mode Offense_Switch  → Role Offense
  Mode Defense_Switch  → Role Defense
  Mode Default_Switch  → Unknown
*/

void player() {
  Serial.print("Player");
  // === Initialization ===
  setAttackMode(BLUE);     // Set goal to shoot on
  setRobotRole(OFFENSE);     // Set initial role
  setupVariables();
  // while(1){
  //   setMotors(20,20,-20,-20);
  // }

  while (1) {
    // === Core Sensor & Input Processing ===
    updateButton();
    displayUpdate();
    getCameraData();
    processSensors();
    getCoordinate();
    // transmitData();
    // receiveData();

    setLED();
    checkButton();
    checkLED();

    // === Role Handling ===
    if (getRobotRole() == OFFENSE) {
      offenseMain();
    } else {
      defenseMain();
    }

    //goToCoordinate(0, 0);
    // === Actuation ===

    move();
    // setMotors(99,99,99,99);

    // === Debugging ===
    // Serial.print(" Dir: ");
    // Serial.print(getDir());
    // Serial.print(" Speed: ");
    // Serial.print(getSpeed());

    // Serial.println(getReceiveArr(0));

  }
}
