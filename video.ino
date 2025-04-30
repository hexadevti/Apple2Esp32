int flashCount = 0;

void videoSetup()
{
  printLog("Video Setup...");
  while (!vga.init(pins, mode, 8, 3))
    delay(1);

  printLog("Video initialized.");
  vga.show();
  vga.start();
  vga.fillRect(0,0,640,240,0);
  xTaskCreate(graphicFlashCharacters, "graphicFlashCharacters", 4096, NULL, 1, NULL);
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

void graphicFlashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (running)
  {
    while (paused)
    {
      delay(100);
    }
    Vertical_blankingOn_Off = false; // IIe video problem with Total Replay
    page_lock.lock();
    int x;
    if (Cols40_80)
    x = margin_x;
    else
    x = margin_x_80cols;
    x = DHiResOn_Off ? margin_x_dhgr : margin_x;
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
                    if (!optionsScreenBlank(x, y))
                    {
                      vga.dotFast(x, y, getLoresColor(secondColor));
                    }
                  }
                  else
                  {
                    if (!optionsScreenBlank(x, y))
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

            for (int block = 0; block < 8; block++)
            {
              bool line[0x50 * 7];
              int lineId = 0;
              bool last7bits = false;
              x = margin_x;
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

                if (videoColor)
                {
                   for (int i = 7; i > 0; i--)
                  {
                    line[lineId] = blockline[i];
                    lineId++;
                  }
                }
                else
                {
                  for (int i = 7; i > 0; i--)
                  {
                    if (!optionsScreenBlank(x, y))
                      vga.dotFast(x, y, blockline[i] ? vga.rgb(255, 255, 255) : vga.rgb(0, 0, 0));
                    x++;
                  }
                }
              }
              if (videoColor) {
                for (int i = 0; i < 0x50 * 7; i = i + 4)
                {
                  int color = (line[i] ? 8 : 0) + (line[i + 1] ? 4 : 0) + (line[i + 2] ? 2 : 0) + (line[i + 3] ? 1 : 0);
                  for (int pixel=0;pixel<pixels_per_dot*2;pixel++) {
                    if (!optionsScreenBlank(x, y))
                      vga.dotFast(x, y, getDoubleHiresColor(color));
                    x++;
                  }
                  
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
                    for (int pixel=0;pixel<pixels_per_dot;pixel++) {
                      if (!optionsScreenBlank(x, y))
                        vga.dotFast(x, y, getHiresColor(pixels[id]));
                      x++;
                    }
                  }
                  std::copy(std::begin(blockline), std::end(blockline), std::begin(blocklineAnt));
                }
                else
                {
                  for (int i = 7; i > 0; i--)
                  {
                    int color = 0;

                    if (blockline[i])
                      color = 0b11111111;
                    else
                      color = 0;

                    for (int pixel=0;pixel<pixels_per_dot;pixel++) {
                      if (!optionsScreenBlank(x, y))
                        vga.dotFast(x, y, color);
                        // sprintf(buf, "%04x: %02x",,chr);
                        // printLog(buf);
                        x++;
                    }
                  }
                }
              }
              x = margin_x;
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
                for (int pixel=0;pixel<pixels_per_dot;pixel++) {
                  if (!optionsScreenBlank(x, y))
                    vga.dotFast(x, y, bpixel ? (inverted ? 0 : 0xffffffff) : (inverted ? 0xffffffff : 0));
                  x++;
                }
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
              for (int k = 0; k < 7; k++)
              {
                ushort addr = (chr * 7 * 8) + (i * 7) + k;
                bool bpixel = AppleIIeFontPixels[addr];
                if (!optionsScreenBlank(x, y))
                  vga.dotFast(x, y, bpixel ? vga.rgb(255,255,255) : vga.rgb(0,0,0));
                  x++;
              }
            }
            x = margin_x;
            y++;
          
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