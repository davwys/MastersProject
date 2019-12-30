#include <Arduino.h>
#include <CyMCP23016.h>
#ifndef BEHAVIORS_H
#define BEHAVIORS_H

void playing_main(CyMCP23016 expander);

//Helper commands
void receive_command(bool usb);
void flash_led(int pin);

#endif
