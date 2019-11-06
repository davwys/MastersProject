#include <Arduino.h>
#include "User_Setup.h"

String incomingBytes = ""; // for incoming serial data
int LED_Red = 3;
int LED_Green = 4;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Red, OUTPUT);
    pinMode(LED_Green, OUTPUT);
    Serial.begin(57600);
}

void loop() {

    if (Serial.available() > 0) {
        // read the incoming data:
        incomingBytes = Serial.readStringUntil('\n');

        Serial.print(incomingBytes);

        if(incomingBytes.indexOf("test") > -1)
        {
            digitalWrite(LED_Green, HIGH);   // turn the LED on
            delay(500);                       // wait for half a second
            digitalWrite(LED_Green, LOW);    // turn the LED offasdf
        }
        else{
            digitalWrite(LED_Red, HIGH);   // turn the LED on
            delay(500);                       // wait for half a second
            digitalWrite(LED_Red, LOW);    // turn the LED off
        }
    }
}