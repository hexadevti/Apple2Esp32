void wifiSetup() {
    if (!wifiConnected)
    {
        WiFi.begin(ssid, password);
        xTaskCreate(wifi_connect, "wifi_connect", 4096, NULL, 1, NULL);
    }
}

void wifi_connect(void *pvParameters)
{
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      wifiConnected = true;
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      if (!MDNS.begin(host)) { 
        Serial.println("Error setting up MDNS responder!");
        while (1) {
          delay(1000);
        }
      }
      Serial.println("mDNS responder started");
      

    //   server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //     request->send_P(200, "text/html", index_html);
    //   });
    
    //   server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    //     request->send_P(200, "text/html", upload_html);
    //   });
    
      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "text/html", FS_HTML, processor_update);
      });
    
      server.on("/filelist", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "text/plain", filelist.c_str());
      });
    
    //   server.on("/testpage", HTTP_GET, [](AsyncWebServerRequest * request) {
    //     request->send(FSTYPE, "/testpage.html", String(), false);
    //   });
    
      server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", "Device will reboot in 2 seconds");
        delay(2000);
        ESP.restart();
      });
    
    //   server.on("/doUpdate", HTTP_POST,
    //   [](AsyncWebServerRequest * request) {},
    //   [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
    //     handleDoUpdate(request, filename, index, data, len, final);
    //   });
    
      server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest * request) {
        opened = false;
      },
      [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
        handleDoUpload(request, filename, index, data, len, final);
      });
    
      server.on("/delete", HTTP_GET, [] (AsyncWebServerRequest * request) {
        String inputMessage;
        String inputParam;
        // GET input1 value on <ESP_IP>/update?state=<inputMessage>
        if (request->hasParam(PARAM)) {
          inputMessage = request->getParam(PARAM)->value();
          inputParam = PARAM;
    
          deleteFile(FSTYPE, inputMessage);
    
          Serial.println("-inputMessage-");
          Serial.print("File=");
          Serial.println(inputMessage);
          Serial.println(" has been deleted");
    
        }
        else {
          inputMessage = "No message sent";
          inputParam = "none";
        }
        request->send(200, "text/plain", "OK");
      });
    
      server.onNotFound(notFound);
      server.begin();
      Update.onProgress(printProgress);
    
    while (running)
    {
        delay(100);
    }
    
    server.end();
}


String processor_update(const String& var) {
    Serial.println(var);
    
    if (var == "list") {
      listDir(FSTYPE, "/", 0);
      return filelist;
    } else if (var == "freeSpace") {
      freeSpace = FSTYPE.totalBytes() - FSTYPE.usedBytes();
      return String(std::to_string(freeSpace).c_str());
    }
  
    return String();
  }

//   void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
//     if (!index) {
//       Serial.println("Update");
//       content_len = request->contentLength();
//       // if filename includes spiffs, update the spiffs partition
//       int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
//       if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
//         Update.printError(Serial);
//       }
//     }
  
//     if (Update.write(data, len) != len) {
//       Update.printError(Serial);
//       Serial.printf("Progress: %d%%\n", (Update.progress() * 100) / Update.size());
  
//     }
  
//     if (final) {
//       AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
//       response->addHeader("Refresh", "30");
//       response->addHeader("Location", "/");
//       request->send(response);
//       if (!Update.end(true)) {
//         Update.printError(Serial);
//       } else {
//         Serial.println("Update complete");
//         Serial.flush();
//         ESP.restart();
//       }
//     }
//   }
  
  void handleDoUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      content_len = request->contentLength();
      Serial.printf("UploadStart: %s\n", filename.c_str());
    }
  
    if (opened == false) {
      opened = true;
      file = FSTYPE.open(String("/") + filename, FILE_WRITE);
      if (!file) {
        Serial.println("- failed to open file for writing");
        return;
      }
    }
  
    if (file.write(data, len) != len) {
      Serial.println("- failed to write");
      return;
    }
  
    if (final) {
  
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
  
  
  void printProgress(size_t prg, size_t sz) {
    Serial.printf("Progress: %d%%\n", (prg * 100) / content_len);
  }
  
  void notFound(AsyncWebServerRequest *request) {
    if (request->url().startsWith("/")) {
      request->send(FSTYPE, request->url(), String(), true);
    } else {
      request->send(404);
    }
  }
  