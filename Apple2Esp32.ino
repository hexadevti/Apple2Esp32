#include <TFT_eSPI.h>
#include "rom.h"
#include "FS.h"
#include "SD.h"
#include <string>
#include <vector>
#include <EEPROM.h>
#include <bitset>
#include <algorithm>
#include <array>
#include <cmath>
#include <queue>
#include <mutex>
#include <thread>

// Keyboard Pins
const int DataPin = 21;
const int IRQpin = 22;

// SD Pins
int sck = 18;
int miso = 19;
int mosi = 23;
int cs = 5;

#define RED_LED_PIN 4
#define GREEN_LED_PIN 17
#define BLUE_LED_PIN 16
#define JOY_MAX 20000
#define JOY_MID 1230
#define JOY_MIN 10

#define FSTYPE SD 

#define HdDiskEEPROMaddress 0
#define IIpIIeEEPROMaddress 1
#define Fast1MhzSpeedEEPROMaddress 2
#define JoystickEEPROMaddress 3
#define VideoColorEEPROMaddress 4
#define NewDeviceConfigEEPROMaddress 50
#define DiskFileNameEEPROMaddress 128
#define HdFileNameEEPROMaddress 256
#define EEPROM_SIZE 1024

String selectedDiskFileName;
String selectedHdFileName;
String NewDeviceConfig;
byte selectedHdFile;
int firstShowFile = 0;
int shownFile;


bool running = true;
bool paused = false;
bool AppleIIe = false;
bool OptionsWindow = false;
bool HdDisk = false;
bool Fast1MhzSpeed = false;
bool joystick = true;
size_t content_len;

//VGA6Bit vga;
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;
TFT_eSPI tft = TFT_eSPI();

char buf[0xff];
int logLineCount = 1;
bool diskAttached = true;
bool hdAttached = false;
bool serialVideoAttached = false;
bool serialKeyboardAttached = false;
bool videoColor = true;



int margin_x = 20;
int margin_y = 24;
int text_margin_x = 2;
int text_margin_y = 3;

bool Graphics_Text;
bool Page1_Page2;
bool DisplayFull_Split;
bool LoRes_HiRes;
bool Cols40_80;
bool SoundClick;
bool lock_video = false;

bool IntCXRomOn_Off = false;
bool IntC8RomOn_Off = false;
bool AltCharSetOn_Off = false;
bool SlotC3RomOn_Off = false;
bool Store80On_Off = false;
bool Vertical_blankingOn_Off = false;
bool RAMReadOn_Off = false;
bool RAMWriteOn_Off = false;
bool AltZPOn_Off = false;
bool IOUDisOn_Off = false;
bool DHiResOn_Off = false;
bool IIEMemoryBankBankSelect1_2 = true;
bool IIEMemoryBankReadRAM_ROM = false;
bool IIEMemoryBankWriteRAM_NoWrite = false;
bool MemoryBankBankSelect1_2 = true;
bool MemoryBankReadRAM_ROM = false;
bool MemoryBankWriteRAM_NoWrite = false;


int IIeExpansionCardBank = 0;

std::vector<std::string> hdFiles;
std::vector<std::string> diskFiles;
std::mutex page_lock;

static unsigned char zp[0x200];
static unsigned char auxzp[0x200];
static unsigned char* ram;
static unsigned char* auxram;
static unsigned char* memoryBankSwitchedRAM1;
static unsigned char* memoryBankSwitchedRAM2_1;
static unsigned char* memoryBankSwitchedRAM2_2;
static unsigned char* IIEAuxBankSwitchedRAM1;
static unsigned char* IIEAuxBankSwitchedRAM2_1;
static unsigned char* IIEAuxBankSwitchedRAM2_2;
static unsigned char* IIEmemoryBankSwitchedRAM1;
static unsigned char* IIEmemoryBankSwitchedRAM2_1;
static unsigned char* IIEmemoryBankSwitchedRAM2_2;
static bool diskUnitNumber1_2 = true;
static bool DrivePhase0ON_OFF;
static bool DrivePhase1ON_OFF;
static bool DrivePhase2ON_OFF;
static bool DrivePhase3ON_OFF;
static bool FlagDO_PO;
static bool DriveQ6H_L;
static bool DriveQ7H_L;
static bool DriveMotorON_OFF;


