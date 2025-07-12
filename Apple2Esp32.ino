#include "config.h"

void setup() {
  #ifdef TFT
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Turn off green LED
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
