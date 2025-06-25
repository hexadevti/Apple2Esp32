void updateOptions(bool downDirection) {
  if (OptionsWindow) {
    //printOptionsBackground(0xff0000);
    std::string result = "";
    int sel = 0;
    int skip = 0;
    //vga.fillRect(42, 42, 236, 147, 0);
    //vga.setCursor(44, 44);
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
      shownFile = 0;
    if (downDirection) {
      if (shownFile >= firstShowFile + 17) {
        firstShowFile = shownFile - 17;
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
      if (id < firstShowFile)
      {
        id++;
        continue;
      }
      if (shown > 17)
        break;        
      // if (id == shownFile)
      //   vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
      // else
      //   vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
      if (i.size() > 39)
        i = i.substr(0, 33) + "..." + i.substr(i.size()-3,3);  
      //vga.println(i.c_str());
      
      shown++;
      id++;
      
    }
    // sprintf(buf, "sel: %d, skip: %d, skiped: %d, shownFile: %d", sel, skip, skiped, shownFile);
    // Serial.println(buf);
  }
}


void updateOptions(bool downDirection, bool reload) {
    if (OptionsWindow) {
      printLoading(0xff, 0, 0);
      if (reload) {
        if (HdDisk)
          loadHD();
        else
          loadDisk();
      }
      printOptionsBackground(0xff, 0, 0);
      std::string result = "";
      int sel = 0;
      int skip = 0;
      // vga.fillRect(42, 42, 236, 147, 0);
      // vga.setCursor(44, 44);
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
        shownFile = 0;
      if (downDirection) {
        if (shownFile >= firstShowFile + 17) {
          firstShowFile = shownFile - 17;
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
        if (id < firstShowFile)
        {
          id++;
          continue;
        }
        if (shown > 17)
          break;        
        
        // if (id == shownFile)
        //   // vga.setTextColor(// vga.rgb(0,0,0), // vga.rgb(0xff, 0xff, 0xff));
        // else
        //   // vga.setTextColor(// vga.rgb(0xff, 0xff, 0xff), // vga.rgb(0,0,0));
  
        if (i.size() > 39)
          i = i.substr(0, 33) + "..." + i.substr(i.size()-3,3);  
          // vga.println(i.c_str());
        
        shown++;
        id++;
        
      }
      
      // sprintf(buf, "sel: %d, skip: %d, skiped: %d, shownFile: %d", sel, skip, skiped, shownFile);
      // Serial.println(buf);
    }
    else
    {
      // vga.fillRect(0, 230, 300, 9, 0);
      // vga.fillRect(0, 0, 300, 20, 0);
    }
    
  }
  
void showHideOptionsWindow() {
OptionsWindow = !OptionsWindow;
//updateOptions(true, OptionsWindow);
updateOptions(true);
}

void printMsg(char msg[], int r, int g, int b)
{
// vga.setFont(FONT_6x8);
// vga.setTextColor(// vga.rgb(r, g, b), // vga.rgb(0, 0, 0));
// vga.setCursor(5, 8);
// vga.print("                                                                   ");
// vga.setCursor(5, 8);
// vga.print(msg);
// // vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
// vga.setTextColor(// vga.rgb(0xff, 0xff, 0xff), // vga.rgb(0, 0, 0));
}
void printLoading(int r, int g, int b)
{
  // vga.fillRect(40, 40, 240, 160, 0);
  // vga.rect(41, 41, 238, 158, // vga.rgb(r, g, b));
  // vga.setCursor(120, 120);
  // vga.setFont(FONT_6x8);
  // vga.setTextColor(// vga.rgb(r, g, b), // vga.rgb(0, 0, 0));
  // vga.print("Loading...");
}

void printStatus(char msg[], int r, int g, int b)
{
  // vga.setFont(FONT_6x8);
  // vga.setTextColor(// vga.rgb(r, g, b), // vga.rgb(0, 0, 0));
  // vga.setCursor(5, 224);
  // vga.print("                                                                   ");
  // vga.setCursor(5, 224);
  // vga.print(msg);
  // // vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
  // vga.setTextColor(// vga.rgb(0xff, 0xff, 0xff), // vga.rgb(0, 0, 0));
}

void demo()
{
  OptionsWindow = true;
  int pos = 40;
  int size = 10;
  int line = 0;
  int col = 0;
  for (int i = 0; i < 256; i++)
  {
    if (i % 16 == 0)
    {
      line++;
      col = 0;
    }
    // vga.fillRect(pos + col * size, pos + line * size, size, size, i);
    // vga.rect(pos + col * size, pos + line * size, size, size, 0);
    col++;
  }
}

bool optionsScreenBlank(int x, int y)
{
  if (OptionsWindow)
  {
    if (x >= 40 && x < 280 && y >= 40 && y < 200)
      return true;
    else
      return false;
  }
  else
    return false;
}

void printOptionsBackground(int r, int g, int b)
{
  // vga.fillRect(40, 40, 240, 160, 0);
  // vga.rect(41, 41, 238, 158, // vga.rgb(r, g, b));
  // vga.setFont(FONT_6x8);
  // vga.setTextColor(// vga.rgb(r, g, b), // vga.rgb(0, 0, 0));
  // vga.setCursor(0, 230);
  sprintf(buf, "%s|%s|%s|%s|%s|%s|%s", HdDisk ? " HD " : "DISK", AppleIIe ? "IIe" : "II+", Fast1MhzSpeed ? "Fast" : "1Mhz", paused ? "Paused " : "Running", joystick ? "Joy On " : "Joy Off", videoColor ? "Color On " : "Color Off", wifiConnected ? WiFi.localIP().toString() : "Not Connected");
  // vga.print(buf);
  // vga.setCursor(0, 10);
  sprintf(buf, " Apple2ESP32S3 %s", HdDisk ? selectedHdFileName.c_str() : selectedDiskFileName.c_str());
  // vga.print(buf);
  // vga.show();
}
  