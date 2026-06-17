#if !defined(TFT)
#include "EspUsbHost.h"

volatile unsigned char keymem_hold = 0;
volatile unsigned char keycode_hold = 0;
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
    
    // printLog(control ? "Control" : "");
    // printLog(shift ? "shift" : "");
    // printLog(left_alt ? "left_alt" : "");
    // printLog(left_win ? "left_win" : "");
    // printLog(right_alt ? "right_alt" : "");
    // printLog(right_win ? "right_win" : "");
    // Serial.println();
    if (last_report.keycode[1] == report.keycode[0] && last_report.keycode[1] != 0) { // up first key
      onKeyUp(last_report.keycode[0]);
    } 
    else {
      if (report.keycode[1] != last_report.keycode[1]) {
        if (report.keycode[1] != 0)
          onKeyDown(report.keycode[1]);
        else 
          onKeyUp(last_report.keycode[1]);
      }
      else if (report.keycode[0] != last_report.keycode[0]) {
        if (report.keycode[0] != 0)
          onKeyDown(report.keycode[0]);
        else 
          onKeyUp(last_report.keycode[0]);
      }
    }
    if (last_report.modifier != report.modifier)
    {
      onModifierChange();
    }
  }

  void onModifierChange() {
    if (shift) {
      Pb0 = true;
    } else {
      Pb0 = false;
    }
    if (control) {
      Pb1 = true;
    } else {
      Pb1 = false;
    }
  }

  void onKeyDown(uint8_t keycode) {
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

  void onKeyUp(uint8_t keycode) {
    // Serial.printf("Up %d", keycode);
    // Serial.println();
    if (joystick) {
      if (keycode == 79 || keycode == 80) // Left || right
      {
        timerpdl0 = JOY_MID;
      }
      else if (keycode == 81 || keycode == 82) // Up || Down
      {
        timerpdl1 = JOY_MID;
      }
    }
    keymem = 0;
    keymem_hold = 0;
    keycode_hold = 0;


  }
  
  
  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) {
    onKeyboardKeyLocal(ascii, keycode, modifier);
  }

  
  void onReceive(const usb_transfer_t *transfer) {
    EspUsbHost *usbHost = (EspUsbHost *)transfer->context;
    endpoint_data_t *endpoint_data = &usbHost->endpoint_data_list[(transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK)];
    if (endpoint_data->bInterfaceProtocol == HID_ITF_PROTOCOL_MOUSE) {
        static uint8_t last_buttons = 0;
        hid_mouse_report_t report = {};
        report.buttons = transfer->data_buffer[1];
        report.x = (int8_t)transfer->data_buffer[2];
        report.y = (int8_t)transfer->data_buffer[3];
        
        if (report.x != 0 || report.y != 0 || report.buttons != last_buttons) {
          onMouseMoveLocal(report.x, report.y, report.buttons);
          last_buttons = report.buttons;
        }
      }
  };

};
MyEspUsbHost usbHost;


  void onKeyboardKeyLocal(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    // Serial.printf("ascii = %d", ascii);
    //Serial.printf(" keycode = %d", keycode);
    // Serial.println();
    // Serial.printf(" modifier = %d", modifier);
    // Serial.printf(" capslock = %d", capslock);
    bool shift_enabled = false;
      
    switch (keycode)
    {
      case 58: // F1
        HdDisk = !HdDisk;
        if (HdDisk) {
          firstShowFile = 0;
          xTaskCreate(loadHdAsync, "loadHdAsync", 4096, NULL, 2, NULL);
        }
        else {
          firstShowFile = 0;
          xTaskCreate(loadDiskAsync, "loadDiskAsync", 4096, NULL, 2, NULL);
        }
        optionsScreenRender();
        // Serial.println("f1");
        break;
      case 59: // F2
        AppleIIe = !AppleIIe;
        activeFlags = AppleIIe ? flagsIIe : flagsIIplus;
        optionsScreenRender();
        // Serial.println("f2");
        break;
      case 60: // F3
        Fast1MhzSpeed = !Fast1MhzSpeed;
        optionsScreenRender();
        break;
      case 61: // F4
        paused = !paused;
        optionsScreenRender();
        break;
      case 62: // F5
        joystick = !joystick;
        optionsScreenRender();
        break;
      case 63: // F6
        videoColor = !videoColor;
        optionsScreenRender();
        break;
      case 64: // F7
        upscale = !upscale;
        optionsScreenRender();
        clearScreen();
        keymem = 0;
        return;
        break;
      case 65: // F8
        smoothUpscale = !smoothUpscale;
        optionsScreenRender();
        clearScreen();
        keymem = 0;
        return;
        break;
    }

    if (OptionsWindow) // Option Window Opened
    {
      if (control) 
      {
        switch (keycode)
        {
          case 40: // CTRL-ENTER  
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
            keymem = 0;
            break;              
        }
      }
      else // without modifier key
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
        else if (keycode == 81) // Down Arrow
        {
            if (!HdDisk)
              nextDiskFile();
            else
              nextHdFile();
            listFiles(true);

          //Serial.println("down");
        }
        else if (keycode == 82) // Up Arrow
        {
          if (!HdDisk)
            prevDiskFile();
          else
            prevHdFile();
          listFiles(false);
          //Serial.println("up");
        }
      }
      keymem = 0;
    }
    else if (DebugWindow) {
      if (control) {
        switch (keycode)
        {
          case 58: // CTRL-F1
            debug = false;
            showHideDebugWindow();
            keymem = 0;
            break;
        }
      } 
      else 
      {
        switch (keycode)
        {
          case 62: // F5
            debugPaused = !debugPaused;
            delay(10);
            //debugStep = true;
            //printDebugLine();
            keymem = 0;
            break;
          case 67: // F10
            if (debugPaused) {
              debugStep = true;
              keycode_hold = keycode;
            }
            keymem = 0;
            break;
          case 41: // ESC
            debug = false;
            showHideDebugWindow();
            keymem = 0;
            break;
          default:
            if (keycode >= 30 && keycode < 40 || // 0-9
                keycode >= 4 && keycode < 10) { // A-Z
              setCursor(inputCursorX, inputCursorY);
              std::string s(1, char(keycode));
              Serial.printf("keycode = %d, ascii = %d, %s\n", keycode, ascii, s);
              // if (keycode >= 30 && keycode <= 40) { // 0-9
              //   print(reinterpret_cast<const char*>(keycode));
              // } else if (keycode >= 4 && keycode <= 10) { // A-Z
              //   print(reinterpret_cast<const char*>(keycode));
              // } 
              inputCursorX++;

            } 
            // else if (keycode == 42 || keycode == 80) {
            //   inputCursorX--;
            //   setCursor(inputCursorX, inputCursorY);
            // }
            // else if (keycode == 40) {
            //   setCursor(29, 5);
            //   uint16_t addr = getAddressValue();
            //   Serial.printf("debug address = 0x%04X\n", addr);
            //   if (addr > 0x0000 && addr <= 0xFFFF) {
            //     debugAddressBreak = addr;
            //     Serial.printf("Set breakpoint at address 0x%04X\n", debugAddressBreak);
            //   } else {
            //     Serial.println("Invalid address");
            //   }
            //   inputCursorX = 29;
            //   inputCursorY = 5;
            //   keymem = 0;
            // } 
            break;
            
        }
      }
    }
    else // All closed windows
    {
      if (control)
      {
        if (keycode >= 4 && keycode <= 29) {
          ascii = keycode - 3;
          keymem = ascii | 0x80;
        }
        else 
        {
          switch (keycode) {
            case 58: // CTRL-F1
              clearScreen();
              showHideDebugWindow();
              debug = true;
              debugScreenRender();
              keymem = 0;
              break;
            // case 60: // CTRL-F3
            //   clearScreen();
            //   colorDemo();
            //   demo = !demo;
            //   keymem = 0;
            //   break;
            case 62: // CTRL-F5
              ESP.restart();
              break;
            case 69: // CTRL-F12
              cpuReset();
              break;
            case 41: // CTRL-ESC
              showHideOptionsWindow();
              keymem = 0;
              break;
          }
        }

        
      } 
      else // without modifier key
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

  void onMouseMoveLocal(int8_t x, int8_t y, uint8_t buttons) {
    Serial.printf("buttons=0x%02x, x=%d, y=%d\n",
                  buttons,
                  x,
                  y);
    
    mouseX += abs(x) > 1 ? x/2 : 1;
    if (mouseX > 560) mouseX = 560;
    if (mouseX < 0) mouseX = 0;
    mouseY += abs(y) > 1 ? y/2 : 1;
    if (mouseY > 192) mouseY = 192;
    if (mouseY < 0) mouseY = 0;
    mouseButton = (buttons & 0x01) != 0; // Left button pressed
    Serial.printf("buttons=%s, x=%d, y=%d\n",
                  mouseButton ? "Pressed" : "Released",
                  mouseX,
                  mouseY);
  }

  


void keyboardSetup()
{
  usbHost.begin();
  usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);
  xTaskCreate(keyboardTask, "keyboardTask", 4096, NULL, 1, NULL);
}

