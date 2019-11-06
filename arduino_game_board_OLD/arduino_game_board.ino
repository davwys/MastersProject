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



//Main setup function (runs on initialization)
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Red, OUTPUT);
    pinMode(LED_Green, OUTPUT);

    //Initial serial communication (via USB)
    Serial.begin(57600);
}

//Flashes the selected LED for 300ms (used for input confirmation)
void flashLed(int pin) {
    digitalWrite(pin, HIGH);   // turn LED on
    delay(300);                //wait
    digitalWrite(pin, LOW);    // turn LED off
}

//Main function
void loop() {
    if (Serial.available() > 0) {
        //Read incoming data via USB
        receivedData = Serial.readStringUntil('\n');


        if (receivedData.length() > 0) {
            //Command type: Mode change
            if (receivedData.indexOf("CHANGE_MODE") == 0) {
                flashLed(LED_Green);
                Serial.print("Mode change OK");
            }
            //Command type:
            else if (receivedData.indexOf("test") == 0) {
                flashLed(LED_Green);
            }
            //Invalid command type: flash red LED
            else {
                flashLed(LED_Red);
            }
        }
    }
}