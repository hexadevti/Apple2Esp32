
unsigned char read8(unsigned short address) {
  unsigned char page = address >> 8;
  if (page < 0x02) {
    
      return ram[address];
  } else if (page >= 0x02 && page < 0xc0) {
    if (AppleIIe) {
      if (!RAMReadOn_Off)
        return ram[address];
    } else {
      return ram[address];
    }
  } else if (page >= 0xc0 && page < 0xc1) {  // Softswitches
    return readSoftSwitches(address);
  } else if (page >= 0xc1 && page < 0xc8) {
    if (AppleIIe && IntCXRomOn_Off) {
      return appleiieenhancedc0ff[address - 0xc000];
    }
    else {
      if (page >= 0xc3 && page < 0xc4) {
        if (AppleIIe)
          return appleiieenhancedc0ff[address - 0xc000];
      }
      else if (page >= 0xc6 && page < 0xc7) {
        return diskAttached ? diskiicardrom[address - 0xc600] : 0;
      } else if (page >= 0xc7 && page < 0xc8) {
        return hdAttached ? hdrom[address - 0xc700] : 0;
      }

    }
  } else if (page >= 0xc8 && page < 0xcf) {
    if (AppleIIe) {
      if (IntC8RomOn_Off)
        return appleiieenhancedc0ff[address - 0xc000];
    }
  } else if (page >= 0xd0) {
    if (MemoryBankReadRAM_ROM) {
      return languagecardRead(address);
    } else {
      if (AppleIIe)
        return appleiieenhancedc0ff[address - 0xc000];
      else
        return rom[address - 0xd000];
    }
  } else {
    return 0;
  }
}

void write8(unsigned short address, unsigned char value) {
  unsigned char page = address >> 8;
  if (page < 0x02) {
      ram[address] = value;
  } else if (page >= 0x02 && page < 0xc0) {
    if (!RAMWriteOn_Off)
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
