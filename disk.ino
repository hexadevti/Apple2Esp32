bool diskUnitNumber1_2 = true;
unsigned short diskTrackNumber;
ushort diskFileHeaderSize = 0;
const uint diskSize = 143360;
const uint trackEncodedSize = 5856;
const uint trackRawSize = 4096;
uint8_t trackRawData[trackRawSize];
uint8_t trackEncodedData[trackEncodedSize];
uint8_t diskVolume;
std::vector<std::string> diskFileExtensions = {".dsk", ".po", ".do"};
std::vector<std::string> diskFiles;

bool DrivePhase0ON_OFF;
bool DrivePhase1ON_OFF;
bool DrivePhase2ON_OFF;
bool DrivePhase3ON_OFF;
bool FlagDO_PO = true;
bool DriveQ6H_L;
bool DriveQ7H_L;
bool DriveMotorON_OFF;

bool Drive1_2 = true;

int pointer = 0;

int diskTrack = -1;
int diskSector = 0;
std::queue<uint8_t> phaseBuffer;

std::array<uint8_t, 4> odd_even_asc = {0x31, 0x20, 0x01, 0x30};
std::array<uint8_t, 4> even_odd_asc = {0x11, 0x00, 0x21, 0x10};
std::array<uint8_t, 4> odd_even_desc = {0x11, 0x20, 0x01, 0x10};
std::array<uint8_t, 4> even_odd_desc = {0x31, 0x00, 0x21, 0x30};

std::array<uint8_t, 4> start_sequence_1 = {0x31, 0x30, 0x21, 0x20};
std::array<uint8_t, 4> start_sequence_2 = {0x11, 0x10, 0x01, 0x00};

