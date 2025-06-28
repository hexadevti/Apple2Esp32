#include "config.h"

void setup() {
  
  logSetup();
  epromSetup();
  memoryAlloc();
  videoSetup();
  SDCardSetup();
  keyboardSetup();
  //sei();
  HDSetup();
  diskSetup();
  speakerSetup();
  //setCpuFrequencyMhz(240);
  //wifiSetup();
  joystickSetup(false);
  
  printLog("Ready.");
}

void loop() {
  cpuLoop();
}
