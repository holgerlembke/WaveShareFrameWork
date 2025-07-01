# WaveShareFrameWork

This is a remix of the development environment for the ESP32-S3-Touch-AMOLED-1.8 found at
https://www.waveshare.com/esp32-s3-touch-amoled-1.8.htm and their support wiki at
https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8

It is more easy, (hopefully) more up to date, simplyfies some things and unclutters all the source code.

It has all the prototypes included with all stuff set up and ready to run. 

Compile time is agonizing long.


# Changes

* LVGL is updated to v 9.3.x
* Expander code is now based on https://github.com/RobTillaart/TCA9554 
* Touch-Driver FT3x68 has been rewritten
* rtc has been adapted into the ESP32 sntp-style, PCF85063A backups time when without WiFi
* WiFi is driven by a FreeRtos task and WiFiMulti
* automatic SD MMC Card detection via FreeRtos task


# Install

Via Arduino IDE install latest versions of

* #include <XPowersLib.h>           // https://github.com/lewisxhe/XPowersLib
* #include <SensorQMI8658.hpp>      // https://github.com/lewisxhe/SensorLib
* #include <TCA9554.h>              // https://github.com/RobTillaart/TCA9554
* #include <lvgl.h>                 // https://lvgl.io/
* #include <Arduino_GFX_Library.h>  // https://github.com/moononournation/Arduino_GFX

All other things are included into this framework.


# The Ugly One

You still need the ESP32-S3-Touch-AMOLED-1.8 Demo from 
https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8/ESP32-S3-Touch-AMOLED-1.8-Demo.zip

From there you need to copy Arduino_SH8601.h and Arduino_SH8601.cpp 
at ESP32-S3-Touch-AMOLED-1.8\Arduino-v3.1.0\libraries\GFX_Library_for_Arduino\src\display
to GFX_Library_for_Arduino\src\display in your libraries folder.


