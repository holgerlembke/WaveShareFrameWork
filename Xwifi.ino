//***********************************************************************************************
void keepWiFiAlive(void* parameter) {
  UBaseType_t lastwm = 50000;
  WiFiMulti wifiMulti;
  bool newconnect = false;

  wifiMulti.addAP(privatedata_mySSID, privatedata_myWAP2);

  do {
    if (WiFi.status() != WL_CONNECTED) {
      if (wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi: not connected");
        vTaskDelay((10000 + random(10000)) / portTICK_PERIOD_MS);  // random delay
      } else {
        newconnect = true;
      }
    } else {
      if (newconnect) {
        Serial.print("IP: ");
        Serial.print(WiFi.localIP());
        Serial.println();
        newconnect = false;
      }
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    /**/
    UBaseType_t cwm = uxTaskGetStackHighWaterMark(NULL);
    if (cwm < lastwm) {
      Serial.print("keepWiFiAlive StackHighWaterMark: ");
      Serial.println(cwm);
      lastwm = cwm;
    }
    /**/
  } while (1);
}

//***********************************************************************************************
void setupWifi() {
  xTaskCreatePinnedToCore(
    keepWiFiAlive,
    "keepWiFiAlive",  
    3500,             // Stack size ausgemessen via cwm
    NULL,             
    1,                
    NULL,             
    0);               // core 0
}

//