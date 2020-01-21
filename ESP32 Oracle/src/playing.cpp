#include <behaviors.h>
#include <definitions.h>
#include <Arduino.h>
#include <stdexcept>

//helper function for splitting string
String split(String s, char parser, int index) {
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    } else parserCnt++;
  }
  return rs;
}


//Determines whether any card position has changed since the last call was sent
bool has_changed = false;

//Checks a sensor for training input and generates correct API call from learned mapping
void play_on_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander){
  try{
    String tmp = readTag(sensor, id, false, expander);
    //Tmp looks like: SensorID=1_CardID=2
    if(tmp.length() > 4){

      //Whether this was a change
      has_changed = activeSensors[id-1] == false;

      //Save sensor as currently active
      activeSensors[id-1] = true;

      //Turn on Sensor's LED
      switch(id){
        case 1:
          expander.digitalWrite(LED_1, HIGH);
          break;
        case 2:
          expander.digitalWrite(LED_2, HIGH);
        case 3:
          expander.digitalWrite(LED_3, HIGH);
          break;
        case 4:
          expander.digitalWrite(LED_4, HIGH);
          break;
        default:
          break;
      }

      //Extract sensor ID string & card ID string
      String sid_str = split(tmp, '_', 0);
      String cid_str = tmp.substring(tmp.indexOf("CardID="));


      //Extract sensor ID to get name afterwards
      sid_str.replace("SensorID=", "");
      cid_str.replace("CardID=", "");
      char sid_char[64];
      char cid_char[64];
      sid_str.toCharArray(sid_char, 64);
      cid_str.toCharArray(cid_char, 64);

      int sid = atoi(sid_char);
      int cid = atoi(cid_char);


      //Whether this was a change
      has_changed = playedCards[id-1] != cid;

      playedCards[id-1] = cid;

    }

    //When card is removed / no longer present
    else{
      if(tmp.indexOf("NONE") >= 0){

        //Whether this was a change
        has_changed = activeSensors[id-1] == true;

        //Turn off Sensor's LED
        switch(id){
          case 1:
            expander.digitalWrite(LED_1, LOW);
            break;
          case 2:
            expander.digitalWrite(LED_2, LOW);
            break;
          case 3:
            expander.digitalWrite(LED_3, LOW);
            break;
          case 4:
            expander.digitalWrite(LED_4, LOW);
            break;
          default:
            break;
        }

        //Save sensor as currently inactive
        activeSensors[id-1] = false;

        playedCards[id-1] = NULL;

      }
    }
  }
  catch (std::runtime_error e){} //Catch errors for incomplete data

  //If this is the last sensor, send call that is a combination of all played cards
  if(has_changed){ //&& id >= sensorCount){

    bool any_played = false;
    String call = "ORACLE={";

    //Check for all sensors
    for(int i = 0; i < 4; i++){
      if(playedCards[i] != NULL){
        any_played = true;
        call += String(playedCards[i]) + '_';
      }
    }
    if(any_played){
      call = call.substring(0, call.length()-1);
      call += "}";
      Serial.println(call);
      BTSerial.println(call);

      has_changed = false;
    }
    else{
      Serial.println("ORACLE={}");
      BTSerial.println("ORACLE={}");
    }
  }
}


//Main function when in PLAYING mode
void playing_main(CyMCP23016 expander){
  play_on_sensor(sensor1, 1, expander);
  play_on_sensor(sensor2, 2, expander);
  play_on_sensor(sensor3, 3, expander);
  play_on_sensor(sensor4, 4, expander);
}
