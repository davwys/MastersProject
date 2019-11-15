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
}Mode;


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
extern Mode currentMode;
extern String receivedData;


/*
==================
NFC Sensor setup
=================
*/

void initialize_sensor(Adafruit_PN532 sensor, int id);
void readTag(Adafruit_PN532 sensor, int id, bool verbose);

extern int sensorCount;

//Sensors defined in main.cpp
extern Adafruit_PN532 sensor1;
extern Adafruit_PN532 sensor2;
extern Adafruit_PN532 sensor3;
extern Adafruit_PN532 sensor4;
extern Adafruit_PN532 sensor5;
extern Adafruit_PN532 sensor6;
extern Adafruit_PN532 sensor7;
extern Adafruit_PN532 sensor8;
//extern Adafruit_PN532 sensor9; //TODO
//extern Adafruit_PN532 sensor10;

#endif
