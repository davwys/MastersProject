#include <behaviors.h>
#include <definitions.h>
#include <definitions.h>
#include <Adafruit_PN532.h>
#include <stdexcept>

/*
    Behavior while in training mode:
    - if ready and card is played, return raw sensor values via serial
    - wait for TRAIN_OK response
    - continue until CHANGE_MODE=4, switch to upload mode
*/

//Determines whether a card can be played at the moment
bool training_ready = true;

void training_main(){

  //If training dashboard is ready for new data
  if(training_ready){
    try{
      String tmp = readTag(sensor1, 1, false);
      if(tmp.length() > 4){
        Serial.println("TRAIN=" + tmp);
      }
    }
    catch (std::runtime_error e){
      //Catch errors for incomplete data
    }
  }
  else{
    //TODO: Await TRAIN_OK
  }
}
