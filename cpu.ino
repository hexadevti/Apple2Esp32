// μ6502 - Barebones 6502 Emulator By Damian Peckett
// dpeckett.com, <damian@pecke.tt>

// Address Modes
#define AD_IMP  0x01
#define AD_A    0x02
#define AD_ABS  0x03
#define AD_ABSX 0x04
#define AD_ABSY 0x05
#define AD_IMM  0x06
#define AD_IND  0x07
#define AD_INDX 0x08
#define AD_INDY 0x09
#define AD_REL  0x0A
#define AD_ZPG  0x0B
#define AD_ZPGX 0x0C
#define AD_ZPGY 0x0D

// SR Flag Modes
#define FL_NONE 0x00  // ---- ----
#define FL_N    0x80  // N--- ----
#define FL_V    0x40  // -V-- ----
#define FL_Z    0x20  // --Z- ----
#define FL_C    0x10  // ---C ----
#define FL_ZN   0xA0  // N-Z- ----
#define FL_NVC  0xD0  // NV-C ----
#define FL_NVZ  0xE0  // NVZ- ----
#define FL_ZNC  0xB0  // N-ZC ----
#define FL_ZC   0x30  // --ZC ----
#define FL_ALL  0xF0  // NVZC ----

//Unimplemented ops
#define UNDF 0x00

//Other constants
#define SR_CARRY      0x01
#define SR_ZERO       0x02
#define SR_INT        0x04
#define SR_DEC        0x08
#define SR_BRK        0x10
#define SR_FIXED_BITS 0x20
#define SR_OVER       0x40
#define SR_NEG        0x80

//Stack pointer base address
#define STP_BASE 0x100
long count = 0;
long cycleCount = 0;
uint32_t cpuCycleCount = 0;
uint32_t lastCpuCycleCount = 0;
uint32_t diffCpuCycleCount = 0;


