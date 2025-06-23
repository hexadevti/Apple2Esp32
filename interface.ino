void updateOptions(bool downDirection, bool reload) {
    if (OptionsWindow) {
      //printLoading(0xff, 0, 0);
      if (reload) {
        if (HdDisk)
          loadHD();
        else
          loadDisk();
      }
      //printOptionsBackground(0xff, 0, 0);
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
        //   vga.setTextColor(vga.rgb(0,0,0), vga.rgb(0xff, 0xff, 0xff));
        // else
        //   vga.setTextColor(vga.rgb(0xff, 0xff, 0xff), vga.rgb(0,0,0));
  
        // if (i.size() > 39)
        //   i = i.substr(0, 33) + "..." + i.substr(i.size()-3,3);  
        //   vga.println(i.c_str());
        
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
  updateOptions(true, OptionsWindow);
}
