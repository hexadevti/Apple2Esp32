#include "config.h"

void setup() {
  logSetup();
  memoryAlloc();
  videoSetup();
  epromSetup();
  SDCardSetup();
  keyboardSetup();
  //sei();
  HDSetup();
  diskSetup();
  speakerSetup();
  //setCpuFrequencyMhz(240);
  wifiSetup();
  analogJoystickSetup();
  printLog("Ready.");
}

void loop() {
  cpuLoop();
}
