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

#define MAX_SENSORS 10

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

    //Initialize NFC/RFID sensors //TODO add more

    sensor1.begin();
    sensor2.begin();
    sensor3.begin();


    // TODO might be needed
    //digitalWrite(SENSOR1, HIGH); etc.


    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Game Board...");
    Serial.print("Current Mode: ");
    Serial.println(currentMode);
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
