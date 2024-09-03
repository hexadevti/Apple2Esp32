char readSoftSwitches(ushort address)
{
  // sprintf(buf,"readSoftSwitches %04X", address);
  // printlog(buf);
  // Keyboard Data
  if (address == 0xC000)
    return keyboard_read();
  // Keyboard Strobe
  else if (address == 0xC010)
    keyboard_strobe();
  // Speaker toggle
  else if(address == 0xC030) speaker_toggle();
  else if (address == 0xc050)
    Graphics_Text = true;
  else if (address == 0xc051)
    Graphics_Text = false;
  else if (address == 0xc052)
    DisplayFull_Split = true;
  else if (address == 0xc053)
    DisplayFull_Split = false;
  else if (address == 0xc054)
  {
    std::lock_guard<std::mutex> lock(page_lock);
    Page1_Page2 = true;
  }
  else if (address == 0xc055) {
  std::lock_guard<std::mutex> lock(page_lock);
    Page1_Page2 = false;
  }
  else if (address == 0xc056)
    LoRes_HiRes = true;
  else if (address == 0xc057)
    LoRes_HiRes = false;
  else if (address >= 0xc080 && address < 0xc090) // Slot 0 - LanguageCard
    return languagecardRead(address);
  else if (address >= 0xc0e0 && address < 0xc0f0) // Slot 6 - Disk
    return diskAttached ? DiskSoftSwitchesRead(address) : 0;
  else if (address >= 0xc0f0 && address < 0xc100) // Slot 7 - HD
    return hdAttached ? HDSoftSwitchesRead(address) : 0;
  return 0;
}

void writeSoftSwitches(ushort address, char value)
{
  // Keyboard Strobe
  if (address == 0xC010)
    keyboard_strobe();
  // Speaker toggle
  // if(address == 0xC030) speaker_toggle();
  else if (address == 0xc050)
    Graphics_Text = true;
  else if (address == 0xc051)
    Graphics_Text = false;
  else if (address == 0xc052)
    DisplayFull_Split = true;
  else if (address == 0xc053)
    DisplayFull_Split = false;
  else if (address == 0xc054) {
   std::lock_guard<std::mutex> lock(page_lock); 
    Page1_Page2 = true;
  }
  else if (address == 0xc055) {
    std::lock_guard<std::mutex> lock(page_lock);
    Page1_Page2 = false;
  }
  else if (address == 0xc056)
    LoRes_HiRes = true;
  else if (address == 0xc057)
    LoRes_HiRes = false;
  else if (address >= 0xc080 && address < 0xc090) // Slot 0 - LanguageCard
    languagecardWrite(address, value);
  else if (address >= 0xc0f0 && address < 0xc100) // Slot 7 - HD
    HDSoftSwitchesWrite(address, value);
}