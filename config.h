#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include "SD.h"
#include <EEPROM.h>
#include "rom.h"

#include <string>
#include <bitset>
#include <algorithm>
#include <array>
#include <cmath>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

// WebServer/Wifi Config
const char* host = "apple2";
const char* ssid = "LUCIANO-ESCRITORIO";
const char* password = "lrbf246!";
const char* PARAM = "file";
AsyncWebServer server(80);
size_t content_len;
File file;
bool opened = false;
static String filelist = "";

static int freeSpace = 0;

// Video Config
TFT_eSPI tft = TFT_eSPI();
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;
static std::mutex page_lock;

int margin_x = 20;
int margin_y = 24;
uint16_t tx = 0, ty = 0; // To store the touch coordinates

// LittleFS
#define U_PART U_SPIFFS
#define FSTYPE SD 

// SD Config
std::vector<std::string> hdFiles;
std::vector<std::string> diskFiles;

// Board Pins
#define SD_SCK_PIN 18
#define SD_MISO_PIN 19
#define SD_MOSI_PIN 23
#define SD_CS_PIN 5
#define ANALOG_X_PIN 35
#define ANALOG_Y_PIN 4
#define DIGITAL_BUTTON1_PIN 34
#define DIGITAL_BUTTON2_PIN 16
//#define RED_LED_PIN 4
#define GREEN_LED_PIN 17
//#define BLUE_LED_PIN 16
#define SPEAKER_PIN 26
#define KEYBOARD_DATA_PIN 21
#define KEYBOARD_IRQ_PIN 22


// Joystick Config

#define JOY_MAX 20000
#define JOY_MID 1230
#define JOY_MIN 10
#define EEPROM_SIZE 1024

// Fetures default Config
static bool running = true;
static bool paused = false;
static bool sound = true;
static bool AppleIIe = true;
static bool OptionsWindow = false;
static bool initializedHdDisk = false;
static bool HdDisk = false;
static bool Fast1MhzSpeed = true;
static bool joystick = true;
static bool diskAttached = false;
static bool hdAttached = false;
static bool serialVideoAttached = false;
static bool serialKeyboardAttached = false;
static bool videoColor = true;
static bool wifiConnected = false;


// Log Config
char buf[0xff];
int logLineCount = 1;

// EPROM Config
#define HdDiskEEPROMaddress 0
#define IIpIIeEEPROMaddress 1
#define Fast1MhzSpeedEEPROMaddress 2
#define JoystickEEPROMaddress 3
#define VideoColorEEPROMaddress 4
#define SoundEEPROMaddress 5
#define NewDeviceConfigEEPROMaddress 50
#define DiskFileNameEEPROMaddress 128
#define HdFileNameEEPROMaddress 256
String selectedDiskFileName;
String selectedHdFileName;
String NewDeviceConfig;
byte selectedHdFile;
uint8_t firstShowFile = 0;
uint8_t shownFile = 0xff;

// Softswitches Config
static bool Graphics_Text = false;
static bool Page1_Page2 = true;
static bool DisplayFull_Split = true;
static bool LoRes_HiRes = true;
static bool Cols40_80 = true;
static bool lock_video = false;
static bool IntCXRomOn_Off = false;
static bool IntC8RomOn_Off = false;
static bool AltCharSetOn_Off = false;
static bool SlotC3RomOn_Off = false;
static bool Store80On_Off = false;
static bool Vertical_blankingOn_Off = false;
static bool RAMReadOn_Off = false;
static bool RAMWriteOn_Off = false;
static bool AltZPOn_Off = false;
static bool IOUDisOn_Off = true;
static bool DHiResOn_Off = false;
static bool IIEMemoryBankBankSelect1_2 = true;
static bool IIEMemoryBankReadRAM_ROM = false;
static bool IIEMemoryBankWriteRAM_NoWrite = false;
static uint8_t IIeExpansionCardBank = 0;
static bool MemoryBankBankSelect1_2 = true;
static bool MemoryBankReadRAM_ROM = false;
static bool MemoryBankWriteRAM_NoWrite = false;
static bool diskUnitNumber1_2 = true;
static bool DrivePhase0ON_OFF;
static bool DrivePhase1ON_OFF;
static bool DrivePhase2ON_OFF;
static bool DrivePhase3ON_OFF;
static bool FlagDO_PO;
static bool DriveQ6H_L;
static bool DriveQ7H_L;
static bool DriveMotorON_OFF;


// Memory Config
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
static unsigned char* menuScreen;

//Speaker Config

boolean speaker_state = false;

// Other Cofig
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

