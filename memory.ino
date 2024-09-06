
unsigned char read8(unsigned short address) {
  unsigned char page = address >> 8;
  if (page < 0xc0) {
    return ram[address];
  } else if (page >= 0xc0 && page < 0xc1) {  // Softswitches
    return readSoftSwitches(address);
  } else if (page >= 0xc6 && page < 0xc7) {
    return diskAttached ? diskiicardrom[address - 0xc600] : 0;
  } else if (page >= 0xc7 && page < 0xc8) {
    return hdAttached ? hdrom[address - 0xc700] : 0;
  } else if (page >= 0xd0) {
    if (MemoryBankReadRAM_ROM) {
      return languagecardRead(address);
    } else
      return rom[address - 0xD000];
  } else {
    return 0;
  }
}

void write8(unsigned short address, unsigned char value) {
  unsigned char page = address >> 8;
  if (page < 0x04) {
    ram[address] = value;
  } else if (page >= 0x04 && page < 0x08) { // LoRes Pages
    ram[address] = value;
    if (!Graphics_Text) 
      textLoResWrite(address, value, 0x400);
  } else if (page >= 0x08 && page < 0xc0) { // 
    ram[address] = value;
  } else if (page >= 0xc0 && page < 0xc1) {  // Softswitched
    writeSoftSwitches(address, value);
  } else if (page >= 0xd0) {
    if (MemoryBankWriteRAM_NoWrite)
      languagecardWrite(address, value);
  }
}

unsigned short read16(unsigned short address) {
  return (unsigned short)read8(address) | (((unsigned short)read8(address + 1)) << 8);
}

void write16(unsigned short address, unsigned short value) {
  write8(address, value & 0x00FF);
  write8(address + 1, (value >> 8) & 0x00FF);
}
