#include <Arduino.h>
#include <CyMCP23016.h>

#ifndef BEHAVIORS_H
#define BEHAVIORS_H

void training_main(CyMCP23016 expander_1, CyMCP23016 expander_2);
void playing_main(CyMCP23016 expander_1, CyMCP23016 expander_2);
void upload_main();

//Helper commands
void receive_command(bool usb);
void flash_led(int pin);
void flash_led(int pin, CyMCP23016 expander);
void update_leds(CyMCP23016 expander);


//Read a string from Flash memory
String read_string_from_flash(int startAddr);

void read_mapping_data();

#endif
