#include "config.h"

void setup() {
  logSetup();

  //pinMode(RED_LED_PIN,INPUT);
  pinMode(GREEN_LED_PIN,OUTPUT);
  //pinMode(BLUE_LED_PIN,INPUT);
  
  epromSetup();
  videoSetup();

  memoryAlloc(); 

  SDCardSetup();
  keyboard_begin();
  sei();
  HDSetup();
  diskSetup();

  speaker_begin();
  joystickSetup(true);
  printLog("Ready.");
}

void loop() {
  cpuLoop();
}
