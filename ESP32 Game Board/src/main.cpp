#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <behaviors.h>
#include <EEPROM.h>


//LED Pins
int LED_Red = 4;
int LED_Green = 2;

BluetoothSerial BTSerial; //Object for Bluetooth

//TODO descr
Status currentStatus = READY;
String receivedData = "";

//Main setup function (runs on initialization)
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Red, OUTPUT);
    pinMode(LED_Green, OUTPUT);

    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("GameBoardBluetooth");

    //Initialize Flash storage (using EEPROM lib)
    EEPROM.begin(EEPROM_SIZE);

    Serial.println();
    Serial.println("Initializing ESP32 Game Board...");
    Serial.print("Current Status: ");
    Serial.println(currentStatus);
}

//Main function
void loop() {

    if(currentStatus != UPLOAD){
      //Received command analysis (USB)
      if (Serial.available() > 0) {
          receive_command(true);
      }
      //Received command analysis (Bluetooth).
      if (BTSerial.available())
       {
         receive_command(false);
       }
     }
    //Main Behavior loop
    switch (currentStatus)
      {
         case READY:
            //TODO?
            break;
         case PRETRAINING:
            //TODO
            break;
         case PREPLAYING:
            //TODO
            break;
         case TRAINING:
            training_main();
            break;
         case UPLOAD:
            upload_main();
            break;
         case PLAYING:
            playing_main();
            break;

         default:
            //default case: Reset to ready (should never happen)
            currentStatus = READY;
            break;
      }
}
