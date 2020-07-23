#include <behaviors.h>
#include <definitions.h>
#include <Arduino.h>
#include <CyMCP23016.h>
#include <Adafruit_PN532.h>
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

//Checks a sensor for training input and generates correct API call from learned mapping
void play_on_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander_1, CyMCP23016 expander_2){
  try{

    String tmp;

    if(id <= 5){
      tmp = readTag(sensor, id, false, expander_1);
    }
    else{
      tmp = readTag(sensor, id, false, expander_2);
    }

    //Get area name from mapping
    String areaName = mapping[id-1];


    if(areaName.length() > 0){

      //Get area type for this sensor from mapping (regular or combinatorial)
      bool regular = sensorTypes[id-1];

      //Tmp looks like: SensorID=1_CardID=2
      if(tmp.length() > 4){
        //Extract sensor ID string & card ID string
        String sid_str = split(tmp, '_', 0);
        String cid_str = tmp.substring(tmp.indexOf("CardID="));

        //Get card ID as int
        int cid = cid_str.substring(7).toInt();

        //Whether this is a new card
        bool change = false;

        //Check if this was a change
        if(playedCards[id-1] == 0 || playedCards[id-1] != cid){
          change = true;

          //For combinatorial, check whether the card was changed (without no-card frames in between)
          if(!regular && playedCards[id-1] != cid && playedCards[id-1] != 0){
            String output = "Area='" + areaName + "'_"+ cid_str + "_Type=CR";
            //Send on both bluetooth and USB
            Serial.println("PLAY={" + output + "}");
            BTSerial.println("PLAY={" + output + "}");

            //Turn on corresponding LED
            switch(id){
              case 1:
                expander_1.digitalWrite(LED_1, LOW);
                break;
              case 2:
                expander_1.digitalWrite(LED_2, LOW);
                break;
              case 3:
                expander_1.digitalWrite(LED_3, LOW);
                break;
              case 4:
                expander_1.digitalWrite(LED_4, LOW);
                break;
              case 5:
                expander_1.digitalWrite(LED_5, LOW);
                break;
              case 6:
                expander_2.digitalWrite(LED_6, LOW);
                break;
              case 7:
                expander_2.digitalWrite(LED_7, LOW);
                break;
              case 8:
                expander_2.digitalWrite(LED_8, LOW);
                break;
              case 9:
                expander_2.digitalWrite(LED_9, LOW);
                break;
              case 10:
                expander_2.digitalWrite(LED_10, LOW);
                break;
            }

            //Flash COM LED
            flash_led(LED_Com);
          }
        }

        if(change){
          playedCards[id-1] = cid;

          String typeName = regular ? "Type=RP" : "Type=CP";
          //Generate output: "Area='something'_CardID=123"
          String output = "Area='" + areaName + "'_"+ cid_str + "_" + typeName;

          //Send on both bluetooth and USB
          Serial.println("PLAY={" + output + "}");
          BTSerial.println("PLAY={" + output + "}");

          if(regular){
            //Flash corresponding sensor's LED
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
          else{
            //Turn on corresponding LED when card is placed
            switch(id){
              case 1:
                expander_1.digitalWrite(LED_1, HIGH);
                break;
              case 2:
                expander_1.digitalWrite(LED_2, HIGH);
                break;
              case 3:
                expander_1.digitalWrite(LED_3, HIGH);
                break;
              case 4:
                expander_1.digitalWrite(LED_4, HIGH);
                break;
              case 5:
                expander_1.digitalWrite(LED_5, HIGH);
                break;
              case 6:
                expander_2.digitalWrite(LED_6, HIGH);
                break;
              case 7:
                expander_2.digitalWrite(LED_7, HIGH);
                break;
              case 8:
                expander_2.digitalWrite(LED_8, HIGH);
                break;
              case 9:
                expander_2.digitalWrite(LED_9, HIGH);
                break;
              case 10:
                expander_2.digitalWrite(LED_10, HIGH);
                break;
            }
          }

          //Flash COM LED
          flash_led(LED_Com);
        }
        //playing_ready = false; TODO add
      }
      //If card is no longer present
      else{
        if(tmp.indexOf("NONE") >= 0){

          //Whether this was a change
          if(playedCards[id-1] != 0){
            //For combinatorial, send call upon removal
            if(!regular){

              String cid = String(playedCards[id-1]);
              String add = cid.length() == 1 ? "00" : cid.length() == 2? "0" : "";
              String cid_str = "CardID=" + add + cid;

              String output = "Area='" + areaName + "'_"+ cid_str+ "_Type=CR";
              //Send on both bluetooth and USB
              Serial.println("PLAY={" + output + "}");
              BTSerial.println("PLAY={" + output + "}");

              //Turn off corresponding LED when card is removed
              switch(id){
                case 1:
                  expander_1.digitalWrite(LED_1, LOW);
                  break;
                case 2:
                  expander_1.digitalWrite(LED_2, LOW);
                  break;
                case 3:
                  expander_1.digitalWrite(LED_3, LOW);
                  break;
                case 4:
                  expander_1.digitalWrite(LED_4, LOW);
                  break;
                case 5:
                  expander_1.digitalWrite(LED_5, LOW);
                  break;
                case 6:
                  expander_2.digitalWrite(LED_6, LOW);
                  break;
                case 7:
                  expander_2.digitalWrite(LED_7, LOW);
                  break;
                case 8:
                  expander_2.digitalWrite(LED_8, LOW);
                  break;
                case 9:
                  expander_2.digitalWrite(LED_9, LOW);
                  break;
                case 10:
                  expander_2.digitalWrite(LED_10, LOW);
                  break;
              }
              //Flash COM LED
              flash_led(LED_Com);
            }

            //Save that card was removed
            playedCards[id-1] = 0;
          }
        }
      }
    }
  }
  catch (std::runtime_error e){} //Catch errors for incomplete data
}

//Reads mapping data from flash memory and saves to local array
void read_mapping_data(){
  //Read the entire saved mapping
  //example: {1, 'AreaA'}{2, 'Oracle'}{3, 'AreaB'}
  String temp = read_string_from_flash(0);

  //Convert to char array (max size 256 bytes)
  char cmd[256];
  temp.toCharArray(cmd, 256);

  //Find divider char: since our input is divided into {} blocks, we use }{ as our division char
  char* command = strtok(cmd, "}{");

  while (command != 0)
  {
      // Split the command in two values: Sensor ID and Area Name
      char* separator = strchr(command, '=');
      if (separator != 0)
      {
          // Actually split the string in 2: replace '=' with 0
          *separator = 0;
          int sensorId = atoi(command);
          ++separator;
          String areaName = separator;

          //Now, area name looks like ex: "Actions_0", thus we remove the sensor type
          String t = areaName.substring(areaName.indexOf('_')+1);
          int type = t.toInt();
          areaName = areaName.substring(0, areaName.indexOf('_'));
          //Serial.print("Area " + areaName + " is of type " + t);

          //Save values to mapping array
          mapping[sensorId-1] = areaName; //Sensor 1 gets saved to position 0
          sensorTypes[sensorId-1] = type; //Area Type 1 gets saved to position 0
      }
      // Find the next command in input string
      command = strtok(0, "}{");
  }
}


//Main function when in PLAYING mode
void playing_main(CyMCP23016 expander_1, CyMCP23016 expander_2){
  //If we haven't loaded the learned mapping yet, load it from flash
  bool load = true;
  for(int i = 0; i < 10; i++){
    if(mapping[i] != NULL){
      load = false;
    }
  }
  if(load){
    read_mapping_data();
  }
  //Actual playing logic: capture sensor input & return API calls
  else{
      play_on_sensor(sensor1, 1, expander_1, expander_2);
      play_on_sensor(sensor2, 2, expander_1, expander_2);
      play_on_sensor(sensor3, 3, expander_1, expander_2);
      play_on_sensor(sensor4, 4, expander_1, expander_2);
      play_on_sensor(sensor5, 5, expander_1, expander_2);
      play_on_sensor(sensor6, 6, expander_1, expander_2);
      play_on_sensor(sensor7, 7, expander_1, expander_2);
      play_on_sensor(sensor8, 8, expander_1, expander_2);
      play_on_sensor(sensor9, 9, expander_1, expander_2);
      play_on_sensor(sensor10, 10, expander_1, expander_2);
  }
}
