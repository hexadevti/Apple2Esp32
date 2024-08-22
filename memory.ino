unsigned char zp[0x200];
unsigned char ram[0xc000];

unsigned char read8(unsigned short address) {
  unsigned char page = address>>8;
  if(page < 0x02) {
    return zp[address];
  } 
  else if (page >= 0x02 && page < 0x04) {  
    return ram[address];
  } 
  else if (page >= 0x04 && page < 0x08) {
    return videoread(address);
  } 
  else if (page >= 0x08 && page < 0xc0) {
    return ram[address];
  } 
  else if (page >= 0xD0) {
    return pgm_read_byte_near(rom+address-0xD000);
  } 
  else {
    // Keyboard Data
    if(address == 0xC000) return keyboard_read();
    // Keyboard Strobe
    if(address == 0xC010) keyboard_strobe();
    // Speaker toggle
    // if(address == 0xC030) speaker_toggle();
    return 0;
  }
}

unsigned short read16(unsigned short address) {
  return (unsigned short)read8(address) | (((unsigned short)read8(address+1))<<8);
}

void write8(unsigned short address, unsigned char value) {
  unsigned char page = address>>8;
  if(page < 0x02) {
    zp[address] = value;
  } 
  else if(page >= 0x02 && page < 0x04) {
    ram[address] = value;
  } 
  else if(page >= 0x04 && page < 0x08) {
    videowrite(address, value);
  } 
  else if (page >= 0x08 && page < 0xc0) {
    ram[address] = value;
  } 
  else { // Softswitched
    // Keyboard Strobe
    if(address == 0xC010) keyboard_strobe();
    // Speaker toggle
    //if(address == 0xC030) speaker_toggle();
  }
}

void write16(unsigned short address, unsigned short value) {
   write8(address, value&0x00FF);
   write8(address+1, (value>>8)&0x00FF);
}

