// Memory map (for slot 7):

//  C0F0	(r)   EXECUTE AND RETURN STATUS
// 	C0F1	(r)   STATUS (or ERROR): b7=busy, b0=error
// 	C0F2	(r/w) COMMAND
// 	C0F3	(r/w) UNIT NUMBER
// 	C0F4	(r/w) LOW BYTE OF MEMORY BUFFER
// 	C0F5	(r/w) HIGH BYTE OF MEMORY BUFFER
// 	C0F6	(r/w) LOW BYTE OF BLOCK NUMBER
// 	C0F7	(r/w) HIGH BYTE OF BLOCK NUMBER
// 	C0F8    (r)   NEXT BYTE (legacy read-only port - still supported)
// 	C0F9    (r)   LOW BYTE OF DISK IMAGE SIZE IN BLOCKS
// 	C0FA    (r)   HIGHT BYTE OF DISK IMAGE SIZE IN BLOCKS

unsigned char hdCommand;
unsigned char hdStatus;
bool hdUnitNumber1_2 = true;
unsigned short hdMemoryBuffer;
unsigned short hdBlockNumber;
size_t hdDiskImageSize ;
ushort fileHeaderSize = 0;
unsigned char actualBlock[512];
std::vector<std::string> fileExtensions = { ".hdv", ".po", ".2mg" };


void HDSetup()
{
  printlog("HS Setup...");
  LoadHD();
  if (HdDisk)
    getHdFileInfo(SD);
}

void LoadHD() 
{
  loadHDDir(SD, "/", 0);
}

char HDSoftSwitchesRead(ushort address)
{
  // sprintf(buf,"HDSoftSwitchesRead %04X", address);
  // printlog(buf);
  if (address == 0xc0f0) { 
    switch (hdCommand) {
      case 0x01:
        hdStatus = 0xb7;
        hdStatus = LoadBlock(hdMemoryBuffer, hdBlockNumber);
        return hdStatus;
      default:
        break;
    }
  } else if (address == 0xc0f1) {
    return hdStatus;
  } else if (address == 0xc0f2) {
    return hdCommand;
  } else if (address == 0xc0f3) {
    return (char)((hdUnitNumber1_2 ? 1 : 0) << 7);
  } else if (address == 0xc0f4) {
    return (char)(hdMemoryBuffer & 0x00ff);
  } else if (address == 0xc0f5) {
    return (char)(hdMemoryBuffer & 0xff00);
  } else if (address == 0xc0f6) {
    return (char)(hdBlockNumber & 0x00ff);
  } else if (address == 0xc0f7) {
    return (char)(hdBlockNumber & 0xff00);
  } else if (address == 0xc0f8) {
  } else if (address == 0xc0f9) {
    return (char)((hdUnitNumber1_2 ? getBlockQty() : getBlockQty()) & 0x00ff);
  } else if (address == 0xc0fa) {
    return (char)((hdUnitNumber1_2 ? getBlockQty() : getBlockQty()) & 0xff00);
  }
}

void HDSoftSwitchesWrite(ushort address, char value) {
  if (address == 0xc0f0) {
  } else if (address == 0xc0f1) {
  } else if (address == 0xc0f2) {
    hdCommand = value;
  } else if (address == 0xc0f3) {
    hdUnitNumber1_2 = value >> 7 == 0;
  } else if (address == 0xc0f4) {
    hdMemoryBuffer = (unsigned short)(hdMemoryBuffer & 0xff00 | value);
  } else if (address == 0xc0f5) {
    hdMemoryBuffer = (unsigned short)(hdMemoryBuffer & 0x00ff | value << 8);
  } else if (address == 0xc0f6) {
    hdBlockNumber = (unsigned short)(hdBlockNumber & 0xff00 | value);
  } else if (address == 0xc0f7) {
    hdBlockNumber = (unsigned short)(hdBlockNumber & 0x00ff | value << 8);
  } else if (address == 0xc0f8) {
  } else if (address == 0xc0f9) {
  } else if (address == 0xc0fa) {
  }
}


