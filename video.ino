#define TFT

#ifdef TFT

const uint16_t colors[8] = {TFT_BLACK, TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_BLACK, tft.color565(255, 20, 0), TFT_SKYBLUE, TFT_WHITE};
const uint16_t colors16[16] = {tft.color565(0, 0, 0), tft.color565(147, 11, 124), tft.color565(98, 76, 0), tft.color565(249, 86, 29),
                                       tft.color565(0, 118, 12), tft.color565(126, 126, 126), tft.color565(67, 200, 0), tft.color565(220, 205, 22),
                                       tft.color565(31, 53, 211), tft.color565(187, 54, 255), tft.color565(126, 126, 126), tft.color565(255, 129, 236),
                                       tft.color565(7, 168, 224), tft.color565(157, 172, 255), tft.color565(93, 247, 132), tft.color565(255, 255, 255)};
#elif defined(TFT_S3)
const uint16_t colors[8] = {gfx->color565(0, 0, 0), gfx->color565(0, 255, 0), gfx->color565(255, 0, 255), gfx->color565(255, 255, 255), 
                                       gfx->color565(0, 0, 0), gfx->color565(255, 0, 0), gfx->color565(0, 0, 255), gfx->color565(255, 255, 255)};
const uint16_t colors16[16] = {gfx->color565(0, 0, 0), gfx->color565(147, 11, 124), gfx->color565(98, 76, 0), gfx->color565(249, 86, 29),
                                       gfx->color565(0, 118, 12), gfx->color565(126, 126, 126), gfx->color565(67, 200, 0), gfx->color565(220, 205, 22),
                                       gfx->color565(31, 53, 211), gfx->color565(187, 54, 255), gfx->color565(126, 126, 126), gfx->color565(255, 129, 236),
                                       gfx->color565(7, 168, 224), gfx->color565(157, 172, 255), gfx->color565(93, 247, 132), gfx->color565(255, 255, 255)};
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
                             0b00100010, // 4 brown
                             0b10100100, // 5 cinza escuro
                             0b00010000, // 6 Green mid *
                             0b00111000, // 7 Green bright
                             0b10001010, // 8 Violeta
                             0b01000010, // 9 violet2
                             0b01011011, // 10 gray bright
                             0b11010010, // 11 Blue Bright
                             0b00000111, // 12 Orange bright
                             0b01010111, // 13 dark pink
                             0b00111111, // 14 yellow
                             0b11111111}; // 15 white

const uint8_t colorsLo[] = { 0b00000000, // 0000 0 black
                             0b10001010, // 1000 8 magenta
                             0b10000000, // 0001 1 dark blue
                             0b01000010, // 1001 9 violet2
                             0b00100000, // 0010 2 dark green *
                             0b01011011, // 1010 10 gray bright
                             0b01000000, // 0011 3 mid blue
                             0b11010010, // 1011 11 Blue Bright
                             0b00001100, // 0100 4 brown
                             0b00000111, // 1100 12 Orange bright
                             0b10100100, // 0101 5 cinza escuro
                             0b01010111, // 1101 13 dark pink
                             0b00010000, // 0110 6 Green mid *
                             0b00111111, // 1110 14 yellow
                             0b00111000, // 0111 7 Green bright
                             0b11111111};// 1111 15 white

  
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
  #ifdef TFT_S3
    gfx->begin();
    pinMode(1, OUTPUT);
    digitalWrite(1, HIGH);
    gfx->fillScreen(BLACK);
  #else

  const PinConfig pins(-1, -1, -1, RED0_PIN, RED1_PIN, 
                       -1, -1, -1, -1, GREEN0_PIN, GREEN1_PIN,
                       -1, -1, -1, BLUE0_PIN, BLUE1_PIN, 
                       HSYNC_PIN,VSYNC_PIN);
  Mode mode(16, 96, 48, 640, 4, 2, 30, 240, 23760000, 0, 0, 2);
  while (!vga.init(pins, mode, 8, 3))
    delay(10);

  printLog("Video initialized.");
  vga.show();
  vga.start();
  vga.fillRect(0,0,320,240,0);
  #endif
