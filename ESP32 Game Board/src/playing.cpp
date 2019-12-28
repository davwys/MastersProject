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


//Determines whether a card can be played at the moment TODO add timer stuff
bool playing_ready = true;


//Checks a sensor for training input and generates correct API call from learned mapping
void play_on_sensor(Adafruit_PN532 sensor, int id){
  try{
    String tmp = readTag(sensor, id, false);

    //Get area name from mapping
    String areaName = mapping[id-1];
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
          String output = "Area='" + areaName + "'_CardID="+ String(playedCards[id-1]) + "_Type=CR";
          //Send on both bluetooth and USB
          Serial.println("PLAY={" + output + "}");
          BTSerial.println("PLAY={" + output + "}");

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

            String output = "Area='" + areaName + "'_CardID="+ String(playedCards[id-1]) + "Type=_CR";
            //Send on both bluetooth and USB
            Serial.println("PLAY={" + output + "}");
            BTSerial.println("PLAY={" + output + "}");

            //Flash COM LED
            flash_led(LED_Com);
          }

          //Save that card was removed
          playedCards[id-1] = 0;
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
void playing_main(){
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
  //Actual playing logic: capture sensor input (if ready) & return API calls
  else{
    if(playing_ready){
      play_on_sensor(sensor1, 1);
      play_on_sensor(sensor2, 2);
      play_on_sensor(sensor3, 3);
      //TODO add other sensors here
    }
    //TODO add stuff for PLAY_OK
    else{

    }
  }
}
