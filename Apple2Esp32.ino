#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/Font6x8.h>
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

// VGA Pins
const int hsyncPin = 32;
const int vsyncPin = 33;
const int red0pin = 12;
const int red1pin = 13;
const int green0pin = 27;
const int green1pin = 14;
const int blue0pin = 25;
const int blue1pin = 26;
// Keyboard Pins
const int DataPin = 35;
const int IRQpin = 34;
// SD Pins
int sck = 18;
int miso = 19;
int mosi = 23;
int cs = 5;

#define LED_PIN 2

bool running = true;
bool paused = false;

VGA6Bit vga;
char buf[0xff];
int logLineCount = 1;
bool diskAttached = true;
bool hdAttached = false;
bool serialVideoAttached = false;
bool serialKeyboardAttached = false;
bool videoColor = true;
#define EEPROM_SIZE 512
int selectedDiskFileEEPROMaddress = 0;
int selectedHdFileEEPROMaddress = 1;
int HdDiskEEPROMaddress = 10;
byte selectedDiskFile;
byte selectedHdFile;
bool HdDisk;
int shownFile;

int margin_x = 14;
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
bool AppleIIe = false;
bool OptionsWindow = false;

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
int IIeExpansionCardBank = 0;
std::mutex page_lock;
//unsigned char zp[0x200];
static unsigned char ram[0xc000];
static unsigned char auxram[0xc000];
//static unsigned char auxzp[0x200];

static unsigned char IIEAuxBankSwitchedRAM1[0x2000];
static unsigned char IIEAuxBankSwitchedRAM2_1[0x1000];
static unsigned char IIEAuxBankSwitchedRAM2_2[0x1000];



void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN,OUTPUT);
  EEPROM.begin(EEPROM_SIZE);
  HdDisk = EEPROM.readBool(HdDiskEEPROMaddress);
  if (HdDisk) {
    selectedHdFile = EEPROM.readByte(selectedHdFileEEPROMaddress);
    shownFile = selectedHdFile;
    sprintf(buf, "EEPROM selectedHdFile value: %x", selectedHdFile);
    printlog(buf);
  } else {
    selectedDiskFile = EEPROM.readByte(selectedDiskFileEEPROMaddress);
    shownFile = selectedDiskFile;
    sprintf(buf, "EEPROM selectedDiskFile value: %x", selectedDiskFile);
    printlog(buf);
  }

  sprintf(buf, "EEPROM HdDisk value: %x", HdDisk);
  printlog(buf);
  diskAttached = (HdDisk == 0);
  hdAttached = !diskAttached;
  videoSetup();

  SDCardSetup();
  serialVideoSetup();
  keyboard_begin();
  sei();
  HDSetup();
  DiskSetup();
  speaker_begin();
  printlog("Ready.");
  setCpuFrequencyMhz(240);
  sprintf(buf, "%s", HdDisk ? "HD" : "DISK");
  printStatus(buf, 0xff0000);
  char a;
  // for (int i = 0; i < 0x200; i++) {
  //   auxzp[i] = 0;
  //   a = auxzp[i];
  // }
  // for (int i = 0; i < 0xc000; i++) {

  //   auxram[i] = rand() % 0x100;
  // }
  // for (int i = 0; i < 0xc000; i++) {

  //   sprintf(buf, "%02x ", auxram[i]);
  //   Serial.print(buf);
  //   if (i % 0xf == 0)
  //     Serial.println();
  // }
  // for (int i = 0; i < 0x1000; i++) {
  //   IIEAuxBankSwitchedRAM2_1[i] = 0;
  //   IIEAuxBankSwitchedRAM2_2[i] = 0;
  //   a = IIEAuxBankSwitchedRAM2_1[i];
  //   a = IIEAuxBankSwitchedRAM2_2[i];
  // }
  // for (int i = 0; i < 0x2000; i++) {
  //   IIEAuxBankSwitchedRAM1[i] = 0;
  //   a = IIEAuxBankSwitchedRAM1[i];
  // }
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

void setHdDisk() {
  HdDisk = !HdDisk;
  sprintf(buf, "%s", HdDisk ? "HD" : "DISK");
  printStatus(buf, 0xff0000);
  EEPROM.writeBool(HdDiskEEPROMaddress, HdDisk);
  portDISABLE_INTERRUPTS();
  EEPROM.commit();
  portENABLE_INTERRUPTS();
}

void setOptions() {
  OptionsWindow = !OptionsWindow;
  if (OptionsWindow)
    printOptionsMsg("Ola!", 0xff0000);
}

void loop() {
  run();
}
