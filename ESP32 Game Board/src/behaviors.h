#include <Arduino.h>

#ifndef BEHAVIORS_H
#define BEHAVIORS_H

void training_main();
void playing_main();
void upload_main();

//Helper commands
void receive_command(bool usb);
void flash_led(int pin);


//Read a string from Flash memory
String readStringFromFlash(int startAddr);

#endif
