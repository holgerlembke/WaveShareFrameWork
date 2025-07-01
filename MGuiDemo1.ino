//***********************************************************************************************
lv_obj_t *Demo1label = NULL;

//***********************************************************************************************
void setupGuiDem01() {
  Demo1label = lv_label_create(lv_scr_act());
  lv_label_set_text(Demo1label, "");
  lv_obj_align(Demo1label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_align(Demo1label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // D:\Arduino\sketches2.0\libraries\lv_conf.h Zeile 388ff: 14, 18, 24, 28, 42, 48
  lv_obj_set_style_text_font(Demo1label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
  /*
LV_SYMBOL_BATTERY_FULL
LV_SYMBOL_BATTERY_3
LV_SYMBOL_BATTERY_2
LV_SYMBOL_BATTERY_1
LV_SYMBOL_BATTERY_EMPTY
LV_SYMBOL_CHARGE
lv_label_set_text(my_label, LV_SYMBOL_OK LV_SYMBOL_WIFI LV_SYMBOL_PLAY);
*/
}

//***********************************************************************************************
inline void loopGuiDemo1() {
  const uint32_t tickertime = 10000;
  static uint32_t ticker = -tickertime;
  if (millis() - ticker >= tickertime) {
    ticker = millis();

    String s = "Hallo\n" + String(millis() / 1000) + "\n";

    uint8_t charge_status = power->getChargerStatus();
    uint8_t battper = power->getBatteryPercent();

    s += (charge_status == XPOWERS_AXP2101_CHG_STOP_STATE) ? " " : LV_SYMBOL_CHARGE;
    if (battper > 98) {
      s += LV_SYMBOL_BATTERY_FULL;
    } else if (battper > 75) {
      s += LV_SYMBOL_BATTERY_3;
    } else if (battper > 50) {
      s += LV_SYMBOL_BATTERY_2;
    } else if (battper > 25) {
      s += LV_SYMBOL_BATTERY_1;
    } else {
      s += LV_SYMBOL_BATTERY_EMPTY;
    }

    lv_label_set_text(Demo1label, s.c_str());
  }
}

//