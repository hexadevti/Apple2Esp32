void serialVideoSetup() {
  //xTaskCreatePinnedToCore(flashSerialCharacters, "flashSerialCharacters", 4096, NULL, 1, NULL, 1);
  if (serialAttached)
  {
    printlog("Serial Video Setup...");
    xTaskCreatePinnedToCore(serialKeyboard, "serialKeyboard", 4096, NULL, 1, NULL, 1);
  }
  else
  {
    printlog("Serial Video Disabled...");
  }
}

void flashSerialCharacters(void *pvParameters) {
  bool inversed = false;
  while (true) {
    textLoResSerialRender(inversed);
    delay(300);
    inversed = !inversed;
  }
}

void serialKeyboard(void *pvParameters) {
  bool inversed = false;
  while (true) {
    if (Serial.available() > 0) {

      keymem = Serial.read() + 0X80;
      sprintf(buf, "Ser: %02x", keymem);
      printlog(buf);
    }
    delay(10);
  }
}

void textLoResSerialWrite(short address, unsigned char value) {
  short addr = convertVideo[address - 0x400];
  ram[0x400 + addr] = value;
  int y = floor(addr / 0x28);
  int x = addr - y * 0x28;
  if (value != 0x60) {
    // Serial.printf("\e[%s;%sH%s", y+1, x+1, (char)(value & 0x7f));
    Serial.print("\e[");
    Serial.print(y + 1);
    Serial.print(";");
    Serial.print(x + 1);
    Serial.print("H");
    Serial.print((char)(value & 0x7f));
  }
}

void textLoResSerialRender(bool inversed) {
  for (int y = 0; y < 24; y++) {
    for (int x = 0; x < 40; x++) {
      char value = ram[0x400 + y * 40 + x];
      if (value >= 0x40 && value < 0x80) {
        if (inversed)
          vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
        else
          vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
        if (value != 0x60) {
          Serial.print("\e[");
          Serial.print(y);
          Serial.print(";");
          Serial.print(x);
          Serial.print("H");
          Serial.print((char)value);
        }
      }
    }
  }
}
