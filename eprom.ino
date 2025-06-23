void epromSetup() {
    
  EEPROM.begin(EEPROM_SIZE);
  //delay(3000);
  readStringFromEEPROM(NewDeviceConfigEEPROMaddress, &NewDeviceConfig);
  sprintf(buf, "EEPROM NewDeviceConfig value: %s", NewDeviceConfig.c_str());
  printLog(buf);

  
  if (NewDeviceConfig != "ok")
  {
    
    Serial.println("New Device");
    int e1 = EEPROM.writeBool(HdDiskEEPROMaddress, false);
    int e2 = EEPROM.writeBool(IIpIIeEEPROMaddress, false);
    int e3 = EEPROM.writeBool(Fast1MhzSpeedEEPROMaddress, false);
    int e4 = EEPROM.writeBool(JoystickEEPROMaddress, true);
    int e8 = EEPROM.writeBool(VideoColorEEPROMaddress, true);
    int e7 = writeStringToEEPROM(NewDeviceConfigEEPROMaddress, "ok");
    int e5 = writeStringToEEPROM(HdFileNameEEPROMaddress, "/");
    int e6 = writeStringToEEPROM(DiskFileNameEEPROMaddress, "/karateka.dsk");
    EEPROM.commit();
  }
  
  HdDisk = EEPROM.readBool(HdDiskEEPROMaddress);
  AppleIIe = EEPROM.readBool(IIpIIeEEPROMaddress);
  Fast1MhzSpeed = EEPROM.readBool(Fast1MhzSpeedEEPROMaddress);
  joystick = EEPROM.readBool(JoystickEEPROMaddress);
  videoColor = EEPROM.readBool(VideoColorEEPROMaddress);

  
  if (HdDisk) {
    int size = readStringFromEEPROM(HdFileNameEEPROMaddress, &selectedHdFileName);
    sprintf(buf, "EEPROM selectedHdFile value: %s", selectedHdFileName.c_str());
    printLog(buf);
  } else {
    int size = readStringFromEEPROM(DiskFileNameEEPROMaddress, &selectedDiskFileName);
    sprintf(buf, "EEPROM selectedDiskFileName value: %s", selectedDiskFileName.c_str());
    printLog(buf);
  }
  HdDisk = true;
  AppleIIe = true;
  videoColor = true;
  Fast1MhzSpeed = true;
  selectedDiskFileName = "/karateka.dsk";
  selectedHdFileName = "/Total Replay v5.1.hdv";
  
  sprintf(buf, "EEPROM values %d,%d,%d,%d,%d,%s,%s,%s", HdDisk,AppleIIe,Fast1MhzSpeed,joystick,videoColor,selectedHdFileName.c_str(),selectedDiskFileName.c_str(),NewDeviceConfig.c_str());
  printLog(buf);
  
}

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; 
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

void saveEEPROM() {
    EEPROM.writeBool(HdDiskEEPROMaddress, HdDisk);
    EEPROM.writeBool(IIpIIeEEPROMaddress, AppleIIe);
    EEPROM.writeBool(Fast1MhzSpeedEEPROMaddress, Fast1MhzSpeed);
    EEPROM.writeBool(JoystickEEPROMaddress, joystick);
    EEPROM.writeBool(VideoColorEEPROMaddress, videoColor);
  }
  