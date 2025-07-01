//*********************************************************************************************************************
void bootinfo(void) {  // esp32
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  /*
  uint32_t flashchipid;
  esp_flash_read_id(NULL, &flashchipid);
  Serial.print(F("chipid: "));
  Serial.println(flashchipid, HEX);
*/
  Serial.print(F("ESP SDK: "));
  Serial.println(ESP.getSdkVersion());

  Serial.print(F("ChipModel: "));
  Serial.print(ESP.getChipModel());
  Serial.print(F(", ChipRevision: 0x"));
  Serial.print(ESP.getChipRevision(), HEX);
  Serial.print(F(", Cores: "));
  Serial.print(chip_info.cores);  // ESP.getChipCores()
  Serial.print(F(", Features:"));
  if ((chip_info.features & CHIP_FEATURE_EMB_FLASH) != 0) {
    Serial.print(F(" embedded flash"));
  }
  if ((chip_info.features & CHIP_FEATURE_WIFI_BGN) != 0) {
    Serial.print(F(" WiFi802bgn"));
  }
  if ((chip_info.features & CHIP_FEATURE_BLE) != 0) {
    Serial.print(F(" BLE"));
  }
  if ((chip_info.features & CHIP_FEATURE_BT) != 0) {
    Serial.print(F(" BT"));
  }
  Serial.println();

  //  Serial.println("Boot-Version " + String(ESP.getBootVersion()) + " -Mode: " + String(ESP.getBootMode()));
  Serial.print(F("CPU-Freq: "));
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(F(" MHz"));

  //  Serial.println("FlashChipID: 0x" + String(ESP.getFlashChipId(), HEX) + "/0x" + String(ESP.getFlashChipVendorId(), HEX));
  Serial.print(F("Flash-Freq: "));
  Serial.print(ESP.getFlashChipSpeed());
  Serial.println(F(" Hz"));  // Hz. Because it makes so much sense....

  Serial.print(F("Flash Size: "));
  Serial.print(ESP.getFlashChipSize());
  Serial.println(F(" B"));

  Serial.print(F("Sketch Size: "));
  Serial.print(ESP.getSketchSize());
  Serial.println(F(" B"));
  //  Serial.println("Reset Reason: " + String(ESP.getResetReason()));
  //  Serial.println("Reset Info: " + String(ESP.getResetInfo()));

  Serial.print(F("Heap Size: "));
  Serial.print(ESP.getHeapSize());
  Serial.print(F(" B, Free Heap Size: "));
  Serial.println(ESP.getFreeHeap());

  Serial.print(F("Min Heap Size: "));
  Serial.print(ESP.getMinFreeHeap());
  Serial.print(F(" B, Max Heap Size: "));
  Serial.println(ESP.getMaxAllocHeap());

  Serial.print(F("Psram Size: "));
  Serial.print(ESP.getPsramSize());
  Serial.print(F(" B, Free Psram Size: "));
  Serial.println(ESP.getFreePsram());

  Serial.print(F("Min Psram Size: "));
  Serial.print(ESP.getMinFreePsram());
  Serial.print(F(" B, Max allocatable Psram Size: "));
  Serial.println(ESP.getMaxAllocPsram());

  Serial.print(F("Sketch Size: "));
  Serial.print(ESP.getSketchSize());
  Serial.println(F(" B"));

  uint64_t chipid = ESP.getEfuseMac();  //The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.print(F("ESP32 Chip ID = "));
  Serial.printf("%04X", (uint16_t)(chipid >> 32));  //print High 2 bytes
  Serial.printf("%08X\n", (uint32_t)chipid);        //print Low 4bytes.

  Serial.print(F("Max Sockets: "));
  Serial.println(CONFIG_LWIP_MAX_SOCKETS);
}

//*********************************************************************************************************************
bool HasDeviceAtI2CAddr(uint8_t addr) {
  Wire.beginTransmission(addr);
  byte error = Wire.endTransmission();

  return (error == 0);
}

//*********************************************************************************************************************
void i2cbusscanner() {
  byte address;
  byte nDevices = 0;
  bool first = true;

  for (address = 1; address < 127; address++) {
    if (HasDeviceAtI2CAddr(address)) {
      if (first) {
        Serial.print("I2C: ");
        first = false;
      }
      Serial.print("0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.print(" ");

      nDevices++;
    }
  }
  if (nDevices == 0) {
    Serial.print("I2C: no devices found.");
  }
  Serial.println();
}


