#include "EspUsbHost.h"

volatile unsigned char keymem = 0;
volatile unsigned char keymem_hold = 0;
volatile bool capslock = true;
volatile bool control = false;
volatile bool shift = false;
volatile bool left_alt = false;
volatile bool left_win = false;
volatile bool right_alt = false;
volatile bool right_win = false;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

const unsigned char ascii_to_apple[] = {
    //$0    $1    $2    $3    $4    $5    $6    $7    $8    $9    $A    $B    $C    $D    $E    $F
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$10
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x7e, 0x26, 0x2a, 0x28, 0x5e, 0x43, 0x00, 0x00, 0x00, 0x3b, //$20
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x63, 0x00, 0x5f, 0x00, 0x3a, //$30
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$40
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x00, //$50
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$60
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, //$70
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$80 0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$90 1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$A0 2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$B0 3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$C0 4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$D0 5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //$E0 6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  //$F0 7
};

class MyEspUsbHost : public EspUsbHost
{
  void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
  {
    
    // Serial.printf("modifier=["BYTE_TO_BINARY_PATTERN"]->["BYTE_TO_BINARY_PATTERN"], Key0=[0x%02x]->[0x%02x], Key1=[0x%02x]->[0x%02x], Key2=[0x%02x]->[0x%02x], Key3=[0x%02x]->[0x%02x], Key4=[0x%02x]->[0x%02x], Key5=[0x%02x]->[0x%02x]",
    // BYTE_TO_BINARY(last_report.modifier),
    // BYTE_TO_BINARY(report.modifier),
    // last_report.keycode[0],
    // report.keycode[0],
    // last_report.keycode[1],
    // report.keycode[1],
    // last_report.keycode[2],
    // report.keycode[2],
    // last_report.keycode[3],
    // report.keycode[3],
    // last_report.keycode[4],
    // report.keycode[4],
    // last_report.keycode[5],
    // report.keycode[5]);
    // Serial.println();
  
    control = (report.modifier & 0b00010001) != 0;
    shift = (report.modifier & 0b00100010) != 0;
    left_alt = (report.modifier & 0b00000100) != 0;
    right_alt = (report.modifier & 0b01000000) != 0;
    left_win = (report.modifier & 0b00001000) != 0;
    right_win = (report.modifier & 0b10000000) != 0;
    
    // Serial.print(control ? "Control" : "");
    // Serial.print(shift ? "shift" : "");
    // Serial.print(left_alt ? "left_alt" : "");
    // Serial.print(left_win ? "left_win" : "");
    // Serial.print(right_alt ? "right_alt" : "");
    // Serial.print(right_win ? "right_win" : "");
    // Serial.println();
    if (last_report.keycode[1] == report.keycode[0] && last_report.keycode[1] != 0) { // up first key
      OnKeyUp(last_report.keycode[0]);
    } 
    else {
      if (report.keycode[1] != last_report.keycode[1]) {
        if (report.keycode[1] != 0)
          OnKeyDown(report.keycode[1]);
        else 
          OnKeyUp(last_report.keycode[1]);
      }
      else if (report.keycode[0] != last_report.keycode[0]) {
        if (report.keycode[0] != 0)
          OnKeyDown(report.keycode[0]);
        else 
          OnKeyUp(last_report.keycode[0]);
      }
    }
    if (last_report.modifier != report.modifier)
    {
      onModifierChange();
    }
  }

  void onModifierChange() {
    if (left_win) {
      Pb0 = true;
    } else {
      Pb0 = false;
    }
    if (right_win || left_alt) {
      Pb1 = true;
    } else {
      Pb1 = false;
    }
  }

  void OnKeyDown(uint8_t keycode) {
    // Serial.printf("Down %d", keycode);
    // Serial.println();
    if (joystick) {
      if (keycode == 80) // Left
      {
        timerpdl0 = JOY_MIN;
      }
      else if (keycode == 79) // Right
      {
        timerpdl0 = JOY_MAX;
      }
      else if (keycode == 82) // Up
      {
        timerpdl1 = JOY_MIN;
      }
      else if (keycode == 81) // Down
      {
        timerpdl1 = JOY_MAX;
      }
    }
  }

  void OnKeyUp(uint8_t keycode) {
    // Serial.printf("Up %d", keycode);
    // Serial.println();
    if (joystick) {
      if (keycode == 80 || keycode == 79) // Left || right
      {
        timerpdl0 = JOY_MID;
      }
      else if (keycode == 82 || keycode == 81) // Up || Down
      {
        timerpdl1 = JOY_MID;
      }
    }
    keymem = 0;
    keymem_hold = 0;

  }

  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    // Serial.printf("ascii = %d", ascii);
    // Serial.printf(" keycode = %d", keycode);
    // Serial.println();
    // Serial.printf(" modifier = %d", modifier);
    // Serial.printf(" capslock = %d", capslock);
    bool shift_enabled = false;
    
