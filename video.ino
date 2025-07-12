#ifdef TFT

const uint16_t colors[8] = {TFT_BLACK, TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_BLACK, tft.color565(255, 20, 0), TFT_SKYBLUE, TFT_WHITE};
const uint16_t colors16[16] = {tft.color565(0, 0, 0), tft.color565(147, 11, 124), tft.color565(98, 76, 0), tft.color565(249, 86, 29),
                                       tft.color565(0, 118, 12), tft.color565(126, 126, 126), tft.color565(67, 200, 0), tft.color565(220, 205, 22),
                                       tft.color565(31, 53, 211), tft.color565(187, 54, 255), tft.color565(126, 126, 126), tft.color565(255, 129, 236),
                                       tft.color565(7, 168, 224), tft.color565(157, 172, 255), tft.color565(93, 247, 132), tft.color565(255, 255, 255)};
#else
const uint8_t colors[] = { 0b00000000, 
                           0b00111000, 
                           0b11000111, 
                           0b11111111, 
                           0b00000000, 
                           0b00000111, 
                           0b11000000, 
                           0b11111111 };
const uint8_t colors16[] = { 0b00000000, // 0 black
                             0b10000000, // 1 dark blue
                             0b00100000, // 2 dark green *
                             0b01000000, // 3 mid blue
                             0b00100010, // 4 dark orange
                             0b10100100, // 5 cinza escuro
                             0b00010000, // 6 Green mid *
                             0b00111000, // 7 Green bright
                             0b10000100, // 8 Violeta
                             0b01000010, // 9 violet2
                             0b01011011, // 10 gray bright
                             0b11010010, // 11 Blue Bright
                             0b00000111, // 12 Orange bright
                             0b01010111, // 13 dark pink
                             0b00111111, // 14 yellow
                             0b11111111}; // 15 white

  
#endif                                    
int flashCount = 0;
int touchCount = 0;
int width = 280;
int height = 192;


void videoSetup()
{
  printLog("Video Setup...");
#ifdef TFT
  tft.begin();
  tft.setRotation(3);
  tft.invertDisplay(true);
  tft.initDMA();
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);
#else
  const PinConfig pins(-1, -1, -1, RED0_PIN, RED1_PIN, 
                       -1, -1, -1, -1, GREEN0_PIN, GREEN1_PIN,
                       -1, -1, -1, BLUE0_PIN, BLUE1_PIN, 
                       HSYNC_PIN,VSYNC_PIN);
  Mode mode(16, 96, 48, 640, 4, 2, 30, 240, 23760000, 0, 0, 2);
  while (!vga.init(pins, mode, 8, 3))
    delay(1);

  printLog("Video initialized.");
  vga.show();
  vga.start();
  vga.fillRect(0,0,320,240,0);
#endif
  xTaskCreate(renderLoop, "renderLoop", 1024, NULL, 1, NULL);
}



