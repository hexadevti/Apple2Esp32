void serialVideoSetup() {
  //xTaskCreatePinnedToCore(flashSerialCharacters, "flashSerialCharacters", 4096, NULL, 1, NULL, 1);
  if (serialVideoAttached)
  {
    printlog("Serial Video Setup...OK");
  }
  else
  {
    printlog("Serial Video Disabled...");
  }
  if (serialKeyboardAttached)
  {
    printlog("Serial Keyboard Setup...OK");
    xTaskCreatePinnedToCore(serialKeyboard, "serialKeyboard", 4096, NULL, 1, NULL, 1);
  } 
  else
    printlog("Serial Keyboard Disabled...");

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
  int prev_char = 0;
  while (true) {
      
    if (Serial.available() > 0) {
      int charRead = Serial.read(); 
      if (charRead == 0x44)
        keymem = 0x88;
      else if (prev_char == 0x5b && charRead == 0x43)
        keymem = 0x95;
      else if (prev_char == 0x5b && charRead == 0x7f)
        keymem = 0x88;
      else if (charRead != 0x1b && charRead != 0x5b)
        keymem = charRead + 0X80;

      Serial.print((char)charRead);
      // sprintf(buf, "Ser: %02x, %02x", prev_char, charRead);
      // printlog(buf);

      prev_char = charRead;
    }
    
    delay(100);
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