    if (control)
    {
      if (keycode >= 4 && keycode <= 29) {
        ascii = keycode - 3;
        keymem = ascii | 0x80;
      }
      else if (keycode == 69) // CTRL-F12
      {
        cpuReset();
      }
      else if (keycode == 62) // CTRL-F5
      {
        ESP.restart();
      }
      else if (keycode == 41) // CTRL-ESC
      {
        showHideOptionsWindow();
        keymem = 0;
      }
      else if (keycode == 40) // CTRL-ENTER
      {
        if (OptionsWindow) 
        {
          if (HdDisk)
            setHdFile();
          else
            setDiskFile();
          if (HdDisk)
            saveHdFile();
          else
            saveDiskFile();
          ESP.restart();
        }
      }
    }
    else
    {

      if (OptionsWindow) // Option Window Opened
      {
        if (keycode == 41) // ESC
        {
          showHideOptionsWindow();
        }
        else if (keycode == 40) // enter
        {
          if (HdDisk)
            setHdFile();
          else
            setDiskFile();
          diskChanged = true;
          showHideOptionsWindow();
        }
        else if (keycode == 58) // F1
        {
          changeHdDisk();
          updateOptions(true, true);
          // Serial.println("f1");
        }
        else if (keycode == 59) // F2
        {
          changeIIpIIe();
          updateOptions(true, false);
          // Serial.println("f2");
        }
        else if (keycode == 60) // F3
        {
          fast1MhzSpeed();
          updateOptions(true, false);
        }
        else if (keycode == 61) // F4
        {
          pauseRunning();
          updateOptions(true, false);
        }
        else if (keycode == 62) // F5
        {
          joystickOnOff();
          updateOptions(true, false);
        }
        else if (keycode == 81) // Down Arrow
        {
            if (!HdDisk)
              nextDiskFile();
            else
              nextHdFile();
            updateOptions(true, false);

          //Serial.println("down");
        }
        else if (keycode == 82) // Up Arrow
        {
          if (!HdDisk)
            prevDiskFile();
          else
            prevHdFile();
          updateOptions(false, false);
          //Serial.println("up");
        }

        keymem = 0;
      }
      else
      {
        if (ascii == 0)
        {
          if (keycode == 53)
            ascii = modifier == 1 ? 0x22 : 0x27;
          else if (keycode == 100)
            ascii = modifier == 1 ? 0x7c : 0x5c;
          else if (keycode == 39)
            ascii = modifier == 1 ? 0x29 : 0x00;
          else if (keycode == 80) // Left
          {
            ascii = 0x08;
          }
          else if (keycode == 79) // Right
          {
            ascii = 0x15;
          }
          else if (keycode == 82) // Up
          {
            ascii = 0x0b;
          }
          else if (keycode == 81) // Down
          {
            ascii = 0x0a;
          }
          else if (keycode == 57) // CapsLock
            capslock = !capslock;
        }
        else if (keycode == 135)
        {
          if (ascii == 100)
            ascii = 0x2f;
          else
            ascii = 0x3f;
        }
        else
        {
          if (ascii_to_apple[ascii] != 0)
            ascii = ascii_to_apple[ascii];
        }
        if (ascii >= 97 && ascii <= 122 && capslock)
        {
          ascii = ascii - 0x20;
        }
        keymem = ascii | 0x80; // scancode_to_apple[ascii + ((shift_enabled) ? 0x80 : 0x00)];
      }
    }
      // Serial.printf(" ascii processed = %d", ascii);

      // Serial.printf(" keymem = %d", keymem);
      // Serial.println();
      keymem_hold = keymem;
  };
};
MyEspUsbHost usbHost;

void keyboard_begin()
{
  usbHost.begin();
  usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);
  xTaskCreate(keyboard_task, "keyboard_task", 4096, NULL, 1, NULL);
}

void keyboard_task(void *pvParameters)
{
  int count = 0;
  int cycles = 0;
  bool holdKey = false;
  unsigned char repeat_keymem = 0;
  while (running)
  {
    
    usbHost.task();
    delay(10);
    repeat_keymem = keymem_hold;
    if (repeat_keymem != 0)
      count++;
    else
      count = 0;

    if (count >= 50) {
      if (cycles == 0) {
        //Serial.println("RELEASE");
        keymem = 0;
      }
      cycles++;
      if (cycles >= 5) {
        //Serial.println("REPEAT");
        cycles = 0;
        keymem = keymem_hold;
      }
    }


    // Serial.printf(" keymem = %d", keymem);
    // Serial.println();
  }
}

unsigned char keyboard_read()
{
  return keymem;
}

void keyboard_strobe()
{
  keymem &= 0x7F;
  // Serial.printf(" keystrobe");
  // Serial.println();
}