void getHdFileInfo(fs::FS &fs)
{
  if (!fs.exists(selectedHdFileName.c_str())) 
  {
    selectedHdFileName = "";
  }
  File file = fs.open(selectedHdFileName.c_str());
  sprintf(buf, "APPLE2ESP32 - %s", selectedHdFileName.c_str());
  printMsg(buf, 0xff0000);
  printlog(buf);
  size_t len = file.size();
  Serial.print("File Size: ");
  hdDiskImageSize = len;
  if (len % 512 > 0)
  {
    fileHeaderSize = len - floor(len / 512) * 512;
  }
  file.close();
  Serial.print("File Header Size: ");Serial.println(fileHeaderSize);
}

void nextHdFile()
{
  if (shownFile < (int)((hdFiles.size())-1)) {
    shownFile++;
    sprintf(buf, "APPLE2ESP32 - %s",hdFiles[shownFile].c_str());
    printMsg(buf, 0x0000ff);
  }
}

void prevHdFile()
{
  if (shownFile > 0) {
    shownFile--;
    sprintf(buf, "APPLE2ESP32 - %s", hdFiles[shownFile].c_str());
    printMsg(buf, 0x0000ff);
  }
}

void saveHdFile()
{
  paused = true;
  writeStringToEEPROM(HdFileNameEEPROMaddress, selectedHdFileName.c_str());
  saveEEPROM();
  EEPROM.commit();
  paused = false;
}

void setHdFile()
{
  paused = true;
  selectedHdFileName = hdFiles[shownFile].c_str();
  sprintf(buf, "APPLE2ESP32 - %s", selectedHdFileName.c_str());
  printMsg(buf, 0xff0000);
  paused = false;
}

char LoadBlock(unsigned short address, unsigned short block)
{
  digitalWrite(LED_PIN,HIGH);
  getBlock(SD, block);
  try
  {
    for (int i = 0; i < 512; i++)
    {
      // sprintf(buf,"Load block %04X:%02X", (address + i), actualBlock[i]);
      // printlog(buf);
      write8((address + i), actualBlock[i]);
    }
    digitalWrite(LED_PIN,LOW);
    return 0;
  }
  catch(std::exception ex)
  {
    digitalWrite(LED_PIN,LOW);
    return 0xb0;
  }
}

ushort getBlockQty()
{
  return (ushort)((hdDiskImageSize - fileHeaderSize) / 512);
}


void getBlock(fs::FS &fs, ushort block) 
{
  size_t positionToRead = block * 512 + fileHeaderSize;
  File file = fs.open(selectedHdFileName.c_str());
  size_t len = file.size();;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    if (file.seek(positionToRead))
    {
      file.read(actualBlock, 512);
      // // // Serial.print("File Read Block:");// // // Serial.println(block);
    }
    end = millis() - start;
    file.close();
  } else {
    printlog("Failed to open file for reading");
  }
}

void loadHDDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  // sprintf(buf,"Loading directory: %s\n", dirname);
  // printlog(buf);

  File root = fs.open(dirname);
  if (!root) {
    printlog("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    printlog("Not a directory");
    return;
  }

  File file = root.openNextFile();
  int i = 0;
  while (file) {
    if (file.isDirectory()) {
      // printlog("  DIR : ");
      // printlog(file.name());
      if (levels) {
        loadHDDir(fs, file.path(), levels - 1);
      }
    } else {
      bool acepted = false;
      std::string fileName = file.name();
      for (int j = 0; j < fileExtensions.size(); j++)
      {
        if ((int)fileName.find(fileExtensions[j].c_str()) > 0)
        {
          acepted = true;
          break;
        }
      }
      
      if (acepted)
      {
        // sprintf(buf, " FOUND FILE: %s SIZE: %d", file.name(), file.size());
        // printlog(buf);
        std::string str(file.name());
        hdFiles.push_back("/" + str);
      }
      i++;
    }
    file = root.openNextFile();
  }
  file.close();
  root.close();
}