//high nibble SR flags, low nibble address mode
const unsigned char flags[] PROGMEM = {
//X0               X1                X2                  X3    X4                   X5                X6                X7    X8              X9                XA               XB    XC                  XD                XE                XF   
  AD_IMP,          AD_INDX,          UNDF,               UNDF, FL_Z|AD_ZPG,/*e*/    FL_ZN | AD_ZPG,   FL_ZNC | AD_ZPG,  UNDF, AD_IMP,         FL_ZN | AD_IMM,   FL_ZNC | AD_A,    UNDF, FL_Z|AD_ABS,/*e*/   FL_ZN | AD_ABS,   FL_ZNC | AD_ABS,  UNDF, // 0X
  AD_REL,          FL_ZN | AD_INDY,  FL_ZN|AD_ZPG/*e*/,  UNDF, FL_Z|AD_ZPG,/*e*/    FL_ZN | AD_ZPGX,  FL_ZNC | AD_ZPGX, UNDF, AD_IMP,         FL_ZN | AD_ABSY,  FL_ZN|AD_A,/*e*/  UNDF, FL_Z|AD_ABS,/*e*/   FL_ZN | AD_ABSX,  FL_ZNC | AD_ABSX, UNDF, // 1X
  AD_ABS,          FL_ZN | AD_INDX,  UNDF,               UNDF, FL_Z | AD_ZPG,       FL_ZN | AD_ZPG,   FL_ZNC | AD_ZPG,  UNDF, AD_IMP,         FL_ZN | AD_IMM,   FL_ZNC | AD_A,    UNDF, FL_Z | AD_ABS,      FL_ZN | AD_ABS,   FL_ZNC | AD_ABS,  UNDF, // 2X
  AD_REL,          FL_ZN | AD_INDY,  FL_ZN|AD_ZPG/*e*/,  UNDF, FL_NVZ|AD_ZPGX/*e*/, FL_ZN | AD_ZPGX,  FL_ZNC | AD_ZPGX, UNDF, AD_IMP,         FL_ZN | AD_ABSY,  FL_ZN|AD_A,/*e*/  UNDF, FL_NVZ|AD_ABSX,/*e*/FL_ZN | AD_ABSX,  FL_ZNC | AD_ABSX, UNDF, // 3X
  AD_IMP,          FL_ZN | AD_INDX,  UNDF,               UNDF, UNDF,                FL_ZN | AD_ZPG,   FL_ZNC | AD_ZPG,  UNDF, AD_IMP,         FL_ZN | AD_IMM,   FL_ZNC | AD_A,    UNDF, AD_ABS,             FL_ZN | AD_ABS,   FL_ZNC | AD_ABS,  UNDF, // 4X
  AD_REL,          FL_ZN | AD_INDY,  FL_ZN|AD_ZPG/*e*/,  UNDF, UNDF,                FL_ZN | AD_ZPGX,  FL_ZNC | AD_ZPGX, UNDF, AD_IMP,         FL_ZN | AD_ABSY,  AD_IMP,/*e*/      UNDF, UNDF,               FL_ZN | AD_ABSX,  FL_ZNC | AD_ABSX, UNDF, // 5X
  AD_IMP,          FL_ALL | AD_INDX, UNDF,               UNDF, AD_ZPG,/*e*/         FL_ALL | AD_ZPG,  FL_ZNC | AD_ZPG,  UNDF, FL_ZN | AD_IMP, FL_ALL | AD_IMM,  FL_ZNC | AD_A,    UNDF, AD_IND,             FL_ALL | AD_ABS,  FL_ZNC | AD_ABS,  UNDF, // 6X
  AD_REL,          FL_ALL | AD_INDY, FL_ALL|AD_ZPG/*e*/, UNDF, AD_ZPGX,/*e*/        FL_ALL | AD_ZPGX, FL_ZNC | AD_ZPGX, UNDF, AD_IMP,         FL_ALL | AD_ABSY, FL_ZN|AD_IMP,/*e*/UNDF, AD_ABSX,            FL_ALL | AD_ABSX, FL_ZNC | AD_ABSX, UNDF, // 7X
  AD_REL,/*e*/     AD_INDX,          UNDF,               UNDF, AD_ZPG,              AD_ZPG,           AD_ZPG,           UNDF, FL_ZN | AD_IMP, FL_Z|AD_IMM/*e*/, FL_ZN | AD_IMP,   UNDF, AD_ABS,             AD_ABS,           AD_ABS,           UNDF, // 8X
  AD_REL,          AD_INDY,          AD_ZPG/*e*/,        UNDF, AD_ZPGX,             AD_ZPGX,          AD_ZPGY,          UNDF, FL_ZN | AD_IMP, AD_ABSY,          AD_IMP,           UNDF, AD_ABS,/*e*/        AD_ABSX,          AD_ABSX,/*e*/     UNDF, // 9X
  FL_ZN | AD_IMM,  FL_ZN | AD_INDX,  FL_ZN | AD_IMM,     UNDF, FL_ZN | AD_ZPG,      FL_ZN | AD_ZPG,   FL_ZN | AD_ZPG,   UNDF, FL_ZN | AD_IMP, FL_ZN | AD_IMM,   FL_ZN | AD_IMP,   UNDF, FL_ZN | AD_ABS,     FL_ZN | AD_ABS,   FL_ZN | AD_ABS,   UNDF, // AX
  AD_REL,          FL_ZN | AD_INDY,  FL_ZN|AD_ZPG/*e*/,  UNDF, FL_ZN | AD_ZPGX,     FL_ZN | AD_ZPGX,  FL_ZN | AD_ZPGY,  UNDF, AD_IMP,         FL_ZN | AD_ABSY,  FL_ZN | AD_IMP,   UNDF, FL_ZN | AD_ABSX,    FL_ZN | AD_ABSX,  FL_ZN | AD_ABSY,  UNDF, // BX
  FL_ZNC | AD_IMM, FL_ZNC | AD_INDX, UNDF,               UNDF, FL_ZNC | AD_ZPG,     FL_ZNC | AD_ZPG,  FL_ZN | AD_ZPG,   UNDF, FL_ZN | AD_IMP, FL_ZNC | AD_IMM,  FL_ZN | AD_IMP,   UNDF, FL_ZNC | AD_ABS,    FL_ZNC | AD_ABS,  FL_ZN | AD_ABS,   UNDF, // CX
  AD_REL,          FL_ZNC | AD_INDY, FL_ZNC|AD_ZPG/*e*/, UNDF, UNDF,                FL_ZNC | AD_ZPGX, FL_ZN | AD_ZPGX,  UNDF, AD_IMP,         FL_ZNC | AD_ABSY, AD_IMP,/*e*/      UNDF, UNDF,               FL_ZNC | AD_ABSX, FL_ZN | AD_ABSX,  UNDF, // DX
  FL_ZNC | AD_IMM, FL_ALL | AD_INDX, UNDF,               UNDF, FL_ZNC | AD_ZPG,     FL_ALL | AD_ZPG,  FL_ZN | AD_ZPG,   UNDF, FL_ZN | AD_IMP, FL_ALL | AD_IMM,  AD_IMP,           UNDF, FL_ZNC | AD_ABS,    FL_ALL | AD_ABS,  FL_ZN | AD_ABS,   UNDF, // EX
  AD_REL,          FL_ALL | AD_INDY, FL_ALL|AD_ZPG/*e*/, UNDF, UNDF,                FL_ALL | AD_ZPGX, FL_ZN | AD_ZPGX,  UNDF, AD_IMP,         FL_ALL | AD_ABSY, FL_ZN|AD_IMP,/*e*/UNDF, UNDF,               FL_ALL | AD_ABSX, FL_ZN | AD_ABSX,  UNDF  // FX
};

