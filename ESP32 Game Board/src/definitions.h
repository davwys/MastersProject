#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Adafruit_PN532.h>
#include <CyMCP23016.h>

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
extern int LED_1;
extern int LED_2;
extern int LED_3;
extern int LED_4;
extern int LED_5;
extern int LED_6;
extern int LED_7;
extern int LED_8;
extern int LED_9;
extern int LED_10;

extern int led_array[3][2];
extern int led_array_expander[10][2];

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

void initialize_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander, CyMCP23016 expander_led);
String readTag(Adafruit_PN532 sensor, int id, bool verbose, CyMCP23016 expander);

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
extern Adafruit_PN532 sensor9;
extern Adafruit_PN532 sensor10;


/*
=============
Training data
=============
*/
extern bool training_ready;

//Names the last played sensor
extern bool playedSensors[10];

//The trained mapping for playing
extern String mapping[10];
extern bool sensorTypes[10];

//Names the last played cards
extern int playedCards[10];

#endif
