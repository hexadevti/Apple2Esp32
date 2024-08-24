#include <Ressources/CodePage437_8x8.h>
#include <Ressources/Font6x8.h>

//pin configuration
const int hsyncPin = 32;
const int vsyncPin = 33;
const int red0pin = 12;
const int red1pin = 13;
const int green0pin = 27;
const int green1pin = 14;
const int blue0pin = 25;
const int blue1pin = 26;


void videoSetup()
{
  printlog("Video Setup...");
  Mode myMode = vga.MODE320x240;
  vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
	vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  printMsg("APPLE2ESP32");
  textLoResRender(false);
  xTaskCreatePinnedToCore(flashCharacters, "flashCharacters", 4096, NULL, 1, NULL, 1);
}

void printMsg(char msg[]) {
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(0xff0000), vga.RGB(0));
  vga.setCursor(5, 10);
  vga.print("                                                  ");
  vga.setCursor(5, 10);
  vga.print(msg);
  vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void flashCharacters(void *pvParameters) {
  bool inversed = false;
  while (true)
  {
    textLoResRender(inversed);
    delay(300);
    inversed = !inversed;
  }
}

unsigned char textLoResRead(short address)
{
    return ram[0x400 + convertVideo[address-0x400]];
}

void textLoResWrite(short address, unsigned char value)
{
    short addr = convertVideo[address-0x400];
    ram[0x400 + addr] = value;
    int y = floor(addr / 0x28);
    int x = addr - y * 0x28;
    vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
    vga.setCursor((x + 2) * 7, (y+3) * 8);
    vga.print((char)value);
    if (serialAttached)
      textLoResSerialWrite(address, value);
      
    // ram[0x400 + convertVideo[address-0x400]] = value;
    // sprintf(buf, "%03x: %02x, ", convertVideo[address-0x400], value);
    // log("x=");
    // log(x);
    // log(", y=");
    // log(y);
    // log(" > ");
    // log(value);
    // log("'");
    // log((char)value);
    // log("'");
    // log(",");
    // log(value2);
    // log("'");
    // log((char)value2);
    // log("'");
}

void textLoResRender(bool inversed)
{
  
  for (int y = 0; y < 24; y++)
  {
    for (int x = 0; x < 40;x++)
    {
      char value = ram[0x400 + y * 40 + x];
      if (value >= 0x40 && value < 0x80)
      {
        if (inversed)
          vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
        else
          vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
        vga.setCursor((x + 2) * 7, (y+3) * 8);
        vga.print((char)value);
      }
    }
  } 
}