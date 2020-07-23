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
=======================
I2C/GPIO expander setup
=======================
*/

#define SDA_Pin 21
#define SCL_Pin 22
CyMCP23016 expander_1;
CyMCP23016 expander_2;
#define I2C_1 0x21 //I2C address of first expander (does not need to be used, as it is the default)
#define I2C_2 0x20  //I2C address of second expander

/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1 (MCP23016_PIN_GPIO1_1)
#define SENSOR2 (MCP23016_PIN_GPIO0_7)
#define SENSOR3 (MCP23016_PIN_GPIO0_5)
#define SENSOR4 (MCP23016_PIN_GPIO0_3)
#define SENSOR5 (MCP23016_PIN_GPIO0_1)
#define SENSOR6 (MCP23016_PIN_GPIO1_1)
#define SENSOR7 (MCP23016_PIN_GPIO0_7)
#define SENSOR8 (MCP23016_PIN_GPIO0_5)
#define SENSOR9 (MCP23016_PIN_GPIO0_3)
#define SENSOR10 (MCP23016_PIN_GPIO0_1)

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1, expander_1);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2, expander_1);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3, expander_1);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4, expander_1);
Adafruit_PN532 sensor5(SCK, MISO, MOSI, SENSOR5, expander_1);
Adafruit_PN532 sensor6(SCK, MISO, MOSI, SENSOR6, expander_2);
Adafruit_PN532 sensor7(SCK, MISO, MOSI, SENSOR7, expander_2);
Adafruit_PN532 sensor8(SCK, MISO, MOSI, SENSOR8, expander_2);
Adafruit_PN532 sensor9(SCK, MISO, MOSI, SENSOR9, expander_2);
Adafruit_PN532 sensor10(SCK, MISO, MOSI, SENSOR10, expander_2);

int sensorCount = 0;

/*
==================
LED setup
=================
*/

int LED_Pwr = 2;
int LED_Sta = 15;
int LED_Com = 4;
int LED_1 = MCP23016_PIN_GPIO1_0;
int LED_2 = MCP23016_PIN_GPIO0_6;
int LED_3 = MCP23016_PIN_GPIO0_4;
int LED_4 = MCP23016_PIN_GPIO0_2;
int LED_5 = MCP23016_PIN_GPIO0_0;
int LED_6 = MCP23016_PIN_GPIO1_0;
int LED_7 = MCP23016_PIN_GPIO0_6;
int LED_8 = MCP23016_PIN_GPIO0_4;
int LED_9 = MCP23016_PIN_GPIO0_2;
int LED_10 = MCP23016_PIN_GPIO0_0;

//Array for storing statuses to turn off LEDs

//System LEDs
int led_array[3][2]={
  {0,-1},
  {0,-1},
  {0,-1},
};

//Expander LEDs
int led_array_expander[10][2]={
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
  {0,-1},
};

/*
==================
Temp storage setup
=================
*/

String mapping[10];
bool sensorTypes[10]; //Saves sensors' types (true = regular, false = combinatorial)
int playedCards[10];

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
Mode currentMode = READY;
String receivedData = "";