std::array<uint8_t, 64> translateTable = {
    0x96, 0x97, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA6,
    0xA7, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB3,
    0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC,
    0xBD, 0xBE, 0xBF, 0xCB, 0xCD, 0xCE, 0xCF, 0xD3,
    0xD6, 0xD7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE,
    0xDF, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xEC,
    0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6,
    0xF7, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

char translateDOTrack[] = {0x00, 0x07, 0x0e, 0x06, 0x0d, 0x05, 0x0c, 0x04, 0x0b, 0x03, 0x0a, 0x02, 0x09, 0x01, 0x08, 0x0f};
char translatePOTrack[] = {0x00, 0x08, 0x01, 0x09, 0x02, 0x0A, 0x03, 0x0b, 0x04, 0x0C, 0x05, 0x0d, 0x06, 0x0E, 0x07, 0x0f};
ushort secoffset[] = {0, 0x700, 0xe00, 0x600, 0xd00, 0x500, 0xc00, 0x400, 0xb00, 0x300, 0xa00, 0x200, 0x900, 0x100, 0x800, 0xf00};

void DiskSetup()
{
  if (diskAttached)
  {
    printlog("DiskII Setup...");
    loadDiskDir(SD, "/", 0);
    getDiskFileInfo(SD);
    phaseBuffer = std::queue<uint8_t>();
    //getTrack(SD, 0);
  }
}

void AddPhase(uint8_t phase)
{
  int track = diskTrack;
  phaseBuffer.push(phase);
  // sprintf(buf, "Add Phase: %02X, size: %d", phase, phaseBuffer.size());
  // printlog(buf);
  if (phaseBuffer.size() > 4)
  {
    phaseBuffer.pop();
    // sprintf(buf, "Buffer > 4");
    // printlog(buf);
  }

  std::array<uint8_t, 4> currentBuffer;
  for (size_t i = 0; i < currentBuffer.size() && !phaseBuffer.empty(); ++i)
  {
    currentBuffer[i] = phaseBuffer.front();
    phaseBuffer.pop();
  }
  // Serial.println("currentBuffer: ");
  // for (int i = 0; i < currentBuffer.size(); i++)
  // {
  //   sprintf(buf, "%02X, ", currentBuffer[i]);
  //   printlog(buf);
  // }
  // Serial.println("");
  for (const auto &val : currentBuffer)
  {
    phaseBuffer.push(val); // Restore the queue
  }

  if (track % 2 == 0 && std::equal(currentBuffer.begin(), currentBuffer.end(), even_odd_asc.begin()))
  {
    track++;
  }
  else if (track % 2 != 0 && std::equal(currentBuffer.begin(), currentBuffer.end(), odd_even_asc.begin()))
  {
    track++;
  }
  else if (track % 2 == 0 && std::equal(currentBuffer.begin(), currentBuffer.end(), even_odd_desc.begin()) && track > 0)
  {
    track--;
  }
  else if (diskTrack % 2 != 0 && std::equal(currentBuffer.begin(), currentBuffer.end(), odd_even_desc.begin()) && track > 0)
  {
    track--;
  }
  else if (track > 0 && (std::equal(currentBuffer.begin(), currentBuffer.end(), start_sequence_1.begin()) ||
                             std::equal(currentBuffer.begin(), currentBuffer.end(), start_sequence_2.begin())))
  {
    track--;
  }

  // sprintf(buf, "Track Change: %d", track);
  // printlog(buf);
  getTrack(SD, track);

}

void getDiskFileInfo(fs::FS &fs)
{
  File file = fs.open(diskFiles[selectedFile].c_str());
  sprintf(buf, "APPLE2ESP32 - %s", diskFiles[selectedFile].c_str());
  printMsg(buf);
  printlog(buf);
  size_t len = file.size();
  sprintf(buf, "File Size: %d", len);
  printlog(buf);
  file.close();
  getTrack(SD, 17);
  diskVolume = trackRawData[0x06];
  sprintf(buf, "Disk Volume: %d", diskVolume);
  printlog(buf);
}

void getTrack(fs::FS &fs, int track) 
{
  if (track != diskTrack)
  {
    size_t positionToRead = GetOffset(track, 0);
    File file = fs.open(diskFiles[selectedFile].c_str());
    size_t len = file.size();;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
      if (file.seek(positionToRead))
      {
        file.read(trackRawData, trackRawSize);
        diskTrack = track;
        TrackRawData(diskTrack);
      }
      // sprintf(buf, "GetTrack: %d, Size: %d", track, trackRawSize);
      // printlog(buf);
      // for (int i = 0; i < 50;i++)
      // {
      //   sprintf(buf, "d[%d]: %02X", i, trackRawData[i]);
      //   printlog(buf);
      // }
      end = millis() - start;
      file.close();


    } else {
      printlog("Failed to open file for reading");
    }
  }
  else{
    // sprintf(buf, "GetTrack: %d, Already loaded", track);
    // printlog(buf);
  }
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

void loadDiskDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  sprintf(buf, "Loading directory: %s\n", dirname);
  printlog(buf);

  File root = fs.open(dirname);
  if (!root)
  {
    printlog("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    printlog("Not a directory");
    return;
  }

  File file = root.openNextFile();
  int i = 0;
  while (file)
  {
    if (file.isDirectory())
    {
      printlog("  DIR : ");
      printlog(file.name());
      if (levels)
      {
        loadDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
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

int GetOffset(int track, int sector)
{
  return diskFileHeaderSize + (sector * 256) + (track * (256 * 16));
}

int GetSectorOffset(int sector)
{
  return (sector * 256);
}

std::vector<uint8_t> GetSectorData(int sector)
{
  std::vector<uint8_t> output(256);
  if (sector < 16)
  {
    int offset = GetSectorOffset(sector);

    for (int i = 0; i < 256; i++)
    {
      output[i] = trackRawData[offset + i];
    }
  }
  else
  {
    // Handle the else case if needed
  }

  return output;
}


std::vector<uint8_t> EncodeByte(uint8_t data)
{
  std::vector<uint8_t> output(2, 0);
  std::bitset<16> bitsEncoded;
  std::bitset<8> bitsData(data);

  for (int i = 0; i < 16; i++)
  {
    if (i % 2 == 0)
      bitsEncoded[i] = true;
    else
    {
      if (i > 8)
        bitsEncoded[i] = bitsData[8 - (i - 7)];
      else
        bitsEncoded[i] = bitsData[8 - i];
    }
  }

  for (int i = 0; i < 8; i++)
  {
    output[0] += (bitsEncoded[i] ? static_cast<uint8_t>(std::pow(2, 7 - i)) : 0);
    output[1] += (bitsEncoded[i + 8] ? static_cast<uint8_t>(std::pow(2, 7 - i)) : 0);
  }
  return output;
}

std::vector<uint8_t> Checksum(uint8_t volume, uint8_t sector, uint8_t track)
{
  std::vector<uint8_t> output(2, 0);
  std::bitset<16> checkedBits;
  std::bitset<16> checkedBitsInverted;
  std::bitset<16> bitsVolume(EncodeByte(volume)[0] | (EncodeByte(volume)[1] << 8));
  std::bitset<16> bitsSector(EncodeByte(sector)[0] | (EncodeByte(sector)[1] << 8));
  std::bitset<16> bitsTrack(EncodeByte(track)[0] | (EncodeByte(track)[1] << 8));

  for (int i = 0; i < 16; i++)
  {
    int sumBits = bitsVolume[i] + bitsSector[i] + bitsTrack[i];
    checkedBits[i] = (sumBits == 1 || sumBits == 3);
  }

  for (int i = 0; i < 16; i++)
  {
    if (i < 8)
      checkedBitsInverted[i] = checkedBits[7 - i];
    else
      checkedBitsInverted[i] = checkedBits[23 - i];
  }

  for (int i = 0; i < 8; i++)
  {
    output[0] += (checkedBitsInverted[i] ? static_cast<uint8_t>(std::pow(2, 7 - i)) : 0);
    output[1] += (checkedBitsInverted[i + 8] ? static_cast<uint8_t>(std::pow(2, 7 - i)) : 0);
  }
  return output;
}


std::vector<uint8_t> Encode6_2(uint8_t sector)
{
  std::vector<uint8_t> input = GetSectorData(sector);


  std::vector<uint8_t> outputData(256);
  std::vector<uint8_t> outputlast2(0x56);
  std::vector<uint8_t> outputlast2Encoded(0x56);
  std::vector<uint8_t> outputDataEncoded(256);

  for (size_t i = 0; i < input.size(); i++)
  {
    outputData[i] = static_cast<uint8_t>(input[i] >> 2);
    if (i < 86)
    {
      std::bitset<8> bitsVolume(input[i]);
      uint8_t last2bits = (bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0);
      outputlast2[i] |= last2bits;
    }
    else if (i < 172)
    {
      std::bitset<8> bitsVolume(input[i]);
      uint8_t last2bits = ((bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0)) << 2;
      outputlast2[i - 86] |= last2bits;
    }
    else
    {
      std::bitset<8> bitsVolume(input[i]);
      uint8_t last2bits = ((bitsVolume[0] ? 2 : 0) + (bitsVolume[1] ? 1 : 0)) << 4;
      outputlast2[i - 172] |= last2bits;
    }
  }

  uint8_t lastByte = 0;
  for (size_t i = 0; i < 86; i++)
  {
    outputlast2Encoded[i] = translateTable[outputlast2[i] ^ lastByte];
    lastByte = outputlast2[i];
  }

  std::vector<uint8_t> agregate(outputlast2Encoded.begin(), outputlast2Encoded.end());

  for (size_t i = 0; i < 256; i++)
  {
    outputDataEncoded[i] = translateTable[outputData[i] ^ lastByte];
    lastByte = outputData[i];
  }

  agregate.insert(agregate.end(), outputDataEncoded.begin(), outputDataEncoded.end());
  std::vector<uint8_t> checksum = {translateTable[lastByte]};
  agregate.insert(agregate.end(), checksum.begin(), checksum.end());

  return agregate;
}

void TrackRawData(int track)
{
  std::vector<uint8_t> selectedSector;
  std::array<uint8_t, 16> sectors = {0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}; // DOS
  for (uint8_t isec : sectors)
  {
    
    std::vector<uint8_t> b;
    selectedSector.insert(selectedSector.end(), {0xff, 0xff, 0xff});
    selectedSector.insert(selectedSector.end(), {0xd5, 0xaa, 0x96}); // Prologue address
    b = EncodeByte(diskVolume);
    selectedSector.insert(selectedSector.end(), b.begin(), b.end()); // Volume
    b = EncodeByte(static_cast<uint8_t>(track));
    selectedSector.insert(selectedSector.end(), b.begin(), b.end()); // Track
    b = EncodeByte(isec);
    selectedSector.insert(selectedSector.end(), b.begin(), b.end()); // Sector
    b = Checksum(diskVolume, static_cast<uint8_t>(track), isec);
    selectedSector.insert(selectedSector.end(), b.begin(), b.end()); // Checksum
    selectedSector.insert(selectedSector.end(), {0xde, 0xaa, 0xeb}); // Epilogue address
    selectedSector.insert(selectedSector.end(), {0xd5, 0xaa, 0xad}); // Prologue data
    b = Encode6_2(FlagDO_PO ? translateDOTrack[isec] : translatePOTrack[isec]);
    selectedSector.insert(selectedSector.end(), b.begin(), b.end()); // Data field + checksum
    selectedSector.insert(selectedSector.end(), {0xde, 0xaa, 0xeb}); // Epilogue
  }

  
  std::copy(selectedSector.begin(), selectedSector.end(), trackEncodedData);
  // printlog("Raw Data");
  // PrintHex(trackRawData, trackRawSize);
  // printlog("Raw Encoded");
  // PrintHex(trackEncodedData,trackEncodedSize);
  
}

void PrintHex(uint8_t data[], int length) {
  for (int i = 0; i < length; i++)
  {
    if (i % 16 == 0) {
      Serial.println();
      sprintf(buf, "%04X: ", i);
      Serial.print(buf);  
    }
    sprintf(buf, "%02X ", data[i]);
    Serial.print(buf);
  }
  Serial.println();
}

char DiskSoftSwitchesRead(ushort address)
{
  diskSector = read8(0x2d);
  if (address == 0xc0ec)
  {
    if (DriveQ6H_L == false && DriveQ7H_L == false)
    {
      if (pointer > trackEncodedSize - 1)
        pointer = 0;
      // sprintf(buf, "Disk Track: %d, Disk Read: %04X, Pointer: %d, Data: %02X", diskTrack, address, pointer, trackEncodedData[pointer]);
      // printlog(buf);
      // sprintf(buf, "(%04x)[R]%04X: %02X", PC, address, trackEncodedData[pointer]);
      // printlog(buf);
      return trackEncodedData[pointer++];
    }
  }

  return ProcessSwitchc0e0(address, 0);
}

void DiskSoftSwitchesWrite(ushort address, char value)
{
  ProcessSwitchc0e0(address, value);
}

char ProcessSwitchc0e0(ushort address, char value)
{
  if (address == 0xc0e0)
  {
    DrivePhase0ON_OFF = false;
    AddPhase(0x00);
  }
  else if (address == 0xc0e1)
  {
    DrivePhase0ON_OFF = true;
    AddPhase(0x01);
  }
  else if (address == 0xc0e2)
  {
    DrivePhase1ON_OFF = false;
    AddPhase(0x10);
  }
  else if (address == 0xc0e3)
  {
    DrivePhase1ON_OFF = true;
    AddPhase(0x11);
  }
  else if (address == 0xc0e4)
  {
    DrivePhase2ON_OFF = false;
    AddPhase(0x20);
  }
  else if (address == 0xc0e5)
  {
    DrivePhase2ON_OFF = true;
    AddPhase(0x21);
  }
  else if (address == 0xc0e6)
  {
    DrivePhase3ON_OFF = false;

    AddPhase(0x30);
  }
  else if (address == 0xc0e7)
  {
    DrivePhase3ON_OFF = true;

    AddPhase(0x31);
  }
  else if (address == 0xc0e8)
  {
    DriveMotorON_OFF = false;
  }
  else if (address == 0xc0e9)
  {
    DriveMotorON_OFF = true;
  }
  else if (address == 0xc0ea)
    Drive1_2 = true;
  else if (address == 0xc0eb)
    Drive1_2 = false;
  else if (address == 0xc0ec)
    DriveQ6H_L = false;
  else if (address == 0xc0ed)
    DriveQ6H_L = true;
  else if (address == 0xc0ee)
  {
    DriveQ7H_L = false;
    return 0; // Not Write Protected, 9f Write protected
  }
  else if (address == 0xc0ef)
  {
    DriveQ7H_L = true;
  }
  return 0;
}