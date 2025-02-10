#include "EspUsbHost.h"

volatile unsigned char keymem = 0;
volatile bool capslock = true;
volatile bool control = false;

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
    Serial.printf("modifier %d ", report.modifier);
    control = report.modifier == 1;
  }

  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    // Serial.printf("ascii = %d", ascii);
    // Serial.printf(" keycode = %d", keycode);
    // Serial.printf(" modifier = %d", modifier);
    // Serial.printf(" capslock = %d", capslock);
    bool shift_enabled = false;
    
    if (control)
    {
      if (keycode >= 4 && keycode <= 29)
        ascii = keycode - 3;
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
          updateOptions(true);
          // Serial.println("f1");
        }
        else if (keycode == 59) // F2
        {
          changeIIpIIe();
          updateOptions(true);
          // Serial.println("f2");
        }
        else if (keycode == 60) // F3
        {
          fast1MhzSpeed();
          updateOptions(true);
        }
        else if (keycode == 61) // F4
        {
          pauseRunning();
          updateOptions(true);
        }
        else if (keycode == 62) // F5
        {
          joystickOnOff();
          updateOptions(true);
        }
        else if (keycode == 81) // Down Arrow
        {
            if (!HdDisk)
              nextDiskFile();
            else
              nextHdFile();
            updateOptions(true);

          //Serial.println("down");
        }
        else if (keycode == 82) // Up Arrow
        {
          if (!HdDisk)
            prevDiskFile();
          else
            prevHdFile();
          updateOptions(false);
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
          else if (keycode == 80)
            ascii = 0x08;
          else if (keycode == 79)
            ascii = 0x15;
          else if (keycode == 82)
            ascii = 0x0b;
          else if (keycode == 81)
            ascii = 0x0a;
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
  while (true)
  {
    usbHost.task();
    delay(100);
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

// // keyboard scan buffer
// unsigned short keyboard_data[3] = { 0, 0, 0 };
// unsigned char keyboard_buf_indx = 0, keyboard_mbyte = 0;
// boolean shift_enabled = false;
// boolean ctrl_enabled = false;

// // In apple II scancode format
// volatile unsigned char keymem = 0;

// unsigned char keyboard_read() {
//   return keymem;
// }

// void keyboard_strobe() {
//   keymem &= 0x7F;
// }

// void keyboard_begin() {
//   pinMode(DataPin, INPUT_PULLUP);
//   attachInterrupt(IRQpin, keyboard_bit, FALLING);
// }

// void keyboard_bit() {
//   if (digitalRead(DataPin))
//     keyboard_data[2] |= _BV(keyboard_buf_indx);
//   else
//     keyboard_data[2] &= ~(_BV(keyboard_buf_indx));
//   if (++keyboard_buf_indx == 11) {
//     // Ignore parity checks for now
//     keyboard_data[2] = (keyboard_data[2] >> 1) & 0xFF;
//     sprintf(buf, "initial keybdata: %02x", keyboard_data[2]);
//     //Serial.println(buf);
//     // extended keys
//     if (keyboard_data[2] == 0xF0 || keyboard_data[2] == 0xE0)
//       keyboard_mbyte = 1;
//     else
//     {
//       //decrement counter for multibyte commands
//       if (keyboard_mbyte)
//         keyboard_mbyte--;
//       // multibyte command is finished / normal command, process it
//       if (!keyboard_mbyte)
//       {
//         if (keyboard_data[1] != 0xF0 && keyboard_data[1] != 0xE0)
//         {
//           //Standard keys
//           if (keyboard_data[2] == 0x12 || keyboard_data[2] == 0x59)
//           {
//             shift_enabled = true;  //shift modifiers
//             Pb1 = true;
//           }
//           else if (keyboard_data[2] == 0x14)
//           {
//             ctrl_enabled = true;
//             Pb0 = true;
//           }
//           else
//           {
//             keymem = scancode_to_apple[keyboard_data[2] + ((shift_enabled) ? 0x80 : 0x00)];
//             if (ctrl_enabled)
//             {
//               if (keyboard_data[2] == 0x07)  // CTRL-F12
//               {
//                 cpuReset();
//               }
//               else if (keyboard_data[2] == 0x03) // CTRL-F5
//               {
//                 ESP.restart();
//               }
//               else if (keyboard_data[2] == 0x76) // CTRL-ESC
//               {
//                 showHideOptionsWindow();
//                 keymem = 0;
//               }
//               else if (keyboard_data[2] == 0x5a) // CTRL-ENTER
//               {
//                 if (HdDisk)
//                   setHdFile();
//                 else
//                   setDiskFile();
//                 if (HdDisk)
//                   saveHdFile();
//                 else
//                   saveDiskFile();
//                 ESP.restart();
//               }
//               else
//               {
//                 keymem -= 0x40;
//               }
//             }
//             else
//             {
//               if (OptionsWindow) // Option Window Opened
//               {
//                 if (keyboard_data[2] == 0x76) // ESC
//                 {
//                   showHideOptionsWindow();
//                 }
//                 else if (keyboard_data[2] == 0x5a) // enter
//                 {
//                   if (HdDisk)
//                     setHdFile();
//                   else
//                     setDiskFile();
//                   diskChanged = true;
//                   showHideOptionsWindow();

//                 }
//                 else if (keyboard_data[2] == 0x05) // F1
//                 {
//                   changeHdDisk();
//                   updateOptions(true);
//                   //Serial.println("f1");
//                 }
//                 else if (keyboard_data[2] == 0x06) // F2
//                 {
//                   changeIIpIIe();
//                   updateOptions(true);
//                   //Serial.println("f2");
//                 }
//                 else if (keyboard_data[2] == 0x04) // F3
//                 {
//                   fast1MhzSpeed();
//                   updateOptions(true);
//                 }
//                 else if (keyboard_data[2] == 0x0c) // F4
//                 {
//                   pauseRunning();
//                   updateOptions(true);
//                 }
//                 else if (keyboard_data[2] == 0x03) // F5
//                 {
//                   joystickOnOff();
//                   updateOptions(true);
//                 }

//                 keymem = 0;

//               }
//             }

//              Serial.print("keyboard_data:");
//              Serial.println(keyboard_data[2]);
//              Serial.print("shift:");
//              Serial.println((shift_enabled) ? "1" : "0");
//              Serial.print("ctrl:");
//              Serial.println((ctrl_enabled) ? "1" : "0");
//              Serial.print("key:");
//              Serial.println(keymem);
//           }
//         }
//         else if (keyboard_data[0] != 0xF0 && keyboard_data[1] == 0xE0)
//         {
//           //Extended keys
//           if (keyboard_data[2] == 0x6B) // LEFT ARROW
//           {
//             keymem = 0x88;  //back key
//             if (joystick) timerpdl0 = JOY_MIN;
//           }
//           else if (keyboard_data[2] == 0x74) // RIGHT ARROW
//           {
//             keymem = 0x95;  //forward key
//             if (joystick) timerpdl0 = JOY_MAX;
//           }
//           else if (keyboard_data[2] == 0x75) // UP ARROW
//           {
//             keymem = 0x8b;
//             if (joystick) timerpdl1 = JOY_MIN;
//           }
//           else if (keyboard_data[2] == 0x72) // DOWN ARROW
//           {
//             keymem = 0x8a;
//             if (joystick) timerpdl1 = JOY_MAX;
//           }

//           // Power management keys, hardware reset
//           if (OptionsWindow)
//           {
//             if (keyboard_data[2] == 0x72) // Down Arrow
//             {
//                 if (!HdDisk)
//                   nextDiskFile();
//                 else
//                   nextHdFile();
//                 updateOptions(true);

//               //Serial.println("down");
//             }
//             else if (keyboard_data[2] == 0x75) // Up Arrow
//             {
//               if (!HdDisk)
//                 prevDiskFile();
//               else
//                 prevHdFile();
//               updateOptions(false);
//               //Serial.println("up");
//             }
//             keymem = 0;

//           }

//         }
//         else if (keyboard_data[1] == 0xF0 && (keyboard_data[2] == 0x12 || keyboard_data[2] == 0x59))
//         {
//           shift_enabled = false;
//           if (joystick) Pb1 = false;
//         }
//         else if (keyboard_data[1] == 0xF0 && keyboard_data[2] == 0x14)
//         {
//           ctrl_enabled = false;
//           if (joystick) Pb0 = false;
//         }
//         else if (keyboard_data[1] == 0xF0 && (keyboard_data[2] == 0x75 || keyboard_data[2] == 0x72))
//         {
//           if (joystick) timerpdl1 = JOY_MID;
//         }
//         else if (keyboard_data[1] == 0xF0 && (keyboard_data[2] == 0x6b || keyboard_data[2] == 0x74))
//         {
//           if (joystick) timerpdl0 = JOY_MID;
//         }
//         else
//         {
//           if (joystick)
//           {
//             timerpdl0 = JOY_MID;
//             timerpdl1 = JOY_MID;
//           }
//         }
//       }
//     }

//     //shuffle buffer
//     keyboard_data[0] = keyboard_data[1];
//     keyboard_data[1] = keyboard_data[2];
//     keyboard_buf_indx = 0;
//   }
// }