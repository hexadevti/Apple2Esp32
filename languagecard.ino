
void languagecardWrite(ushort address, byte b)
{
    if (address >= 0xd000)
    {
        if (address >= 0xd000 && address < 0xe000)
        {
            if (memoryBankBankSelect1_2)
                memoryBankSwitchedRAM2_1[address - 0xd000] = b;
            else
                memoryBankSwitchedRAM2_2[address - 0xd000] = b;
        }
        else
            memoryBankSwitchedRAM1[address - 0xe000] = b;
    }
    ProcessSwitch(address, b);
}

char languagecardRead(ushort address)
{
    char ret = 0;
    if (address >= 0xd000 && address < 0xe000)
    {
        if (memoryBankBankSelect1_2)
            ret = memoryBankSwitchedRAM2_1[address - 0xd000];
        else
            ret = memoryBankSwitchedRAM2_2[address - 0xd000];
    }
    else if (address >= 0xd000)
        ret = memoryBankSwitchedRAM1[address - 0xe000];

    return ProcessSwitch(address, ret);
}

char ProcessSwitch(ushort address, byte b)
{
    if (address >= 0xc080 && address < 0xc090)
    {
        ushort last4bits = (address & 0b00001111);

        bool bits[4];
        for(int i = 0; i < 4; i++)
        {
          bits[i] = (last4bits >> i) & 1;
        }
        
        sprintf(buf, "Languege card switch: %04X", address);
        // Serial.println(buf);
        memoryBankBankSelect1_2 = bits[3];
        if (bits[1] && bits[0])
        {
            memoryBankReadRAM_ROM = true;
            memoryBankWriteRAM_NoWrite = true;
        }
        else if (!bits[1] && bits[0])
        {
            memoryBankReadRAM_ROM = false;
            memoryBankWriteRAM_NoWrite = true;
        }
        else if (bits[1] && !bits[0])
        {
            memoryBankReadRAM_ROM = false;
            memoryBankWriteRAM_NoWrite = false;
        }
        else if (!bits[1] && !bits[0])
        {
            memoryBankReadRAM_ROM = true;
            memoryBankWriteRAM_NoWrite = false;
        }
    }
    return b;
}


