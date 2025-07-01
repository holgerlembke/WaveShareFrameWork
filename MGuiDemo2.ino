//***********************************************************************************************
lv_obj_t* lbclock = NULL;
lv_obj_t* lbcharger = NULL;

//***********************************************************************************************
// follows https://github.com/lvgl/lvgl/blob/master/examples/layouts/flex/lv_example_flex_1.c
void setupGuiDem02() {
  const uint8_t topheight = 75;
  const uint8_t borderindent = 30;

  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  static int32_t topColDsc[] = { LCD_WIDTH / 2, LCD_WIDTH / 2, LV_GRID_TEMPLATE_LAST };
  static int32_t topRowDsc[] = { topheight, LV_GRID_TEMPLATE_LAST };

  /*Create a container with ROW flex direction*/
  lv_obj_t* topguibar = lv_obj_create(lv_screen_active());
  lv_obj_remove_style_all(topguibar);
  lv_obj_set_size(topguibar, LCD_WIDTH, topheight);
  lv_obj_set_layout(topguibar, LV_LAYOUT_GRID);
  lv_obj_set_style_grid_column_dsc_array(topguibar, topColDsc, 0);
  lv_obj_set_style_grid_row_dsc_array(topguibar, topRowDsc, 0);
  lv_obj_remove_flag(topguibar, LV_OBJ_FLAG_SCROLLABLE);

  /*Create a container with COLUMN flex direction*/
  lv_obj_t* contentpanel = lv_obj_create(lv_screen_active());
  lv_obj_remove_style_all(contentpanel);
  lv_obj_set_size(contentpanel, LCD_WIDTH, LCD_HEIGHT - topheight);
  lv_obj_align_to(contentpanel, contentpanel, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_flex_flow(contentpanel, LV_FLEX_FLOW_COLUMN);
  lv_obj_remove_flag(contentpanel, LV_OBJ_FLAG_SCROLLABLE);

  // Style for Clock upper left bar
  static lv_style_t styclock;  // static!
  lv_style_init(&styclock);
  // wg. displayrundung
  lv_style_set_pad_left(&styclock, borderindent);

  // Clock upper left bar
  lbclock = lv_label_create(topguibar);
  lv_label_set_text(lbclock, "12:23");
  lv_obj_align(lbclock, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_grid_cell(lbclock, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_style_text_align(lbclock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // D:\Arduino\sketches2.0\libraries\lv_conf.h Zeile 388ff: 14, 18, 24, 28, 42, 48
  lv_obj_set_style_text_font(lbclock, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_style(lbclock, &styclock, 0);

  // Charger etc upper right bar
  static lv_style_t stycharger;  // static!
  lv_style_init(&stycharger);
  // wg. displayrundung
  lv_style_set_pad_right(&stycharger, borderindent);

  // Charger etc upper right bar
  lbcharger = lv_label_create(topguibar);
  lv_label_set_text(lbcharger, "===");
  lv_obj_set_grid_cell(lbcharger, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_style_text_font(lbcharger, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_flex_align(lbcharger, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
  lv_obj_add_style(lbcharger, &stycharger, 0);
}

//***********************************************************************************************
inline void loopGuiDemo2() {
  const uint32_t tickertime = 1000;
  static uint32_t ticker = -tickertime;
  if (millis() - ticker >= tickertime) {
    ticker = millis();

    // Time top left
    time_t now;
    tm tm;
    time(&now);
    localtime_r(&now, &tm);
    char buf[6];
    strftime(buf, sizeof(buf), "%H:%M", &tm);

    String s = buf;

    static String lasttime = "";
    if (s != lasttime) {
      lv_label_set_text(lbclock, buf);
      lasttime = s;
    }

    uint8_t charge_status = power->getChargerStatus();
    uint8_t battper = power->getBatteryPercent();

    s = (autoPowerOffWarning) ? LV_SYMBOL_POWER : "";
    s += WiFi.status() == WL_CONNECTED ? LV_SYMBOL_WIFI : "";
    s += (charge_status == XPOWERS_AXP2101_CHG_STOP_STATE) ? "   " : " " LV_SYMBOL_CHARGE " ";
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

    static String lastcharger = "";
    if (s != lastcharger) {
      lastcharger = s;
      lv_label_set_text(lbcharger, s.c_str());
    }
  }
}

//
