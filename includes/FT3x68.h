/*
    Waveshare uses Arduino_DriveBus, which adds a lot of overhead.

    All it need for the touch driver is now in this small class.

    Some code is lifted from Arduino_DriveBus, because, well, you can't do it different, anyway.
*/

//***********************************************************************************************
#include <FunctionalInterrupt.h>

//***********************************************************************************************
#define FT3x68_RD_DEVICE_GESTUREID 0xD3                  // GestureID
#define FT3x68_RD_DEVICE_FINGERNUM 0x02                  // FingerNum
#define FT3x68_RD_DEVICE_X1POSH 0x03                     // X1posH
#define FT3x68_RD_DEVICE_X1POSL 0x04                     // X1posL
#define FT3x68_RD_DEVICE_Y1POSH 0x05                     // Y1posH
#define FT3x68_RD_DEVICE_Y1POSL 0x06                     // Y1posL
#define FT3x68_RD_DEVICE_X2POSH 0x09                     // X2posH
#define FT3x68_RD_DEVICE_X2POSL 0x0A                     // X2posL
#define FT3x68_RD_DEVICE_Y2POSH 0x0B                     // Y2posH
#define FT3x68_RD_DEVICE_Y2POSL 0x0C                     // Y2posL
#define FT3x68_RD_WR_DEVICE_GESTUREID_MODE 0xD0          // GestureID
#define FT3x68_RD_WR_DEVICE_POWER_MODE 0xA5              // Power Mode
#define FT3x68_RD_WR_DEVICE_PROXIMITY_SENSING_MODE 0xB0  // Proximity Sensing Mode
#define FT3x68_RD_DEVICE_ID 0xA0                         // Device ID Register
// (0x00:FT6456 0x04:FT3268 0x01:FT3067 0x05:FT3368 0x02:FT3068 0x03:FT3168)

//***********************************************************************************************
class FT3x68 {
public:
  bool Interrupt_Flag;

private:
  uint8_t i2caddr = 0;
  uint8_t PinIsr = 0;

  //---------------------------------------------------------------------------------------------
  void ARDUINO_ISR_ATTR TouchISR() {
    Interrupt_Flag = true;
  }

  //---------------------------------------------------------------------------------------------
  bool ReadC8D8(uint8_t c, uint8_t *d) {
    Wire.beginTransmission(i2caddr);
    Wire.write(c);
    if (Wire.endTransmission() != 0) {
      return false;
    }
    if (Wire.requestFrom(i2caddr, 1) != 1) {
      return false;
    }
    *d = Wire.read();

    return true;
  }

  //---------------------------------------------------------------------------------------------
  bool ReadC8D16(uint8_t c, uint16_t *d) {
    Wire.beginTransmission(i2caddr);
    Wire.write(c);
    if (Wire.endTransmission() != 0) {
      return false;
    }
    if (Wire.requestFrom(i2caddr, 2) != 2) {
      return false;
    }
    *d = (Wire.read() << 8) + Wire.read();

    return true;
  }


public:
  //---------------------------------------------------------------------------------------------
  FT3x68(uint8_t _i2caddr, uint8_t _PinIsr) {
    i2caddr = _i2caddr;
    PinIsr = _PinIsr;
    Interrupt_Flag = false;

    // Touch-Reset in setupExpander()

    // libraries/ESP32/examples/GPIO/FunctionalInterrupt/FunctionalInterrupt.ino
    attachInterrupt(PinIsr, std::bind(&FT3x68::TouchISR, this), RISING);
  }
  //---------------------------------------------------------------------------------------------
  ~FT3x68() {
    detachInterrupt(PinIsr);
  }

  //---------------------------------------------------------------------------------------------
  bool begin(void) {
    uint8_t temp = 0;
    if (ReadC8D8(FT3x68_RD_DEVICE_ID, &temp) == true) {
      return temp == 0x03;  //FT3168
    }
    return false;
  }
  //---------------------------------------------------------------------------------------------
  uint16_t getX() {
    uint16_t temp;
    if (ReadC8D16(FT3x68_RD_DEVICE_X1POSH, &temp)) {
      return temp & 0xfff;
    }
    return -1;
  }
  //---------------------------------------------------------------------------------------------
  uint16_t getY() {
    uint16_t temp;
    if (ReadC8D16(FT3x68_RD_DEVICE_Y1POSH, &temp)) {
      return temp & 0xfff;
    }
    return -1;
  }
};

//
