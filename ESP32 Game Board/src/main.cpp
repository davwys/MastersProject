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
#define SENSOR1   (33)
#define SENSOR2   (32)
#define SENSOR3   (4)
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

String mapping[10];

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

    digitalWrite(LED_Sta, LOW);
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

/*
    // TODO might be needed
    digitalWrite(SENSOR1, HIGH);
    digitalWrite(SENSOR2, HIGH);
    digitalWrite(SENSOR3, HIGH);
*/

    //Try initializing each sensor
    initialize_sensor(sensor1, 1);
    initialize_sensor(sensor2, 2);
    initialize_sensor(sensor3, 3);
    initialize_sensor(sensor4, 4);
    initialize_sensor(sensor5, 5);
    initialize_sensor(sensor6, 6);
    initialize_sensor(sensor7, 7);
    initialize_sensor(sensor8, 8);

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
