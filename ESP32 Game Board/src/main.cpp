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
CyMCP23016 expander_sens;
CyMCP23016 expander_led;
#define I2C_SENS 0x20 //I2C address of sensor expander (does not need to be used, as it is the default)
#define I2C_LED 0x21  //I2C address of LED expander

/*
==================
NFC Sensor setup
=================
*/

#define SCK  (18)
#define MOSI (23)
#define MISO (19)
#define SENSOR1 (MCP23016_PIN_GPIO0_0)
#define SENSOR2 (MCP23016_PIN_GPIO0_1)
#define SENSOR3 (MCP23016_PIN_GPIO0_2)
#define SENSOR4 (MCP23016_PIN_GPIO0_3)
#define SENSOR5 (MCP23016_PIN_GPIO0_4)
#define SENSOR6 (MCP23016_PIN_GPIO0_5)
#define SENSOR7 (MCP23016_PIN_GPIO0_6)
#define SENSOR8 (MCP23016_PIN_GPIO0_7)
#define SENSOR9 (MCP23016_PIN_GPIO1_0)
#define SENSOR10 (MCP23016_PIN_GPIO1_1)

Adafruit_PN532 sensor1(SCK, MISO, MOSI, SENSOR1, expander_sens);
Adafruit_PN532 sensor2(SCK, MISO, MOSI, SENSOR2, expander_sens);
Adafruit_PN532 sensor3(SCK, MISO, MOSI, SENSOR3, expander_sens);
Adafruit_PN532 sensor4(SCK, MISO, MOSI, SENSOR4, expander_sens);
Adafruit_PN532 sensor5(SCK, MISO, MOSI, SENSOR5, expander_sens);
Adafruit_PN532 sensor6(SCK, MISO, MOSI, SENSOR6, expander_sens);
Adafruit_PN532 sensor7(SCK, MISO, MOSI, SENSOR7, expander_sens);
Adafruit_PN532 sensor8(SCK, MISO, MOSI, SENSOR8, expander_sens);
Adafruit_PN532 sensor9(SCK, MISO, MOSI, SENSOR9, expander_sens);
Adafruit_PN532 sensor10(SCK, MISO, MOSI, SENSOR10, expander_sens);

int sensorCount = 0;

/*
==================
LED setup
=================
*/

