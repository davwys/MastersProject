#include <Arduino.h>
#include <CyMCP23016.h>

#ifndef BEHAVIORS_H
#define BEHAVIORS_H

void training_main();
void playing_main(CyMCP23016 expander_sens, CyMCP23016 expander_led);
void upload_main();

//Helper commands
void receive_command(bool usb);
void flash_led(int pin);
void flash_led(int pin, CyMCP23016 expander);


//Read a string from Flash memory
String read_string_from_flash(int startAddr);

void read_mapping_data();

#endif
