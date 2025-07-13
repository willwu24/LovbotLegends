#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include "core.h"

// === BLUETOOTH CONSTANTS ===
#define ARR_LENGTH 4

// === BLUETOOTH FUNCTIONS ===
void setupBluetooth();
void transmitData();
void transmit(int data);
void receiveData();
int receive();
int getReceiveArr(int index);
int getSendArr(int index);
void changeSendArr(int index, int data);
int getArrLength();

#endif // COMMUNICATION_H
