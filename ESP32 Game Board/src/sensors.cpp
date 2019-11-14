#include <Arduino.h>
#include <definitions.h>
#include <Adafruit_PN532.h>

//Initializes a given NFC sensor
void initialize_sensor(Adafruit_PN532 sensor, int id){

    // TODO might be needed
    //digitalWrite(SENSOR1, HIGH);

    sensor.begin();
    uint32_t versiondata = sensor.getFirmwareVersion();
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
      sensor.SAMConfig();
    }
}
