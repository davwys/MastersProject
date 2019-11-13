#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Adafruit_PN532.h>

// Main definitions
#ifndef DEFINITIONS_H
#define DEFINITIONS_H


/*
==========================
Status Enum for game board
==========================

READY       = ready for training or game start
PRETRAINING   = connected to training dashboard, not training yet (serial port open - prevents game start)
PREPLAYING   = connected to game dashboard, not playing yet (serial port open)
TRAINING    = training new data via dashboard (sends sensor inputs to dashboard via specified port)
UPLOAD      = ready to upload new training data (after training is finished)
PLAYING     = actively playing the game (outputs API calls for game logic)
*/

typedef enum  {
    READY,
    PRETRAINING,
    PREPLAYING,
    TRAINING,
    UPLOAD,
    PLAYING
}Status;


/*
==================
Physical pin setup
==================
*/

//LED Pins
extern int LED_Pwr;
extern int LED_Sta;
extern int LED_Com;

//Storage handling
extern int currentByte;
//Bluetooth Serial object
extern BluetoothSerial BTSerial;

//Core logic data
extern Status currentStatus;
extern String receivedData;


/*
==================
NFC Sensor setup
=================
*/

extern Adafruit_PN532 sensor1;
extern Adafruit_PN532 sensor2;
extern Adafruit_PN532 sensor3;


#endif