// const unsigned char flags[] = {
//   AD_IMP, AD_INDX, UNDF, UNDF, UNDF, FL_ZN | AD_ZPG, FL_ZNC | AD_ZPG, UNDF, AD_IMP, FL_ZN | AD_IMM, FL_ZNC | AD_A, UNDF, UNDF, FL_ZN | AD_ABS, FL_ZNC | AD_ABS, UNDF,
//   AD_REL, FL_ZN | AD_INDY, UNDF, UNDF, UNDF, FL_ZN | AD_ZPGX, FL_ZNC | AD_ZPGX, UNDF, AD_IMP, FL_ZN | AD_ABSY, UNDF, UNDF, UNDF, FL_ZN | AD_ABSX, FL_ZNC | AD_ABSX, UNDF,
//   AD_ABS, FL_ZN | AD_INDX, UNDF, UNDF, FL_Z | AD_ZPG, FL_ZN | AD_ZPG, FL_ZNC | AD_ZPG, UNDF, AD_IMP, FL_ZN | AD_IMM, FL_ZNC | AD_A, UNDF, FL_Z | AD_ABS, FL_ZN | AD_ABS, FL_ZNC | AD_ABS, UNDF,
//   AD_REL, FL_ZN | AD_INDY, UNDF, UNDF, UNDF, FL_ZN | AD_ZPGX, FL_ZNC | AD_ZPGX, UNDF, AD_IMP, FL_ZN | AD_ABSY, UNDF, UNDF, UNDF, FL_ZN | AD_ABSX, FL_ZNC | AD_ABSX, UNDF,
//   AD_IMP, FL_ZN | AD_INDX, UNDF, UNDF, UNDF, FL_ZN | AD_ZPG, FL_ZNC | AD_ZPG, UNDF, AD_IMP, FL_ZN | AD_IMM, FL_ZNC | AD_A, UNDF, AD_ABS, FL_ZN | AD_ABS, FL_ZNC | AD_ABS, UNDF,
//   AD_REL, FL_ZN | AD_INDY, UNDF, UNDF, UNDF, FL_ZN | AD_ZPGX, FL_ZNC | AD_ZPGX, UNDF, AD_IMP, FL_ZN | AD_ABSY, UNDF, UNDF, UNDF, FL_ZN | AD_ABSX, FL_ZNC | AD_ABSX, UNDF,
//   AD_IMP, FL_ALL | AD_INDX, UNDF, UNDF, UNDF, FL_ALL | AD_ZPG, FL_ZNC | AD_ZPG, UNDF, FL_ZN | AD_IMP, FL_ALL | AD_IMM, FL_ZNC | AD_A, UNDF, AD_IND, FL_ALL | AD_ABS, FL_ZNC | AD_ABS, UNDF,
//   AD_REL, FL_ALL | AD_INDY, UNDF, UNDF, UNDF, FL_ALL | AD_ZPGX, FL_ZNC | AD_ZPGX, UNDF, AD_IMP, FL_ALL | AD_ABSY, UNDF, UNDF, UNDF, FL_ALL | AD_ABSX, FL_ZNC | AD_ABSX, UNDF,
//   UNDF, AD_INDX, UNDF, UNDF, AD_ZPG, AD_ZPG, AD_ZPG, UNDF, FL_ZN | AD_IMP, UNDF, FL_ZN | AD_IMP, UNDF, AD_ABS, AD_ABS, AD_ABS, UNDF,
//   AD_REL, AD_INDY, UNDF, UNDF, AD_ZPGX, AD_ZPGX, AD_ZPGY, UNDF, FL_ZN | AD_IMP, AD_ABSY, AD_IMP, UNDF, UNDF, AD_ABSX, UNDF, UNDF,
//   FL_ZN | AD_IMM, FL_ZN | AD_INDX, FL_ZN | AD_IMM, UNDF, FL_ZN | AD_ZPG, FL_ZN | AD_ZPG, FL_ZN | AD_ZPG, UNDF, FL_ZN | AD_IMP, FL_ZN | AD_IMM, FL_ZN | AD_IMP, UNDF, FL_ZN | AD_ABS, FL_ZN | AD_ABS, FL_ZN | AD_ABS, UNDF,
//   AD_REL, FL_ZN | AD_INDY, UNDF, UNDF, FL_ZN | AD_ZPGX, FL_ZN | AD_ZPGX, FL_ZN | AD_ZPGY, UNDF, AD_IMP, FL_ZN | AD_ABSY, FL_ZN | AD_IMP, UNDF, FL_ZN | AD_ABSX, FL_ZN | AD_ABSX, FL_ZN | AD_ABSY, UNDF,
//   FL_ZNC | AD_IMM, FL_ZNC | AD_INDX, UNDF, UNDF, FL_ZNC | AD_ZPG, FL_ZNC | AD_ZPG, FL_ZN | AD_ZPG, UNDF, FL_ZN | AD_IMP, FL_ZNC | AD_IMM, FL_ZN | AD_IMP, UNDF, FL_ZNC | AD_ABS, FL_ZNC | AD_ABS, FL_ZN | AD_ABS, UNDF,
//   AD_REL, FL_ZNC | AD_INDY, UNDF, UNDF, UNDF, FL_ZNC | AD_ZPGX, FL_ZN | AD_ZPGX, UNDF, AD_IMP, FL_ZNC | AD_ABSY, UNDF, UNDF, UNDF, FL_ZNC | AD_ABSX, FL_ZN | AD_ABSX, UNDF,
//   FL_ZNC | AD_IMM, FL_ALL | AD_INDX, UNDF, UNDF, FL_ZNC | AD_ZPG, FL_ALL | AD_ZPG, FL_ZN | AD_ZPG, UNDF, FL_ZN | AD_IMP, FL_ALL | AD_IMM, AD_IMP, UNDF, FL_ZNC | AD_ABS, FL_ALL | AD_ABS, FL_ZN | AD_ABS, UNDF,
//   AD_REL, FL_ALL | AD_INDY, UNDF, UNDF, UNDF, FL_ALL | AD_ZPGX, FL_ZN | AD_ZPGX, UNDF, AD_IMP, FL_ALL | AD_ABSY, UNDF, UNDF, UNDF, FL_ALL | AD_ABSX, FL_ZN | AD_ABSX, UNDF
// };

