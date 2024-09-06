int colors[] = {0b000000, 0b001100, 0b110011, 0b111111, 0b000000, 0b000011, 0b100000, 0b111111};
int colors16[] = {0x000000, 0x3300dd, 0x990000, 0xdd22dd, 0x227700, 0x444444, 0xff2222, 0xcc8844, 0x004477, 0x0044dd, 0x888888, 0x6677dd, 0x00dd11, 0x00ffff, 0x88ee33, 0xffffff };

void videoSetup()
{
  printlog("Video Setup...");
  Mode myMode = vga.MODE320x240; //.custom(320,190);
  vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
  vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  printMsg("APPLE2ESP32", 0xff0000);
  textLoResRender(false);
  xTaskCreatePinnedToCore(graphicFlashCharacters, "graphicFlashCharacters", 4096, NULL, 1, NULL, 1);
}

void printMsg(char msg[], int color)
{
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(color), vga.RGB(0));
  vga.setCursor(5, 8);
  vga.print("                                                                   ");
  vga.setCursor(5, 8);
  vga.print(msg);
  vga.setFont(AppleFont_7x8);
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
  vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void graphicFlashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (true)
  {
    if (Graphics_Text)
    {
      std::lock_guard<std::mutex> lock(page_lock);
      int x = margin_x;
      int y = margin_y;
      int ytext=0;
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
                      vga.dotFast(x, y, vga.RGB(colors16[secondColor]));
                    }
                    else
                    {
                      vga.dotFast(x, y, vga.RGB(colors16[firstColor]));
                    }
                    x++;
                  }
                }
                y++;
              }
            }
            else
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
            for (int c = 0; c < 0x28; c++)
            {
              char value = ram[(textPage + (b * 0x28) + (l * 0x80) + c)];
              if (value >= 0x40 && value < 0x80)
              {
                  vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
              }
              else
              {
                  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
              }
              vga.setCursor((c + text_margin_x) * 7, (ytext + 20 + text_margin_y) * 8);
              vga.print((char)value);
            }
            ytext++;
          }
        }
      }
    }
    else
    {
      textLoResRender(inversed);
      delay(300);
      inversed = !inversed;
    }
  }
}

unsigned char textLoResRead(short address)
{
  return ram[0x400 + convertVideo[address - 0x400]];
}

void textLoResWrite(short address, unsigned char value, ushort pageAdress)
{
  short addr = convertVideo[address - pageAdress];
  int y = floor(addr / 0x28);
  int x = addr - y * 0x28;
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  vga.setCursor((x + text_margin_x) * 7, (y + text_margin_y) * 8);
  vga.print((char)value);
  if (serialVideoAttached)
    textLoResSerialWrite(address, value);
}

void textLoResRender(bool inversed)
{
  ushort textPage = Page1_Page2 ? 0x400 : 0x800;
  int y = 0;
  for (int b = 0; b < 3; b++)
  {
    for (int l = 0; l < 8; l++)
    {
      for (int c = 0; c < 0x28; c++)
      {
        char value = ram[(textPage + (b * 0x28) + (l * 0x80) + c)];
        if (value >= 0x40 && value < 0x80)
        {
          if (inversed)
            vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
          else
            vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
          vga.setCursor((c + text_margin_x) * 7, (y + text_margin_y) * 8);
          vga.print((char)value);
        }
      }
      y++;
    }
  }
}
