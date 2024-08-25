#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/Font6x8.h>
#include "rom.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <string>
#include <vector>
#include <EEPROM.h>
#include <bitset>
#include <algorithm>
#include <array>

// VGA Pins
const int hsyncPin = 32;
const int vsyncPin = 33;
const int red0pin = 12;
const int red1pin = 13;
const int green0pin = 27;
const int green1pin = 14;
const int blue0pin = 25;
const int blue1pin = 26;
// Keyboard Pins
const int DataPin = 35;
const int IRQpin = 34;
// SD Pins
#define REASSIGN_PINS
int sck = 18;
int miso = 19;
int mosi = 23;
int cs = 5;



VGA6Bit vga;
char buf[0xff];
int logLineCount = 1;
bool hdAttached = true;
bool serialAttached = false;
bool diskAttached = true;
#define EEPROM_SIZE 12
int selectedFileEEPROMaddress = 0;
int selectedFile;



void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  //EEPROM.write(selectedFileEEPROMaddress, 6);
  //EEPROM.commit();
  selectedFile = EEPROM.read(selectedFileEEPROMaddress); 
  sprintf(buf, "EEPROM value: %x", selectedFile);
  printlog(buf);
	Serial.begin(500000);
  SDCardSetup();
  videoSetup();
  serialVideoSetup();
	keyboard_begin();
	sei();
  HDSetup();
  printlog("Ready.");
}

void printlog(String txt)
{
  if (serialAttached)
  {
    if (logLineCount > 24)
      logLineCount = 1;
      Serial.print("\e[");
      Serial.print(logLineCount);
      Serial.print(";");
      Serial.print(41);
      Serial.print("H");
      Serial.print(txt.c_str());
      logLineCount++;
  }
  else
    Serial.println(txt.c_str());
}

void loop()
{
	run();
}
