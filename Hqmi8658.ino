//***********************************************************************************************
SensorQMI8658* QMI = NULL;

//***********************************************************************************************
void setupQMI8658() {
  QMI = new SensorQMI8658();
  if (!QMI->begin(Wire, I2cQMI8658Addr)) {
    Serial.println("Panic: QMI8658 begin() failed.");
    while (1) delay(50);
  }
  Serial.print(F("QMI8658 Chip ID: "));
  Serial.println(QMI->getChipID());

  QMI->configAccelerometer(
    SensorQMI8658::ACC_RANGE_4G,    // ACC_RANGE_2G / ACC_RANGE_4G / ACC_RANGE_8G / ACC_RANGE_16G
    SensorQMI8658::ACC_ODR_1000Hz,  // ACC_ODR_1000H / ACC_ODR_500Hz / ACC_ODR_250Hz / ACC_ODR_125Hz / ACC_ODR_62_5Hz / ACC_ODR_31_25Hz / ACC_ODR_LOWPOWER_128Hz / ACC_ODR_LOWPOWER_21Hz / ACC_ODR_LOWPOWER_11Hz / ACC_ODR_LOWPOWER_3Hz
    SensorQMI8658::LPF_MODE_0);     //LPF_MODE_0 (2.66% of ODR) / LPF_MODE_1 (3.63% of ODR) / LPF_MODE_2 (5.39% of ODR) / LPF_MODE_3 (13.37% of ODR)
  QMI->configGyroscope(
    SensorQMI8658::GYR_RANGE_64DPS,  // GYR_RANGE_16DPS / GYR_RANGE_32DPS / GYR_RANGE_64DPS / GYR_RANGE_128DPS / GYR_RANGE_256DPS / GYR_RANGE_512DPS / GYR_RANGE_1024DPS
    SensorQMI8658::GYR_ODR_896_8Hz,  // GYR_ODR_7174_4Hz / GYR_ODR_3587_2Hz / GYR_ODR_1793_6Hz / GYR_ODR_896_8Hz / GYR_ODR_448_4Hz / GYR_ODR_224_2Hz / GYR_ODR_112_1Hz / GYR_ODR_56_05Hz / GYR_ODR_28_025H
    SensorQMI8658::LPF_MODE_3);      // LPF_MODE_0 (2.66% of ODR) / LPF_MODE_1 (3.63% of ODR) / LPF_MODE_2 (5.39% of ODR) / LPF_MODE_3 (13.37% of ODR)

  // In 6DOF mode (accelerometer and gyroscope are both enabled),
  // the output data rate is derived from the nature frequency of gyroscope
  QMI->enableGyroscope();
  QMI->enableAccelerometer();
}

//***********************************************************************************************
void loopQMI8658() {
  const uint32_t tickertime = 10000;
  static uint32_t ticker = -tickertime;
  if (millis() - ticker >= tickertime) {
    ticker = millis();

    if (QMI->getDataReady()) {
      IMUdata Accel;
      if (QMI->getAccelerometer(Accel.x, Accel.y, Accel.z)) {
        float r = sqrt(Accel.x * Accel.x + Accel.y * Accel.y + Accel.z * Accel.z);
        Serial.printf("ACCEL:  %f  %f  %f  %f\r\n", Accel.x, Accel.y, Accel.z, r);
      }

      /*
https://github.com/lewisxhe/SensorLib/issues/14
x, y, z return values of getAccelerometer() method are rated relative to G (9.81 m/c^2) value.

Thus, when a sensor is stationary on the Earth:
sqrt( x^2 + y^2 + z^2 ) = 9.81
*/
      IMUdata Gyro;
      if (QMI->getGyroscope(Gyro.x, Gyro.y, Gyro.z)) {
        //      Serial.printf("GYRO:  %f  %f  %f\r\n", Gyro.x, Gyro.y, Gyro.z);
      }
    }
  }
}

/*
GYRO:  -0.335938  0.341797  0.150391
ACCEL:  0.025879  0.057739  -0.990356
GYRO:  -0.347656  0.380859  0.205078
ACCEL:  0.026001  0.057739  -0.990356
GYRO:  -0.410156  0.380859  0.195312
ACCEL:  0.026123  0.057739  -0.990479
GYRO:  -0.460938  0.341797  0.099609
ACCEL:  0.026245  0.057739  -0.990479
GYRO:  -0.462891  0.277344  -0.052734
ACCEL:  0.026367  0.057739  -0.990479
GYRO:  -0.435547  0.208984  -0.203125
ACCEL:  0.026367  0.057739  -0.990479
GYRO:  -0.419922  0.154297  -0.285156
ACCEL:  0.026489  0.057861  -0.990601
GYRO:  -0.445312  0.125000  -0.263672
ACCEL:  0.026489  0.057861  -0.990479
GYRO:  -0.656250  0.126953  0.093750
ACCEL:  0.026611  0.057861  -0.990479
GYRO:  -0.789062  0.152344  0.294922
ACCEL:  0.026611  0.057983  -0.990479
GYRO:  -0.865234  0.191406  0.400391
ACCEL:  0.026611  0.057983  -0.990356
GYRO:  -0.855469  0.224609  0.400391
ACCEL:  0.026733  0.057983  -0.990234


*/

//