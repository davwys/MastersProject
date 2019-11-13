#include <Arduino.h>
#include <definitions.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

/*

This file specifies analysis and activation behavior for command input/output.

================
Possible states:
================

0	  READY
1	  PREPLAYING
2	  PRETRAINING
3	  TRAINING
4	  UPLOAD
5	  PLAYING


=============
Command List:
=============

CHANGE_MODE=x where x = 0..5		Input
TRAIN_OK					              Input
UPLOAD_START				            Input
[...] {json}					          Input
UPLOAD_END			                Input
PLAY_OK					                Input
REBOOT                          Input

TRAIN=P3_0x123				          Output
UPLOAD_OK					              Output
PLAY=A_123					            Output

*/

const char *InputCommands[6] = {
  "CHANGE_MODE=",
  "TRAIN_OK",
  "PLAY_OK",
  "UPLOAD_START",
  "UPLOAD_END",
  "REBOOT"
};

//Force the ESP32 to restart
void hard_restart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}

//Flashes the selected LED for 300ms
void flash_led(int pin) {
    digitalWrite(pin, HIGH);   // turn LED on
    delay(300);                //wait
    digitalWrite(pin, LOW);    // turn LED off
}


//Validates a given command - TODO validate upload data
bool validate_command(String command){
  for(int i = 0; i <= 5; i++){
    if (receivedData.indexOf(InputCommands[i]) == 0)
      return true;
  }
  return false;
}


//Checks whether the command is a mode change and applies it
//(returns true if it is, false otherwise)
bool apply_mode_change(String command){
  //Check whether this is a mode change command
  if (command.indexOf(InputCommands[0]) == 0){
      //Get command value
      char temp = command.charAt(strlen(InputCommands[0])); // Use number after, for example CHANGE_MODE=1
      int cmd = (int)temp - 48; //Due to UTF/ASCII-Encoding, we subtract 48 to get the actual number

      //Apply status change
      if(cmd >=0 && cmd <= 5){
        currentStatus = Status(cmd);
        Serial.print("Changed to mode ");
        Serial.print(cmd);
        return true;
      }
      else{
         Serial.println("Mode change failed, wrong input data?");
         return false;
      }
  }
  else
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


      //Check if this is a mode change (and apply if yes), else handle other commands
      if(!apply_mode_change(receivedData))
      {

        //Reboot command: Reboot ESP
        if(receivedData.indexOf("REBOOT") >= 0){
          hard_restart();
        }

        //TODO other command types
        Serial.println("Got a valid non-mode change command");
      }
    }
    //Invalid command type: flash red LED
    else
    {
        Serial.println("Invalid command");
    }
  }

//Sends serial output commands
void send_command(String cmd){
  //TODO
  Serial.print(cmd);
}
