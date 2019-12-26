#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <behaviors.h>
#include <Adafruit_PN532.h>


/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1   (4)   //Working: 4,
#define SENSOR2   (13)
//#define SENSOR3   (15)
//#define SENSOR4   (13) //TODO test
//#define SENSOR5   (25) //TODO test
//#define SENSOR6   (26) //TODO test

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2);/*
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4);
Adafruit_PN532 sensor5(SCK, MISO, MOSI, SENSOR5);
Adafruit_PN532 sensor6(SCK, MISO, MOSI, SENSOR6);*/

int sensorCount = 0;

/*
==================
LED Pin setup
=================
*/

//int LED_Pwr = 2;
//int LED_Sta = 22;
//int LED_Com = 21;
int LED_1 = 22;
int LED_2 = 21;

//Bluetooth serial object
BluetoothSerial BTSerial;

/*
==================
Mode logic setup
=================
*/
Mode currentMode = PLAYING;
String receivedData = "";

//Main setup function (runs on initialization)
void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    //pinMode(LED_Com, OUTPUT);

    //Turn on power LED
    //digitalWrite(LED_Pwr, HIGH);

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("OracleBluetooth");


    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Oracle Board...");
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
    delay(1000);
    initialize_sensor(sensor1, 1);
    initialize_sensor(sensor2, 2);
    /*initialize_sensor(sensor3, 3);
    initialize_sensor(sensor4, 4);
    initialize_sensor(sensor5, 5);
    initialize_sensor(sensor6, 6);*/

    Serial.print("Sensor search complete, found "); Serial.print(sensorCount); Serial.println(" sensors.");

    //digitalWrite(LED_Sta, HIGH);

}

//Main function
void loop() {
    //Received command analysis (USB)
    if (Serial.available() > 0) {
        receive_command(true);
    }
    //Received command analysis (Bluetooth).
    if (BTSerial.available())
     {
       receive_command(false);
     }
    //Main Behavior loop
    switch (currentMode)
      {
         case READY:
            //TODO?
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
