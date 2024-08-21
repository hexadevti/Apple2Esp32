//This example shows a simple "Hello world!" on a VGA screen.
//You need to connect a VGA screen cable to the pins specified below.
//cc by-sa 4.0 license
//bitluni

#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>

#include "rom.h"

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device
VGA3Bit vga;

unsigned char videoram[1048];

void setup()
{
  vga.setFrameBufferCount(2);
  Mode myMode = vga.MODE320x240;
	//initializing vga at the specified pins
	vga.init(myMode, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//selecting the font
	vga.setFont(CodePage437_8x8);
	//displaying the text
  vga.println();
	vga.println("Apple2Esp32");
	Serial.begin(115200);
	keyboard_begin();
	sei();
	Serial.print("Serial On");
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  //xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 1);
  
}


void loop2(void *pvParameters) {  // Core 0 - Blink loop
  while (1) {
    for (int i = 0; i < 25; i++) {
      for (int j = 0; j < 0x28; j++) {
        vga.setCursor(j * 8, (i + 3) * 8);
        vga.print((char)videoram[i * 0x28 + j]);
      }
    }
    delay(100);
  }
}

void loop()
{
	run();
}
