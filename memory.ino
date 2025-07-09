void memoryAlloc() {
  showFreeMem();
  ram = (unsigned char*)malloc(0xc000 * sizeof(unsigned char));
  auxram = (unsigned char*)malloc(0xc000 * sizeof(unsigned char));
  memoryBankSwitchedRAM1 = (unsigned char*)malloc(0x2000 * sizeof(unsigned char));
  memoryBankSwitchedRAM2_1 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  memoryBankSwitchedRAM2_2 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  IIEAuxBankSwitchedRAM1 = (unsigned char*)malloc(0x2000 * sizeof(unsigned char));
  IIEAuxBankSwitchedRAM2_1 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  IIEAuxBankSwitchedRAM2_2 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  IIEmemoryBankSwitchedRAM1 = (unsigned char*)malloc(0x2000 * sizeof(unsigned char));
  IIEmemoryBankSwitchedRAM2_1 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  IIEmemoryBankSwitchedRAM2_2 = (unsigned char*)malloc(0x1000 * sizeof(unsigned char));
  menuScreen = (unsigned char*)malloc(0x546 * sizeof(unsigned char));
  menuColor = (unsigned char*)malloc(0x546 * sizeof(unsigned char));
  showFreeMem();
  memset(ram, 0, 0xc000 * sizeof(unsigned char));
  memset(auxram, 0, 0xc000 * sizeof(unsigned char));
  memset(memoryBankSwitchedRAM1, 0, 0x2000 * sizeof(unsigned char));
  memset(memoryBankSwitchedRAM2_1, 0, 0x1000 * sizeof(unsigned char));
  memset(memoryBankSwitchedRAM2_2, 0, 0x1000 * sizeof(unsigned char));
  memset(IIEAuxBankSwitchedRAM1, 0, 0x2000 * sizeof(unsigned char));
  memset(IIEAuxBankSwitchedRAM2_1, 0, 0x1000 * sizeof(unsigned char));
  memset(IIEAuxBankSwitchedRAM2_2, 0, 0x1000 * sizeof(unsigned char));
  memset(IIEmemoryBankSwitchedRAM1, 0, 0x2000 * sizeof(unsigned char));
  memset(IIEmemoryBankSwitchedRAM2_1, 0, 0x1000 * sizeof(unsigned char));
  memset(IIEmemoryBankSwitchedRAM2_2, 0, 0x1000 * sizeof(unsigned char));
  memset(menuScreen, 0xa0, 0x546 * sizeof(unsigned char));
  memset(menuColor, 0xf0, 0x546 * sizeof(unsigned char));
  //memset(sourceDiskData, 0, trackRawSize * 35 * sizeof(unsigned char));
  //memset(tempDiskData, 0, trackRawSize * 35 * sizeof(unsigned char));
  showFreeMem();
}

void showFreeMem() {
  Serial.print("Free mem:");
  Serial.print(heap_caps_get_free_size(MALLOC_CAP_8BIT));
  Serial.print(" (");
  Serial.print(heap_caps_get_free_size(MALLOC_CAP_8BIT) / sizeof(float));
  Serial.println(" floats)");
}

