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

#define SDA_Pin 21
#define SCL_Pin 22
CyMCP23016 expander;

/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1 (MCP23016_PIN_GPIO0_4)
#define SENSOR2 (MCP23016_PIN_GPIO0_5)
#define SENSOR3 (MCP23016_PIN_GPIO0_6)
#define SENSOR4 (MCP23016_PIN_GPIO0_7)

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1, expander);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2, expander);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3, expander);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4, expander);

int sensorCount = 0;
bool activeSensors[4];
int playedCards[4];

/*
==================
LED Pin setup
=================
*/

int LED_Pwr = 2;
int LED_1 = MCP23016_PIN_GPIO0_0;
int LED_2 = MCP23016_PIN_GPIO0_1;
int LED_3 = MCP23016_PIN_GPIO0_2;
int LED_4 = MCP23016_PIN_GPIO0_3;

/*
==================
Bluetooth setup
=================
*/

BluetoothSerial BTSerial;

/*
==================
Input setup
=================
*/

String receivedData = "";

//Main setup function (runs on initialization)
void setup() {

    //Pre-fill card status arrays
    for(int i = 0; i < 4; i++){
      activeSensors[i] = false;
      playedCards[i] = NULL;
    }

    //I2C setup for GPIO expander
    expander.begin(SDA_Pin, SCL_Pin);

    //Set LED pins as output
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Pwr, OUTPUT);
    expander.pinMode(LED_1, OUTPUT);
    expander.pinMode(LED_2, OUTPUT);
    expander.pinMode(LED_3, OUTPUT);
    expander.pinMode(LED_4, OUTPUT);
    expander.pinMode(SENSOR1, OUTPUT);
    expander.pinMode(SENSOR2, OUTPUT);
    expander.pinMode(SENSOR3, OUTPUT);
    expander.pinMode(SENSOR4, OUTPUT);

    //Turn off all LEDs
    expander.digitalWrite(LED_1, LOW);
    expander.digitalWrite(LED_2, LOW);
    expander.digitalWrite(LED_3, LOW);
    expander.digitalWrite(LED_4, LOW);
    //Turn off all Sensors
    expander.digitalWrite(SENSOR1, HIGH);
    expander.digitalWrite(SENSOR2, HIGH);
    expander.digitalWrite(SENSOR3, HIGH);
    expander.digitalWrite(SENSOR4, HIGH);

    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("Oracle");

    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Oracle Board...");

    delay(200);
    Serial.print("Expander Status: ");
    Serial.println(expander.detected());

    /* TODO restart main controller if missing expander
    if(!expander.detected()){
      expander.begin(SDA_Pin, SCL_Pin);
    }
    */

    expander.digitalWrite(LED_2, HIGH);

    Serial.println("Beginning sensor search...");

    //Try initializing each sensor
    delay(1000);
    initialize_sensor(sensor1, 1, expander);
    initialize_sensor(sensor2, 2, expander);
    initialize_sensor(sensor3, 3, expander);
    initialize_sensor(sensor4, 4, expander);

    Serial.print("Sensor search complete, found "); Serial.print(sensorCount); Serial.println(" sensors.");
    //expander.digitalWrite(LED_2, LOW);
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
    //Main Behavior
    playing_main(expander);
}
