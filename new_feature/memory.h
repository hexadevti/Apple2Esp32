unsigned char zp[0x200];
unsigned char ram[0xc000];

unsigned char read8(unsigned short address);
unsigned short read16(unsigned short address);
void write16(unsigned short address, unsigned short value);