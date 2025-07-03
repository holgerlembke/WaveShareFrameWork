//*********************************************************************************************************************
//*********************************************************************************************************************
void setupFileSystem() {
  if (FFat.begin(true)) {
    Serial.print("FFAT: ");
    Serial.print(FFat.totalBytes() / 1024);
    Serial.print(" KiB total, ");
    Serial.print(FFat.usedBytes() / 1024);
    Serial.print(" KiB used, ");
    Serial.print(FFat.freeBytes() / 1024);
    Serial.println(" KiB free.");
  } else {
    Serial.println(F("FFat File System not inited."));
  }
}

//*********************************************************************************************************************
//*********************************************************************************************************************
// https://github.com/espressif/esp-idf/issues/892
void tryMountSDcard(void *parameter) {
  UBaseType_t lastwm = 50000;

// call sdmmc_card_init?
// https://github.com/espressif/esp-idf/blob/master/examples/storage/sd_card/sdmmc/main/sd_card_example_main.c#L102

  do {
    if (SDCardAvailable) {
      if (!SD_MMC.exists("/")) {
        SDCardAvailable = false;
        SD_MMC.end();
      }
    } else {
      if (!SD_MMC.begin("/sdcard", true)) {
        //Serial.println("Card Mount Failed");
        SDCardAvailable = false;
      }

      uint8_t cardType = SD_MMC.cardType();
      if (cardType == CARD_NONE) {
        //Serial.println("No SD_MMC card attached");
        SDCardAvailable = false;
      } else {
        SDCardAvailable = true;
      }
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    /**/
    UBaseType_t cwm = uxTaskGetStackHighWaterMark(NULL);
    if (cwm < lastwm) {
      Serial.print("tryMountSDcard StackHighWaterMark: ");
      Serial.println(cwm);
      lastwm = cwm;
    }
    /**/
  } while (1);
}

//*********************************************************************************************************************
void setupSDCard() {
  return;
  
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);

  xTaskCreatePinnedToCore(
    tryMountSDcard,
    "tryMountSDCard",
    3500,  // Stack size ausgemessen via cwm
    NULL,
    1,
    NULL,
    0);  // core 0
}

//*********************************************************************************************************************
//*********************************************************************************************************************
void setupTimeHandling() {
  if (!HasDeviceAtI2CAddr(I2cPCF85063AAddr)) {
    Serial.println("Panic: PCF85063A communication failed.");
    while (1) delay(50);
  }

  // sets the callback and updates the RTC
  sntp_set_time_sync_notification_cb(time_is_set);
  configTzTime(ntpTimeZone, ntpSource, "");
  // gets the initial time from the RTC
  getRTCTime();
}

//*********************************************************************************************************************
//*********************************************************************************************************************
// PCF85063A is easy, no need for a lib
#define RTC_SECOND_ADDR 0x04
#define RTC_DAY_ADDR 0x07
//*********************************************************************************************************************
uint8_t PCF85063AdecToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

//*********************************************************************************************************************
uint8_t PCF85063AbcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

/*
struct tm, all int                                     PCF85063
tm_sec	int	seconds after the minute	0-61*              0..59
tm_min	int	minutes after the hour	0-59                 0..59
tm_hour	int	hours since midnight	0-23                   0..23 (24 hour mode)
tm_mday	int	day of the month	1-31                       1..31
tm_mon	int	months since January	0-11                   1..12
tm_year	int	years since 1900	                           0..99 /me give arbitrary offset
tm_wday	int	days since Sunday	0-6                        0..6 since Sunday
tm_yday	int	days since January 1	0-365
tm_isdst	int	Daylight Saving Time flag	
*/

