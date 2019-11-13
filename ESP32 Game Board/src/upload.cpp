#include <behaviors.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>

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
      /*
        EEPROM.write(address, value)
        EEPROM.commit()
        EEPROM.read(address)
      */
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
