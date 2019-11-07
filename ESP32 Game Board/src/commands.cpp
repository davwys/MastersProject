#include <Arduino.h>
#include <definitions.h>

// This file specifies analysis behavior for command input/output



//Flashes the selected LED for 300ms (used for input confirmation)
void flash_led(int pin) {
    digitalWrite(pin, HIGH);   // turn LED on
    delay(300);                //wait
    digitalWrite(pin, LOW);    // turn LED off
}



void read_command(){
  //Read incoming data via USB
  receivedData = Serial.readStringUntil('\n');

  if (receivedData.length() > 0) {
      //Command type: Mode change
      if (receivedData.indexOf("CHANGE_MODE=") == 0) {
          flash_led(LED_Green);

          //Get command value
          char temp = receivedData.charAt(12); // Use number after, for example CHANGE_MODE=1
          int cmd = (int)temp;

          Serial.print("Mode change to {}" + cmd);
      }
      //Command type:
      else if (receivedData.indexOf("test") == 0) {
          flash_led(LED_Green);
      }
      //Invalid command type: flash red LED
      else {
          flash_led(LED_Red);
      }
  }
}