unsigned char read8(unsigned short address)
{
  unsigned char page = address >> 8;
  if (page < 0x02)
  {
    if (AppleIIe)
    {
      if (AltZPOn_Off)
        return auxzp[address];
      else
        return zp[address];
    }
    else
      return zp[address];
  }
  else if (page >= 0x02 && page < 0xc0)
  {
    if (AppleIIe)
    {
      if (!Store80On_Off)
      {
        if (RAMReadOn_Off)
          return auxram[address];
        else
          return ram[address];
      }
      else
      {
        if (address >= 0x0400 && address < 0x0800)
        {
          if (!Page1_Page2) // Page 2
            return auxram[address];
          else // Page 1
            return ram[address];
        } // Text Pages
        else if (address >= 0x2000 && address < 0x4000) // Graphics Pages
        {
          if (LoRes_HiRes)
          {
            if (RAMReadOn_Off)
              return auxram[address];
            else
              return ram[address];
          }
          else
          {
            if (!Page1_Page2) // Page 2
              return auxram[address];
            else // Page 1
              return ram[address];
          }
        }
        else
        {
          if (RAMReadOn_Off)
            return auxram[address];
          else
            return ram[address];
        }
      }
    }
    else
    {
      return ram[address];
    }
  }
  else if (page >= 0xc0 && page < 0xc1)
  { // Softswitches
    return readSoftSwitches(address);
  }
  else if (page >= 0xc1 && page < 0xc8)
  {
    if (AppleIIe && IntCXRomOn_Off)
    {
      return appleiieenhancedc0ff[address - 0xc000];
    }
    else
    {
      if (page >= 0xc3 && page < 0xc4)
      {
        if (AppleIIe && !SlotC3RomOn_Off)
        { 
          IntC8RomOn_Off = true;
          return appleiieenhancedc0ff[address - 0xc000];
        }
        else
        {
          IntC8RomOn_Off = false;
          return 0;
        }
      }
      else if (page >= 0xc6 && page < 0xc7)
      {
        return diskAttached ? diskiicardrom[address - 0xc600] : 0;
      }
      else if (page >= 0xc7 && page < 0xc8)
      {
        return hdAttached ? hdrom[address - 0xc700] : 0;
      }
    }
  }
  else if (page >= 0xc8 && page < 0xd0)
  {
    if (AppleIIe)
    {
      if (IntC8RomOn_Off)
        return appleiieenhancedc0ff[address - 0xc000];
    }
  }
  else if (page >= 0xd0)
  {
    if (MemoryBankReadRAM_ROM)
    {
      return languagecardRead(address);
    }
    else
    {

      if (AppleIIe)
      {
        if (IIEMemoryBankReadRAM_ROM)
        {
          if (address >= 0xd000 && address < 0xe000)
          {
            if (IIEMemoryBankBankSelect1_2)
            {
              if (AltZPOn_Off)
                return IIEAuxBankSwitchedRAM2_1[address - 0xd000];
              else
                return IIEmemoryBankSwitchedRAM2_1[address - 0xd000];
            }
            else
            {
              if (AltZPOn_Off)
                return IIEAuxBankSwitchedRAM2_2[address - 0xd000];
              else
                return IIEmemoryBankSwitchedRAM2_2[address - 0xd000];
            }
          }
          else if (address >= 0xd000)
          {
            if (AltZPOn_Off)
              return IIEAuxBankSwitchedRAM1[address - 0xe000];
            else
              return IIEmemoryBankSwitchedRAM1[address - 0xe000];
          }
        }
        else
          return appleiieenhancedc0ff[address - 0xc000];
      }
      else
        return rom[address - 0xd000];
    }
  }
  else
  {
    return 0;
  }
}

void write8(unsigned short address, unsigned char value)
{
  unsigned char page = address >> 8;
  if (page < 0x02)
  {
    if (AppleIIe)
    {
      if (AltZPOn_Off)
        auxzp[address] = value;
      else
        zp[address] = value;
    }
    else
      zp[address] = value;
  }
  else if (page >= 0x02 && page < 0xc0)
  {
    if (AppleIIe)
    {
      if (!Store80On_Off)
      {
        if (RAMWriteOn_Off)
          auxram[address] = value;
        else
          ram[address] = value;
      }
      else // softswitches.Store80On_Off
      {
        if (address >= 0x0400 && address < 0x0800) // Text Pages
        {
          if (!Page1_Page2)
            auxram[address] = value;
          else
            ram[address] = value;
        }
        else if (address >= 0x2000 && address < 0x4000) // Graphics Pages
        {
          if (LoRes_HiRes)
          {
            if (RAMWriteOn_Off)
              auxram[address] = value;
            else
              ram[address] = value;
          }
          else
          {
            if (!Page1_Page2) // Page 2
              auxram[address] = value;
            else // Page 1
              ram[address] = value;
          }
        }
        else
        {
          if (RAMWriteOn_Off)
            auxram[address] = value;
          else
            ram[address] = value;
        }
      }
    }
    else
    {
      ram[address] = value;
    }
  }
  else if (page >= 0xc0 && page < 0xc1)
  { // Softswitched
    writeSoftSwitches(address, value);
  }
  else if (page >= 0xd0)
  {
    if (AppleIIe)
    {
      if (address >= 0xd000 && address < 0xe000)
      {
        if (IIEMemoryBankBankSelect1_2)
        {
          if (AltZPOn_Off)
            IIEAuxBankSwitchedRAM2_1[address - 0xd000] = value;
          else
            IIEmemoryBankSwitchedRAM2_1[address - 0xd000] = value;
        }
        else
        {
          if (AltZPOn_Off)
            IIEAuxBankSwitchedRAM2_2[address - 0xd000] = value;
          else
            IIEmemoryBankSwitchedRAM2_2[address - 0xd000] = value;
        }
      }
      else
      {
        if (AltZPOn_Off)
          IIEAuxBankSwitchedRAM1[address - 0xe000] = value;
        else
          IIEmemoryBankSwitchedRAM1[address - 0xe000] = value;
      }
    }
    else
    {
      if (MemoryBankWriteRAM_NoWrite)
        languagecardWrite(address, value);
    }
  }
}

unsigned short read16(unsigned short address)
{
  return (unsigned short)read8(address) | (((unsigned short)read8(address + 1)) << 8);
}

void write16(unsigned short address, unsigned short value)
{
  write8(address, value & 0x00FF);
  write8(address + 1, (value >> 8) & 0x00FF);
}
