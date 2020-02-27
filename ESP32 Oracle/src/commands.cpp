#include <Arduino.h>
#include <definitions.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

/*

This file specifies analysis and activation behavior for command input/output.

=============
Command List:
=============

PLAY_OK					                Input
REBOOT                          Input
ORACLE={...}                    Output

*/

const char *InputCommands[2] = {
  "PLAY_OK",
  "REBOOT",
};

//Force the ESP32 to restart
void hard_restart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}


//Update all LED statuses (called in loop)
void update_leds(){

    int currentTime = int(millis());

    //Update COM LED
    if(com_timer != -1 && com_timer < currentTime){
      //Turn off LED
      digitalWrite(LED_Com, LOW);

      //Disable timer for this LED
      com_timer = -1;
    }
}


//Flashes the selected LED for 300ms
void flash_led(int pin){
    //Save current system time
    unsigned long currentMillis = millis();
    //Desired delay in ms
    int blinkTime = 300;
    unsigned long endMillis = currentMillis + blinkTime;

    com_timer = int(endMillis);

    //Turn LED on
    digitalWrite(pin, HIGH);
    //Call update_leds() to turn LED off after specified time
}




//Validates a given command - TODO validate upload data
bool validate_command(String command){
  for(int i = 0; i < 2; i++){
    if (receivedData.indexOf(InputCommands[i]) == 0)
      return true;
  }
  return false;
}

// Receives and executes serial input commands.
// "usb" argument determines whether to check for input on the USB or Bluetooth serial port.
void receive_command(bool usb){

  //Read incoming data via USB
  if(usb)
    receivedData = Serial.readStringUntil('\n');
  else
    receivedData = BTSerial.readStringUntil('\n');

  //Validate command input
  if (receivedData.length() > 0 && validate_command(receivedData) == true )
  {
      flash_led(LED_Com);
      //Reboot command: Reboot ESP
      if(receivedData.indexOf("REBOOT") >= 0){
        hard_restart();
      }
      //Play_ok: flash COM LED
      else if(receivedData.indexOf("PLAY_OK") >= 0){
        //Maybe do something
      }

    }
    //Invalid command type: flash red LED
    else
    {
        if(usb)
          Serial.println("Invalid command");
        else
          BTSerial.println("Invalid command");
    }
  }
