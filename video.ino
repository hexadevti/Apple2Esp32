const uint16_t colors[8] PROGMEM = {TFT_BLACK, TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_BLACK, tft.color565(255, 20, 0), TFT_SKYBLUE, TFT_WHITE};
const uint16_t colors16[16] PROGMEM = { tft.color565(0, 0, 0), tft.color565(147, 11, 124), tft.color565(98, 76, 0), tft.color565(249, 86, 29), 
  tft.color565(0, 118, 12), tft.color565(126, 126, 126), tft.color565(67, 200, 0), tft.color565(220, 205, 22), 
  tft.color565(31, 53, 211), tft.color565(187, 54, 255), tft.color565(126, 126, 126), tft.color565(255, 129, 236), 
  tft.color565(7, 168, 224), tft.color565(157, 172, 255), tft.color565(93, 247, 132), tft.color565(255, 255, 255)
};
int flashCount = 0;
int width = 280;
int height = 192;

void videoSetup()
{
  printLog("Video Setup...");
  tft.begin();
  tft.setRotation(1);
  tft.invertDisplay(true);
  tft.initDMA(); 
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);

  tft.fillRect(20, 24, 280, 192, TFT_BLACK);

  tft.setCursor(10, 0);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Apple //e");

  printMsg("APPLE2ESP32", TFT_BLUE);
  xTaskCreate(graphicFlashCharacters, "graphicFlashCharacters", 1024, NULL, 1, NULL);
}

void printMsg(char msg[], uint16_t color)
{
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(color, TFT_BLACK);
  tft.setCursor(10, 0);
  tft.print("                                                                         ");
  tft.setCursor(10, 0);
  tft.println(msg);
}

void printStatus(char msg[], uint16_t color)
{
  // tft.setTextFont(1);
  // tft.setTextSize(1);
  // tft.setTextColor(color, TFT_BLACK);
  // tft.setCursor(5, 224);
  // tft.print("                                                                   ");
  // tft.setCursor(5, 224);
  // tft.print(msg);
}

void graphicFlashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (running)
  {
    while (paused) { vTaskDelay(pdMS_TO_TICKS(20)); }
    if (OptionsWindow)
    {
      tft.fillRect(40, 40, 240, 160, 0);
      tft.drawRect(41, 41, 238, 158, TFT_RED);

      while (OptionsWindow)
      {
        vTaskDelay(pdMS_TO_TICKS(20));
      }
    }
    else
    {
      if (AppleIIe && !Cols40_80 && !DHiResOn_Off)
        tft.setAddrWindow(0, margin_y, 320, 192); // Set the area to draw
      else 
        tft.setAddrWindow(margin_x, margin_y, 280, 192); 

      page_lock.lock();

      tft.startWrite();
      Vertical_blankingOn_Off = false; // IIe video problem with Total Replay

      int x = 0;
      int y = 0;
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
                x = 0;
                for (int c = 0; c < 0x28; c++)
                {
                  for (int k = 0; k < 7; k++)
                  {
                    char value = ram[(textPage + (b * 0x28) + (l * 0x80) + c)];
                    int firstColor = (value & 0b11110000) >> 4;
                    int secondColor = value & 0b00001111;
                    if (j < 4)
                    {
                      if (!optionsScreenBlank(x, y))
                        tft.writeColor(colors16[secondColor], 1);
                    }
                    else
                    {
                      if (!optionsScreenBlank(x, y))
                        tft.writeColor(colors16[firstColor], 1);
                    }
                    x++;
                  }
                }
                y++;
              }
            }
            else if (DHiResOn_Off)
            {
              if (videoColor) 
              {
                for (int block = 0; block < 8; block++)
                {
                  uint8_t rep = 2;
                  uint8_t val = 0;
                  uint8_t prevVal = 0;
                  uint8_t currVal = 0;
                  uint8_t prevCount = 0;
                  for (ushort c = 0; c < 0x50; c++)
                  {
                    char chr;
                    if (c % 2 == 0)
                    {
                      chr = auxram[(ushort)((0x2000 + (b * 0x28) + (l * 0x80) + c / 2) + block * 0x400)];
                    }
                    else
                    {
                      chr = ram[(ushort)((0x2000 + (b * 0x28) + (l * 0x80) + (c - 1) / 2) + block * 0x400)];
                    }
                    
                    // if (prevCount == 0) {
                    //   if (prevCont > 0) {
                    //     val = prevVal | ((0b0000000 & chr) << 4);
                    //     tft.writeColor(colors16[val], rep);
                    //   }
                    //   currVal = (0b0001111 & chr);
                    //   tft.writeColor(colors16[currVal], rep);
                    //   prevVal = (0b1110000 & chr) >> 4;
                    //   prevCount = 1;
                    // } 
                    // else if (prevCount == 1) {
                    //   val = prevVal | ((0b0000001 & chr) << 3);
                    //   tft.writeColor(colors16[val], rep);
                    //   currVal = (0b0011110 & chr) >> 1;
                    //   tft.writeColor(colors16[currVal], rep);
                    //   prevVal = (0b1100000 & chr) >> 5;
                    //   prevCount = 2;
                    // } 
                    // else if (prevCount == 2) {
                    //   val = prevVal | ((0b0000011 & chr) << 2);
                    //   tft.writeColor(colors16[val], rep);
                    //   currVal = (0b0111100 & chr) >> 2;
                    //   tft.writeColor(colors16[currVal], rep);
                    //   prevVal = (0b1000000 & chr) >> 6;
                    //   prevCount = 3;
                    // } 
                    // else if (prevCount == 3) {
                    //   val = prevVal | ((0b0000111 & chr) << 1);
                    //   tft.writeColor(colors16[val], rep);
                    //   currVal = (0b1111000 & chr) >> 3;
                    //   tft.writeColor(colors16[currVal], rep);
                    //   prevVal = 0;
                    //   prevCount = 0;
                    // } 


                    val = prevVal | (((0xf >> (4 - prevCount)) & chr) << (4 - prevCount));
                    if (prevCount > 0) {
                      tft.writeColor(colors16[val], rep); 
                      x++;
                    }
                    currVal = ((0xf << prevCount) & chr) >> prevCount;
                    tft.writeColor(colors16[currVal], rep); 
                    x++;
                    prevVal = ((0xf << (4 + prevCount)) & chr) >> (4 + prevCount);
                    prevCount++;
                    if (prevCount == 4) prevCount = 0;

                  }
                  y++;
                }
              }
              else // monochrome
              {
                for (int block = 0; block < 8; block++)
                {
                  bool last7bits = false;
                  for (ushort c = 0; c < 0x50; c++)
                  {
                    char chr;
                    if (c % 2 == 0)
                    {
                      chr = auxram[(ushort)((0x2000 + (b * 0x28) + (l * 0x80) + c / 2) + block * 0x400)];
                    }
                    else
                    {
                      chr = ram[(ushort)((0x2000 + (b * 0x28) + (l * 0x80) + (c - 1) / 2) + block * 0x400)];
                    }
                    bool blockline[8];
                    for (int i = 0; i < 8; i++)
                      blockline[7 - i] = (chr & (1 << i)) != 0;

                    for (int i = 7; i > 0; i--)
                    {
                      uint16_t color = 0;

                      if (i % 2 != 0)
                      {
                        if (i == 7)
                        {
                          if (blockline[i] && last7bits)
                            color = tft.color565(255, 255, 255);
                          else if (blockline[i] != last7bits)
                            color = tft.color565(127, 127, 127);
                          else
                            color = tft.color565(0, 0, 0);
                        }
                        else
                        {
                          if (blockline[i] && blockline[i + 1])
                            color = tft.color565(255, 255, 255);
                          else if (blockline[i] != blockline[i + 1])
                            color = tft.color565(127, 127, 127);
                          else
                            color = tft.color565(0, 0, 0);
                        }

                        tft.writeColor(color, 1);

                        if (i == 1)
                          last7bits = blockline[i];
                        x++;
                      }
                    }
                  }
                  y++;
                }
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
                      if (!optionsScreenBlank(x, y))
                        tft.writeColor(colors[pixels[id]], 1);
                      x++;
                    }
                    std::copy(std::begin(blockline), std::end(blockline), std::begin(blocklineAnt));
                  }
                  else
                  {
                    for (int i = 7; i > 0; i--)
                    {
                      uint16_t color = TFT_BLACK;

                      if (blockline[i])
                        color = TFT_WHITE;
                      else
                        color = TFT_BLACK;

                      if (!optionsScreenBlank(x, y))
                      {
                        tft.writeColor(color, 1);
                      }
                      x++;
                    }
                  }
                }
                x = 0;
                y++;
              }
            }
          }
          else if (Cols40_80) // Text modes
          {
            for (int i = 0; i < 8; i++) // char lines
            {
              x = 0;
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
                  if (!optionsScreenBlank(x, y))
                    tft.writeColor(bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK), 1);
                  x++;
                }
              }
              y++;
            }
          }
          else if (AppleIIe && !Cols40_80)
          {
            for (int i = 0; i < 8; i++)
            {
              for (int j = 0; j < 0x50; j++)
              {
                char chr;
                if (j % 2 == 0)
                {
                  chr = auxram[0, (ushort)(0x400 + (b * 0x28) + (l * 0x80) + j / 2)];
                }
                else
                {
                  chr = ram[(ushort)(0x400 + (b * 0x28) + (l * 0x80) + (j - 1) / 2)];
                }
                bool last7bits = false;
                for (int k = 0; k < 7; k++)
                {
                  ushort addr = (chr * 7 * 8) + (i * 7) + k;
                  bool bpixel = AppleIIeFontPixels[addr];
                  uint16_t color = 0;
                  if (k % 2 == 0)
                  {
                    if (bpixel && last7bits)
                      color = tft.color565(255, 255, 255);
                    else if (bpixel != last7bits)
                      color = tft.color565(127, 127, 127);
                    else
                      color = tft.color565(0, 0, 0);
                    
                    if (!optionsScreenBlank(x, y))
                      tft.writeColor(color, 1);

                    x++;
                  }
                  last7bits = bpixel;
                }
                
              }
              y++;
            }
          }
        }
      }
      tft.endWrite();
      Vertical_blankingOn_Off = true;
      page_lock.unlock();
      vTaskDelay(pdMS_TO_TICKS(20));
      tft.invertDisplay(true);
      flashCount++;
      if (flashCount > 7)
      {

        inversed = !inversed;
        flashCount = 0;
      }
    }
  }
}



