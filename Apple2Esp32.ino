#include <ESP32Lib.h>
#include "rom.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <string>
#include <vector>
#include <EEPROM.h>


VGA6Bit vga;
char buf[0xff];
int logLineCount = 1;
bool hdAttached = true;
bool serialAttached = false;
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
