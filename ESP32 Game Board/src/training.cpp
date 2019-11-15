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

//Checks a sensor for training input and generates training output packet
void trainOnSensor(Adafruit_PN532 sensor, int id){
    try{
      String tmp = readTag(sensor, id, false);
      if(tmp.length() > 4){
        Serial.println("TRAIN={" + tmp + "}");
      }
    }
    catch (std::runtime_error e){
      //Catch errors for incomplete data
    }
}


void training_main(){

  //If training dashboard is ready for new data
  if(training_ready){
    //TODO add other sensors
    trainOnSensor(sensor1, 1);
    trainOnSensor(sensor2, 2);
    trainOnSensor(sensor3, 3);
  }
  else{
    //TODO: Await TRAIN_OK
  }
}
