#include "config.h"

void setup() {
  logSetup();
  pinMode(GREEN_LED_PIN,OUTPUT);
  epromSetup();
  videoSetup();
  memoryAlloc(); 
  SDCardSetup();
  keyboardBegin();
  
  HDSetup();
  diskSetup();
  speakerBegin();
  joystickSetup(true);
  printLog("Ready.");
  
}

void loop() {
  cpuLoop();
}
