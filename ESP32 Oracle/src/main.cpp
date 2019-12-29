#include <Arduino.h>
#include <BluetoothSerial.h>
#include <definitions.h>
#include <behaviors.h>
#include <Adafruit_PN532.h>
#include <CyMCP23016.h>


/*
=======================
I2C/GPIO expander setup
=======================
*/

#define SDA (21)
#define SCL (22)
CyMCP23016 expander;

/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1 MCP23016_PIN_GPIO0_0
#define SENSOR2 MCP23016_PIN_GPIO0_1
#define SENSOR3 MCP23016_PIN_GPIO0_2
#define SENSOR4 MCP23016_PIN_GPIO0_3

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1, expander);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2, expander);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3, expander);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4, expander);

int sensorCount = 0;
bool activeSensors[maxSensors];
int playedCards[maxSensors];

/*
==================
LED Pin setup
=================
*/

int LED_Pwr = 2;
#define LED_1 MCP23016_PIN_GPIO0_4
#define LED_2 MCP23016_PIN_GPIO0_5
#define LED_3 MCP23016_PIN_GPIO0_6
#define LED_4 MCP23016_PIN_GPIO0_7

/*
==================
Bluetooth setup
=================
*/

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

    //Pre-fill card status arrays
    for(int i = 0; i < maxSensors; i++){
      activeSensors[i] = false;
      playedCards[i] = NULL;
    }

    //I2C setup for GPIO expander
    expander.begin(SDA, SCL);
    //Set LED pins as output
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Pwr, OUTPUT);
    expander.pinMode(LED_1, OUTPUT);
    expander.pinMode(LED_2, OUTPUT);
    expander.pinMode(LED_3, OUTPUT);
    expander.pinMode(LED_4, OUTPUT);

    //Turn off all LEDs, turn on power LED
    expander.digitalWrite(LED_1, LOW);
    expander.digitalWrite(LED_2, LOW);
    expander.digitalWrite(LED_3, LOW);
    expander.digitalWrite(LED_4, LOW);
    digitalWrite(LED_Pwr, HIGH);

    //Initial serial communication (via USB)
    Serial.begin(57600);
    //Initial serial communication (via Bluetooth)
    BTSerial.begin("Oracle");

    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Oracle Board...");
    Serial.print("Current Mode: ");
    Serial.println(currentMode);

    Serial.println("Beginning sensor search...");

    //Try initializing each sensor
    delay(1000);
    initialize_sensor(sensor1, 1);
    initialize_sensor(sensor2, 2);
    initialize_sensor(sensor3, 3);
    initialize_sensor(sensor4, 4);

    Serial.print("Sensor search complete, found "); Serial.print(sensorCount); Serial.println(" sensors.");

    //TODO visualize readyness state/number of found sensors?
    //Probably turn each LED on when sensor found, then turn them off here

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