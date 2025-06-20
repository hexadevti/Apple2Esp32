const uint16_t colors[] PROGMEM = {TFT_BLACK, TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_BLACK, tft.color565(255,20,0), TFT_SKYBLUE, TFT_WHITE};
const int colors16[] PROGMEM = {0x000000, 0x3300dd, 0x990000, 0xdd22dd, 0x227700, 0x444444, 0xff2222, 0xcc8844, 0x004477, 0x0044dd, 0x888888, 0x6677dd, 0x00dd11, 0x00ffff, 0x88ee33, 0xffffff};
int flashCount = 0;
int width = 280;
int height = 192;

void videoSetup()
{
  printlog("Video Setup...");
  tft.begin();        /*初始化*/
  tft.setRotation(1); /* 旋转 */
  tft.invertDisplay(true);
  tft.initDMA();      /* 初始化DMA */
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);

  tft.fillRect(20, 24, 280, 192, TFT_BLACK);

  tft.setCursor(10, 0);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //tft.println("Apple //e");

  printMsg("APPLE2ESP32", TFT_BLUE);
  xTaskCreate(graphicFlashCharacters, "graphicFlashCharacters", 1024, NULL, 1, NULL);
}

void printMsg(char msg[], uint16_t color)
{
  // tft.setTextFont(1);
  // tft.setTextSize(1);
  // tft.setTextColor(color, TFT_BLACK);
  // tft.setCursor(10, 0);
  // tft.print("                                                                         ");
  // tft.setCursor(10, 0);
  // tft.println(msg);
}

void printStatus(char msg[], int color)
{
  // // vga.setFont(Font6x8);
  // // vga.setTextColor(// vga.RGB(color), // vga.RGB(0));
  // // vga.setCursor(5, 224);
  // // vga.print("                                                                   ");
  // // vga.setCursor(5, 224);
  // // vga.print(msg);
  // // vga.setFont(AppleIIe ? AppleIIeFont_7x8 : AppleFont_7x8);
  // // vga.setTextColor(// vga.RGB(0xffffff), // vga.RGB(0));
}

void graphicFlashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (true)
  {
    page_lock.lock();
    tft.setAddrWindow(margin_x, margin_y, 280, 192); // Set the area to draw
    
    tft.startWrite();
    
    int x = margin_x;
    int y = margin_y;
    Vertical_blankingOn_Off = false; // IIe video problem with Total Replay
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
                      tft.pushColor(colors16[secondColor], 1);
                    //  vga.dotFast(x, y, // vga.RGB(colors16[secondColor]));
                  }
                  else
                  {
                    if (!optionsScreenBlank(x,y))
                      tft.pushColor(colors16[firstColor], 1);
                    //  vga.dotFast(x, y, // vga.RGB(colors16[firstColor]));
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
                    if (!optionsScreenBlank(x, y))
                      tft.pushColor(colors[pixels[id]], 1);
                      // vga.dotFast(x, y, colors[pixels[id]]);
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
                      
                      if (!optionsScreenBlank(x, y)) {
                        tft.pushColor(color, 1);
                        //tft.drawPixel(x,y, color);
                      }

                      // vga.dotFast(x, y, color);

                      // sprintf(buf, "%04x: %02x",x,color);
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
                //if (!optionsScreenBlank(x, y)) {
                  //*pixel++ = bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK);
                  tft.pushColor(bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK), 1);
                //}
                  //tft.drawPixel(x,y, bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK));
                  // vga.dotFast(x, y, bpixel ? (inverted ? // vga.RGB(0) : // vga.RGB(0xffffff)) : (inverted ? // vga.RGB(0xffffff) : // vga.RGB(0)) );
                  x++;
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

bool optionsScreenBlank(int x, int y)
{
  if (OptionsWindow)
  {
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
  // vga.fillRect(40, 40, 240, 160, 0);
  // vga.rect(41, 41, 238, 158, // vga.RGB(color));
  // vga.setFont(Font6x8);
  // vga.setTextColor(// vga.RGB(color), // vga.RGB(0));
  // vga.setCursor(44, 188);
  sprintf(buf, " %s | %s | %s | %s | %s", HdDisk ? " HD " : "DISK", AppleIIe ? "IIe" : "II+", Fast1MhzSpeed ? "Fast" : "1Mhz", paused ? "Paused " : "Running", joystick ? "Joy On " : "Joy Off");
  // vga.print(buf);
}

void printOptionsText(const char *text)
{
  // vga.fillRect(42, 42, 236, 147, 0);
  // vga.setCursor(44, 44);
  // vga.setTextColor(// vga.RGB(0xffffff), // vga.RGB(0));
  // vga.print(text);
}

void printOptionsTextEx(char text[])
{
  // vga.setCursor(44, 44);
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.setTextColor(// vga.RGB(0), // vga.RGB(0xff0000));
  // vga.println("12345678901234567890123456789");
  // vga.setTextColor(// vga.RGB(0xff0000), // vga.RGB(0));
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
  // vga.println("12345678901234567890123456789");
}