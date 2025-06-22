void SDCardSetup()
{
  Serial.println("SD Card Setup");
  SPI.begin(sck, miso, mosi, cs);
  delay(500);
  int sdMountRetry = 0;
  while (!SD.begin(cs) && sdMountRetry < 10) {
    printLog("Card Mount Failed");
    delay(100);
    sdMountRetry++;
  }

  if (sdMountRetry == 10) {
    hdAttached = false;
    diskAttached = false;
    return;
  }
  

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    printLog("No SD card attached");
    return;
  }

  printLog("SD Card Type: ");
  if (cardType == CARD_MMC) {
    printLog("MMC");
  } else if (cardType == CARD_SD) {
    printLog("SDSC");
  } else if (cardType == CARD_SDHC) {
    printLog("SDHC");
  } else {
    printLog("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  sprintf(buf,"SD Card Size: %lluMB\n", cardSize);
  printLog(buf);
}