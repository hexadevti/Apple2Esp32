char readSoftSwitches(ushort address) {
  // Keyboard Data
  if (address == 0xC000)
    return keyboard_read();
  // Keyboard Strobe
  else if (address == 0xC010)
    keyboard_strobe();
  // Speaker toggle
  // else if(address == 0xC030) speaker_toggle();
  else if (address >= 0xc080 && address < 0xc090) // Slot 0 - LanguageCard
    return languagecardRead(address);
  else if (address >= 0xc0f0 && address < 0xc100) // Slot 7 - HD
    return HDSoftSwitchesRead(address);
}

void writeSoftSwitches(ushort address, char value) {
  // Keyboard Strobe
  if (address == 0xC010) keyboard_strobe();
  // Speaker toggle
  //if(address == 0xC030) speaker_toggle();
  else if (address >= 0xc080 && address < 0xc090) // Slot 0 - LanguageCard
    languagecardWrite(address, value);
  else if (address >= 0xc0f0 && address < 0xc100) // Slot 7 - HD
    HDSoftSwitchesWrite(address, value);
  
}