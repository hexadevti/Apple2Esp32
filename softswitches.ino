char processSoftSwitches(ushort address, char value, bool Read_Write = true)
{
  // Keyboard Data
  if (address == 0xC000)
  {
    if (Read_Write)
      return keyboard_read();
    else
      Store80On_Off = false;
  }
  else if (address == 0xc001)
  {
    if (!Read_Write)
      Store80On_Off = true;
  }
  else if (address == 0xc002)
    RAMReadOn_Off = false;
  else if (address == 0xc003)
    RAMReadOn_Off = true;
  else if (address == 0xc004)
    RAMWriteOn_Off = false;
  else if (address == 0xc005)
    RAMWriteOn_Off = true;
  else if (address == 0xc006)
    IntCXRomOn_Off = false;
  else if (address == 0xc007)
    IntCXRomOn_Off = true;
  else if (address == 0xc008)
    AltZPOn_Off = false;
  else if (address == 0xc009)
    AltZPOn_Off = true;
  else if (address == 0xc00a)
    SlotC3RomOn_Off = false;
  else if (address == 0xc00b)
    SlotC3RomOn_Off = true;
  else if (address == 0xc00c)
  {
    if (!Read_Write)
      Cols40_80 = true; // Apple IIc IIe
  }
  else if (address == 0xc00d)
  {
    if (!Read_Write)
      Cols40_80 = false; // Apple IIc IIe
  }
  else if (address == 0xc00e)
    AltCharSetOn_Off = false;
  else if (address == 0xc00f)
    AltCharSetOn_Off = true;
  else if (address == 0xC010)
    keyboard_strobe();
  else if (address == 0xc011)
    return (char)(IIEMemoryBankBankSelect1_2 ? 0x00 : 0xff);
  else if (address == 0xc012)
    return (char)(IIEMemoryBankReadRAM_ROM ? 0xff : 0x00);
  else if (address == 0xc014)
    return (byte)(RAMWriteOn_Off ? 0xff : 0x00);
  else if (address == 0xc015)
    return (char)(IntCXRomOn_Off ? 0xff : 0x00);
  else if (address == 0xc016)
    return (char)(AltZPOn_Off ? 0xff : 0x00);
  else if (address == 0xc017)
    return (char)(SlotC3RomOn_Off ? 0xff : 0x00);
  else if (address == 0xc018)
    return (char)(Store80On_Off ? 0xff : 0x00);
  else if (address == 0xc019)
    return (char)(Vertical_blankingOn_Off ? 0xff : 0x00);
  else if (address == 0xc01a)
    return (char)(Graphics_Text ? 0x00 : 0xff);
  else if (address == 0xc01b)
    return (char)(DisplayFull_Split ? 0x00 : 0xff);
  else if (address == 0xc01c)
    return (char)(Page1_Page2 ? 0x00 : 0xff);
  else if (address == 0xc01d)
    return (char)(LoRes_HiRes ? 0x00 : 0xff);
  else if (address == 0xc01e)
    return (char)(AltCharSetOn_Off ? 0xff : 0x00);
  else if (address == 0xc01f)
    return (char)(Cols40_80 ? 0x00 : 0xff);
  else if (address == 0xC030)
    speaker_toggle();
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
    page_lock.lock();
    Page1_Page2 = true;
    page_lock.unlock();
  }
  else if (address == 0xc055)
  {
    page_lock.lock();
    Page1_Page2 = false;
    page_lock.unlock();
  }
  else if (address == 0xc056)
    LoRes_HiRes = true;
  else if (address == 0xc057)
    LoRes_HiRes = false;
  else if (address == 0xc058)
    Cols40_80 = true;
  else if (address == 0xc059)
    Cols40_80 = false;
  else if (address == 0xc05e)
  {
    if (IOUDisOn_Off)
      DHiResOn_Off = true;
  }
  else if (address == 0xc05f)
  {
    if (IOUDisOn_Off)
      DHiResOn_Off = false;
  }
  else if (address == 0xc061)
    return (char)(Pb0 ? 0x80 : 0x00);
  else if (address == 0xc062)
    return (char)(Pb1 ? 0x80 : 0x00);
  else if (address == 0xc063)
    return (char)(Pb2 ? 0x80 : 0x00);
  // else if (address == 0xc063)
  //     return 0x80; // Apple II+ default. For Apple IIe it is defined by shift key pressed
  else if (address == 0xc064)
    return (char)(Cg0 ? 0x80 : 0x00);
  else if (address == 0xc065)
    return (char)(Cg1 ? 0x80 : 0x00);
  else if (address == 0xc066)
    return (char)(Cg2 ? 0x80 : 0x00);
  else if (address == 0xc067)
    return (char)(Cg3 ? 0x80 : 0x00);
  else if (address == 0xc070)
  {
    CgReset0 = true;
    CgReset1 = true;
    CgReset2 = true;
    CgReset3 = true;
    Cg0 = true;
    Cg1 = true;
    Cg2 = true;
    Cg3 = true;
  }
  else if (address == 0xc071 || address == 0xc073 || address == 0xc075 || address == 0xc077)
  {
    /*if (b < mainBoard.IIEAuxBanks)
        IIeExpansionCardBank = b;*/
  }
  else if (address == 0xc07f)
  {
    if (Read_Write)
    {
      return (byte)(DHiResOn_Off ? 0xff : 0x00);
    }
    else
      IOUDisOn_Off = false;
  }
  else if (address == 0xc07e)
  {
    if (Read_Write)
    {
      return (byte)(IOUDisOn_Off ? 0x00 : 0xff);
    }
    else
      IOUDisOn_Off = true;
  }
  else if (address >= 0xc080 && address < 0xc090) // Slot 0 - LanguageCard
  {
    if (AppleIIe)
    {
      if (address >= 0xc080 && address < 0xc090)
      {
        ushort last4bits = (address & 0b00001111);

        bool bits[4];
        for (int i = 0; i < 4; i++)
        {
          bits[i] = (last4bits >> i) & 1;
        }
        IIEMemoryBankBankSelect1_2 = bits[3];
        if (bits[1] && bits[0])
        {
          IIEMemoryBankReadRAM_ROM = true;
          IIEMemoryBankWriteRAM_NoWrite = true;
        }
        else if (!bits[1] && bits[0])
        {
          IIEMemoryBankReadRAM_ROM = false;
          IIEMemoryBankWriteRAM_NoWrite = true;
        }
        else if (bits[1] && !bits[0])
        {
          IIEMemoryBankReadRAM_ROM = false;
          IIEMemoryBankWriteRAM_NoWrite = false;
        }
        else if (!bits[1] && !bits[0])
        {
          IIEMemoryBankReadRAM_ROM = true;
          IIEMemoryBankWriteRAM_NoWrite = false;
        }
      }
    }
    else
    {
      if (Read_Write)
        return languagecardRead(address);
      else
        languagecardWrite(address, value);
    }
  }
  else if (address >= 0xc0e0 && address < 0xc0f0) // Slot 6 - Disk
    if (Read_Write)
      return diskAttached ? DiskSoftSwitchesRead(address) : 0;
    else
      DiskSoftSwitchesWrite(address, value);
  else if (address >= 0xc0f0 && address < 0xc100) // Slot 7 - HD
  {
    if (Read_Write)
      return hdAttached ? HDSoftSwitchesRead(address) : 0;
    else
      HDSoftSwitchesWrite(address, value);
  }
  return 0;
}

char readSoftSwitches(ushort address)
{
  return processSoftSwitches(address, 0, true);
}

void writeSoftSwitches(ushort address, char value)
{
  processSoftSwitches(address, value, false);
}