const int cycles[] PROGMEM = { 7, 6, 1, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
                       6, 6, 1, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
                       6, 6, 1, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
                       6, 6, 1, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
                       0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, 
                       2, 6, 1, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, 
                       2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, 
                       2, 5, 1, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0, 
                       2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
                       2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
                       2, 5, 1, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0 };

// CPU registers
unsigned short PC;
unsigned short lastPC;
unsigned char STP = 0xFD, A = 0x00, X = 0x00, Y = 0x00, SR = SR_FIXED_BITS;

//Execution variables
unsigned char opcode, opflags;
unsigned short argument_addr;

//Temporary variables for flag generation
unsigned char value8;
unsigned short value16, value16_2, result;

bool debug = false;

void setflags() {
  // Mask out affected flags
  switch (opflags & 0xF0) {
    case  FL_ZN: SR &= 0x7D; break; // 1010 0000   0111 1101
    case FL_ZNC: SR &= 0x7C; break; // 1011 0000   0111 1100
    case  FL_ZC: SR &= 0xFC; break; // 0011 0000   1111 1100
    case FL_ALL: SR &= 0x3C; break; // 1111 0000   0011 1100
    case   FL_Z: SR &= 0xFD; break; // 0010 0000   1111 1101
  }
  
  // if (lastPC >= 0x5d00 && lastPC < 0x5e00) {
  //   sprintf(buf, "setflag: opflag=%02X result=%02X opflags & 0x80=%02X result & 0x80=%02X", opflags, result, opflags & 0x80, result & 0x80);
  //   printlog(buf);
  // }
  
  // Set various status flags
  if (opflags & FL_N) SR |= (result & 0x80);                    //negative
  if (opflags & FL_Z) SR |= (((result & 0xFF) == 0) ? 0x02 : 0);  //zero
  if (opflags & FL_C) SR |= ((result & 0xFF00) ? 0x01 : 0);       //carry
  if (opflags & FL_V) SR |= ((result ^ ((unsigned short)A)) & (result ^ value16) & 0x0080) >> 1; // overflow
}

