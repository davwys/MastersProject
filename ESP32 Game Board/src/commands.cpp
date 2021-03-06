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


//Update all LED statuses (called in loop)
void update_leds(CyMCP23016 expander){

    int currentTime = int(millis());

    //Update system LEDs
    for(int i = 0; i < 3; i++){
      //Serial.print("checking status for LED at position ");
      //Serial.println(i);

      //If this LED has a turn-off time currently set and it's already passed
      if(led_array[i][1] != -1 && led_array[i][1] < currentTime){
        //Turn off LED
        digitalWrite(led_array[i][0], LOW);

        //Disable timer for this LED
        led_array[i][0] = 0;
        led_array[i][1] = -1;
      }

    }

    //Update expander LEDs
    for(int i = 0; i < 10; i++){
      //If this LED has a turn-off time currently set and it's already passed
      if(led_array_expander[i][0] != 0 && led_array_expander[i][1] != -1 && led_array_expander[i][1] < currentTime){
        //Turn off LED
        expander.digitalWrite(led_array_expander[i][0], LOW);

        //Disable timer for this LED
        led_array_expander[i][0] = 0;
        led_array_expander[i][1] = -1;
      }
    }
}


//Flashes the selected LED for 300ms
void flash_led(int pin){
    //Save current system time
    unsigned long currentMillis = millis();
    //Desired delay in ms
    int blinkTime = 300;
    unsigned long endMillis = currentMillis + blinkTime;

    //Find free spot in LED array
    int pos = -1;
    for(int i = 0; i < 3; i++){
      if((led_array[i][0] == 0 ||  led_array[i][0] == pin) && (i <= pos || pos == -1)){
        pos = i;
      }
    }

    if(pos >= 0){
      led_array[pos][0] = pin;
      led_array[pos][1] = int(endMillis);

      //Turn LED on
      digitalWrite(pin, HIGH);
      //Call update_leds() to turn LED off after specified time
    }
}

void flash_all_leds(int loops){

  for(int i = 0; i < loops; i++){
    expander_1.digitalWrite(LED_1, LOW);
    expander_1.digitalWrite(LED_2, LOW);
    expander_1.digitalWrite(LED_3, LOW);
    expander_1.digitalWrite(LED_4, LOW);
    expander_1.digitalWrite(LED_5, LOW);
    expander_2.digitalWrite(LED_6, LOW);
    expander_2.digitalWrite(LED_7, LOW);
    expander_2.digitalWrite(LED_8, LOW);
    expander_2.digitalWrite(LED_9, LOW);
    expander_2.digitalWrite(LED_10, LOW);

    delay(400);

    expander_1.digitalWrite(LED_1, HIGH);
    expander_1.digitalWrite(LED_2, HIGH);
    expander_1.digitalWrite(LED_3, HIGH);
    expander_1.digitalWrite(LED_4, HIGH);
    expander_1.digitalWrite(LED_5, HIGH);
    expander_2.digitalWrite(LED_6, HIGH);
    expander_2.digitalWrite(LED_7, HIGH);
    expander_2.digitalWrite(LED_8, HIGH);
    expander_2.digitalWrite(LED_9, HIGH);
    expander_2.digitalWrite(LED_10, HIGH);

    delay(400);
  }

  expander_1.digitalWrite(LED_1, LOW);
  expander_1.digitalWrite(LED_2, LOW);
  expander_1.digitalWrite(LED_3, LOW);
  expander_1.digitalWrite(LED_4, LOW);
  expander_1.digitalWrite(LED_5, LOW);
  expander_2.digitalWrite(LED_6, LOW);
  expander_2.digitalWrite(LED_7, LOW);
  expander_2.digitalWrite(LED_8, LOW);
  expander_2.digitalWrite(LED_9, LOW);
  expander_2.digitalWrite(LED_10, LOW);
}

//Flashes the selected LED for 300ms on an expander
void flash_led(int pin, CyMCP23016 expander) {
    //Save current system time
    unsigned long currentMillis = millis();
    //Desired delay in ms
    int blinkTime = 300;
    unsigned long endMillis = currentMillis + blinkTime;

    //Find free spot in LED array
    int pos = -1;
    for(int i = 0; i < 10; i++){
      if((led_array_expander[i][0] == 0 || led_array_expander[i][0] == pin) && (i <= pos || pos == -1) ){
        pos = i;
      }
    }

    if(pos >= 0){
      led_array_expander[pos][0] = pin;
      led_array_expander[pos][1] = int(endMillis);

      //Turn LED on
      expander.digitalWrite(pin, HIGH);
      //Call update_leds() to turn LED off after specified time
    }
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

          //Upon entering training mode, clear data
          if(cmd == 1){
            for(int i = 0; i < 10; i++){
              playedSensors[i] = false;
            }
          }
        }

        flash_led(LED_Com);
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

      //Check if this is a mode change (and apply if yes), else handle other commands
      if(!apply_mode_change(receivedData))
      {
        //Reboot command: Reboot ESP
        if(receivedData.indexOf("REBOOT") >= 0){
          flash_led(LED_Com);
          hard_restart();
        }
        //Training OK command
        else if(receivedData.indexOf("TRAIN_OK") >= 0 && currentMode == Mode(TRAINING)){
          flash_led(LED_Com);
          training_ready = true;
        }
        //Training restart command
        else if(receivedData.indexOf("RESTART_TRAINING") >= 0 && currentMode == Mode(TRAINING)){
          flash_led(LED_Com);
          restart_training();
        }
        //Training undo command
        else if(receivedData.indexOf("TRAIN_UNDO=") >= 0 && currentMode == Mode(TRAINING)){
          flash_led(LED_Com);
          //Get sensor number for which to undo training
          int sid = receivedData.substring(11).toInt();
          undo_training(sid);
        }
        //Play_ok: only flash COM LED
        else if(receivedData.indexOf("PLAY_OK") >= 0 && currentMode == Mode(PLAYING)){
          flash_led(LED_Com);
        }
      }
    }
    //Invalid command type
    else
    {
        if(usb)
          Serial.println("Invalid command");
        else
          BTSerial.println("Invalid command");
    }
  }
