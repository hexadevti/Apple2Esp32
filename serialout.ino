void serialwrite(short address, unsigned char value)
{
  	unsigned char value2 = value & 0x7f;
    short addr = convertVideo[address-0x400];
    videoram[addr] = value2;
    int y = floor(addr / 0x28);
    int x = addr - y * 0x28;
    vga.setCursor(x * 8, (y+3) * 8);
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

unsigned char serialread(short address)
{
    return videoram[convertVideo[address-0x400]];
}

