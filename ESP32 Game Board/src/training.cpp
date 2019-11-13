#include <behaviors.h>
#include <Wire.h>
#include <SPI.h>
// #include <Adafruit_PN532.h>

/*
    Behavior while in training mode:
    - if ready and card is played, return raw sensor values via serial
    - wait for TRAIN_OK response
    - continue until CHANGE_MODE=4, switch to upload mode
*/

//Determines whether a card can be played at the moment
bool training_ready = true;

void training_main(){
  if(training_ready){

  }
  else{
    //TODO: Await TRAIN_OK
  }
}
