#ifndef TFT
int flashCount = 0;
int touchCount = 0;
int width = 280;
int height = 192;

void videoSetup()
{
  printLog("Video Setup...");
  const PinConfig pins(-1,-1,-1,RED0_PIN,RED1_PIN,  -1,-1,-1,-1,GREEN0_PIN,GREEN1_PIN,  -1,-1,-1,BLUE0_PIN,BLUE1_PIN, HSYNC_PIN,VSYNC_PIN);
  Mode mode(16, 96, 48, 640, 4, 2, 30, 240, 23760000, 0, 0, 2);
  while (!vga.init(pins, mode, 8, 3))
    delay(1);

  printLog("Video initialized.");
  vga.show();
  vga.start();
  vga.fillRect(0,0,320,240,0);
  xTaskCreate(vgaRender, "graphicFlashCharacters", 1024, NULL, 1, NULL);
}

void vgaRender(void *pvParameters)
{
  bool inversed = false;
  
  while (running)
  {
    page_lock.lock();
    Vertical_blankingOn_Off = false; // IIe video problem with Total Replay

    int x = 0;
    int y = 0;
    ushort textPage = Page1_Page2 ? 0x400 : 0x800;
    ushort graphicsPage = Page1_Page2 ? 0x2000 : 0x4000;
    
    if (OptionsWindow)
    {
      for (int y = 0; y < 30; y++)
      {
        for (int i = 0; i < 8; i++) // char lines
        {
          for (int x = 0; x < 45; x++)
          {
            uint8_t chr = menuScreen[y * 45 + x];
            for (int c = 0; c < 7; c++) // char cols
            {
              bool bpixel = AppleIIeFontPixels[(chr*7*8) + (i * 7) + c];
              uint8_t color = menuColor[y * 45 + x];
              uint8_t fgColor = (color & 0xf0) >> 4;
              uint8_t bgColor = (color & 0x0f);
            }
          }
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
                      // if (!optionsScreenBlank(x, y))
                      vga.dotFast(x, y, getLoresColor(secondColor));
                    }
                    else
                    {
                      // if (!optionsScreenBlank(x, y))
                      vga.dotFast(x, y, getLoresColor(firstColor));
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

                    val = prevVal | (((0xf >> (4 - prevCount)) & chr) << (4 - prevCount));
                    if (prevCount > 0)
                    {
                      vga.dotFast(x, y, getDoubleHiresColor(val));  
                      x++;
                      vga.dotFast(x, y, getDoubleHiresColor(val));  
                      x++;
                    }
                    currVal = ((0xf << prevCount) & chr) >> prevCount;
                    vga.dotFast(x, y, getDoubleHiresColor(currVal));  
                    x++;
                    vga.dotFast(x, y, getDoubleHiresColor(currVal));  
                    x++;
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
                            color = vga.rgb(255, 255, 255);
                          else if (blockline[i] != last7bits)
                            color = vga.rgb(127, 127, 127);
                          else
                            color = vga.rgb(0, 0, 0);
                        }
                        else
                        {
                          if (blockline[i] && blockline[i + 1])
                            color = vga.rgb(255, 255, 255);
                          else if (blockline[i] != blockline[i + 1])
                            color = vga.rgb(127, 127, 127);
                          else
                            color = vga.rgb(0, 0, 0);
                        }

                        vga.dotFast(x, y, color);

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
                      // if (!optionsScreenBlank(x, y))
                      vga.dotFast(x, y, getDoubleHiresColor(pixels[id]));
                      x++;
                    }
                    std::copy(std::begin(blockline), std::end(blockline), std::begin(blocklineAnt));
                  }
                  else
                  {
                    for (int i = 7; i > 0; i--)
                    {
                      uint16_t color = vga.rgb(0,0,0);

                      if (blockline[i])
                        color = vga.rgb(255,255,255);
                      else
                        color = vga.rgb(0,0,0);

                      // if (!optionsScreenBlank(x, y))

                      vga.dotFast(x, y, color);
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
                  // if (!optionsScreenBlank(x, y))

                  vga.dotFast(x, y, bpixel ? (inverted ? 0 : vga.rgb(255,255,255)) : (inverted ? vga.rgb(255,255,255) : 0));
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
                      color = vga.rgb(255, 255, 255);
                    else if (bpixel != last7bits)
                      color = vga.rgb(127, 127, 127);
                    else
                      color = vga.rgb(0, 0, 0);

                    vga.dotFast(x, y, color);

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
      
    }
    Vertical_blankingOn_Off = true;
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


int getLoresColor(int id)
{
  int ret = 0;
  switch (id)
  {
  case 0:
    ret = vga.rgb(0, 0, 0);
    break;
  case 1:
    ret = vga.rgb(147, 11, 124);
    break;
  case 2:
    ret = vga.rgb(31, 53, 211);
    break;
  case 3:
    ret = vga.rgb(187, 54, 255);
    break;
  case 4:
    ret = vga.rgb(0, 118, 12);
    break;
  case 5:
    ret = vga.rgb(126, 126, 126);
    break;
  case 6:
    ret = vga.rgb(7, 168, 224);
    break;
  case 7:
    ret = vga.rgb(157, 172, 255);
    break;
  case 8:
    ret = vga.rgb(98, 76, 0);
    break;
  case 9:
    ret = vga.rgb(249, 86, 29);
    break;
  case 10:
    ret = vga.rgb(126, 126, 126);
    break;
  case 11:
    ret = vga.rgb(255, 129, 236);
    break;
  case 12:
    ret = vga.rgb(67, 200, 0);
    break;
  case 13:
    ret = vga.rgb(220, 205, 22);
    break;
  case 14:
    ret = vga.rgb(93, 247, 132);
    break;
  case 15:
    ret = vga.rgb(255, 255, 255);
    break;
  default:
    break;
  }
  return ret;
}

int getHiresColor(int id)
{
  int ret = 0;
  switch (id)
  {
  case 0:
    ret = vga.rgb(0, 0, 0);
    break;
  case 1:
    ret = vga.rgb(67, 200, 0);
    break;
  case 2:
    ret = vga.rgb(187, 54, 255);
    break;
  case 3:
    ret = vga.rgb(255, 255, 255);
    break;
  case 4:
    ret = vga.rgb(0, 0, 0);
    break;
  case 5:
    ret = vga.rgb(249, 62, 29);
    break;
  case 6:
    ret = vga.rgb(7, 168, 224);
    break;
  case 7:
    ret = vga.rgb(255, 255, 255);
    break;
  default:
    break;
  }
  return ret;
}

int getDoubleHiresColor(int id)
{
  int ret = 0;
  switch (id)
  {
  case 0:
    ret = vga.rgb(0, 0, 0);
    break;
  case 1:
    ret = vga.rgb(147, 11, 124);
    break;
  case 2:
    ret = vga.rgb(98, 76, 0);
    break;
  case 3:
    ret = vga.rgb(249, 86, 29);
    break;
  case 4:
    ret = vga.rgb(0, 118, 12);
    break;
  case 5:
    ret = vga.rgb(126, 126, 126);
    break;
  case 6:
    ret = vga.rgb(67, 200, 0);
    break;
  case 7:
    ret = vga.rgb(220, 205, 22);
    break;
  case 8:
    ret = vga.rgb(31, 53, 211);
    break;
  case 9:
    ret = vga.rgb(187, 54, 255);
    break;
  case 10:
    ret = vga.rgb(126, 126, 126);
    break;
  case 11:
    ret = vga.rgb(255, 129, 236);
    break;
  case 12:
    ret = vga.rgb(7, 168, 224);
    break;
  case 13:
    ret = vga.rgb(157, 172, 255);
    break;
  case 14:
    ret = vga.rgb(93, 247, 132);
    break;
  case 15:
    ret = vga.rgb(255, 255, 255);
    break;
  default:
    break;
  }
  return ret;
}
#endif