//*********************************************************************************************************************
void PCF85063AsetDateTime(time_t rawtime) {
  struct tm *ptm;
  ptm = gmtime(&rawtime);

  Wire.beginTransmission(I2cPCF85063AAddr);
  Wire.write(RTC_SECOND_ADDR);
  Wire.write(PCF85063AdecToBcd(ptm->tm_sec));
  Wire.write(PCF85063AdecToBcd(ptm->tm_min));
  Wire.write(PCF85063AdecToBcd(ptm->tm_hour));
  Wire.write(PCF85063AdecToBcd(ptm->tm_mday));
  Wire.write(PCF85063AdecToBcd(ptm->tm_wday));
  Wire.write(PCF85063AdecToBcd(ptm->tm_mon + 1));
  Wire.write(PCF85063AdecToBcd(ptm->tm_year + 1900 - 1970));
  bool allesgut = Wire.endTransmission() == 0;
}

//*********************************************************************************************************************
time_t PCF85063AreadUnixTime() {
  Wire.beginTransmission(I2cPCF85063AAddr);
  Wire.write(RTC_SECOND_ADDR);  // datasheet 8.4.
  bool allesgut = Wire.endTransmission() == 0;
  allesgut = Wire.requestFrom(I2cPCF85063AAddr, 7) == 7;
  uint8_t second = PCF85063AbcdToDec(Wire.read() & 0x7F);  // ignore bit 7
  uint8_t minute = PCF85063AbcdToDec(Wire.read() & 0x7F);
  uint8_t hour = PCF85063AbcdToDec(Wire.read() & 0x3F);  // ignore bits 7 & 6
  uint8_t day = PCF85063AbcdToDec(Wire.read() & 0x3F);
  uint8_t weekday = PCF85063AbcdToDec(Wire.read() & 0x07);  // ignore bits 7,6,5,4 & 3
  uint8_t month = PCF85063AbcdToDec(Wire.read() & 0x1F);    // ignore bits 7,6 & 5
  uint16_t year = PCF85063AbcdToDec(Wire.read()) + 1970;

  // https://de.wikipedia.org/wiki/Unixzeit#Beispiel-Implementierung
  const uint16_t tage_seit_jahresanfang[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
  uint32_t schaltjahre = ((year - 1) - 1968) / 4 - ((year - 1) - 1900) / 100 + ((year - 1) - 1600) / 400;
  uint32_t tage_seit_1970 = (year - 1970) * 365 + schaltjahre + tage_seit_jahresanfang[month - 1] + day - 1;
  if ((month > 2) && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
    tage_seit_1970 += 1;
  }

  return second + 60 * (minute + 60 * (hour + 24 * tage_seit_1970));
}

//*********************************************************************************************************************
enum sntprtcsyncprocess_t { srspwait,
                            srspdoit,
                            srspdone } sntprtcsyncprocess = srspwait;
//*********************************************************************************************************************
void time_is_set(struct timeval *tv) {
  sntprtcsyncprocess = srspdoit;
  /* looks like we can not do much in the callback... the println() fails:
  Serial.println("The NTP server was called!");
  setupRTC();
  */
}

//*********************************************************************************************************************
void getRTCTime() {
  struct timeval tv;
  tv.tv_sec = PCF85063AreadUnixTime();
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
  Serial.print("Get ");
  showTime();
}

//*********************************************************************************************************************
void setRTCTime() {
  // https://cplusplus.com/reference/ctime/gmtime/
  time_t rawtime;
  time(&rawtime);
  PCF85063AsetDateTime(rawtime);
  Serial.print("Sync ");
  showTime();
}

//*********************************************************************************************************************
inline void loopRTCTime() {
  if (sntprtcsyncprocess == srspdoit) {
    sntprtcsyncprocess = srspdone;
    setRTCTime();
  }
}

//*********************************************************************************************************************
void showTime() {          // found somewhere
  time_t now;              // this are the seconds since Epoch (1970) GMT
  tm tm;                   // a readable structure
  time(&now);              // read the current time and store to now
  localtime_r(&now, &tm);  // update the structure tm with the current time
  char buf[50];
  strftime(buf, sizeof(buf), " %F %T %Z wday=%w", &tm);  // https://www.cplusplus.com/reference/ctime/strftime/
  Serial.print("now:");
  Serial.print(now);  // in UTC!
  Serial.print(buf);
  if (tm.tm_isdst == 1) {  // Daylight Saving Time flag
    Serial.print(" DST");
  } else {
    Serial.print(" standard");
  }
  Serial.println();
}

//