void printOptionsBackground(int color)
{
  // tft.fillRect(40, 40, 240, 160, 0);
  // tft.drawRect(41, 41, 238, 158, TFT_RED);
  // // vga.setFont(Font6x8);
  // // vga.setTextColor(// tft.color565(color), // tft.color565(0));
  // // vga.setCursor(44, 188);
  // sprintf(buf, " %s | %s | %s | %s | %s", HdDisk ? " HD " : "DISK", AppleIIe ? "IIe" : "II+", Fast1MhzSpeed ? "Fast" : "1Mhz", paused ? "Paused " : "Running", joystick ? "Joy On " : "Joy Off");
  // // vga.print(buf);
}

void printOptionsText(const char *text)
{
  // // vga.fillRect(42, 42, 236, 147, 0);
  // // vga.setCursor(44, 44);
  // // vga.setTextColor(// tft.color565(0xffffff), // tft.color565(0));
  // // vga.print(text);
}

void printOptionsTextEx(char text[])
{
  // // vga.setCursor(44, 44);
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.setTextColor(// tft.color565(0), // tft.color565(0xff0000));
  // // vga.println("12345678901234567890123456789");
  // // vga.setTextColor(// tft.color565(0xff0000), // tft.color565(0));
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
  // // vga.println("12345678901234567890123456789");
}