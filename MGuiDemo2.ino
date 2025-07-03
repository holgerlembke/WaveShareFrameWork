//***********************************************************************************************
lv_obj_t* lbclock = NULL;
lv_obj_t* lbcharger = NULL;

//***********************************************************************************************
// follows https://github.com/lvgl/lvgl/blob/master/examples/layouts/flex/lv_example_flex_1.c
void setupGuiDem02() {
  const uint8_t topheight = 40;
  const uint8_t borderindent = 30;

  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  static int32_t topColDsc[] = { LCD_WIDTH / 2, LCD_WIDTH / 2, LV_GRID_TEMPLATE_LAST };
  static int32_t topRowDsc[] = { topheight, LV_GRID_TEMPLATE_LAST };

  lv_point_t result;
  //lv_text_get_size(&result, "Hy", &lv_font_montserrat_28,0,0,0,0);

  /*  
lv_txt_get_size(
  &size, 
  lv_table_get_cell_value(table, row, col), 
  dsc->label_dsc->font, 
  dsc->label_dsc->letter_space, 
  dsc->label_dsc->line_space,
  LV_COORD_MAX, 
  dsc->label_dsc->flag);    
    
    lv_text_get_size

void lv_text_get_size(lv_point_t *size_res, const char *text, const lv_font_t *font, int32_t letter_space, int32_t line_space, int32_t max_width, lv_text_flag_t flag)

    Get size of a text

    line breaks

    Parameters:
            size_res – pointer to a 'point_t' variable to store the result
            text – pointer to a text
            font – pointer to font of the text
            letter_space – letter space of the text
            line_space – line space of the text
            max_width – max width of the text (break the lines to fit this size). Set COORD_MAX to avoid
            flag – settings for the text from lv_text_flag_t
*/

  lv_color_t black = lv_color_hex(0x0);
  lv_color_t white = lv_color_hex(0xFFFFFF);

  /*Guibar: tabelle*/
  lv_obj_t* topguibar = lv_obj_create(lv_screen_active());
  lv_obj_remove_style_all(topguibar);
  lv_obj_set_size(topguibar, LCD_WIDTH, topheight);
  lv_obj_set_layout(topguibar, LV_LAYOUT_GRID);
  lv_obj_set_style_grid_column_dsc_array(topguibar, topColDsc, 0);
  lv_obj_set_style_grid_row_dsc_array(topguibar, topRowDsc, 0);
  lv_obj_remove_flag(topguibar, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(topguibar, black, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(topguibar, LV_OPA_COVER, LV_PART_MAIN);

  // Style for Clock upper left bar
  static lv_style_t styclock;  // static!
  lv_style_init(&styclock);
  // wg. displayrundung
  lv_style_set_pad_left(&styclock, borderindent);
  lv_style_set_bg_opa(&styclock, LV_OPA_COVER);

  // Clock upper left bar
  lbclock = lv_label_create(topguibar);
  lv_label_set_text(lbclock, "12:23");
  lv_obj_align(lbclock, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_grid_cell(lbclock, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_style_text_align(lbclock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  // D:\Arduino\sketches2.0\libraries\lv_conf.h Zeile 388ff: 14, 18, 24, 28, 42, 48
  lv_obj_set_style_text_font(lbclock, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_style(lbclock, &styclock, 0);
  lv_obj_set_style_text_color(lbclock, white, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lbclock, black, LV_PART_MAIN);

  // Charger etc upper right bar
  static lv_style_t stycharger;  // static!
  lv_style_init(&stycharger);
  // wg. displayrundung
  lv_style_set_pad_right(&stycharger, borderindent);
  lv_style_set_bg_opa(&stycharger, LV_OPA_COVER);

  // Charger etc upper right bar
  lbcharger = lv_label_create(topguibar);
  lv_label_set_text(lbcharger, "===");
  lv_obj_set_grid_cell(lbcharger, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_style_text_font(lbcharger, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_flex_align(lbcharger, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
  lv_obj_add_style(lbcharger, &stycharger, 0);
  lv_obj_set_style_text_color(lbcharger, white, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lbcharger, black, LV_PART_MAIN);

  // https://docs.lvgl.io/master/details/widgets/list.html
  /*Create a container with COLUMN flex direction*/
  /*
  lv_obj_t* contentpanel = lv_obj_create(lv_screen_active());
  lv_obj_remove_style_all(contentpanel);
  lv_obj_set_size(contentpanel, LCD_WIDTH, LCD_HEIGHT - topheight);
  lv_obj_align_to(contentpanel, topguibar, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_flex_flow(contentpanel, LV_FLEX_FLOW_COLUMN);
  // lv_obj_remove_flag(contentpanel, LV_OBJ_FLAG_SCROLLABLE);
  */

  lv_obj_t* contentpanel = lv_list_create(lv_screen_active());
  //  lv_obj_remove_style_all(contentpanel);
  lv_obj_set_size(contentpanel, LCD_WIDTH, LCD_HEIGHT - topheight);
  lv_obj_align_to(contentpanel, topguibar, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_style_pad_all(contentpanel, 0, LV_PART_MAIN);
  //lv_obj_set_style_pad_row(contentpanel, 5, 0);


  String buttonitems[] = {
    "AAAA",
    "BBBBBBBBBBBBBBBBB",
    "CCCCC",
    "DdDDDDdddddDDDD",
    "EEEEE",
    "FF",
    "GGGGGGGGG",
    "HHHHHHHHHHHHHHHHHHHHHHHHHH",
    "MM",
    "KKKKKKKKKKKK",
    "LLLL",
    "QQQQQQQ",
    "TTTT",
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
    "WWWWWWWWWWWWWW",
    ""
  };

  uint8_t i = 0;
  do {
    lv_obj_t* button = lv_button_create(contentpanel);
    lv_obj_set_size(button, LCD_WIDTH - 10, 55);
    lv_obj_set_style_pad_all(button, 2, LV_PART_MAIN);
    lv_obj_set_style_margin_all(button, 3, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LCD_WIDTH - 20);
    lv_label_set_text(label, buttonitems[i].c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(label);

    i++;
  } while (buttonitems[i] != "");
}

/*
#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SWITCH

static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    lv_obj_t * btn = lv_event_get_target_obj(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%" LV_PRIu32, cnt);
    cnt++;
}


void lv_example_event_click(void)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);
}




*/



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
