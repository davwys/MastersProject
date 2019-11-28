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
    //Tmp looks like: SensorID=1_CardID=2
    if(tmp.length() > 4){

      //Extract sensor ID string & card ID string
      String sid_str = split(tmp, '_', 0);
      String cid_str = tmp.substring(tmp.indexOf("CardID="));


      //Extract sensor ID to get name afterwards
      sid_str.replace("SensorID=", "");
      char sid_char[64];
      sid_str.toCharArray(sid_char, 64);
      int sid = atoi(sid_char);

      //Get area name from mapping
      String areaName = mapping[sid-1];

      //Generate output: "Area='something'_CardID=123"
      String output = "Area='" + areaName + "'_"+ cid_str;

      //Send on both bluetooth and USB
      Serial.println("PLAY={" + output + "}");
      BTSerial.println("PLAY={" + output + "}");

      //playing_ready = false; TODO add
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

          //Save values to mapping array
          mapping[sensorId-1] = areaName; //Sensor 1 gets saved to position 0
      }
      // Find the next command in input string
      command = strtok(0, "}{");
  }
}


//Main function when in PLAYING mode
void playing_main(){
  //If we haven't loaded the learned mapping yet, load it from flash
  if(mapping[0] == NULL){
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
