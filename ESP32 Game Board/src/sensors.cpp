#include <Arduino.h>
#include <definitions.h>
#include <Adafruit_PN532.h>
#include <stdexcept>


/*
   Information about tags we use:

   TAG Type       PAGES   USER START    USER STOP
   --------       -----   ----------    ---------
   NTAG 213       45      4             39

*/



//Initializes a given NFC sensor.
//Timeout (per sensor) is set in the library's Adafruit_PN532.h file
void initialize_sensor(Adafruit_PN532 sensor, int id, CyMCP23016 expander, CyMCP23016 expander_led){

    sensor.begin(expander);

    uint32_t versiondata = sensor.getFirmwareVersion(expander);
    //If no sensor found, return
    if (!versiondata) {
      Serial.print("Didn't find Sensor #"); Serial.println(id);
    }
    else{
      //Sensor Found
      sensorCount++;
      Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
      Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
      Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
      //Configure sensor to read RFID tags
      sensor.SAMConfig(expander);

      //Turn on corresponding sensor LED
      switch(id){
        case 1:
          expander_led.digitalWrite(LED_1, HIGH);
          break;
        case 2:
          expander_led.digitalWrite(LED_2, HIGH);
          break;
        case 3:
          expander_led.digitalWrite(LED_3, HIGH);
          break;
        case 4:
          expander_led.digitalWrite(LED_4, HIGH);
          break;
        case 5:
          expander_led.digitalWrite(LED_5, HIGH);
          break;
        case 6:
          expander_led.digitalWrite(LED_6, HIGH);
          break;
        case 7:
          expander_led.digitalWrite(LED_7, HIGH);
          break;
        case 8:
          expander_led.digitalWrite(LED_8, HIGH);
          break;
        case 9:
          expander_led.digitalWrite(LED_9, HIGH);
          break;
        case 10:
          expander_led.digitalWrite(LED_10, HIGH);
          break;
      }
    }
}


//Reads an NFC tag and returns data contained within as a String
//Params: Sensor, sensor ID (number), whether to be print additional info or not)
String readTag(Adafruit_PN532 sensor, int id, bool verbose, CyMCP23016 expander){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  //Buffer to store the returned UID
  uint8_t uidLength;                        //Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t pageNumber = 9; //Number of pages to read. Max is 45, we only use the first 9
  success = sensor.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, expander);
  String text;        //Final read text string
  bool valid = true;  //Whether we got all needed data
  if (success) {
    //Print card UID if any is found
    if(verbose){
        Serial.print("Found card with UID "); sensor.PrintHex(uid, uidLength);
    }
    uint8_t data[32];

    for (uint8_t i = 7; i <= pageNumber; i++) //Start at 7, because we don't need non-user data / format encoding stuff
    {
      success = sensor.ntag2xx_ReadPage(i, data, expander);

      if(success){
        //Verbose logging of card data
        if(verbose){
          // Display the current page number
          Serial.print("PAGE ");
          if (i < 10)
          {
            Serial.print("0");
            Serial.print(i);
          }
          else
          {
            Serial.print(i);
          }
          Serial.print(": ");

          // Display the results, depending on 'success'
          if (success)
          {
            // Dump the page data
            sensor.PrintHexChar(data, 4);
          }
          else
          {
            Serial.println("Unable to read the requested page!");
          }
        }
        //Read 4 bytes from the data we just read
        char temp[4];
        for(int i = 0; i < 4; i++){
          temp[i] = data[i];
        }

        //Convert to string & append to our return string
        String txt_temp((char*) temp);
        String txt = txt_temp.substring(0,4);
        //This is the final string
        text += txt;
      }
      else{
        valid = false;
      }
    }

    //If we got the complete data block
    if(valid && text.length() >= 8){
      //Subtract encoding ("en" for plain text)
      text = text.substring(2);

      //Add sensor ID
      text = "SensorID=" + String(id) + "_" + text;
      //return final read text
      return text;
    }
    else{
      //Serial.print("Error: Couldn't finish reading data");
      throw std::runtime_error("Error: Couldn't finish reading data");
    }
  }
  else{
    return "NONE";
  }

}
