#include <Arduino.h>
#include <definitions.h>
#include <behaviors.h>



//LED Pins
int LED_Red = 4;
int LED_Green = 2;


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
}

//Main function
void loop() {

    //Received command analysis TODO check different serial sources: bluetooth, USB etc.
    if (Serial.available() > 0) {
        receive_command();
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
