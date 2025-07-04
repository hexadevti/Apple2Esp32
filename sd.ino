void FSSetup()
{
   #ifdef SDFS
  Serial.println("SD Card Setup");
  SPI.begin(); //SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  delay(500);
  int sdMountRetry = 0;
  while (!FSTYPE.begin(SD_CS_PIN) && sdMountRetry < 10) {
    printLog("Card Mount Failed");
    delay(100);
    sdMountRetry++;
  }

  if (sdMountRetry == 10) {
    hdAttached = false;
    diskAttached = false;
    return;
  }
  
  uint8_t cardType = FSTYPE.cardType();

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
  
  uint64_t cardSize = FSTYPE.cardSize() / (1024 * 1024);
  sprintf(buf,"SD Card Size: %lluMB\n", cardSize);
  printLog(buf);
  #else
  Serial.println("LittleFS Setup");
  //SPI.begin();
  //delay(500);
  int sdMountRetry = 0;
  while (!FSTYPE.begin() && sdMountRetry < 10) {
    printLog("Little FS Failed");
    delay(100);
    sdMountRetry++;
  }

  if (sdMountRetry == 10) {
    hdAttached = false;
    diskAttached = false;
    return;
  }
  #endif
}