#include <cstring>
#include <iostream>


uint8_t cursorX = 0;
uint8_t cursorY = 0;

void setCursor(uint8_t x, uint8_t y) {
  cursorX = x;
  cursorY = y;
}

void print(const char * txt, bool inverted = false, uint8_t color = 0xf0) {
  size_t length = std::strlen(txt);

  uint16_t addr = cursorY * 45 + cursorX;
  for (size_t i = 0; i < length; i++) {
    char currChar = txt[i];
    if (inverted && currChar < 0x60 && currChar >= 0x40) currChar-=0x40;
    menuScreen[addr+i] = currChar+(inverted ? 0 : 0x80);
    menuColor[addr+i] = color;
    cursorX++;
    if (cursorX > 44) {
      cursorY++;
      cursorX = 0;
    }
  }
}

void clearScreen() {
  memset(menuScreen, 0xa0, 0x546 * sizeof(unsigned char));
}

void listFiles(bool downDirection)
{
  uint8_t startX = 0;
  uint8_t startY = 2;
  uint8_t pageSize = 9;
  uint8_t fileNameMax = 45;

  for (int y = startY; y < startY + pageSize; y++)
  {
    for (int x = startX; x < fileNameMax; x++)
    {
      setCursor(x,y);
      print(" ");
    }
  }
  
  pageSize--; // transform to index
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
  // sprintf(buf, "firstShowFile: %d, shownFile: %d, filename: %s", firstShowFile, shownFile, HdDisk ? selectedHdFileName.c_str() : selectedDiskFileName.c_str());
  // Serial.println(buf);
  
  for (auto &&i : files)
  {
    if (id < firstShowFile)
    {
      id++;
      continue;
    }
    if (shown > pageSize)
      break;
    setCursor(startX, startY + id - firstShowFile);
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
  if (OptionsWindow) {
    clearScreen();
    delay(100);
  }
  OptionsWindow = !OptionsWindow;

  if (OptionsWindow) {
    listFiles(true);
    optionsScreenRender();
  }

  paused = OptionsWindow;
}

void showHideDebugWindow() {
  if (DebugWindow) {
    clearScreen();
    delay(100);
  }
  DebugWindow = !DebugWindow;

  if (DebugWindow) {
    debugScreenRender();
  }
  paused = DebugWindow;
}

void debugScreenRender()
{
  setCursor(0, 0);
  print("debug:");

  setCursor(0,12);
  print("< CTRL-F1 >");
  setCursor(0,13);
  print(" Debug     ",debug);
  setCursor(0,14);
  print(" Off ", !debug);
  
}


void optionsScreenRender()
{
  setCursor(0, 0);
  print("Available files:", fnSelected == 0, 0xA0);
  setCursor(0,12);
  print("< F1 >", fnSelected == 1);
  setCursor(0,13);
  print(" HD   ", HdDisk);
  setCursor(0,14);
  print(" DISK ", !HdDisk);

  setCursor(8,12);
  print("< F2 >", fnSelected == 2);
  setCursor(8,13);
  print(" IIe  ", AppleIIe);
  setCursor(8,14);
  print(" II+  ", !AppleIIe);

  setCursor(16,12);
  print("< F3 >", fnSelected == 3);
  setCursor(16,13);
  print(" Fast ", Fast1MhzSpeed);
  setCursor(16,14);
  print(" 1Mhz ", !Fast1MhzSpeed);

  setCursor(24,12);
  print("< F4 >", fnSelected == 4);
  setCursor(24,13);
  print(" Spkr ", sound);
  setCursor(24,14);
  print(" Mute ", !sound);

  setCursor(32,12);
  print("< F5 >", fnSelected == 5);
  setCursor(32,13);
  print(" Joy ", joystick);
  setCursor(32,14);
  print(" Off ", !joystick);

  setCursor(0,16);
  print("< F6 >", fnSelected == 6);
  setCursor(0,17);
  print(" Col. ", videoColor);
  setCursor(0,18);
  print(" Mono ", !videoColor);
  #ifdef DAC
  setCursor(8,16);
  print("< F7 >", fnSelected == 7);
  setCursor(8,17);
  print(" DAC ", dacSound);
  setCursor(8,18);
  print(" DIG ", !dacSound);
  #endif

  setCursor(0,20);
  print("<ESC> Exit from menu");
  setCursor(0,21);
  print("<Enter> Select Disk");
  setCursor(0,22);
  print("<Crtl> + <Enter> Save");
  setCursor(0,23);
  print(" and Reboot");
#ifdef DAC
  setCursor(22,20);
  print("<F11> Volume Down");
  setCursor(22,21);
  print("<F12> Volume Up");
  setCursor(22,22);
  print("[", fnSelected == 8);
  for (int v = 0; v <= 0xf0; v+=0x10) {
    if (!dacSound) {
      print("#", fnSelected == 8);
    }
    else if (v == 0) { }
    else if (v <= volume)
      print("#", fnSelected == 8);
    else
      print(" ", fnSelected == 8);
  }
  print("]", fnSelected == 8);
#endif
}
  