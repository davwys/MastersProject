#include <Arduino.h>
#include <definitions.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include <CyMCP23016.h>

/*

This file specifies analysis and activation behavior for command input/output.

================
Possible states:
================

0	  READY
1	  TRAINING
2	  UPLOAD
3	  PLAYING


=============
Command List:
=============

CHANGE_MODE=x where x = 0..3		Input
TRAIN_OK					              Input
UPLOAD_END			                Input -> only applicable in upload mode, thus not handled here
PLAY_OK					                Input
REBOOT                          Input
RESTART_TRAINING                Input
TRAIN={SensorID=3_CardID=72}    Output
PLAY={Area='Oracle'_CardID=12_RP}  Output  --> Available types: RP = Regular, played, CP = Combinatorial, played, CR = Combinatorial, removed

*/

const char *InputCommands[6] = {
  "CHANGE_MODE=",
  "TRAIN_OK",
  "PLAY_OK",
  "REBOOT",
  "RESTART_TRAINING",
  "TRAIN_UNDO="
};

//Force the ESP32 to restart
void hard_restart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true); //Infinite loop forces ESP32 to do full reboot
}

//Flashes the selected LED for 300ms
void flash_led(int pin) {
    digitalWrite(pin, HIGH);   //turn LED on
    delay(300);                //wait TODO rewrite without delay
    digitalWrite(pin, LOW);    //turn LED off
}

//Flashes the selected LED for 300ms on an expander
void flash_led(int pin, CyMCP23016 expander) {
    expander.digitalWrite(pin, HIGH);   //turn LED on
    delay(300);                //wait TODO rewrite without delay
    expander.digitalWrite(pin, LOW);    //turn LED off
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
      if(cmd >=0 && cmd <= 3){
        currentMode = Mode(cmd);
        //Don't print mode change to training mode
        if(cmd != 1){
          Serial.print("Changed to mode ");
          Serial.print(cmd);
          BTSerial.print("Changed to mode ");
          BTSerial.print(cmd);
        }
        return true;
      }
      else{
         Serial.println("Mode change failed, wrong input data?");
         BTSerial.println("Mode change failed, wrong input data?");
         return false;
      }
  }
  else
    return false;
}

//Restarts the training process
void restart_training(){
  for(int i = 0; i < 10; i++){
    playedSensors[i] = false;
  }
  training_ready = true;
}

//Undoes training step on a certain sensor
void undo_training(int sid){
  //Apply only for valid sensor numbers
  if(sid >= 0 && sid < 10){
      playedSensors[sid] = false;
      training_ready = true;;
  }
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
        //Training OK command
        else if(receivedData.indexOf("TRAIN_OK") >= 0 && currentMode == Mode(TRAINING)){
          training_ready = true;
        }
        //Training restart command
        else if(receivedData.indexOf("RESTART_TRAINING") >= 0 && currentMode == Mode(TRAINING)){
          restart_training();
        }
        //Training undo command
        else if(receivedData.indexOf("TRAIN_UNDO=") >= 0 && currentMode == Mode(TRAINING)){
          //Get sensor number for which to undo training
          int sid = receivedData.substring(11).toInt();
          undo_training(sid);
        }
        //Play_ok: flash COM LED
        else if(receivedData.indexOf("PLAY_OK") >= 0 && currentMode == Mode(PLAYING)){
          flash_led(LED_Com);
        }
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
