#include <cstring>
#include <iostream>


uint8_t cursorX = 0;
uint8_t cursorY = 0;

void setCursor(uint8_t x, uint8_t y) {
  cursorX = x;
  cursorY = y;
}

void print(const char * txt, bool inverted = false) {
  size_t length = std::strlen(txt);

  uint16_t addr = cursorY * 40 + cursorX;
  //Serial.printf("txt size = %d", length);
  for (size_t i = 0; i < length; i++) {
    //Serial.printf("char %d = %02x\n", i, txt[i]);
    char currChar = txt[i];
    if (inverted && currChar < 0x60 && currChar >= 0x40) currChar-=0x40;
    menuScreen[addr+i] = currChar+(inverted ? 0 : 0x80);
    cursorX++;
    if (cursorX > 39) {
      cursorY++;
      cursorX = 0;
    }
  }
}

void listFiles(bool downDirection)
{
  uint8_t sel = 0;
  uint8_t skip = 0;
  uint8_t startX = 0;
  uint8_t startY = 0;
  uint8_t pageSize = 10;
  uint8_t fileNameMax = 38;
  setCursor(startX, startY);
  print("Available files:");
  std::vector<std::string> files;
  if (!HdDisk)
  {
    files = diskFiles;
  }
  else
  {
    files = hdFiles;
  }
  if (shownFile > files.size())
    shownFile = 0xff;
  if (downDirection)
  {
    if (shownFile != 0xff && shownFile >= firstShowFile + pageSize)
    {
      firstShowFile = shownFile - pageSize;
    }
  }
  else
  {
    if (shownFile < firstShowFile && firstShowFile > 0)
      firstShowFile--;
  }
  int shown = 0;
  // sprintf(buf, "sel: %d, firstShowFile: %d, shownFile: %d", sel, firstShowFile, shownFile);
  // Serial.println(buf);
  
  int id = 0;
  for (auto &&i : files)
  {
    if (shownFile == 0xff && i.compare(selectedDiskFileName.c_str()) == 0) {
      shownFile = id;
      // sprintf(buf, "i: %s, selectedDiskFileName: %s", i.c_str(), selectedDiskFileName.c_str());
      // Serial.println(buf);
    }
    if (id < firstShowFile)
    {
      id++;
      continue;
    }
    if (shown > pageSize)
    break;
    setCursor(startX, startY + 2 + id - firstShowFile);
    // sprintf(buf, "i: %s, selectedDiskFileName: %s", i.c_str(), selectedDiskFileName.c_str());
    // Serial.println(buf);
    if (id == shownFile)
      print("> ");
    else
      print("  ");

    if (i.size() > fileNameMax)
      i = i.substr(0, fileNameMax-6) + "..." + i.substr(i.size() - 3, 3);
    print(i.c_str(), id == shownFile);
    for (size_t b = 0; b<fileNameMax-i.size();b++)
      print(" ", id == shownFile);
    shown++;
    id++;
  }
}

void showHideOptionsWindow() {
  OptionsWindow = !OptionsWindow;
  paused = OptionsWindow;
  listFiles(true);
  printOptionsBackground();
}


void printOptionsBackground()
{
  setCursor(0,17);
  print("< F1 >");
  setCursor(0,18);
  print(" HD   ", HdDisk);
  setCursor(0,19);
  print(" DISK ", !HdDisk);

  setCursor(7,17);
  print("< F2 >");
  setCursor(7,18);
  print(" IIe  ", AppleIIe);
  setCursor(7,19);
  print(" II+  ", !AppleIIe);

  setCursor(14,17);
  print("< F3 >");
  setCursor(14,18);
  print(" Fast ", Fast1MhzSpeed);
  setCursor(14,19);
  print(" 1Mhz ", !Fast1MhzSpeed);

  setCursor(21,17);
  print("< F4 >");
  setCursor(21,18);
  print(" Stop ", paused);
  setCursor(21,19);
  print(" Run  ", !paused);

  setCursor(28,17);
  print("< F5 >");
  setCursor(28,18);
  print(" Joy  ", joystick);
  setCursor(28,19);
  print(" Off ", !joystick);

  setCursor(0,21);
  print("< F6 >");
  setCursor(0,22);
  print(" Col. ", videoColor);
  setCursor(0,23);
  print(" Mono ", !videoColor);

  setCursor(7,21);
  print("<ESC> Exit from menu");
  setCursor(7,22);
  print("<Return> Select Disk");
  setCursor(7,23);
  print("<Crtl> + <Return> Save and Reboot");

}
  