void renderLoop(void *pvParameters)
{
  bool inversed = false;
  
  while (running)
  {
    page_lock.lock();

    #ifdef TFT
    if (!OptionsWindow && AppleIIe && !Cols40_80 && !DHiResOn_Off)
    tft.setAddrWindow(0, margin_y, 320, 192); // Set the area to draw
    else if (!OptionsWindow && AppleIIe && DHiResOn_Off && !videoColor)
    tft.setAddrWindow(0, margin_y, 320, 192); // Set the area to draw
    else if (OptionsWindow)
    tft.setAddrWindow(2, 0, 315, 240);
    else
    tft.setAddrWindow(margin_x, margin_y, 280, 192);
    tft.startWrite();
    #endif
    
    Vertical_blankingOn_Off = false; // IIe video problem with Total Replay

    int x = margin_x;
    int y = margin_y;
    ushort textPage = Page1_Page2 ? 0x400 : 0x800;
    ushort graphicsPage = Page1_Page2 ? 0x2000 : 0x4000;
    
    if (OptionsWindow || DebugWindow)
    {
      y=0;
      for (int v = 0; v < 30; v++)
      {
        for (int i = 0; i < 8; i++) // char lines
        {
          x=0;
          for (int h = 0; h < 45; h++)
          {
            uint8_t chr = menuScreen[v * 45 + h];
            for (int c = 0; c < 7; c++) // char cols
            {
              bool bpixel = AppleIIeFontPixels[(chr*7*8) + (i * 7) + c];
              uint8_t color = menuColor[v * 45 + h];
              uint8_t fgColor = (color & 0xf0) >> 4;
              uint8_t bgColor = (color & 0x0f);
              #ifdef TFT
              tft.writeColor((bpixel ? colors16[fgColor] : colors16[bgColor]), 1);
              #else
              vga.dotFast(x, y, bpixel ? colors16[fgColor] : colors16[bgColor]);
              x++;
              vga.dotFast(x, y, bpixel ? colors16[fgColor] : colors16[bgColor]);
              #endif
              x++;
            }
          }
          y++;
        }
      }
    }
    else
    {
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
                      #ifdef TFT
                      tft.writeColor(colors16[secondColor], 1);
                      #else
                      vga.dotFast(x, y, colors16[secondColor]);
                      x++;
                      vga.dotFast(x, y, colors16[secondColor]);
                      #endif
                    }
                    else
                    {
                      #ifdef TFT
                      tft.writeColor(colors16[firstColor], 1);
                      #else
                      vga.dotFast(x, y, colors16[firstColor]);
                      x++;
                      vga.dotFast(x, y, colors16[firstColor]);
                      #endif
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
                  x = margin_x;
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
                    if (prevCount > 0)
                    {
                      #ifdef TFT
                      tft.writeColor(colors16[val], rep);
                      x++;
                      #else
                      vga.dotFast(x, y, colors16[val]);  
                      x++;
                      vga.dotFast(x, y, colors16[val]);  
                      x++;
                      vga.dotFast(x, y, colors16[val]);  
                      x++;
                      vga.dotFast(x, y, colors16[val]);  
                      x++;
                      #endif
                    }
                    currVal = ((0xf << prevCount) & chr) >> prevCount;
                    #ifdef TFT
                    tft.writeColor(colors16[currVal], rep);
                    x++;
                    #else
                    vga.dotFast(x, y, colors16[currVal]);  
                    x++;
                    vga.dotFast(x, y, colors16[currVal]);  
                    x++;
                    vga.dotFast(x, y, colors16[currVal]);  
                    x++;
                    vga.dotFast(x, y, colors16[currVal]);  
                    x++;
                    #endif
                    prevVal = ((0xf << (4 + prevCount)) & chr) >> (4 + prevCount);
                    prevCount++;
                    if (prevCount == 4)
                      prevCount = 0;
                  }
                  y++;
                }
              }
              else // monochrome
              {
                for (int block = 0; block < 8; block++)
                {
                  x = margin_x;
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
                      #ifdef TFT
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
                      #else
                        vga.dotFast(x, y, blockline[i] ? colors[7] : colors[0]);
                        x++;
                      #endif
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
                x = margin_x;
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
                      #ifdef TFT
                      tft.writeColor(colors[pixels[id]], 1);
                      #else
                      vga.dotFast(x, y, colors[pixels[id]]);
                      x++;
                      vga.dotFast(x, y, colors[pixels[id]]);
                      #endif
                      x++;
                    }
                    std::copy(std::begin(blockline), std::end(blockline), std::begin(blocklineAnt));
                  }
                  else
                  {
                    for (int i = 7; i > 0; i--)
                    {
                      #ifdef TFT
                      uint16_t color = TFT_BLACK;
                      if (blockline[i])
                        color = TFT_WHITE;
                      else
                        color = TFT_BLACK;
                      tft.writeColor(color, 1);
                      #else
                      uint16_t color = colors[0];
                      if (blockline[i])
                        color = colors[7];
                      else
                        color = colors[0];
                      vga.dotFast(x, y, color);
                      x++;
                      vga.dotFast(x, y, color);
                      #endif
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
                  #ifdef TFT
                  tft.writeColor(bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK), 1);
                  #else
                  vga.dotFast(x, y, bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]));
                  x++;
                  vga.dotFast(x, y, bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]));
                  #endif
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
              x = margin_x;
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
                  #ifdef TFT
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
                    tft.writeColor(color, 1);
                    x++;
                  }
                  last7bits = bpixel;
                  #else
                  ushort addr = (chr * 7 * 8) + (i * 7) + k;
                  bool bpixel = AppleIIeFontPixels[addr];
                  vga.dotFast(x, y, bpixel ? colors[7] : colors[0]);
                  x++;
                  #endif
                }
              }
              y++;
            }
          }
        }
      }
    }
    #ifdef TFT
    tft.endWrite();
    #endif
    Vertical_blankingOn_Off = true;
    page_lock.unlock();
    vTaskDelay(pdMS_TO_TICKS(20));
    #ifdef TFT
    tft.invertDisplay(true);
    #endif
    flashCount++;
    if (flashCount > 7)
    {
      inversed = !inversed;
      flashCount = 0;
    }
  }
}
