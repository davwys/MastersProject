#include <behaviors.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <EEPROM.h>


//Write a String flash memory, using a given start address
void saveString(const String input, int startAddr) {
  //Convert input string to char array
  int len = input.length();
  char toStore[len];
  strcpy(toStore, input.c_str());

  //Save char* to flash
  int i = 0;
  for (; i < strlen(toStore) + 1; i++) {
    EEPROM.write(startAddr + i, toStore[i]);
    Serial.print(toStore[i]);
  }
  EEPROM.write(startAddr + i, '\0');
  EEPROM.commit();

  //Increase current byte
  currentByte += len;
}

//Read a string from Flash memory
String readStringFromFlash(int startAddr) {
  char in[128];
  char curIn;
  int i = 0;
  curIn = EEPROM.read(startAddr);
  for (; i < 128; i++) {
    curIn = EEPROM.read(startAddr + i);
    in[i] = curIn;
  }
  return String(in);
}


//Main function for receiving upload data
void read_data(bool usb){

    String receivedData;

    //Read incoming data via USB
    if(usb)
      receivedData = Serial.readStringUntil('\n');
    else
      receivedData = BTSerial.readStringUntil('\n');

    //Validate command input
    if (receivedData.length() > 0)// && validate_command(receivedData) == true ) TODO validate data
    {

      Serial.print("Saving to Flash: ");
      Serial.println(receivedData);

      //Save to flash memory
      saveString(receivedData, currentByte);


      Serial.print("current byte is now:");
      Serial.println(currentByte);


      //TEST: Print saved data (probably as bytes lel)
      int len = receivedData.length();
      Serial.print("Loaded data: ");
      Serial.println(readStringFromFlash(currentByte - len));


    }
}

void upload_main(){
  if (Serial.available() > 0) {
      read_data(true);
  }
  //Received command analysis (Bluetooth).
  if (BTSerial.available())
   {
     read_data(false);
   }
}
