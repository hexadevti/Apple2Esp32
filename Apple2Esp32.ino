#include "config.h"

void setup() {
  #ifdef TFT
  pinMode(GREEN_LED_PIN, OUTPUT);
  #endif
  logSetup();
  epromSetup();
  memoryAlloc();
  videoSetup();
  FSSetup();
  keyboardSetup();
  HDSetup();
  diskSetup();
  speakerSetup();
  //wifiSetup();
  joystickSetup();
  printLog("Ready.");
}

void loop() {
  cpuLoop();
}
