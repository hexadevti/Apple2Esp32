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
  Mode myMode = vga.MODE320x240;
  vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
  vga.setFont(Font6x8);
  vga.println();
  vga.setTextColor(vga.RGB(0xff0000), vga.RGB(0));
	vga.println("Apple2Esp32");
	vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void videowrite(short address, unsigned char value)
{
  	unsigned char value2 = value & 0x7f;
    short addr = convertVideo[address-0x400];
    videoram[addr] = value2;
    int y = floor(addr / 0x28);
    int x = addr - y * 0x28;
    vga.setCursor((x + 2) * 7, (y+3) * 8);
    vga.print((char)value2);
    //videoram[convertVideo[address-0x400]] = value;
    // sprintf(buf, "%03x: %02x, ", convertVideo[address-0x400], value);
    // Serial.print("x=");
    // Serial.print(x);
    // Serial.print(", y=");
    // Serial.print(y);
    // Serial.print(" > ");
    // Serial.print(value);
    // Serial.print("'");
    // Serial.print((char)value);
    // Serial.print("'");
    // Serial.print(",");
    // Serial.print(value2);
    // Serial.print("'");
    // Serial.print((char)value2);
    // Serial.println("'");
}

unsigned char videoread(short address)
{
    return videoram[convertVideo[address-0x400]];
}