#include "VGA.h"
#include "FONT_6x8.h"
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ESPmDNS.h>

const char* host = "apple2";
const char* ssid = "LUCIANO-ESCRITORIO";
const char* password = "lrbf246!";
String filelist = "";
int freeSpace = 0;
const char* PARAM = "file";
#define U_PART U_SPIFFS
bool wifiConnected = false;

AsyncWebServer server(80);
size_t content_len;
File file;
bool opened = false;

#include "rom.h"
#include "FS.h"
#include <LittleFS.h>
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

VGA vga;
const PinConfig pins(-1,-1,-1,16,17,  -1,-1,-1,-1,7,15,  -1,-1,-1,5,6,  13,14);
Mode mode = Mode::MODE_320x240x60;

// SD Pins
// int sck = 18;
// int miso = 19;
// int mosi = 23;
// int cs = 5;


#define JOY_MAX 20000
#define JOY_MID 1230
#define JOY_MIN 10
#define EEPROM_SIZE 1024
#define FSTYPE LittleFS

bool running = true;
bool paused = false;
bool AppleIIe = true;
bool OptionsWindow = false;
bool HdDisk = false;
bool Fast1MhzSpeed = true;
bool joystick = true;

char buf[0xff];
int logLineCount = 1;
bool diskAttached = false;
bool hdAttached = false;
bool serialVideoAttached = false;
bool serialKeyboardAttached = false;
bool videoColor = true;
int HdDiskEEPROMaddress = 10;
int IIpIIeEEPROMaddress = 11;
int Fast1MhzSpeedEEPROMaddress = 12;
int JoystickEEPROMaddress = 13;
int DiskFileNameEEPROMaddress = 512;
int HdFileNameEEPROMaddress = 768;
String selectedDiskFileName;
String selectedHdFileName;
byte selectedHdFile;
int firstShowFile = 0;
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
std::vector<std::string> hdFiles;
std::vector<std::string> diskFiles;
std::mutex page_lock;
//unsigned char zp[0x200];
static unsigned char ram[0xc000];
static unsigned char auxram[0xc000];
static unsigned char auxzp[0x200];

unsigned char IIEAuxBankSwitchedRAM1[0x2000];
static unsigned char IIEAuxBankSwitchedRAM2_1[0x1000];
static unsigned char IIEAuxBankSwitchedRAM2_2[0x1000];

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
  Serial.begin(115200);
  //pinMode(LED_BUILTIN,OUTPUT);
  EEPROM.begin(EEPROM_SIZE);


  // EEPROM.writeBool(HdDiskEEPROMaddress, true);
  // EEPROM.writeBool(HdDiskEEPROMaddress, false);
  // EEPROM.writeBool(IIpIIeEEPROMaddress, false);
  // EEPROM.writeBool(Fast1MhzSpeedEEPROMaddress, true);
  // EEPROM.writeBool(JoystickEEPROMaddress, true);
  // writeStringToEEPROM(HdFileNameEEPROMaddress, "");
  // writeStringToEEPROM(DiskFileNameEEPROMaddress, "/karateka.dsk");


  HdDisk = EEPROM.readBool(HdDiskEEPROMaddress);
  AppleIIe = EEPROM.readBool(IIpIIeEEPROMaddress);
  Fast1MhzSpeed = EEPROM.readBool(Fast1MhzSpeedEEPROMaddress);
  joystick = EEPROM.readBool(JoystickEEPROMaddress);

  if (HdDisk) {
    int size = readStringFromEEPROM(HdFileNameEEPROMaddress, &selectedHdFileName);
    sprintf(buf, "EEPROM selectedHdFile value: %s", selectedHdFileName.c_str());
    printlog(buf);
  } else {
    int size = readStringFromEEPROM(DiskFileNameEEPROMaddress, &selectedDiskFileName);
    sprintf(buf, "EEPROM selectedDiskFileName value: %s", selectedDiskFileName.c_str());
    printlog(buf);
  }

  diskAttached = (HdDisk == 0);
  hdAttached = !diskAttached;
  videoSetup();
  //SDCardSetup();
  //serialVideoSetup();
  keyboard_begin();
  sei();
  if (HdDisk) {
    HDSetup();
  }
  else
  {
    DiskSetup();
  }
  
  //speaker_begin();
  printlog("Ready.");
  setCpuFrequencyMhz(240);
  wifiSetup();

  

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
  
  //   char a;
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
  printMsg("APPLE2ESP32S3 - Hit Ctrl-ESC to menu", 0xff, 0xff, 0xff);
  
  xTaskCreate(InfoMessage, "InfoMessage", 4096, NULL, 1, NULL);

  sprintf(buf, "Total heap: %d", ESP.getHeapSize());
  printlog(buf);
  sprintf(buf, "Free heap: %d", ESP.getFreeHeap());
  printlog(buf);
  sprintf(buf, "Total PSRAM: %d", ESP.getPsramSize());
  printlog(buf);
  sprintf(buf, "Free PSRAM: %d", ESP.getFreePsram());
  printlog(buf);
}

void InfoMessage(void *pvParameters) {
  delay(5000);
  vga.fillRect(0, 0, 300, 20, 0);
  vTaskDelete(NULL);
}

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; 
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
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

void saveEEPROM() {
  EEPROM.writeBool(HdDiskEEPROMaddress, HdDisk);
  EEPROM.writeBool(IIpIIeEEPROMaddress, AppleIIe);
  EEPROM.writeBool(Fast1MhzSpeedEEPROMaddress, Fast1MhzSpeed);
  EEPROM.writeBool(JoystickEEPROMaddress, joystick);
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
  updateOptions(true, OptionsWindow);
}

void updateOptions(bool downDirection, bool reload) {
  if (OptionsWindow) {
    if (reload) {
      if (HdDisk)
        LoadHD();
      else
        LoadDisk();
    }
    printOptionsBackground(0xff, 0, 0);
    std::string result = "";
    int sel = 0;
    int skip = 0;
    vga.fillRect(42, 42, 236, 147, 0);
    vga.setCursor(44, 44);
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
      
      if (id == shownFile)
        vga.setTextColor(vga.rgb(0,0,0), vga.rgb(0xff, 0xff, 0xff));
      else
        vga.setTextColor(vga.rgb(0xff, 0xff, 0xff), vga.rgb(0,0,0));

      if (i.size() > 39)
        i = i.substr(0, 33) + "..." + i.substr(i.size()-3,3);  
        vga.println(i.c_str());
      
      shown++;
      id++;
      
    }
    
    // sprintf(buf, "sel: %d, skip: %d, skiped: %d, shownFile: %d", sel, skip, skiped, shownFile);
    // Serial.println(buf);
  }
  else
  {
    vga.fillRect(0, 230, 300, 9, 0);
    vga.fillRect(0, 0, 300, 20, 0);
  }
  
}
void loop() {
  run();
}
