#include <behaviors.h>
#include <definitions.h>
#include <definitions.h>
#include <Adafruit_PN532.h>
#include <CyMCP23016.h>
#include <stdexcept>

/*
    Behavior while in training mode:
    - if ready and card is played, return raw sensor values via serial
    - wait for TRAIN_OK response
    - continue until CHANGE_MODE=4, switch to upload mode
*/

//Determines whether a card can be played at the moment
bool training_ready = true;

//Names the last played sensor
bool playedSensors[10] = {false, false, false, false, false, false, false, false, false, false};

//Checks a sensor for training input and generates training output packet
void train_on_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander_sens, CyMCP23016 expander_led){
  if(!playedSensors[id]){
    try{
      String tmp = readTag(sensor, id, false, expander_sens);
      if(tmp.length() > 4){
        playedSensors[id] = true;
        Serial.println("TRAIN={" + tmp + "}");
        BTSerial.println("TRAIN={" + tmp + "}");
        training_ready = false;
        //Flash communication LED to confirm sending command
        flash_led(LED_Com);
      }
    }
    catch (std::runtime_error e){} //Catch errors for incomplete data
  }
}

//Main training function
void training_main(CyMCP23016 expander_sens, CyMCP23016 expander_led){
  //If training dashboard is ready for new data - otherwise, we await TRAIN_OK command
  if(training_ready){
     //TODO add other sensors
     train_on_sensor(sensor1, 1, expander_sens, expander_led);
     train_on_sensor(sensor2, 2, expander_sens, expander_led);
     train_on_sensor(sensor3, 3, expander_sens, expander_led);

  }
}
