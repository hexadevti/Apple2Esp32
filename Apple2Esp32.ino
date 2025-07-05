#include "config.h"

void setup() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  logSetup();
  epromSetup();
  memoryAlloc();
  videoSetup();
  FSSetup();
  keyboardSetup();
  HDSetup();
  diskSetup();
  speakerSetup();
  // wifiSetup();
  joystickSetup();
  printLog("Ready.");
}

void loop() {
  cpuLoop();
}
