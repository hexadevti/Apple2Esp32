void videoSetup()
{
  printlog("Video Setup...");
  Mode myMode = vga.MODE320x240;
  vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
  vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  printMsg("APPLE2ESP32");
  textLoResRender(false);
  xTaskCreatePinnedToCore(flashCharacters, "flashCharacters", 4096, NULL, 1, NULL, 1);
}

void printMsg(char msg[]) {
  vga.setFont(Font6x8);
  vga.setTextColor(vga.RGB(0xff0000), vga.RGB(0));
  vga.setCursor(5, 10);
  vga.print("                                                  ");
  vga.setCursor(5, 10);
  vga.print(msg);
  vga.setFont(AppleFont_7x8);
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
}

void flashCharacters(void *pvParameters)
{
  bool inversed = false;
  while (true)
  {
    if (Graphics_Text)
    {

      int margin_x = 14;
      int margin_y = 24;
      int x = margin_x;
      int y = margin_y;

      ushort graphicsPage = Page1_Page2 ? 0x2000 : 0x4000;
      for (int b = 0; b < 3; b++)
      {
        for (int l = 0; l < 8; l++)
        {
          for (int block = 0; block < 8; block++)
          {
            bool blocklineAnt[] = {false, false, false, false, false, false, false, false};
            for (ushort c = 0; c < 0x28; c++)
            {
              char chr = ram[(ushort)(((graphicsPage) + (b * 0x28) + (l * 0x80) + c) + block * 0x400)];
              bool blockline[8];
              for (int i = 0; i < 8; i++)
                blockline[7-i] = (chr & (1 << i)) != 0;
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
                  vga.dotFast(x, y, pixels[id]);
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

void textLoResWrite(short address, unsigned char value)
{
  short addr = convertVideo[address - 0x400];
  ram[0x400 + addr] = value;
  int y = floor(addr / 0x28);
  int x = addr - y * 0x28;
  vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
  vga.setCursor((x + 2) * 7, (y + 3) * 8);
  vga.print((char)value);
  if (serialVideoAttached)
    textLoResSerialWrite(address, value);

  // ram[0x400 + convertVideo[address-0x400]] = value;
  // sprintf(buf, "%03x: %02x, ", convertVideo[address-0x400], value);
  // log("x=");
  // log(x);
  // log(", y=");
  // log(y);
  // log(" > ");
  // log(value);
  // log("'");
  // log((char)value);
  // log("'");
  // log(",");
  // log(value2);
  // log("'");
  // log((char)value2);
  // log("'");
}

void textLoResRender(bool inversed)
{

  for (int y = 0; y < 24; y++)
  {
    for (int x = 0; x < 40; x++)
    {
      char value = ram[0x400 + y * 40 + x];
      if (value >= 0x40 && value < 0x80)
      {
        if (inversed)
          vga.setTextColor(vga.RGB(0), vga.RGB(0xffffff));
        else
          vga.setTextColor(vga.RGB(0xffffff), vga.RGB(0));
        vga.setCursor((x + 2) * 7, (y + 3) * 8);
        vga.print((char)value);
      }
    }
  }
}
