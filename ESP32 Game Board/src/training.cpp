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
void train_on_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander_1, CyMCP23016 expander_2){
  if(!playedSensors[id]){
    try{

      String tmp;

      //Use ID to determine which expander to use
      if(id <= 5){
        tmp = readTag(sensor, id, false, expander_1);
      }
      else{
        tmp = readTag(sensor, id, false, expander_2);
      }

      if(tmp.length() > 4){
        playedSensors[id] = true;
        Serial.println("TRAIN={" + tmp + "}");
        BTSerial.println("TRAIN={" + tmp + "}");
        training_ready = false;
        //Flash communication LED to confirm sending command
        flash_led(LED_Com);

        //Flash corresponding LED
        switch(id){
          case 1:
            flash_led(LED_1, expander_1);
            break;
          case 2:
            flash_led(LED_2, expander_1);
            break;
          case 3:
            flash_led(LED_3, expander_1);
            break;
          case 4:
            flash_led(LED_4, expander_1);
            break;
          case 5:
            flash_led(LED_5, expander_1);
            break;
          case 6:
            flash_led(LED_6, expander_2);
            break;
          case 7:
            flash_led(LED_7, expander_2);
            break;
          case 8:
            flash_led(LED_8, expander_2);
            break;
          case 9:
            flash_led(LED_9, expander_2);
            break;
          case 10:
            flash_led(LED_10, expander_2);
            break;
          default:
            break;
        }
      }
    }
    catch (std::runtime_error e){} //Catch errors for incomplete data
  }
}

//Main training function
void training_main(CyMCP23016 expander_1, CyMCP23016 expander_2){
  //If training dashboard is ready for new data - otherwise, we await TRAIN_OK command
  if(training_ready){
     train_on_sensor(sensor1, 1, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor2, 2, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor3, 3, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor4, 4, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor5, 5, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor6, 6, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor7, 7, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor8, 8, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor9, 9, expander_1, expander_2);
     delay(200);
     train_on_sensor(sensor10, 10, expander_1, expander_2);
     delay(200);
  }
}