void keyboardTask(void *pvParameters)
{
  int countKeymem = 0;
  int countKeycode = 0;
  int cyclesKeymem = 0;
  int cyclesKeycode = 0;
  bool holdKey = false;
  unsigned char repeat_keymem = 0;
  unsigned char repeat_keycode = 0;
  while (running)
  {
    usbHost.task();
    delay(1);
    repeat_keymem = keymem_hold;
    if (repeat_keymem != 0)
      countKeymem++;
    else
      countKeymem = 0;

    if (countKeymem >= 70) {
      if (cyclesKeymem == 0) {
        //Serial.println("RELEASE");
        keymem = 0;
      }
      cyclesKeymem++;
      if (cyclesKeymem >= 10) {
        //Serial.println("REPEAT");
        cyclesKeymem = 0;
        keymem = keymem_hold;
      }
    }



    repeat_keycode = keycode_hold;
    if (repeat_keycode != 0)
      countKeycode++;
    else
      countKeycode = 0;

    if (countKeycode >= 70) {
      cyclesKeycode++;
      if (cyclesKeycode >= 5) {
        cyclesKeycode = 0;
        onKeyboardKeyLocal(0, keycode_hold, 0);
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

void keyboardStrobe()
{
  keymem &= 0x7F;
  // Serial.printf(" keystrobe");
  // Serial.println();
}
#endif