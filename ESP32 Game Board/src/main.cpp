#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <behaviors.h>
#include <EEPROM.h>
#include <Adafruit_PN532.h>

//Flash storage size (for Lolin D32 Pro: max. 4MB, we use 2MB -> 2000000)
#define STORAGE_SIZE 4096

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1   (4)
#define SENSOR2   (32) //Don't use Pin 0, TODO pin definition!
#define SENSOR3   (33)

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3);


//LED Pins
int LED_Pwr = 2;
int LED_Sta = 22; //TODO Pins: tested but not working: 12, 13, 14...
int LED_Com = 21;

//Storage handling
int currentByte = 0;

//Bluetooth serial object
BluetoothSerial BTSerial;

//TODO descr
Status currentStatus = READY;
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
    Serial.print("Current Status: ");
    Serial.println(currentStatus);
}

//Main function
void loop() {

    if(currentStatus != UPLOAD){
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
    switch (currentStatus)
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
            currentStatus = READY;
            break;
      }
}
