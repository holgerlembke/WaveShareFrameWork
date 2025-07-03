/*
      Waveshare Remote

      Hardware: https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8#Screen_Description
      Examples: https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8#Resources

        usb cdc on boot: enabled
        flash size: 16 mb
        psram: OPI
        partition scheme: 16 mb (3 mb app, 9.9 mb fatfs)

      display-board: MaTouch ESP32-S3 AMOLED with Touch 1.8″ FT3168.         

*/

// fürs Bootinfo
#include <esp_chip_info.h>
#include <esp_sntp.h>

#include <WiFi.h>
#include <WiFiMulti.h>

// File System Support
#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <FFat.h>

#include <XPowersLib.h>           // https://github.com/lewisxhe/XPowersLib
#include <TCA9554.h>              // https://github.com/RobTillaart/TCA9554
#include <lvgl.h>                 // https://lvgl.io/
#include <Arduino_GFX_Library.h>  // https://github.com/moononournation/Arduino_GFX
#include <./includes/FT3x68.h>    // rewrite based on Arduino_DriveBus\src\touch_chip\Arduino_FT3x68
#include <SensorQMI8658.hpp>      // https://github.com/lewisxhe/SensorLib
#include <./includes/Buttons.h>   // losely based on "By Jeff Saltzman Oct. 13, 2009"

#include <privatedata.h>  // private credentials



// begin pinconfig.h
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7
#define LCD_SCLK 11
#define LCD_CS 12
#define LCD_WIDTH 368
#define LCD_HEIGHT 448

// TOUCH
#define IIC_SDA 15
#define IIC_SCL 14
#define TP_INT 21

// ES8311
#define I2S_MCK_IO 16
#define I2S_BCK_IO 9
#define I2S_DI_IO 10
#define I2S_WS_IO 45
#define I2S_DO_IO 8

#define MCLKPIN 16
#define BCLKPIN 9
#define WSPIN 45
#define DOPIN 10
#define DIPIN 8
#define PA 46

/*
Buttons:

  oben: pwr
        6 sekunden gedrückt halten: geht aus, funktioniert autonom ohne software
        drücken wenn aus: geht an
  unten: boot gpio0

  (USB Links, Display vorne)
*/

// TCA9554 Extender Ports
const uint8_t PinLCDReset = 0;      // MaTouch-Display-Modul
const uint8_t PinLCDPwrEnable = 1;  // MaTouch-Display-Modul
const uint8_t PinTouchReset = 2;    // MaTouch-Display-Modul
const uint8_t PinRTCIRQ = 3;        // PCF85063A
const uint8_t PinPwrButton = 4;     // schematic: SysOut
const uint8_t PinAXP2101IRQ = 5;    // AXP2101
const uint8_t PinQMIIRQ = 6;        // QMI8658C
const uint8_t PinSDCardSDCS = 7;    // SD-Card CD/D3

/*
.PWR: In the power-on state, press and hold for 6s to power off, in the power-off state 
(power off to charge the battery), click to power on; Under normal working conditions, 
the action can be judged by the high and low levels of the EXIO4 detection buttons of 
the extended IO, and the high level is pressed, which can identify the actions of single 
click, double click, multiple click and long press (long press should not exceed 6s, 
otherwise the power will be turned off). 
*/

//I2C: 0x18      0x7E
const uint8_t I2cTCA9554Addr = 0x20;
const uint8_t I2cPCF85063AAddr = 0x51;
const uint8_t I2cFT3x68Addr = 0x38;
const uint8_t I2cQMI8658Addr = 0x6b;
const uint8_t I2cAXP2101Addr = 0x34;

#define ntpSource "pool.ntp.org"
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define ntpTimeZone "CET-1CEST,M3.5.0,M10.5.0/3"

// SD
const int SDMMC_CLK = 2;   // SCK
const int SDMMC_CMD = 1;   // MOSI
const int SDMMC_DATA = 3;  // MISO
bool SDCardAvailable = false;
// end


// setupButtons()
pinButton *button0 = NULL;       // Button unten (USB links)
expanderButton *button4 = NULL;  // Button oben (USB links)

// Turn off automatically if not charging...
bool autoPowerOffWatchdog = false;
bool autoPowerOffWarning = false;
const int16_t autoPowerOffTime = 5 * 60;
const int16_t autoPowerOffWarnTime = 1 * 60;

//***********************************************************************************************
void setup() {
  delay(2500);
  Serial.begin(115200);
  // while (!Serial) {
  //   delay(100);
  // }
  delay(2500);

  Serial.println(F("\n\nWaveshare FrameWork " __DATE__ " " __TIME__));

  Wire.begin(IIC_SDA, IIC_SCL);
  // 400 khz possible?
  // Wire.setClock(400000);

  bootinfo();

  // First...
  setupExpander();
  // power does some Resets on some devices, so early
  setupPower();
  // now we can scan
  i2cbusscanner();

  setupTimeHandling();
  setupFileSystem();
  setupSDCard();
  setupTouchSensor();
  setupQMI8658();
  setupButtons();

  setupWifi();

  setupGfx();
  toggleBacklight();

  // A simple Gui demo
  setupGuiDem02();

  // hu?
  pinMode(0, INPUT);
}

//***********************************************************************************************
void loop() {
  loopExpander();
  loopPower();
  loopGfx();
  loopRTCTime();
  loopQMI8658();
  loopAutoPowerOff();

  // demo GUI
  loopGuiDemo2();

  // for tests.
  loopTestsomething();

  loopMemoryMonitor();
}


//