static bool CgReset0 = false;
static bool CgReset1 = false;
static bool CgReset2 = false;
static bool CgReset3 = false;
static bool Cg0 = false;
static bool Cg1 = false;
static bool Cg2 = false;
static bool Cg3 = false;
static float timerpdl0 = 0;
static float timerpdl1 = 0;
static float timerpdl2 = 0;
static float timerpdl3 = 0;
static bool Pb0 = false;
static bool Pb1 = false;
static bool Pb2 = false;

void setup() {
  logSetup();

  pinMode(RED_LED_PIN,INPUT);
  pinMode(GREEN_LED_PIN,OUTPUT);
  pinMode(BLUE_LED_PIN,INPUT);
  
  epromSetup();
  diskAttached = (HdDisk == 0);
  hdAttached = !diskAttached;
  videoSetup();

  memoryAlloc(); 

  joystick = true;
  SDCardSetup();
  //serialVideoSetup();
  keyboard_begin();
  sei();
  HDSetup();
  diskSetup();
  // hdAttached = false;
  // diskAttached = false;

  speaker_begin();
  //analogJoystickSetup();
  printlog("Ready.");
  setCpuFrequencyMhz(240);

  if (joystick)
  {
    timerpdl0 = JOY_MID;
    timerpdl1 = JOY_MID;
  }
  else
  {
    timerpdl0 = JOY_MAX;
    timerpdl1 = JOY_MAX;
  }
  
  
  
}

void printlog(String txt) {
  if (serialVideoAttached) {
    if (logLineCount > 24)
      logLineCount = 1;
    Serial.print("\e[");
    Serial.print(logLineCount);
    Serial.print(";");
    Serial.print(41);
    Serial.print("H");
    Serial.print(txt.c_str());
    logLineCount++;
  } else
    Serial.println(txt.c_str());
}


void changeHdDisk() {
  HdDisk = !HdDisk;
  
}

void changeIIpIIe() {
  AppleIIe = !AppleIIe;
}

void fast1MhzSpeed() {
  Fast1MhzSpeed = !Fast1MhzSpeed;
}

void pauseRunning() {
  paused = !paused;
}

void joystickOnOff() {
  joystick = !joystick;
}

void showHideOptionsWindow() {
  OptionsWindow = !OptionsWindow;
  updateOptions(true);
}

void updateOptions(bool downDirection) {
  if (OptionsWindow) {
    printOptionsBackground(0xff0000);
    std::string result = "";
    int sel = 0;
    int skip = 0;
    // vga.fillRect(42, 42, 236, 147, 0);
    // vga.setCursor(44, 44);
    std::vector<std::string> files;
    if (!HdDisk) 
    {
      files = diskFiles;
    }
    else
    {
      files = hdFiles;
    }
    if (shownFile > files.size())
      shownFile = 0;
    if (downDirection) {
      if (shownFile >= firstShowFile + 17) {
        firstShowFile = shownFile - 17;
      }
    }
    else
    {
      if (shownFile < firstShowFile && firstShowFile > 0)
        firstShowFile--;
    }
    
    int shown = 0;
    // sprintf(buf, "sel: %d, firstShowFile: %d, shownFile: %d", sel, firstShowFile, shownFile);
    // Serial.println(buf);
    int id = 0;
    for (auto &&i : files)
    {
      if (id < firstShowFile)
      {
        id++;
        continue;
      }
      if (shown > 17)
        break;        
      //if (id == shownFile)
        // vga.setTextColor(// vga.RGB(0), // vga.RGB(0xffffff));
      //else
        // vga.setTextColor(// vga.RGB(0xffffff), // vga.RGB(0));
      if (i.size() > 39)
        i = i.substr(0, 33) + "..." + i.substr(i.size()-3,3);  
      // vga.println(i.c_str());
      
      shown++;
      id++;
      
    }
    // sprintf(buf, "sel: %d, skip: %d, skiped: %d, shownFile: %d", sel, skip, skiped, shownFile);
    // Serial.println(buf);
  }
}
void loop() {
  cpuLoop();
}
