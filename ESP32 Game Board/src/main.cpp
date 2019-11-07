#include <Arduino.h>
#include <definitions.h>

#include <training.cpp>
#include <upload.cpp>
#inclue <playing.cpp>

//Flashes the selected LED for 300ms (used for input confirmation)
void flashLed(int pin) {
    digitalWrite(pin, HIGH);   // turn LED on
    delay(300);                //wait
    digitalWrite(pin, LOW);    // turn LED off
}


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

    //Received command analysis TODO move/make available to each file
    if (Serial.available() > 0) {
        //Read incoming data via USB
        receivedData = Serial.readStringUntil('\n');


        if (receivedData.length() > 0) {
            //Command type: Mode change
            if (receivedData.indexOf("CHANGE_MODE=") == 0) {
                flashLed(LED_Green);

                //Get command value
                char temp = receivedData.charAt(12); // Use number after, for example CHANGE_MODE=1
                int cmd = (int)temp;

                Serial.print("Mode change to {}" + cmd);
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

    //Main Behavior loop
    switch (currentStatus)
      {
         case READY:
            //TODO
            break;
         case PRETRAINING:
            //TODO
            break;
         case PREPLAYING:
            //TODO
            break;
         case TRAINING:
            //TODO
            break;
         case UPLOAD:
            //TODO
            break;
         case PLAYING:
            //TODO
            break;

         default:
            //TODO default case? Reset to ready?
            break;
      }
}