// Stack functions
void push16(unsigned short pushval) {
  write8(STP_BASE + (STP--), (pushval >> 8) & 0xFF);
  write8(STP_BASE + (STP--), pushval & 0xFF);
}

void push8(unsigned char pushval) {
  write8(STP_BASE + (STP--), pushval);
}

unsigned short pull16() {
  value16 = read8(STP_BASE + (++STP)) | ((unsigned short)read8(STP_BASE + (++STP)) << 8);
  return value16;
}

unsigned char pull8() {
  return read8(STP_BASE + (++STP));
}

void cpuReset()
{
  PC = read16(0xFFFC);
  STP = 0xFD;
}


void run() {
  // Load the reset vector
  PC = read16(0xFFFC);
  STP = 0xFD;
  
  
  while (running) 
  {
    while (paused) 
    {
      delay(100);
    }

    opcode = read8(PC++);

    if (!Fast1MhzSpeed)
    {
      int cycleCount = cycles[opcode];
      cpuCycleCount = ESP.getCycleCount();
      uint32_t expectedDiff = 300;

      diffCpuCycleCount = cpuCycleCount - lastCpuCycleCount;
      while (diffCpuCycleCount < expectedDiff * cycleCount)  
      {
        cpuCycleCount = ESP.getCycleCount();
        diffCpuCycleCount = cpuCycleCount - lastCpuCycleCount;
      }

      lastCpuCycleCount = cpuCycleCount;
    }
    
    if (joystick) processJoystick(0.4);

    lastPC = PC;

    opflags = flags[opcode];

    
    // Addressing modes
    switch (opflags & 0x0F) {
      case AD_IMP:
      case AD_A: argument_addr = 0xFFFF; break;
      case AD_ABS:
        argument_addr = read16(PC);
        PC += 2;
        break;
      case AD_ABSX:
        argument_addr = read16(PC) + (unsigned short)X;
        PC += 2;
        break;
      case AD_ABSY:
        argument_addr = read16(PC) + (unsigned short)Y;
        PC += 2;
        break;
      case AD_IMM:
        argument_addr = PC++;
        break;
      case AD_IND:
        argument_addr = read16(PC);
        value16 = (argument_addr & 0xFF00) | ((argument_addr + 1) & 0x00FF);  // Page wrap
        argument_addr = (unsigned short)read8(argument_addr) | ((unsigned short)read8(value16) << 8);
        PC += 2;
        break;
      case AD_INDX:
        argument_addr = ((unsigned short)read8(PC++) + (unsigned short)X) & 0xFF;
        value16 = (argument_addr & 0xFF00) | ((argument_addr + 1) & 0x00FF);  // Page wrap
        argument_addr = (unsigned short)read8(argument_addr) | ((unsigned short)read8(value16) << 8);
        break;
      case AD_INDY:
        argument_addr = (unsigned short)read8(PC++);
        value16 = (argument_addr & 0xFF00) | ((argument_addr + 1) & 0x00FF);  // Page wrap
        argument_addr = (unsigned short)read8(argument_addr) | ((unsigned short)read8(value16) << 8);
        argument_addr += Y;
        break;
      case AD_REL:
        argument_addr = (unsigned short)read8(PC++);
        argument_addr |= ((argument_addr & 0x80) ? 0xFF00 : 0);
        break;
      case AD_ZPG:
        argument_addr = (unsigned short)read8(PC++);
        break;
      case AD_ZPGX:
        argument_addr = ((unsigned short)read8(PC++) + (unsigned short)X) & 0xFF;
        break;
      case AD_ZPGY:
        argument_addr = ((unsigned short)read8(PC++) + (unsigned short)Y) & 0xFF;
        break;
    }

    
    // if (lastPC == 0x800 && argument_addr == 0xbf00)
    //   debug = true;

    // if (debug) {
    // //if (lastPC >= 0x800 && lastPC < 0x900) {
    //   printCPUStatus();
    // }
    
    //opcodes
    switch (opcode) {
      //ADC
      case 0x69:
      case 0x65:
      case 0x75:
      case 0x6D:
      case 0x7D:
      case 0x79:
      case 0x61:
      case 0x71:
      case 0x72: //e
        value16 = (unsigned short)read8(argument_addr);
        if (SR & SR_DEC) { // Decimal
          result = (unsigned short)(A & 0x0F) + (unsigned short)(value16 & 0x0f) + (SR & 0x01 > 0);
          if (result > 0x09)
            result += 0x06;
          if (result <= 0x0F)
            result = (unsigned short)(result & 0x0F) + (unsigned short)(A & 0xF0) + (unsigned short)(value16 & 0xF0);
          else
            result = (unsigned short)(result & 0x0F) + (unsigned short)(A & 0xF0) + (unsigned short)(value16 & 0xF0) + 0x10;
          
          if (result == 0) // Zero Flag
            SR |= 0x02;
          else
            SR &= 0xfd;
          
          if (result < 0x80) // Negative
            SR |= 0x80;
          else
            SR &= 0x7f;
          
          if ((((A ^ result) & 0x80) > 0) && !(((A ^ result) & 0x80) > 0)) // Overflow
            SR |= 0x40;
          else
            SR &= 0xbf;

          if ((result & 0x1F0) > 0x90)
            result += 0x60;

          if ((result & 0xFF0) > 0xF0) // Carry
            SR |= 0x01;
          else
            SR &= 0xfe;
        }
        else
        { // Binary
          result = (unsigned short)A + value16 + (unsigned short)(SR & SR_CARRY);
        }
        setflags();
        A = result & 0xFF;
        break;
      //AND
      case 0x29:
      case 0x25:
      case 0x35:
      case 0x2D:
      case 0x3D:
      case 0x39:
      case 0x21:
      case 0x31:
      case 0x32: //e
        result = A & read8(argument_addr);
        A = result & 0xFF;
        setflags();
        break;
      //ASL A
      case 0x0A:
        value16 = (unsigned short)A;
        result = value16 << 1;
        setflags();
        A = result & 0xFF;
        break;
      //ASL
      case 0x06:
      case 0x16:
      case 0x0E:
      case 0x1E:
        value16 = read8(argument_addr);
        result = value16 << 1;
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //BCC
      case 0x90:
        if (!(SR & SR_CARRY)) PC += argument_addr;
        break;
      //BCS
      case 0xB0:
        if ((SR & SR_CARRY)) PC += argument_addr;
        break;
      //BEQ
      case 0xF0:
        if ((SR & SR_ZERO)) PC += argument_addr;
        break;
      //BNE
      case 0xD0:
        if (!(SR & SR_ZERO)) PC += argument_addr;
        break;
      //BIT
      case 0x24:
      case 0x2C:
      case 0x89: //e
      case 0x34: //e
      case 0x3C: //e
        value8 = read8(argument_addr);
        result = A & value8;
        setflags();
        SR = (SR & 0x3F) | (value8 & 0xC0);
        break;
      //BMI
      case 0x30:
        if ((SR & SR_NEG)) PC += argument_addr;
        break;
      //BPL
      case 0x10:
        if (!(SR & SR_NEG)) PC += argument_addr;
        break;
      //BRK
      case 0x00:
        PC++;
        push16(PC);
        push8(SR | SR_BRK);
        SR |= SR_INT;
        PC = read16(0xFFFE);
        SR &= 0xF7; //e
        break;
      //BRA
      case 0x80: //e
        PC += argument_addr;
        break;
      //BVC
      case 0x50:
        if (!(SR & SR_OVER)) PC += argument_addr;
        break;
      //BVS
      case 0x70:
        if (SR & SR_OVER) PC += argument_addr;
        break;
      //CLC
      case 0x18:
        SR &= 0xFE;
        break;
      //CLD
      case 0xD8:
        SR &= 0xF7;
        break;
      //CLI
      case 0x58:
        SR &= 0xFB;
        break;
      //CLV
      case 0xB8:
        SR &= 0xBF;
        break;
      //CMP
      case 0xC9:
      case 0xC5:
      case 0xD5:
      case 0xCD:
      case 0xDD:
      case 0xD9:
      case 0xC1:
      case 0xD1:
      case 0xD2: //e
        value16 = ((unsigned short)read8(argument_addr)) ^ 0x00FF;
        result = (unsigned short)A + value16 + (unsigned short)1;
        setflags();
        break;
      //CPX
      case 0xE0:
      case 0xE4:
      case 0xEC:
        value16 = ((unsigned short)read8(argument_addr)) ^ 0x00FF;
        result = (unsigned short)X + value16 + (unsigned short)1;
        setflags();
        break;
      //CPY
      case 0xC0:
      case 0xC4:
      case 0xCC:
        value16 = ((unsigned short)read8(argument_addr)) ^ 0x00FF;
        result = (unsigned short)Y + value16 + (unsigned short)1;
        setflags();
        break;
      //DEC
      case 0xC6:
      case 0xD6:
      case 0xCE:
      case 0xDE:
      case 0x3A: //e
        value16 = (unsigned short)read8(argument_addr);
        result = value16 - 1;
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //DEX
      case 0xCA:
        result = --X;
        setflags();
        break;
      //DEY
      case 0x88:
        result = --Y;
        setflags();
        break;
      //EOR
      case 0x49:
      case 0x45:
      case 0x55:
      case 0x4D:
      case 0x5D:
      case 0x59:
      case 0x41:
      case 0x51:
      case 0x52: //e
        value8 = read8(argument_addr);
        result = A ^ value8;
        setflags();
        A = result & 0xFF;
        break;
      //INC
      case 0xE6:
      case 0xF6:
      case 0xEE:
      case 0xFE:
      case 0x1A: //e
        value16 = (unsigned short)read8(argument_addr);
        result = value16 + 1;
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //INX
      case 0xE8:
        result = ++X;
        setflags();
        break;
      //INY
      case 0xC8:
        result = ++Y;
        setflags();
        break;
      //JMP
      case 0x4C:
      case 0x6C:
      case 0x7C: //e
        PC = argument_addr;
        break;
      //JSR
      case 0x20:
        push16(PC - 1);
        PC = argument_addr;
        break;
      //LDA
      case 0xA9:
      case 0xA5:
      case 0xB5:
      case 0xAD:
      case 0xBD:
      case 0xB9:
      case 0xA1:
      case 0xB1:
      case 0xB2: //e
        A = read8(argument_addr);
        result = A;
        setflags();
        break;
      //LDX
      case 0xA2:
      case 0xA6:
      case 0xB6:
      case 0xAE:
      case 0xBE:
        X = read8(argument_addr);
        result = X;
        setflags();
        break;
      //LDY
      case 0xA0:
      case 0xA4:
      case 0xB4:
      case 0xAC:
      case 0xBC:
        Y = read8(argument_addr);
        result = Y;
        setflags();
        break;
      //LSR A
      case 0x4A:
        value8 = A;
        result = value8 >> 1;
        result |= (value8 & 0x1) ? 0x8000 : 0;
        setflags();
        A = result & 0xFF;
        break;
      //LSR
      case 0x46:
      case 0x56:
      case 0x4E:
      case 0x5E:
        value8 = read8(argument_addr);
        result = value8 >> 1;
        result |= (value8 & 0x1) ? 0x8000 : 0;
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //NOP
      case 0xEA:
        break;
      //ORA
      case 0x09:
      case 0x05:
      case 0x15:
      case 0x0D:
      case 0x1D:
      case 0x19:
      case 0x01:
      case 0x11:
      case 0x12: //e
        value8 = read8(argument_addr);
        result = value8 | A;
        setflags();
        if ((result & 0x80) == 0x80)
          SR |= 0x80;
        else
          SR &= 0x7f;
        if ((result & 0xFF) == 0)
          SR |= 0x02;
        else
          SR &= 0xfd;
        A = result;
        break;
      //PHA
      case 0x48:
        push8(A);
        break;
      //PHX
      case 0xDA: //e
        push8(X);
        break;
      //PHY
      case 0x5A: //e
        push8(Y);
        break;
      //PHP
      case 0x08:
        push8(SR | SR_BRK);
        break;
      //PLA
      case 0x68:
        result = pull8();
        setflags();
        A = result;
        break;
      //PLX
      case 0xFA: //e
        result = pull8();
        setflags();
        X = result;
        break;
      //PLY
      case 0x7A: //e
        result = pull8();
        setflags();
        Y = result;
        break;
        //PLP
      case 0x28:
        SR = pull8() | SR_FIXED_BITS;
        break;
      //ROL A
      case 0x2A:
        value16 = (unsigned short)A;
        result = (value16 << 1) | (SR & SR_CARRY);
        setflags();
        A = result & 0xFF;
        break;
      //ROL
      case 0x26:
      case 0x36:
      case 0x2E:
      case 0x3E:
        value16 = (unsigned short)read8(argument_addr);
        result = (value16 << 1) | (SR & SR_CARRY);
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //ROR A
      case 0x6A:
        value16 = (unsigned short)A;
        result = (value16 >> 1) | ((SR & SR_CARRY) << 7);
        result |= (value16 & 0x1) ? 0x8000 : 0;
        setflags();
        A = result & 0xFF;
        break;
      //ROR
      case 0x66:
      case 0x76:
      case 0x6E:
      case 0x7E:
        value16 = (unsigned short)read8(argument_addr);
        result = (value16 >> 1) | ((SR & SR_CARRY) << 7);
        result |= (value16 & 0x1) ? 0x8000 : 0;
        setflags();
        write8(argument_addr, result & 0xFF);
        break;
      //RTI
      case 0x40:
        SR = pull8();
        PC = pull16();
        break;
      //RTS
      case 0x60:
        PC = pull16() + 1;
        break;
      //SBC
      case 0xE9:
      case 0xE5:
      case 0xF5:
      case 0xED:
      case 0xFD:
      case 0xF9:
      case 0xE1:
      case 0xF1:
      case 0xF2:
        if (SR & SR_DEC) { // Decimal
          value16 = (unsigned short)read8(argument_addr);
          unsigned short value2 = (unsigned short)(A - value16 - (!(SR & 0x01 > 0)));
          result = (unsigned short)((unsigned short)(A & 0x0F) - (unsigned short)(value16 & 0x0F) - (unsigned short)(!(SR & 0x01 > 0)));
          if ((result & 0x10) > 0) 
              result = ((result - 0x06) & 0x0F) | ((A & 0xF0) - (value16 & 0xF0) - 0x10);
          else
              result = (result & 0x0F) | ((A & 0xF0) - (value16 & 0xF0));
          if ((result & 0x100) > 0)
              result -= 0x60;

          if ((unsigned short)value2 < (unsigned short)0x0100) // carry
            SR |= 0x01;
          else
            SR &= 0xfe;
          if (((value2 & 0xFF) & 0x80) > 0) // Negative
            SR |= 0x80;
          else
            SR &= 0x7f;
          if (!(((value2 & 0xFF) & 0xFF) > 0)) // Zero
            SR |= 0x02;
          else
            SR &= 0xfd;
          if ((((A ^ value2) & 0x80) > 0) && (((A ^ value16) & 0x80) > 0)) // overflow
            SR |= 0x40;
          else
            SR &= 0xbf;
        }
        else {
          value16 = ((unsigned short)read8(argument_addr)) ^ 0x00FF;
          result = (unsigned short)A + value16 + (unsigned short)(SR & SR_CARRY);
          setflags();
        }
        A = result & 0xFF;
        break;
      //SEC
      case 0x38:
        SR |= SR_CARRY;
        break;
      //SED
      case 0xF8:
        SR |= SR_DEC;
        break;
      //SEI
      case 0x78:
        SR |= SR_INT;
        break;
      //STA
      case 0x85:
      case 0x95:
      case 0x8D:
      case 0x9D:
      case 0x99:
      case 0x81:
      case 0x91:
      case 0x92:
        write8(argument_addr, A);
        break;
      //STX
      case 0x86:
      case 0x96:
      case 0x8E:
        write8(argument_addr, X);
        break;
      //STY
      case 0x84:
      case 0x94:
      case 0x8C:
        write8(argument_addr, Y);
        break;
      //STZ
      case 0x64:
      case 0x74:
      case 0x9C:
      case 0x9E:
        write8(argument_addr, 0);
        break;
      //TAX
      case 0xAA:
        X = A;
        result = A;
        setflags();
        break;
      //TAY
      case 0xA8:
        Y = A;
        result = A;
        setflags();
        break;
      //TSX
      case 0xBA:
        X = STP;
        result = STP;
        setflags();
        break;
      //TXA
      case 0x8A:
        A = X;
        result = X;
        setflags();
        break;
      //TXS
      case 0x9A:
        STP = X;
        result = X;
        setflags();
        break;
      //TYA
      case 0x98:
        A = Y;
        result = Y;
        setflags();
        break;
      //TRB
      case 0x14:
      case 0x1C:
        value8 = read8(argument_addr);
        result = value8 & A;
        value8 = (char)(value8 & ~A);
        write8(argument_addr, value8);
        setflags();
        break;
      //TSB
      case 0x04:
      case 0x0C:
        value8 = read8(argument_addr);
        result = value8 & A;
        value8 = (char)(value8 | A);
        write8(argument_addr, value8);
        setflags();
        break;
    }
    
  }
}

void printCPUStatus() {
  char sFlags[8]; 
      for (int f = 0;f<8;f++) {
        sFlags[7-f] = (SR & (1 << f)) != 0 ? '1' : '0';
      }
      sprintf(buf, "[PC]%04X: %02X ,[Addr]%04X(%02X): A=%02X X=%02X Y=%02X FL=%02X(%s) OPFlag=%02X, cycleCount=%d, diffCycleCount=%d", lastPC, opcode, argument_addr, read8(argument_addr), A, X, Y, SR, sFlags, opflags, cycleCount, diffCpuCycleCount);
      printlog(buf);
}