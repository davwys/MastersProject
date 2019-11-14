#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <behaviors.h>
#include <EEPROM.h>
#include <Adafruit_PN532.h>


/*
===================
Flash storage setup
===================
*/

//Flash storage size (for Lolin D32 Pro: max. 4KB with default partition size)
#define STORAGE_SIZE 4096
int currentByte = 0;


/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1   (4)
#define SENSOR2   (32)
#define SENSOR3   (33)
#define SENSOR4   (13) //TODO test
#define SENSOR5   (25) //TODO test
#define SENSOR6   (26) //TODO test
#define SENSOR7   (27) //TODO test
#define SENSOR8   (34) //TODO test
//#define SENSOR9   (33) //TODO find pins! --> test 14 & 15
//#define SENSOR10  (33) //TODO find pins!

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4);
Adafruit_PN532 sensor5(SCK, MISO, MOSI, SENSOR5);
Adafruit_PN532 sensor6(SCK, MISO, MOSI, SENSOR6);
Adafruit_PN532 sensor7(SCK, MISO, MOSI, SENSOR7);
Adafruit_PN532 sensor8(SCK, MISO, MOSI, SENSOR8);
//Adafruit_PN532 sensor9(SCK, MISO, MOSI, SENSOR9);
//Adafruit_PN532 sensor10(SCK, MISO, MOSI, SENSOR10);

int sensorCount = 0;

/*
==================
LED Pin setup
=================
*/

int LED_Pwr = 2;
int LED_Sta = 22; //TODO Pins: tested but not working: 12, 13, 14...
int LED_Com = 21;


//Bluetooth serial object
BluetoothSerial BTSerial;

/*
==================
Mode logic setup
=================
*/
Mode currentMode = READY;
String receivedData = "";

//Initializes a given NFC sensor
void initializeSensor(Adafruit_PN532 sensor, int id){

    // TODO might be needed
    //digitalWrite(SENSOR1, HIGH);

    sensor.begin();
    uint32_t versiondata = sensor.getFirmwareVersion();
    //If no sensor found, return
    if (!versiondata) {
      Serial.print("Didn't find Sensor #"); Serial.println(id);
      return;
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

//Main setup function (runs on initialization)
void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Pwr, OUTPUT);
    pinMode(LED_Sta, OUTPUT);
    pinMode(LED_Com, OUTPUT);

    //Turn on power LED
    digitalWrite(LED_Pwr, HIGH);
    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("GameBoardBluetooth");

    //Initialize Flash storage (using EEPROM lib)
    EEPROM.begin(STORAGE_SIZE);

    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Game Board...");
    Serial.print("Current Mode: ");
    Serial.println(currentMode);

    Serial.println("Beginning sensor search...");

    initializeSensor(sensor1, 1);
    initializeSensor(sensor2, 2);
    initializeSensor(sensor3, 3);
    initializeSensor(sensor4, 4);
    initializeSensor(sensor5, 5);
    initializeSensor(sensor6, 6);
    initializeSensor(sensor7, 7);
    initializeSensor(sensor8, 8);

    Serial.print("Sensor search complete, found "); Serial.print(sensorCount); Serial.println(" sensors.");

    digitalWrite(LED_Sta, HIGH);

}

//Main function
void loop() {

    if(currentMode != UPLOAD){
      //Received command analysis (USB)
      if (Serial.available() > 0) {
          receive_command(true);
      }
      //Received command analysis (Bluetooth).
      if (BTSerial.available())
       {
         receive_command(false);
       }
     }
    //Main Behavior loop
    switch (currentMode)
      {
         case READY:
            //TODO?
            break;
         case PRETRAINING:
            //TODO
            break;
         case PREPLAYING:
            //TODO
            break;
         case TRAINING:
            training_main();
            break;
         case UPLOAD:
            upload_main();
            break;
         case PLAYING:
            playing_main();
            break;

         default:
            //default case: Reset to ready (should never happen)
            currentMode = READY;
            break;
      }
}
