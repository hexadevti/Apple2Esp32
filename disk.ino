bool diskUnitNumber1_2 = true;
unsigned short diskTrackNumber;
size_t diskImageSize;
ushort diskFileHeaderSize = 0;
const uint diskSize = 143360;
unsigned char diskImage[diskSize];
std::vector<std::string> diskFileExtensions = { ".dsk", ".po", ".do" };
std::vector<std::string> diskFiles;

std::array<uint8_t, 4> odd_even_asc = { 0x31, 0x20, 0x01, 0x30 };
std::array<uint8_t, 4> even_odd_asc = { 0x11, 0x00, 0x21, 0x10 };
std::array<uint8_t, 4> odd_even_desc = { 0x11, 0x20, 0x01, 0x10 };
std::array<uint8_t, 4> even_odd_desc = { 0x31, 0x00, 0x21, 0x30 };

std::array<uint8_t, 4> start_sequence_1 = { 0x31, 0x30, 0x21, 0x20 };
std::array<uint8_t, 4> start_sequence_2 = { 0x11, 0x10, 0x01, 0x00 };

std::array<uint8_t, 64> translateTable = {{
    0x96, 0x97, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA6,
    0xA7, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB3,
    0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC,
    0xBD, 0xBE, 0xBF, 0xCB, 0xCD, 0xCE, 0xCF, 0xD3,
    0xD6, 0xD7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE,
    0xDF, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xEC,
    0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6,
    0xF7, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
}};

void DiskSetup()
{
  if (diskAttached)
  {
    printlog("DiskII Setup...");
    loadDir(SD, "/", 0);
    getDiskFileInfo(SD);
  }
}

char DiskSoftSwitchesRead(ushort address)
{
  return 0;
}

void DiskSoftSwitchesWrite(ushort address, char value) {
  
}


void getDiskFileInfo(fs::FS &fs)
{
  File file = fs.open(diskFiles[selectedFile].c_str());
  sprintf(buf, "APPLE2ESP32 - %s", diskFiles[selectedFile].c_str());
  printMsg(buf);
  size_t len = file.size();
  Serial.print("File Size: ");// // // Serial.println(len);
  diskImageSize = len;
  if (len % 512 > 0)
  {
    diskFileHeaderSize = len - floor(len / 512) * 512;
  }
  file.close();
  Serial.print("File Header Size: ");Serial.println(diskFileHeaderSize);
}

void nextDiskFile()
{
  selectedFile++;
  sprintf(buf, "APPLE2ESP32 - %s", diskFiles[selectedFile].c_str());
  printMsg(buf);
  EEPROM.write(selectedFileEEPROMaddress, selectedFile);
  EEPROM.commit();
  
}

void prevDiskFile()
{
  selectedFile--;
  sprintf(buf, "APPLE2ESP32 - %s", diskFiles[selectedFile].c_str());
  printMsg(buf);
  EEPROM.write(selectedFileEEPROMaddress, selectedFile);
  EEPROM.commit();
  
}

void loadDisk(fs::FS &fs)
{
  size_t positionToRead = diskFileHeaderSize;
  File file = fs.open(diskFiles[selectedFile].c_str());
  size_t len = file.size();;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    if (file.seek(positionToRead))
    {
      file.read(diskImage, diskSize);
    }
    end = millis() - start;
    file.close();
  } else {
    printlog("Failed to open file for reading");
  }
}

void loadDiskDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  sprintf(buf,"Loading directory: %s\n", dirname);
  printlog(buf);

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
      printlog("  DIR : ");
      printlog(file.name());
      if (levels) {
        loadDir(fs, file.path(), levels - 1);
      }
    } else {
      bool acepted = false;
      std::string fileName = file.name();
      for (int j = 0; j < diskFileExtensions.size(); j++)
      {
        if ((int)fileName.find(diskFileExtensions[j].c_str()) > 0)
        {
          acepted = true;
          break;
        }
      }
      
      if (acepted)
      {
        sprintf(buf, " FOUND FILE: %s SIZE: %d", file.name(), file.size());
        printlog(buf);
        std::string str(file.name());
        diskFiles.push_back("/" + str);
      }
      i++;
    }
    file = root.openNextFile();
  }
  file.close();
  root.close();
}

int GetOffset(int track, int sector) {
        return diskFileHeaderSize + (sector * 256) + (track * (256 * 16));
    }

std::vector<uint8_t> GetSectorData(int track, int sector)
{
    std::vector<uint8_t> output(256);
    if (track < 35 && sector < 16)
    {
        int offset = GetOffset(track, sector);

        for (int i = 0; i < 256; i++)
        {
            output[i] = diskImage[offset + i];
        }
    }
    else
    {
        // Handle the else case if needed
    }

    return output;
}


std::vector<uint8_t> Encode6_2(int track, int sector) {
    std::vector<uint8_t> input = GetSectorData(track, sector);
    std::vector<uint8_t> outputData(256);
    std::vector<uint8_t> outputlast2(0x56);
    std::vector<uint8_t> outputlast2Encoded(0x56);
    std::vector<uint8_t> outputDataEncoded(256);

    for (size_t i = 0; i < input.size(); i++) {
        outputData[i] = static_cast<uint8_t>(input[i] >> 2);
        if (i < 86) {
            std::bitset<8> bitsVolume(input[i]);
            uint8_t last2bits = (bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0);
            outputlast2[i] |= last2bits;
        } else if (i < 172) {
            std::bitset<8> bitsVolume(input[i]);
            uint8_t last2bits = ((bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0)) << 2;
            outputlast2[i - 86] |= last2bits;
        } else {
            std::bitset<8> bitsVolume(input[i]);
            uint8_t last2bits = ((bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0)) << 4;
            outputlast2[i - 172] |= last2bits;
        }
    }

    uint8_t lastByte = 0;
    for (size_t i = 0; i < 86; i++) {
        outputlast2Encoded[i] = translateTable[outputlast2[i] ^ lastByte];
        lastByte = outputlast2[i];
    }

    std::vector<uint8_t> agregate(outputlast2Encoded.begin(), outputlast2Encoded.end());

    for (size_t i = 0; i < 256; i++) {
        outputDataEncoded[i] = translateTable[outputData[i] ^ lastByte];
        lastByte = outputData[i];
    }

    agregate.insert(agregate.end(), outputDataEncoded.begin(), outputDataEncoded.end());
    std::vector<uint8_t> checksum = { translateTable[lastByte] };
    agregate.insert(agregate.end(), checksum.begin(), checksum.end());

    return agregate;
}
