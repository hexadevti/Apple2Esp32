#include "config.h"

void setup() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  logSetup();
  epromSetup();
  memoryAlloc();
  videoSetup();
  SDCardSetup();
  keyboardSetup();
  HDSetup();
  diskSetup();
  speakerSetup();
  setCpuFrequencyMhz(240);
  // wifiSetup();
  joystickSetup();
  printLog("Ready.");
  
}



void loop() {
  cpuLoop();
}
