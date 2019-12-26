#include <Arduino.h>

#ifndef BEHAVIORS_H
#define BEHAVIORS_H

void playing_main();

//Helper commands
void receive_command(bool usb);
void flash_led(int pin);

#endif
