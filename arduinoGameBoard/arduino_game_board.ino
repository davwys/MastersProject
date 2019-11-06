#include <Arduino.h>
#include "User_Setup.h"

/*  ==========================
    Status Enum for game board
    ==========================

    READY       = ready for training or game start
    CONNECTED   = connected to training dashboard, not training yet (serial port open - prevents game start)
    TRAINING    = training new data via dashboard (sends sensor inputs to dashboard via specified port)
    PLAYING     = actively playing the game (outputs API calls for game logic)
 */

enum status {
    READY,
    CONNECTED,
    TRAINING,
    PLAYING
};


/* ==================
   Physical pin setup
   ================== */

//LED Pins
int LED_Red = 3;
int LED_Green = 4;

/* =====================
   Logic variables setup
   ===================== */

String receivedData = "";       //Incoming Serial data string
status currentStatus = READY;   //Game board's current status


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Red, OUTPUT);
    pinMode(LED_Green, OUTPUT);
    Serial.begin(57600);


}

void loop() {

    if (Serial.available() > 0) {
        // read the incoming data:
        receivedData = Serial.readStringUntil('\n');

        Serial.print(receivedData);

        if(receivedData.indexOf("test") > -1)
        {
            digitalWrite(LED_Green, HIGH);   // turn the LED on
            delay(500);                       // wait for half a second
            digitalWrite(LED_Green, LOW);    // turn the LED off
        }
        else{
            digitalWrite(LED_Red, HIGH);   // turn the LED on
            delay(500);                       // wait for half a second
            digitalWrite(LED_Red, LOW);    // turn the LED off
        }
    }
}