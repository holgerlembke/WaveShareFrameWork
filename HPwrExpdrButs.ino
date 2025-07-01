XPowersAXP2101 *power = NULL;

bool powerflag = false;
bool adc_switch = false;
bool backlight_on = true;

//***********************************************************************************************
void setFlagIRQ(void) {
  powerflag = true;
}

//***********************************************************************************************
void adcOn() {
  power->enableTemperatureMeasure();
  // Enable internal ADC detection
  power->enableBattDetection();
  power->enableVbusVoltageMeasure();
  power->enableBattVoltageMeasure();
  power->enableSystemVoltageMeasure();
}

//***********************************************************************************************
void adcOff() {
  power->disableTemperatureMeasure();
  // Enable internal ADC detection
  power->disableBattDetection();
  power->disableVbusVoltageMeasure();
  power->disableBattVoltageMeasure();
  power->disableSystemVoltageMeasure();
}

//***********************************************************************************************
void setupPower() {
  power = new XPowersAXP2101();

  bool beginok = power->begin(Wire, I2cAXP2101Addr, IIC_SDA, IIC_SCL);

  if (beginok) {
    adcOn();
    power->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    power->setChargeTargetVoltage(3);
    // Clear all interrupt flags
    power->clearIrqStatus();
    // Enable the required interrupt function
    power->enableIRQ(
      XPOWERS_AXP2101_PKEY_SHORT_IRQ  //POWER KEY
    );
  } else {
    Serial.println("Panic: PMU begin() failed.");
    while (1) delay(50);
  }
}

//***********************************************************************************************
inline void loopPower() {
  return;

  /*
  int backlight_ctrl = expander->digitalRead(4);

  if (backlight_ctrl == HIGH) {
    while (expander->digitalRead(4) == HIGH) {
      delay(50);
    }
    toggleBacklight();
  }
  */
  const uint32_t tickerdelay = 10000l;
  static uint32_t ticker = -tickerdelay;

  if (millis() - ticker > tickerdelay) {
    ticker = millis();

    if (powerflag) {
      powerflag = false;
      // Get PMU Interrupt Status Register
      uint32_t status = power->getIrqStatus();
      if (power->isPekeyShortPressIrq()) {
        if (adc_switch) {
          adcOn();
          Serial.println("Enable ADC\n\n\n");
        } else {
          adcOff();
          Serial.println("Disable ADC\n\n\n");
        }
        adc_switch = !adc_switch;
      }
      power->clearIrqStatus();
    }

    String info = "";

    uint8_t charge_status = power->getChargerStatus();

    info += "power\n Temperature: " + String(power->getTemperature()) + "*C\n";
    info += " is Charging: " + String(power->isCharging() ? "YES" : "NO");
    info += " Discharge: " + String(power->isDischarge() ? "YES" : "NO");
    info += " Standby: " + String(power->isStandby() ? "YES" : "NO");
    info += " VbusIn: " + String(power->isVbusIn() ? "YES" : "NO");
    info += " VbusGood: " + String(power->isVbusGood() ? "YES" : "NO") + "\n";

    switch (charge_status) {
      case XPOWERS_AXP2101_CHG_TRI_STATE:
        info += " Charger Status: tri_charge\n";
        break;
      case XPOWERS_AXP2101_CHG_PRE_STATE:
        info += " Charger Status: pre_charge\n";
        break;
      case XPOWERS_AXP2101_CHG_CC_STATE:
        info += " Charger Status: constant charge\n";
        break;
      case XPOWERS_AXP2101_CHG_CV_STATE:
        info += " Charger Status: constant voltage\n";
        break;
      case XPOWERS_AXP2101_CHG_DONE_STATE:
        info += " Charger Status: charge done\n";
        break;
      case XPOWERS_AXP2101_CHG_STOP_STATE:
        info += " Charger Status: not charging\n";
        break;
    }

    info += " Voltages Battery: " + String(power->getBattVoltage() / 1000.0 + 0.0005);
    info += " Vbus: " + String(power->getVbusVoltage() / 1000.0 + 0.0005);
    info += " System: " + String(power->getSystemVoltage() / 1000.0 + 0.0005) + " V\n";

    if (power->isBatteryConnect()) {
      info += " Battery Percent: " + String(power->getBatteryPercent()) + "%\n";
    }

    static String lastinfo = "";
    if (lastinfo != info) {
      lastinfo = info;
      Serial.print(info);
    }
  }
}

