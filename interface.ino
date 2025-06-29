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
  for (size_t i = 0; i < length; i++) {
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
  uint8_t startX = 0;
  uint8_t startY = 0;
  uint8_t pageSize = 6;
  uint8_t fileNameMax = 40;
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
    if (shownFile != 0xff && shownFile < firstShowFile && firstShowFile > 0)
      firstShowFile--;
  }
  int id = 0;
  if (shownFile == 0xff) {
    if ((HdDisk && selectedHdFileName == "/") || (!HdDisk && selectedDiskFileName == "/")) {
      shownFile = 0;
    } 
    else {
      for (auto &&i : files)
      {
        if (i.compare(HdDisk ? selectedHdFileName.c_str() : selectedDiskFileName.c_str()) == 0) {
          shownFile = id;
          if (shownFile > pageSize)
            firstShowFile = shownFile - pageSize;
          break;
        }
        id++;
      }
    }
  }
  id = 0;
  int shown = 0;
  sprintf(buf, "firstShowFile: %d, shownFile: %d, filename: %s", firstShowFile, shownFile, HdDisk ? selectedHdFileName.c_str() : selectedDiskFileName.c_str());
  Serial.println(buf);
  
  for (auto &&i : files)
  {
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

void printMsg(char msg[], uint16_t color)
{
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(color, TFT_BLACK);
  tft.setCursor(10, 0);
  tft.print("                                                                         ");
  tft.setCursor(10, 0);
  tft.println(msg);
}

void printStatus(char msg[], uint16_t color)
{
  // tft.setTextFont(1);
  // tft.setTextSize(1);
  // tft.setTextColor(color, TFT_BLACK);
  // tft.setCursor(5, 224);
  // tft.print("                                                                   ");
  // tft.setCursor(5, 224);
  // tft.print(msg);
}

void printOptionsBackground()
{
  setCursor(0,17);
  print("< F1 >");
  setCursor(0,18);
  print(" HD   ", HdDisk);
  setCursor(0,19);
  print(" DISK ", !HdDisk);

  setCursor(8,17);
  print("< F2 >");
  setCursor(8,18);
  print(" IIe  ", AppleIIe);
  setCursor(8,19);
  print(" II+  ", !AppleIIe);

  setCursor(16,17);
  print("< F3 >");
  setCursor(16,18);
  print(" Fast ", Fast1MhzSpeed);
  setCursor(16,19);
  print(" 1Mhz ", !Fast1MhzSpeed);

  setCursor(24,17);
  print("< F4 >");
  setCursor(24,18);
  print(" Loud ", sound);
  setCursor(24,19);
  print(" Mute ", !sound);

  setCursor(32,17);
  print("< F5 >");
  setCursor(32,18);
  print(" Joy  ", joystick);
  setCursor(32,19);
  print(" Off ", !joystick);

  setCursor(0,21);
  print("< F6 >");
  setCursor(0,22);
  print(" Col. ", videoColor);
  setCursor(0,23);
  print(" Mono ", !videoColor);

  setCursor(8,21);
  print("<ESC> Exit from menu");
  setCursor(8,22);
  print("<Enter> Select Disk");
  setCursor(8,23);
  print("<Crtl> + <Enter> Save and Reboot");

}
  