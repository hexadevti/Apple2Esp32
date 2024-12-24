const int colors[] PROGMEM = {0b000000, 0b001100, 0b110011, 0b111111, 0b000000, 0b000011, 0b100000, 0b111111};
const int colors16[] PROGMEM = {0x000000, 0x3300dd, 0x990000, 0xdd22dd, 0x227700, 0x444444, 0xff2222, 0xcc8844, 0x004477, 0x0044dd, 0x888888, 0x6677dd, 0x00dd11, 0x00ffff, 0x88ee33, 0xffffff };
int flashCount = 0;

void videoSetup()
{
  printlog("Video Setup...");
  Mode myMode = vga.MODE320x240; //.custom(320,190);
  //vga.init(myMode, red0pin, green0pin, blue0pin, hsyncPin, vsyncPin);
  vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
  //vga.setFrameBufferCount(1);
  
  vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  printMsg("APPLE2ESP32", 0xff0000);
  xTaskCreate(graphicFlashCharacters, "graphicFlashCharacters", 2048, NULL, 1, NULL);
}



void printMsg(char msg[], int color)
{
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(color), vga.RGB(0));
  vga.setCursor(5, 8);
  vga.print("                                                                   ");
  vga.setCursor(5, 8);
  vga.print(msg);
  vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void printStatus(char msg[], int color)
{
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(color), vga.RGB(0));
  vga.setCursor(5, 224);
  vga.print("                                                                   ");
  vga.setCursor(5, 224);
  vga.print(msg);
  vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void graphicFlashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (true)
  {
    
    page_lock.lock();
    int x = margin_x;
    int y = margin_y;
    ushort textPage = Page1_Page2 ? 0x400 : 0x800;
    ushort graphicsPage = Page1_Page2 ? 0x2000 : 0x4000;
    for (int b = 0; b < 3; b++)
    {
      for (int l = 0; l < 8; l++)
      {
        if ((Graphics_Text && DisplayFull_Split) || (Graphics_Text && !DisplayFull_Split && (b < 2 || (b == 2 && l < 4))))
        {
          if (LoRes_HiRes)
          {
            for (int j = 0; j < 8; j++)
            {
              x = margin_x;
              for (int c = 0; c < 0x28; c++)
              {
                for (int k = 0; k < 7; k++)
                {
                  char value = ram[(textPage + (b * 0x28) + (l * 0x80) + c)];
                  int firstColor = (value & 0b11110000) >> 4;
                  int secondColor = value & 0b00001111;
                  if (j < 4)
                  {
                    if (!optionsScreenBlank(x,y))
                      vga.dotFast(x, y, vga.RGB(colors16[secondColor]));
                  }
                  else
                  {
                    if (!optionsScreenBlank(x,y))
                      vga.dotFast(x, y, vga.RGB(colors16[firstColor]));
                  }
                  x++;
                }
              }
              y++;
            }
          }
          else // hires
          {
            
            for (int block = 0; block < 8; block++)
            {
              bool blocklineAnt[] = {false, false, false, false, false, false, false, false};
              for (ushort c = 0; c < 0x28; c++)
              {
                char chr = ram[(ushort)(((graphicsPage) + (b * 0x28) + (l * 0x80) + c) + block * 0x400)];
                bool blockline[8];
                for (int i = 0; i < 8; i++)
                  blockline[7 - i] = (chr & (1 << i)) != 0;
                if (videoColor)
                {

                  char pixels[7];
                  if (c % 2 == 0) // Odd
                  {
                    pixels[0] = (blockline[0] ? 4 : 0) + (blockline[7] ? 2 : 0) + (blocklineAnt[1] ? 1 : 0);
                    pixels[1] = (blockline[0] ? 4 : 0) + (blockline[7] ? 2 : 0) + (blockline[6] ? 1 : 0);
                    pixels[2] = (blockline[0] ? 4 : 0) + (blockline[5] ? 2 : 0) + (blockline[6] ? 1 : 0);
                    pixels[3] = (blockline[0] ? 4 : 0) + (blockline[5] ? 2 : 0) + (blockline[4] ? 1 : 0);
                    pixels[4] = (blockline[0] ? 4 : 0) + (blockline[3] ? 2 : 0) + (blockline[4] ? 1 : 0);
                    pixels[5] = (blockline[0] ? 4 : 0) + (blockline[3] ? 2 : 0) + (blockline[2] ? 1 : 0);
                    pixels[6] = (blockline[0] ? 4 : 0) + (blockline[1] ? 2 : 0) + (blockline[2] ? 1 : 0);
                  }
                  else // Even
                  {
                    pixels[0] = (blockline[0] ? 4 : 0) + (blocklineAnt[1] ? 2 : 0) + (blockline[7] ? 1 : 0);
                    pixels[1] = (blockline[0] ? 4 : 0) + (blockline[6] ? 2 : 0) + (blockline[7] ? 1 : 0);
                    pixels[2] = (blockline[0] ? 4 : 0) + (blockline[6] ? 2 : 0) + (blockline[5] ? 1 : 0);
                    pixels[3] = (blockline[0] ? 4 : 0) + (blockline[4] ? 2 : 0) + (blockline[5] ? 1 : 0);
                    pixels[4] = (blockline[0] ? 4 : 0) + (blockline[4] ? 2 : 0) + (blockline[3] ? 1 : 0);
                    pixels[5] = (blockline[0] ? 4 : 0) + (blockline[2] ? 2 : 0) + (blockline[3] ? 1 : 0);
                    pixels[6] = (blockline[0] ? 4 : 0) + (blockline[2] ? 2 : 0) + (blockline[1] ? 1 : 0);
                  }

                  for (int id = 0; id < 7; id++)
                  {
                    if (!optionsScreenBlank(x,y))
                      vga.dotFast(x, y, colors[pixels[id]]);
                    x++;
                  }
                  std::copy(std::begin(blockline), std::end(blockline), std::begin(blocklineAnt));
                }
                else
                {
                  for (int i = 7; i > 0; i--)
                  {
                    int color = 0;

                    if (blockline[i])
                      color = 0b111111;
                    else
                      color = 0;

                    if (!optionsScreenBlank(x,y))
                      vga.dotFast(x, y, color);

                    // sprintf(buf, "%04x: %02x",,chr);
                    // printlog(buf);
                    x++;
                  }
                }
              }
              x = margin_x;
              y++;
            }
            
          }
        }
        else // Text modes
        {
          for (int i = 0; i < 8; i++) // char lines
          {
            x = margin_x;
            for (int c = 0; c < 0x28; c++)
            {
              for (int k = 0; k < 7; k++) // char cols
              {
                char chr = ram[(ushort)(textPage + (b * 0x28) + (l * 0x80) + c)];
                ushort addr = (chr * 7 * 8) + (i * 7) + k;
                bool bpixel = AppleIIe ? AppleIIeFontPixels[addr] : AppleFontPixels[addr];
                bool inverted = false; 
                if (!AppleIIe)
                  inverted = chr >= 0x40 && chr < 0x80 && inversed;
                if (!optionsScreenBlank(x,y))
                  vga.dotFast(x, y, bpixel ? (inverted ? vga.RGB(0) : vga.RGB(0xffffff)) : (inverted ? vga.RGB(0xffffff) : vga.RGB(0)) );
                x++;
              }
            }
            y++;
          }
          
        }
      }
    }
    page_lock.unlock();
    vTaskDelay(pdMS_TO_TICKS(20));
    flashCount++;
    if (flashCount > 7)  
    {
      inversed = !inversed;
      flashCount = 0;
    }

  }
}

bool optionsScreenBlank(int x, int y) {
  if (OptionsWindow) {
    if (x >= 40 && x < 280 && y >= 40 && y < 200)
      return true;
    else
      return false;
  }
  else
    return false;
}

void printOptionsBackground(int color)
{
  vga.fillRect(40, 40, 240, 160, 0);
  vga.rect(41, 41, 238, 158, vga.RGB(color));
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(color), vga.RGB(0));
  vga.setCursor(44, 188);
  sprintf(buf, "%s      %s                    ", HdDisk ? "HD" : "DISK", AppleIIe ? "Apple IIe" : "Apple II+");
  vga.print(buf);
}

void printOptionsText(const char *text)
{
  vga.fillRect(42, 42, 236, 147, 0);
  vga.setCursor(44, 44);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  vga.print(text);
}

void printOptionsTextEx(char text[])
{
  vga.setCursor(44, 44);
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.setTextColor(vga.RGB(0), vga.RGB(0xff0000));
  vga.println("12345678901234567890123456789");
  vga.setTextColor(vga.RGB(0xff0000), vga.RGB(0));
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");
  vga.println("12345678901234567890123456789");

}