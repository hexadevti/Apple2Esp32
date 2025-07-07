
void wifiSetup()
{
#ifndef TFT
  if (!wifiConnected)
  {
    printLog("1");
    WiFi.begin(ssid, password);
    printLog("2");
    xTaskCreate(wifiConnect, "wifiConnect", 4096, NULL, 1, NULL);
  }
 #endif
}

#ifndef TFT
void deleteFile(fs::FS &fs, String filename)
{
  Serial.printf("Deleting file: %s\r\n", filename);
  if (!filename.startsWith("/"))
    filename = "/" + filename;
  if (fs.remove(filename))
  {
    Serial.println("- file deleted");
  }
  else
  {
    Serial.println("- delete failed");
  }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  int i = 0;
  String partlist;
  // Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      // printLog("  DIR : ");
      // Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      // printLog("  FILE: ");
      // printLog(file.name());
      // printLog("  SIZE: ");
      // Serial.println(file.size());
      i++;
      String st_after_symb = String(file.name()).substring(String(file.name()).indexOf("/") + 1);

      partlist += String("<tr><td>") + String(i) + String("</td><td>") + String("<a href='") + String(file.name()) + String("'>") + st_after_symb + String("</td><td>") + String(file.size() / 1024) + String("</td><td>") + String("<input type='button' class='btndel' onclick=\"deletef('") + String(file.name()) + String("')\" value='X'>") + String("</td></tr>");
      filelist = String("<table><tbody><tr><th>#</th><th>File name</th><th>Size(KB)</th><th></th></tr>") + partlist + String(" </tbody></table>");
    }
    freeSpace = FSTYPE.totalBytes() - FSTYPE.usedBytes();
    file = root.openNextFile();
  }
}

void wifiConnect(void *pvParameters)
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    printLog(".");
  }
  printLog("4");
  wifiConnected = true;
  Serial.println("");
  printLog("Connected to ");
  Serial.println(ssid);
  printLog("IP address: ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin(host))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", FS_HTML, processorUpdate); });

  server.on("/filelist", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", filelist.c_str()); });

  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        request->send(200, "text/plain", "Device will reboot in 2 seconds");
        delay(2000);
        ESP.restart(); });

  server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request)
            { opened = false; }, [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
            { handleDoUpload(request, filename, index, data, len, final); });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        String inputMessage;
        String inputParam;
        // GET input1 value on <ESP_IP>/update?state=<inputMessage>
        if (request->hasParam(PARAM)) {
          inputMessage = request->getParam(PARAM)->value();
          inputParam = PARAM;
    
          deleteFile(FSTYPE, inputMessage);
    
          Serial.println("-inputMessage-");
          printLog("File=");
          Serial.println(inputMessage);
          Serial.println(" has been deleted");
    
        }
        else {
          inputMessage = "No message sent";
          inputParam = "none";
        }
        request->send(200, "text/plain", "OK"); });

  server.onNotFound(notFound);
  server.begin();
  Update.onProgress(printProgress);

  while (running)
  {
    delay(100);
  }

  server.end();
}

String processorUpdate(const String &var)
{
  Serial.println(var);

  if (var == "list")
  {
    listDir(FSTYPE, "/", 0);
    return filelist;
  }
  else if (var == "freeSpace")
  {
    freeSpace = FSTYPE.totalBytes() - FSTYPE.usedBytes();
    return String(std::to_string(freeSpace).c_str());
  }

  return String();
}

void handleDoUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    content_len = request->contentLength();
    Serial.printf("UploadStart: %s\n", filename.c_str());
  }

  if (opened == false)
  {
    opened = true;
    file = FSTYPE.open(String("/") + filename, FILE_WRITE);
    if (!file)
    {
      Serial.println("- failed to open file for writing");
      return;
    }
  }

  if (file.write(data, len) != len)
  {
    Serial.println("- failed to write");
    return;
  }

  if (final)
  {

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    response->addHeader("Refresh", "20");
    response->addHeader("Location", "/");
    request->send(response);
    file.close();
    opened = false;
    Serial.println("---------------");
    Serial.println("Upload complete");
  }
}

void notFound(AsyncWebServerRequest *request)
{
  if (request->url().startsWith("/"))
  {
    request->send(FSTYPE, request->url(), String(), true);
  }
  else
  {
    request->send(404);
  }
}
#endif