#endif
  xTaskCreate(renderLoop, "renderLoop", 4096, NULL, 1, NULL);
}

int red(int color) {
  return (color & 0xf800) >> 8;
}
int green(int color) {
  return (color & 0x7e0) >> 3;
}
int blue(int color) {
  return (color & 0x1f) << 3;
}

#ifdef TFT_S3
int avarage(int color1, int color2) {
  return gfx->color565((red(color1)+red(color2))/2,(green(color1)+green(color2))/2,(blue(color1)+blue(color2))/2);
}

void colorTest() {
  for (int i = 0; i < 7; i++)
  {
    int color1 = colors[i];
    Serial.printf("Orginal color%04X\n",color1);
    Serial.printf("RGB color %d, %d, %d\n",red(color1), green(color1), blue(color1));
    Serial.printf("reconverted color %04X\n",gfx->color565(red(color1),green(color1),blue(color1)));
  }
}
#endif
bool inversed = false;
float screen_width = 280;
float screen_height = 192;
uint16_t last_y = 0;
uint16_t last_x = 0;

void renderLoop(void *pvParameters)
{
  
  while (running)
  {
    Vertical_blankingOn_Off = false;
    unsigned long startTime = millis();
    page_lock.lock();

#ifdef TFT
    // 320x240 TFT: center the 280x192 raster (overriding the S3/VGA margins below).
    margin_x = 20;
    margin_y = 24;
    screen_width = 280;
    screen_height = 192;
#else
    if (upscale) {
      margin_x = 0;
      margin_y = 0;
      screen_width = 480;
      screen_height = 272;
    } else {
      margin_x = 100;
      margin_y = 40;
      screen_width = 280;
      screen_height = 192;
    }
#endif
    last_y = margin_y;
    last_x = margin_x;

    float coef192 = screen_height / 192;
    float coef560 = screen_width / 560;
    float coef280 = screen_width / 280;
    float coef140 = screen_width / 140;

    #ifdef TFT
    if (!OptionsWindow && AppleIIe && !Cols40_80 && !DHiResOn_Off)
    tft.setAddrWindow(0, margin_y, 320, 192); // Set the area to draw
    else if (!OptionsWindow && AppleIIe && DHiResOn_Off && !videoColor)
    tft.setAddrWindow(margin_x, margin_y, 280, 192); // DHiRes mono (centered 280)
    else if (OptionsWindow || clearScr)
    tft.setAddrWindow(2, 0, 315, 240);
    else
    tft.setAddrWindow(margin_x, margin_y, 280, 192);
    tft.startWrite();
    #endif
    
    #ifdef TFT_S3
    gfx->startWrite();
    #endif
    

    int x = margin_x;
    int y = margin_y;
    int x_upscaled = margin_x;
    int y_upscaled = margin_y;
    ushort textPage = Page1_Page2 ? 0x400 : 0x800;
    ushort graphicsPage = Page1_Page2 ? 0x2000 : 0x4000;
    unsigned long endTime1 = millis();
    // if (demo) {
    // y=0;
    //   for (int v = 0; v < 30; v++)
    //   {
    //     for (int i = 0; i < 8; i++) // char lines
    //     {
    //       x=0;
    //       for (int h = 0; h < 45; h++)
    //       {
    //         uint8_t chr = menuScreen[v * 45 + h];
    //         for (int c = 0; c < 7; c++) // char cols
    //         {
    //           bool bpixel = AppleIIeFontPixels[(chr*7*8) + (i * 7) + c];
    //           uint8_t color = menuColor[v * 45 + h];
    //           uint8_t fgColor = 0xff;
    //           uint8_t bgColor = color;
    //           #ifdef TFT
    //           tft.writeColor((bpixel ? fgColor : bgColor), 1);
    //           #else
    //           vga.dotFast(x, y, bpixel ? fgColor : bgColor);
    //           x++;
    //           vga.dotFast(x, y, bpixel ? fgColor : bgColor);
    //           #endif
    //           x++;
    //         }
    //       }
    //       y++;
    //     }
    //   }
    // }
    if (clearScr) {
      y=0;
      #ifdef TFT_S3
      for (int y = 0; y < 272; y++)
      {
        for (int x = 0; x < 480; x++)
        {
          gfx->writePixelPreclipped(x, y, colors[0]);
        }
      }
      #else
      for (int v = 0; v < 30; v++)
      {
        for (int i = 0; i < 8; i++) // char lines
        {
          x=0;
          for (int h = 0; h < 45; h++)
          {
            for (int c = 0; c < 7; c++) // char cols
            {
              #ifdef TFT
              tft.writeColor(colors[0], 1);
              #else
                #ifdef TFT_S3
                  gfx->writePixelPreclipped(x, y, colors[0]);
                #else
                  vga.dotFast(x, y, colors[0]);
                  x++;
                  vga.dotFast(x, y, colors[0]);
                #endif
              #endif
              x++;
            }
          }
          y++;
        }
      }
      #endif
      clearScr = false;
    }
    else if (OptionsWindow || DebugWindow) 
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
                #ifdef TFT_S3
                  gfx->writePixelPreclipped(x, y, bpixel ? colors16[fgColor] : colors16[bgColor]);
                #else
                  vga.dotFast(x, y, bpixel ? colors16[fgColor] : colors16[bgColor]);
                  x++;
                  vga.dotFast(x, y, bpixel ? colors16[fgColor] : colors16[bgColor]);
                  x++;
                #endif
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
                // Init Upscale
                int repeat_y = 0;
                uint16_t upscaleCoef_y = floor(coef192 * (float)(y+1));
                uint16_t repeatTimes_y = upscaleCoef_y - last_y;
                last_y = upscaleCoef_y;
                while (repeat_y < repeatTimes_y)
                {
                  x_upscaled = margin_x;
                  x = margin_x;
                  last_x = margin_x;
                  uint16_t lastPixel = 0;
                  bool lastLine = false;
                  bool lastCol = false;
                  // End Upscale
                  for (int c = 0; c < 0x28; c++)
                  {
                    for (int k = 0; k < 7; k++)
                    {
                      char value = ram[(textPage + (b * 0x28) + (l * 0x80) + c)];
                      int firstColor = (value & 0b11110000) >> 4;
                      int secondColor = value & 0b00001111;
                      #ifdef TFT
                        if (j < 4)
                          tft.writeColor(colors16[secondColor], 1);
                        else
                          tft.writeColor(colors16[firstColor], 1);
                      #else
                        #ifdef TFT_S3
                          
                        // Init Upscale
                        int repeat_x = 0;
                        uint16_t upscaleCoef_x = floor(coef280 * (float)(x+1));
                        uint8_t repeatTimes_x = upscaleCoef_x - last_x;
                        last_x = upscaleCoef_x;
                        while (repeat_x < repeatTimes_x)
                        {
                            uint16_t actualPixel = (j < 4 ? colors16[secondColor] : colors16[firstColor]);
                            gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                            repeat_x++;
                            x_upscaled++;
                          }  
                          // End upscale pixel
                        #else
                          if (j < 4)
                          {
                          vga.dotFast(x, y, colors16[secondColor]);
                          x++;
                          vga.dotFast(x, y, colors16[secondColor]);
                          }
                          else
                          {
                            vga.dotFast(x, y, colors16[firstColor]);
                            x++;
                            vga.dotFast(x, y, colors16[firstColor]);
                          }
                        #endif
                      #endif
                      
                      x++;
                    }
                  }
                  repeat_y++;
                  y_upscaled++;
                }
                y++;
              }
            }
            else if (DHiResOn_Off)
            {
              static bool line[0x50 * 7]; // 560 DHGR bits; static avoids per-scanline malloc churn

              for (int block = 0; block < 8; block++)
              {
                // Init Upscale
                int repeat_y = 0;
                uint16_t upscaleCoef_y = floor(coef192 * (float)(y+1));
                //Serial.printf("upscaleCoef_y=%d screen_width=%f (float)(y+1)=%f last_y=%d\n",upscaleCoef_y, screen_width, (float)(y+1), last_y);
                uint16_t repeatTimes_y = upscaleCoef_y - last_y;
                //Serial.printf("y=%d repeatTimes_y=%d\n", y, repeatTimes_y);
                last_y = upscaleCoef_y;
                while (repeat_y < repeatTimes_y)
                {
                  int lineId = 0;
                  x_upscaled = margin_x;
                  x = margin_x;
                  last_x = margin_x;
                  uint16_t lastPixel = 0;
                  bool lastLine = false;
                  bool lastCol = false;
                  char bottomChr;
                  // End Upscale
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

                    bool fillLine = videoColor;
                    #ifdef TFT
                    fillLine = true; // TFT renders both color & mono from line[] after the row
                    #endif
                    if (fillLine)
                    {
                      for (int i = 7; i > 0; i--)
                      {
                        *(line + lineId) = blockline[i];
                        lineId++;
                      }
                    }
                    else
                    {
                      for (int i = 7; i > 0; i--)
                      {
                        // Init Upscale
                        int repeat_x = 0;
                        uint16_t upscaleCoef_x = floor(coef560 * (float)(x+1));
                        //Serial.printf("upscaleCoef_x=%d screen_width=%f (float)(x+1)=%f last_x=%d\n",upscaleCoef_x, screen_width, (float)(x+1), last_x);
                        bool downScale = upscaleCoef_x < last_x;
                        uint8_t repeatTimes_x = upscaleCoef_x - last_x;
                        //Serial.printf("x=%d repeatTimes_x=%d\n", x, repeatTimes_x);
                        last_x = upscaleCoef_x;
                        while (repeat_x < repeatTimes_x)
                        {
                        #ifdef TFT_S3
                          uint16_t actualPixel = blockline[i] ? gfx->color565(255, 255, 255) : gfx->color565(0, 0, 0);
                          if (downScale)
                            break;
                          gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                          x_upscaled++;
                        #endif
                          repeat_x++;
                        }
                        x++;
                      }
                    }
                  }
                  #ifdef TFT
                  if (videoColor) {
                    // 140 DHGR color pixels (4 bits each) x2 = 280 px to match the window
                    for (int i = 0; i < 0x50 * 7; i += 4) {
                      int color = (line[i] ? 8 : 0) + (line[i + 1] ? 4 : 0) + (line[i + 2] ? 2 : 0) + (line[i + 3] ? 1 : 0);
                      tft.writeColor(colors16[color], 2);
                    }
                  } else {
                    // 560 mono DHGR bits downsampled 2:1 = 280 px to match the window
                    for (int i = 0; i < 0x50 * 7; i += 2)
                      tft.writeColor(line[i] ? TFT_WHITE : TFT_BLACK, 1);
                  }
                  #else
                  if (videoColor) {
                    for (int i = 0; i < 0x50 * 7; i = i + 4)
                    {
                      int color = (line[i] ? 8 : 0) + (line[i + 1] ? 4 : 0) + (line[i + 2] ? 2 : 0) + (line[i + 3] ? 1 : 0);

                      // Init Upscale
                        int repeat_x = 0;
                        uint16_t upscaleCoef_x = floor(coef140 * (float)(x+1));
                        //Serial.printf("upscaleCoef_x=%d screen_width=%f (float)(x+1)=%f last_x=%d\n",upscaleCoef_x, screen_width, (float)(x+1), last_x);
                        uint8_t repeatTimes_x = upscaleCoef_x - last_x;
                        if (repeatTimes_x > 10)
                          repeatTimes_x = 1;
                        //Serial.printf("x=%d repeatTimes_x=%d\n", x, repeatTimes_x);
                        last_x = upscaleCoef_x;
                        while (repeat_x < repeatTimes_x)
                        {
                        #ifdef TFT_S3
                          uint16_t actualPixel = colors16[color];
                          //Serial.printf("x=%d, y=%d\n", x_upscaled, y_upscaled);
                          gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                          x_upscaled++;
                        #endif
                          repeat_x++;
                        }
                        x++;
                    }
                  }
                  #endif
                  repeat_y++;
                  y_upscaled++;
                }
                y++;
              }

            }
            else // hires
            {
              for (int block = 0; block < 8; block++)
              {
                // Init Upscale
                int repeat_y = 0;
                uint16_t repeatTimes_y = 1;
                uint16_t upscaleCoef_y = floor(coef192 * (float)(y+1));
                repeatTimes_y = upscaleCoef_y - last_y;
                last_y = upscaleCoef_y;
                while (repeat_y < repeatTimes_y)
                {
                  x_upscaled = margin_x;
                  x = margin_x;
                  last_x = margin_x;
                  uint16_t lastPixel = 0;
                  bool lastLine = false;
                  bool lastCol = false;
                  // End Upscale
                  for (ushort c = 0; c < 0x28; c++)
                  {
                    if (c == 0x27)
                      lastCol = true;
                    char chr;
                    char prevChr;
                    char pixels[7];
                    char chrBottom;
                    char prevChrBottom;
                    char pixelsBottom[7];
                    if (smoothUpscale) {
                      if (videoColor && repeat_y > 0) {
                        int blockb = block;
                        int lb = l;
                        int bb = b;
                        blockb++;
                        if (blockb == 8) { blockb=0; lb++; }
                        if (lb == 8) { lb=0; bb++; }
                        if (bb == 3) { bb=0; lastLine = true; }

                        chrBottom = ram[(ushort)(((graphicsPage) + (bb * 0x28) + (lb * 0x80) + c) + blockb * 0x400)];
                        if (c % 2 == 0) // Odd
                        {
                          pixelsBottom[0] = (chrBottom & 0x80) >> 5 | (chrBottom & 1) << 1 | (prevChrBottom & 0x40) >> 6;
                          pixelsBottom[1] = (chrBottom & 0x80) >> 5 | (chrBottom & 1) << 1 | (chrBottom & 0x2) >> 1;
                          pixelsBottom[2] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x4) >> 1 | (chrBottom & 0x2) >> 1;
                          pixelsBottom[3] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x4) >> 1 | (chrBottom & 0x8) >> 3;
                          pixelsBottom[4] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x10) >> 3 | (chrBottom & 0x8) >> 3;
                          pixelsBottom[5] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x10) >> 3 | (chrBottom & 0x20) >> 5;
                          pixelsBottom[6] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x40) >> 5 | (chrBottom & 0x20) >> 5;
                        }
                        else // Even
                        {
                          pixelsBottom[0] = (chrBottom & 0x80) >> 5 | (prevChrBottom & 0x40) >> 5 | (chrBottom & 0x1); 
                          pixelsBottom[1] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x2) | (chrBottom & 0x1);
                          pixelsBottom[2] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x2) | (chrBottom & 0x4) >> 2;
                          pixelsBottom[3] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x8) >> 2 | (chrBottom & 0x4) >> 2;
                          pixelsBottom[4] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x8) >> 2 | (chrBottom & 0x10) >> 4;
                          pixelsBottom[5] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x20) >> 4 | (chrBottom & 0x10) >> 4;
                          pixelsBottom[6] = (chrBottom & 0x80) >> 5 | (chrBottom & 0x20) >> 4 | (chrBottom & 0x40) >> 6;
                          
                        }
                      }
                    }
                    
                    chr = ram[(ushort)(((graphicsPage) + (b * 0x28) + (l * 0x80) + c) + block * 0x400)];

                    if (videoColor)
                    {
                      char pixels[7];
                      if (c % 2 == 0) // Odd
                      {
                        pixels[0] = (chr & 0x80) >> 5 | (chr & 1) << 1 | (prevChr & 0x40) >> 6;
                        pixels[1] = (chr & 0x80) >> 5 | (chr & 1) << 1 | (chr & 0x2) >> 1;
                        pixels[2] = (chr & 0x80) >> 5 | (chr & 0x4) >> 1 | (chr & 0x2) >> 1;
                        pixels[3] = (chr & 0x80) >> 5 | (chr & 0x4) >> 1 | (chr & 0x8) >> 3;
                        pixels[4] = (chr & 0x80) >> 5 | (chr & 0x10) >> 3 | (chr & 0x8) >> 3;
                        pixels[5] = (chr & 0x80) >> 5 | (chr & 0x10) >> 3 | (chr & 0x20) >> 5;
                        pixels[6] = (chr & 0x80) >> 5 | (chr & 0x40) >> 5 | (chr & 0x20) >> 5;
                      }
                      else // Even
                      {
                        pixels[0] = (chr & 0x80) >> 5 | (prevChr & 0x40) >> 5 | (chr & 0x1); 
                        pixels[1] = (chr & 0x80) >> 5 | (chr & 0x2) | (chr & 0x1);
                        pixels[2] = (chr & 0x80) >> 5 | (chr & 0x2) | (chr & 0x4) >> 2;
                        pixels[3] = (chr & 0x80) >> 5 | (chr & 0x8) >> 2 | (chr & 0x4) >> 2;
                        pixels[4] = (chr & 0x80) >> 5 | (chr & 0x8) >> 2 | (chr & 0x10) >> 4;
                        pixels[5] = (chr & 0x80) >> 5 | (chr & 0x20) >> 4 | (chr & 0x10) >> 4;
                        pixels[6] = (chr & 0x80) >> 5 | (chr & 0x20) >> 4 | (chr & 0x40) >> 6;
                      }

                      for (int id = 0; id < 7; id++)
                      { 
                        #ifdef TFT
                        tft.writeColor(colors[pixels[id]], 1);
                        #else
                          #ifdef TFT_S3
                            // Init upscale pixel 280px -> 480px
                            
                            int repeat_x = 0;
                            uint8_t repeatTimes_x = 1;
                            uint16_t upscaleCoef_x = floor(coef280 * (float)(x+1));
                            repeatTimes_x = upscaleCoef_x - last_x;
                            last_x = upscaleCoef_x;
                            while (repeat_x < repeatTimes_x)
                            {
                              uint16_t actualPixel = colors[pixels[id]];
                              if (smoothUpscale) {
                                if (c == 0x27 && id == 6)
                                  lastCol = true;
                                if (!lastCol)
                                  actualPixel = avarage(actualPixel,lastPixel);
                                if (!lastLine && repeat_y > 0) {
                                  uint16_t bottomPixel = colors[pixelsBottom[id]];
                                  actualPixel = avarage(actualPixel,bottomPixel);
                                }
                                lastPixel = colors[pixels[id]];
                              }
                              gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                              repeat_x++;
                              x_upscaled++;
                            }  
                          #else
                            vga.dotFast(x, y, colors[pixels[id]]);
                            x++;
                            vga.dotFast(x, y, colors[pixels[id]]);
                          #endif
                        #endif
                        x++;
                      }
                      prevChr = chr;
                      if (smoothUpscale) {
                        if (repeat_y > 0) {
                          prevChrBottom = chrBottom;
                        }
                      }
                    }
                    else
                    {
                      bool blockline[8];
                      for (int i = 0; i < 8; i++)
                        blockline[7 - i] = (chr & (1 << i)) != 0;
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
                          
                        // uint16_t color = colors[0];
                        // if (blockline[i])
                        //   color = colors[7];
                        // else
                        //   color = colors[0];
                        //   #ifdef TFT_S3
                        //     gfx->writePixelPreclipped(x, y, color);
                        //   #else
                        //     vga.dotFast(x, y, color);
                        //     x++;
                        //     vga.dotFast(x, y, color);
                        //   #endif
                        #endif
                        x++;
                      }
                    }
                  }
                  repeat_y++;
                  y_upscaled++;
                }
                y++;
              }
            }
          }
          else if (Cols40_80) // Text modes
          {
            for (int i = 0; i < 8; i++) // char lines
            {
              // Init Upscale
              int repeat_y = 0;
              uint16_t repeatTimes_y = 1;
              uint16_t upscaleCoef_y = floor(coef192 * (float)(y+1));
              repeatTimes_y = upscaleCoef_y - last_y;
              last_y = upscaleCoef_y;
              while (repeat_y < repeatTimes_y)
              {
                x_upscaled = margin_x;
                x = margin_x;
                last_x = margin_x;
                uint16_t lastPixel = 0;
                bool lastLine = false;
                bool lastCol = false;
                // End Upscale

                for (int c = 0; c < 0x28; c++)
                {
                  for (int k = 0; k < 7; k++)
                  {
                    // Init Upscale
                    bool bbPixel = 0;
                    if (smoothUpscale) {
                      if (c == 0x27 && k == 6)
                        lastCol = true;
                      if (repeat_y > 0) {
                        int ib = i;
                        int lb = l;
                        int bb = b;
                        ib++;
                        if (ib == 8) { ib=0; lb++; }
                        if (lb == 8) { lb=0; bb++; }
                        if (bb == 3) { bb=0; lastLine = true; }

                        char bottomChr = ram[(ushort)(textPage + (bb * 0x28) + (lb * 0x80) + c)];
                        
                        ushort bottomAddr = (bottomChr * 7 * 8) + (ib * 7) + k;
                        bbPixel = AppleIIe ? AppleIIeFontPixels[bottomAddr] : AppleFontPixels[bottomAddr];
                      }
                    }
                    // End Upscale
                    char chr = ram[(ushort)(textPage + (b * 0x28) + (l * 0x80) + c)];
                    ushort addr = (chr * 7 * 8) + (i * 7) + k;
                    bool bpixel = AppleIIe ? AppleIIeFontPixels[addr] : AppleFontPixels[addr];
                    bool inverted = false;
                    if (!AppleIIe)
                      inverted = chr >= 0x40 && chr < 0x80 && inversed;
                    #ifdef TFT
                      tft.writeColor(bpixel ? (inverted ? TFT_BLACK : TFT_WHITE) : (inverted ? TFT_WHITE : TFT_BLACK), 1);
                    #else
                      #ifdef TFT_S3
                        // Init Upscale
                        int repeat_x = 0;
                        uint8_t repeatTimes_x = 1;
                        uint16_t upscaleCoef_x = floor(coef280 * (float)(x+1));
                        repeatTimes_x = upscaleCoef_x - last_x;
                        last_x = upscaleCoef_x;
                        while (repeat_x < repeatTimes_x)
                        {
                          uint16_t actualPixel = bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]);
                          if (smoothUpscale) {
                            if (!lastCol)
                              actualPixel = avarage(actualPixel,lastPixel);
                            if (!lastLine && repeat_y > 0) {
                              uint16_t bottomPixel = bbPixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]);
                              actualPixel = avarage(actualPixel,bottomPixel);
                            }
                            lastPixel = bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]);
                          }
                          gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                          repeat_x++;
                          x_upscaled++;
                        }
                      #else
                      vga.dotFast(x, y, bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]));
                      x++;
                      vga.dotFast(x, y, bpixel ? (inverted ? colors[0] : colors[7]) : (inverted ? colors[7] : colors[0]));
                      #endif
                    #endif
                    x++;
                  }
                }
                repeat_y++;
                y_upscaled++;
              }
              y++;
            }
          }
          else if (AppleIIe && !Cols40_80)
          {
            for (int i = 0; i < 8; i++)
            {
              // Init Upscale
              int repeat_y = 0;
              uint16_t upscaleCoef_y = floor(coef192 * (float)(y+1));
              uint16_t repeatTimes_y = upscaleCoef_y - last_y;
              last_y = upscaleCoef_y;
              while (repeat_y < repeatTimes_y)
              {
                x_upscaled = margin_x;
                x = margin_x;
                last_x = margin_x;
                uint16_t lastPixel = 0;
                bool lastLine = false;
                bool lastCol = false;
                char bottomChr;
                // End Upscale
                for (int j = 0; j < 0x50; j++)
                {
                  // Init Upscale
                  bool bbPixel = 0;
                  int ib = i;
                  if (smoothUpscale) {
                    if (j == 0x4f)
                      lastCol = true;
                    int lb = l;
                    int bb = b;
                    if (repeat_y > 0) {
                      ib++;
                      if (ib == 8) { ib=0; lb++; }
                      if (lb == 8) { lb=0; bb++; }
                      if (bb == 3) { bb=0; lastLine = true; }

                      if (j % 2 == 0)
                      {
                        bottomChr = auxram[0, (ushort)(0x400 + (bb * 0x28) + (lb * 0x80) + j / 2)];
                      }
                      else
                      {
                        bottomChr = ram[(ushort)(0x400 + (bb * 0x28) + (lb * 0x80) + (j - 1) / 2)];
                      }
                      
                    }
                  }
                  // End Upscale

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
                    #ifdef TFT
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
                    if (smoothUpscale) {
                      ushort bottomAddr = (bottomChr * 7 * 8) + (ib * 7) + k;
                      bbPixel = AppleIIeFontPixels[bottomAddr];
                      }

                    // Init Upscale
                    int repeat_x = 0;
                    uint16_t upscaleCoef_x = floor(coef560 * (float)(x+1));
                    //Serial.printf("upscaleCoef_x=%d screen_width=%f (float)(x+1)=%f last_x=%d\n",upscaleCoef_x, screen_width, (float)(x+1), last_x);
                    bool downScale = upscaleCoef_x < last_x;
                    uint8_t repeatTimes_x = upscaleCoef_x - last_x;
                    //Serial.printf("x=%d repeatTimes_x=%d\n", x, repeatTimes_x);
                    last_x = upscaleCoef_x;
                    while (repeat_x < repeatTimes_x)
                    {
                      lastPixel = bpixel ? colors[7] : colors[0];
                      uint16_t actualPixel = lastPixel;
                      if (downScale)
                        break;  
                      if (smoothUpscale) {
                        if (!lastCol)
                          actualPixel = avarage(actualPixel,lastPixel);
                        //Serial.printf("lastLine=%s repeat_y=%d, repeat_x=%d\n", lastLine ? "T" : "F", repeat_y, repeat_x);
                        //Serial.printf("x_upscaled=%d y_upscaled=%d\n", x_upscaled, y_upscaled);
                        if (!lastLine && repeat_y > 0) {
                          uint16_t bottomPixel = bbPixel ? colors[7] : colors[0];
                          actualPixel = avarage(actualPixel,bottomPixel);
                        }
                      }
                      #ifdef TFT_S3
                        gfx->writePixelPreclipped(x_upscaled, y_upscaled, actualPixel);
                      #else
                        vga.dotFast(x, y, bpixel ? colors[7] : colors[0]);
                      #endif
                      repeat_x++;
                      x_upscaled++;
                    }
                    x++;
                    #endif
                  }
                }
                repeat_y++;
                y_upscaled++;
              }
              y++;
            }
          }
        }
      }
    }
    
    unsigned long endTime2 = millis();
    #ifdef TFT
    tft.endWrite();
    #endif
    #ifdef TFT_S3
    gfx->endWrite();
    #endif
    Vertical_blankingOn_Off = true;
    page_lock.unlock();
    unsigned long endTime3 = millis();
    vTaskDelay(pdMS_TO_TICKS(5));
    unsigned long endTime4 = millis();
    #ifdef TFT
    tft.invertDisplay(true);
    #endif
    #ifdef TFT_S3
      gfx->flush();
    #endif
    flashCount++;
    if (flashCount > 7)
    {
      inversed = !inversed;
      flashCount = 0;
    }
    unsigned long endTime5 = millis();
    // Calculate and print the duration
    unsigned long duration1 = endTime1 - startTime;
    unsigned long duration2 = endTime2 - endTime1;
    unsigned long duration3 = endTime3 - endTime2;
    unsigned long duration4 = endTime4 - endTime3;
    unsigned long duration5 = endTime5 - endTime4;
    
    unsigned long duration = endTime5 - startTime;

  Serial.printf("Execution time: %d %d %d %d %d total: %d\n", duration1, duration2, duration3, duration4, duration5, duration);
  
  }
}