int LED_Pwr = 2;
int LED_Sta = 15;
int LED_Com = 4;
int LED_1 = MCP23016_PIN_GPIO0_0;
int LED_2 = MCP23016_PIN_GPIO0_1;
int LED_3 = MCP23016_PIN_GPIO0_2;
int LED_4 = MCP23016_PIN_GPIO0_3;
int LED_5 = MCP23016_PIN_GPIO0_4;
int LED_6 = MCP23016_PIN_GPIO0_5;
int LED_7 = MCP23016_PIN_GPIO0_6;
int LED_8 = MCP23016_PIN_GPIO0_7;
int LED_9 = MCP23016_PIN_GPIO1_0;
int LED_10 = MCP23016_PIN_GPIO1_1;

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
    expander_sens.begin(SDA_Pin, SCL_Pin);
    expander_led.begin(SDA_Pin, SCL_Pin, I2C_LED);


    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_Pwr, OUTPUT);
    pinMode(LED_Sta, OUTPUT);
    pinMode(LED_Com, OUTPUT);
    expander_led.pinMode(LED_1, OUTPUT);
    expander_led.pinMode(LED_2, OUTPUT);
    expander_led.pinMode(LED_3, OUTPUT);
    expander_led.pinMode(LED_4, OUTPUT);
    expander_led.pinMode(LED_5, OUTPUT);
    expander_led.pinMode(LED_6, OUTPUT);
    expander_led.pinMode(LED_7, OUTPUT);
    expander_led.pinMode(LED_8, OUTPUT);
    expander_led.pinMode(LED_9, OUTPUT);
    expander_led.pinMode(LED_10, OUTPUT);
    expander_sens.pinMode(SENSOR1, OUTPUT);
    expander_sens.pinMode(SENSOR2, OUTPUT);
    expander_sens.pinMode(SENSOR3, OUTPUT);
    expander_sens.pinMode(SENSOR4, OUTPUT);
    expander_sens.pinMode(SENSOR5, OUTPUT);
    expander_sens.pinMode(SENSOR6, OUTPUT);
    expander_sens.pinMode(SENSOR7, OUTPUT);
    expander_sens.pinMode(SENSOR8, OUTPUT);
    expander_sens.pinMode(SENSOR9, OUTPUT);
    expander_sens.pinMode(SENSOR10, OUTPUT);

    //Turn on power LED
    digitalWrite(LED_Pwr, HIGH);

    //Turn off all other LEDs
    digitalWrite(LED_Sta, LOW);
    digitalWrite(LED_Com, LOW);
    expander_led.digitalWrite(LED_1, LOW);
    expander_led.digitalWrite(LED_2, LOW);
    expander_led.digitalWrite(LED_3, LOW);
    expander_led.digitalWrite(LED_4, LOW);
    expander_led.digitalWrite(LED_5, LOW);
    expander_led.digitalWrite(LED_6, LOW);
    expander_led.digitalWrite(LED_7, LOW);
    expander_led.digitalWrite(LED_8, LOW);
    expander_led.digitalWrite(LED_9, LOW);
    expander_led.digitalWrite(LED_10, LOW);


    //Turn off all Sensors
    expander_sens.digitalWrite(SENSOR1, HIGH);
    expander_sens.digitalWrite(SENSOR2, HIGH);
    expander_sens.digitalWrite(SENSOR3, HIGH);
    expander_sens.digitalWrite(SENSOR4, HIGH);
    expander_sens.digitalWrite(SENSOR5, HIGH);
    expander_sens.digitalWrite(SENSOR6, HIGH);
    expander_sens.digitalWrite(SENSOR7, HIGH);
    expander_sens.digitalWrite(SENSOR8, HIGH);
    expander_sens.digitalWrite(SENSOR9, HIGH);
    expander_sens.digitalWrite(SENSOR10, HIGH);

    //Initial serial communication (via USB)
    Serial.begin(57600);

    //Initial serial communication (via Bluetooth)
    BTSerial.begin("GameBoard");

    //Initialize Flash storage (using EEPROM lib)
    EEPROM.begin(STORAGE_SIZE);

    //Print initialzation data
    Serial.println();
    Serial.println("Initializing ESP32 Game Board...");
    Serial.print("Current Mode: ");
    Serial.println(currentMode);

    //Verify Expander status
    delay(200);
    Serial.print("Sensor Expander Status: ");
    Serial.println(expander_sens.detected());
    Serial.print("LED Expander Status: ");
    Serial.println(expander_led.detected());


    Serial.println("Beginning sensor search...");

    //Try initializing each sensor
    delay(1000);
    initialize_sensor(sensor1, 1, expander_sens);
    initialize_sensor(sensor2, 2, expander_sens);
    initialize_sensor(sensor3, 3, expander_sens);
    initialize_sensor(sensor4, 4, expander_sens);
    initialize_sensor(sensor5, 5, expander_sens);
    initialize_sensor(sensor6, 6, expander_sens);
    initialize_sensor(sensor7, 7, expander_sens);
    initialize_sensor(sensor8, 8, expander_sens);
    initialize_sensor(sensor9, 9, expander_sens);
    initialize_sensor(sensor10, 10, expander_sens);

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
            training_main(expander_sens, expander_led);
            break;
         case UPLOAD:
            upload_main();
            break;
         case PLAYING:
            playing_main(expander_sens, expander_led);
            break;

         default:
            //default case: Reset to ready (should never happen)
            currentMode = READY;
            break;
      }

      //Update all LEDs
      update_leds(expander_led);
}