//Main setup function (runs on initialization)
void setup() {

    //I2C setup for GPIO expanders
    expander_1.begin(SDA_Pin, SCL_Pin, I2C_1);
    expander_2.begin(SDA_Pin, SCL_Pin);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Pwr, OUTPUT);
    pinMode(LED_Sta, OUTPUT);
    pinMode(LED_Com, OUTPUT);

    expander_1.pinMode(LED_1, OUTPUT);
    expander_1.pinMode(LED_2, OUTPUT);
    expander_1.pinMode(LED_3, OUTPUT);
    expander_1.pinMode(LED_4, OUTPUT);
    expander_1.pinMode(LED_5, OUTPUT);

    expander_1.pinMode(SENSOR1, OUTPUT);
    expander_1.pinMode(SENSOR2, OUTPUT);
    expander_1.pinMode(SENSOR3, OUTPUT);
    expander_1.pinMode(SENSOR4, OUTPUT);
    expander_1.pinMode(SENSOR5, OUTPUT);

    expander_2.pinMode(LED_6, OUTPUT);
    expander_2.pinMode(LED_7, OUTPUT);
    expander_2.pinMode(LED_8, OUTPUT);
    expander_2.pinMode(LED_9, OUTPUT);
    expander_2.pinMode(LED_10, OUTPUT);

    expander_2.pinMode(SENSOR6, OUTPUT);
    expander_2.pinMode(SENSOR7, OUTPUT);
    expander_2.pinMode(SENSOR8, OUTPUT);
    expander_2.pinMode(SENSOR9, OUTPUT);
    expander_2.pinMode(SENSOR10, OUTPUT);

    //Turn on power LED
    digitalWrite(LED_Pwr, HIGH);

    //Turn off all other LEDs
    digitalWrite(LED_Sta, LOW);
    digitalWrite(LED_Com, LOW);
    expander_1.digitalWrite(LED_1, LOW);
    expander_1.digitalWrite(LED_2, LOW);
    expander_1.digitalWrite(LED_3, LOW);
    expander_1.digitalWrite(LED_4, LOW);
    expander_1.digitalWrite(LED_5, LOW);
    expander_2.digitalWrite(LED_6, LOW);
    expander_2.digitalWrite(LED_7, LOW);
    expander_2.digitalWrite(LED_8, LOW);
    expander_2.digitalWrite(LED_9, LOW);
    expander_2.digitalWrite(LED_10, LOW);


    //Turn off all Sensors
    expander_1.digitalWrite(SENSOR1, HIGH);
    expander_1.digitalWrite(SENSOR2, HIGH);
    expander_1.digitalWrite(SENSOR3, HIGH);
    expander_1.digitalWrite(SENSOR4, HIGH);
    expander_1.digitalWrite(SENSOR5, HIGH);
    expander_2.digitalWrite(SENSOR6, HIGH);
    expander_2.digitalWrite(SENSOR7, HIGH);
    expander_2.digitalWrite(SENSOR8, HIGH);
    expander_2.digitalWrite(SENSOR9, HIGH);
    expander_2.digitalWrite(SENSOR10, HIGH);

    //Initialize Flash storage (using EEPROM lib)
    EEPROM.begin(STORAGE_SIZE);

    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("GameBoard");

    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Game Board...");
    Serial.print("Current Mode: ");
    Serial.println(currentMode);

    //Verify Expander status
    delay(200);
    Serial.print("Expander 1 Status: ");
    Serial.println(expander_1.detected());
    Serial.print("Expander 2 Status: ");
    Serial.println(expander_2.detected());

    Serial.println("Beginning sensor search...");

    initialize_sensor(sensor1, 1, expander_1);
    initialize_sensor(sensor2, 2, expander_1);
    initialize_sensor(sensor3, 3, expander_1);
    initialize_sensor(sensor4, 4, expander_1);
    initialize_sensor(sensor5, 5, expander_1);
    initialize_sensor(sensor6, 6, expander_2);
    initialize_sensor(sensor7, 7, expander_2);
    initialize_sensor(sensor8, 8, expander_2);
    initialize_sensor(sensor9, 9, expander_2);
    initialize_sensor(sensor10, 10, expander_2);

    Serial.print("Sensor search complete, found "); Serial.print(sensorCount); Serial.println(" sensors.");

    //Flash all sensor LEDs
    flash_all_leds(3);

    //Turn on status LED
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
            training_main(expander_1, expander_2);
            break;
         case UPLOAD:
            upload_main();
            break;
         case PLAYING:
            playing_main(expander_1, expander_2);
            break;

         default:
            //default case: Reset to ready (should never happen)
            currentMode = READY;
            break;
      }

      //Update all LEDs
      update_leds(expander_1);
      update_leds(expander_2);
}