//***********************************************************************************************
inline void loopAutoPowerOff() {
  static uint32_t ticker = 0;
  static int16_t downcount = autoPowerOffTime;

  if (millis() - ticker >= 1000) {
    ticker = millis();

    bool chargeing = power->getChargerStatus() != XPOWERS_AXP2101_CHG_STOP_STATE;
    if (!chargeing) {
      // Dog feed? Reset timer
      if (autoPowerOffWatchdog) {
        downcount = autoPowerOffTime;
        autoPowerOffWatchdog = false;
        Serial.println("PowerOff: Watchdog feed");
      } else {
        downcount--;
      }
      autoPowerOffWarning = downcount < autoPowerOffWarnTime;
      if (downcount <= 0) {
        // turn off
        Serial.println("PowerOff: powering off");
        power->shutdown();
      }
    } else {
      // Charging
      downcount = autoPowerOffTime;
      autoPowerOffWarning = false;
    }
  }
}

//***********************************************************************************************
//***********************************************************************************************
TCA9554 *expander = NULL;

//***********************************************************************************************
void setupExpander() {
  expander = new TCA9554(I2cTCA9554Addr);

  bool beginok = expander->begin();
  if (beginok) {
    expander->pinMode1(PinAXP2101IRQ, INPUT);
    expander->pinMode1(PinSDCardSDCS, INPUT);

    // Resets
    expander->pinMode1(PinLCDPwrEnable, OUTPUT);
    expander->pinMode1(PinTouchReset, OUTPUT);
    // do the Resets
    expander->write1(PinLCDPwrEnable, LOW);
    expander->write1(PinTouchReset, LOW);
    delay(20);
    expander->write1(PinLCDPwrEnable, HIGH);
    expander->write1(PinTouchReset, HIGH);

    int pmu_irq = expander->read1(PinAXP2101IRQ);
    if (pmu_irq == 1) {
      setFlagIRQ();
    }
  } else {
    Serial.println("Panic: TCA9554 begin() failed.");
    while (1) delay(50);
  }
}

//***********************************************************************************************
void toggleBacklight() {
  if (backlight_on) {
    for (int i = 255; i >= 0; i--) {
      //gfx->Display_Brightness(i);
      delay(3);
    }
  } else {
    for (int i = 0; i <= 255; i++) {
      //gfx->Display_Brightness(i);
      delay(3);
    }
  }
  backlight_on = !backlight_on;
}

//***********************************************************************************************
inline void loopExpander() {  // for pwr button functions
}

//*****************************************************************************************************
uint32_t uint32Abs(uint32_t a, uint32_t b) {
  if (a > b) {
    return (a - b);
  } else {
    return (b - a);
  }
}

//***********************************************************************************************
inline void loopMemoryMonitor() {
  static uint32_t ticker = 0;
  if (millis() - ticker > 10000) {
    ticker = millis();

    static uint32_t lastfreeheap = 0;
    static uint32_t lastgetMaxAllocHeap = 0;
    const uint32_t limitheap = 1024;
    const uint32_t limitmaxheap = 1024;

    static uint32_t ticker = 0;

    // great var name selection!
    uint32_t t1 = ESP.getFreeHeap();
    uint32_t t2 = ESP.getMaxAllocHeap();
    if ((uint32Abs(t1, lastfreeheap) > limitheap) || (uint32Abs(t2, lastgetMaxAllocHeap) > limitmaxheap)) {
      lastfreeheap = t1;
      lastgetMaxAllocHeap = t2;

      Serial.print(F("Memory free: "));
      Serial.print(lastfreeheap);
      Serial.print(F(" B max: "));
      Serial.print(lastgetMaxAllocHeap);
      Serial.println(F(" B"));
    }

    static uint32_t lastfreePsram = 0;
    static uint32_t lastgetMaxAllocPsram = 0;

    t1 = ESP.getFreePsram();
    t2 = ESP.getMaxAllocPsram();

    if ((uint32Abs(t1, lastfreePsram) > limitheap) || (uint32Abs(t2, lastgetMaxAllocPsram) > limitmaxheap)) {
      lastfreePsram = t1;
      lastgetMaxAllocPsram = t2;

      Serial.print(F("PsRam free: "));
      Serial.print(lastfreePsram);
      Serial.print(F(" B max: "));
      Serial.print(lastgetMaxAllocPsram);
      Serial.println(F(" B"));
    }
  }
}

//***********************************************************************************************
inline void loopTestsomething() {
  static uint32_t ticker = 0;
  if (millis() - ticker > 1000) {
    ticker = millis();
    /*
    Serial.print("SD-Card: ");
    Serial.print(SDCardAvailable);
    Serial.println();
    */
  }
}

//***********************************************************************************************
//***********************************************************************************************
void setupButtons() {
  button0 = new pinButton(0);
  button4 = new expanderButton(4, expander);
}


//