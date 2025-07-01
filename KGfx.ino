/*
  Install:
   * Arduino_SH8601.h + .cpp aus dem Waveshare-Archive kopieren
   * in Arduino_GFX_Library.h in die #include-liste eintragen
   * die "override" bei void Display_Brightness(uint8_t brightness);
     und void SetContrast(uint8_t Contrast); entfernen

  Brightness:   ((Arduino_SH8601*)gfx)->Display_Brightness(204);

https://github.com/moononournation/Arduino_GFX/issues/649     

  NXP GUI Guider
*/

lv_draw_buf_t draw_buf;
lv_color_t *drawbuffer = NULL;
uint16_t drawbuffersize = LCD_WIDTH * LCD_HEIGHT / 10;

Arduino_DataBus *bus = NULL;
Arduino_GFX *gfx = NULL;
FT3x68 *touchsensor = NULL;
bool touchsensorIIC_Interrupt_Flag = false;

/*
  Rotation: 
    0 (buttons rechts) oder 3 (buttons links)

*/
uint8_t screenRotation = 3;

//***********************************************************************************************
static uint32_t my_tick(void) {
  return millis();
}

//***********************************************************************************************
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  // https://github.com/moononournation/Arduino_GFX/blob/master/examples/LVGL/LVGL_Arduino_v9/LVGL_Arduino_v9.ino
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
  lv_disp_flush_ready(disp);
}

//***********************************************************************************************
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (touchsensor->Interrupt_Flag) {
    touchsensor->Interrupt_Flag = false;

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touchsensor->getX();
    data->point.y = touchsensor->getY();

    if (screenRotation == 3) {
      data->point.x = LCD_WIDTH - data->point.x;
      data->point.y = LCD_HEIGHT - data->point.y;
    }
    autoPowerOffWatchdog = true;
    /*
    Serial.print("X: ");
    Serial.print(data->point.x);
    Serial.print(" Y: ");
    Serial.print(data->point.y);
    Serial.println();
    */
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

//***********************************************************************************************
void setupGfx() {
  bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

  gfx = new Arduino_SH8601(bus, -1 /* RST */,
                           screenRotation /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

  gfx->begin();
  ((Arduino_SH8601 *)gfx)->Display_Brightness(204);

  lv_init();
  lv_tick_set_cb(my_tick);

  drawbuffer = (lv_color_t *)malloc(drawbuffersize);

  lv_display_t *disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, drawbuffer, NULL, drawbuffersize, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // Touch Driver
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  //Touchpad should have POINTER type
  lv_indev_set_read_cb(indev, my_touchpad_read);
}

//***********************************************************************************************
inline void loopGfx() {
  /* let the GUI do its work */
  lv_timer_handler();
}

//***********************************************************************************************
void setupTouchSensor() {
  if (!HasDeviceAtI2CAddr(I2cFT3x68Addr)) {
    Serial.println("Panic: FT3168 communication failed.");
    while (1) delay(50);
  }

  touchsensor = new FT3x68(I2cFT3x68Addr, TP_INT);

  if (!touchsensor->begin()) {
    Serial.println("Panic: FT3168 begin() failed.");
    while (1) delay(50);
  }
}

//
