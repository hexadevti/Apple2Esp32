#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/Font6x8.h>
#include "rom.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

unsigned char videoram[1048];
//VGA Device
VGA6Bit vga;


void setup()
{
  videoSetup();
	Serial.begin(115200);
	keyboard_begin();
	sei();
	Serial.print("Serial On");
  SDCardSetup();
  //xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 1);
}

// void loop2(void *pvParameters) {  // Core 0 - Blink loop
// }

void loop()
{
	run();
}
