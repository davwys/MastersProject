#include <behaviors.h>
#include <definitions.h>
#include <Arduino.h>

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

void playing_main(){

  //If we haven't loaded the learned mapping yet, load it from flash
  if(mapping[0] == NULL){
    read_mapping_data();
  }
  else{
    for(int i = 0; i < 10; i++){
      Serial.print(mapping[i]);
      Serial.print(", ");
    }
    Serial.println();
    Serial.println();
  }
  //TODO function
}
