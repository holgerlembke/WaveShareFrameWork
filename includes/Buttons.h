// losely based on "By Jeff Saltzman Oct. 13, 2009"
// 4-Way Button:  Click, Double-Click, Press+Hold, and Press+Long-Hold Test Sketch

union CBits {
  struct Bits {
    unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
  };
  unsigned char byte;
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class baseButton {
protected:
  uint8_t pin;
  virtual uint8_t readit() = 0;  // abstract || pure virtual
private:

  // Button timing variables
  uint32_t debounce = 20;        // ms debounce period to prevent flickering when pressing or releasing the button
  uint32_t DCgap = 250;          // max ms between clicks for a double click event
  uint32_t holdTime = 1000;      // ms hold period: how long to wait for press+hold event
  uint32_t longHoldTime = 3000;  // ms long hold period: how long to wait for press+hold event

  int32_t downTime = -1;  // time the button was pressed down
  int32_t upTime = -1;    // time the button was released

  // save some bytes by using single bits
  union CBits {
    struct Bits {
      boolean buttonVal : 1,
        buttonLast : 1,
        DCwaiting : 1,
        DConUp : 1,
        singleOK : 1,
        ignoreUp : 1,
        waitForUp : 1,
        holdEventPast : 1;
    } b;
    unsigned char byte;
  } b;

  // Button variables
  /*
  boolean buttonVal2 = HIGH;          // value read from button
  boolean buttonLast2 = HIGH;          // buffered value of the button's previous state
  boolean DCwaiting2 = false;          // whether we're waiting for a double click (down)
  boolean DConUp2 = false;             // whether to register a double click on next release, or whether to wait and click
  boolean singleOK2 = true;            // whether it's OK to do a single click
  boolean ignoreUp2 = false;           // whether to ignore the button release because the click+hold was triggered
  boolean waitForUp2 = false;          // when held, whether to wait for the up event
  boolean holdEventPast2 = false;      // whether or not the hold event happened already
  */
  boolean longHoldEventPast = false;  // whether or not the long hold event happened already

public:
  enum clickEvent { none,         // 0
                    click,        // 1
                    doubleClick,  // 2
                    hold,         // 3
                    longHold };   // 4

  clickEvent checkButton() {
    clickEvent event = none;
    b.b.buttonVal = readit();
    // Button pressed down
    if (b.b.buttonVal == LOW && b.b.buttonLast == HIGH && (millis() - upTime) > debounce) {
      downTime = millis();
      b.b.ignoreUp = false;
      b.b.waitForUp = false;
      b.b.singleOK = true;
      b.b.holdEventPast = false;
      longHoldEventPast = false;
      if ((millis() - upTime) < DCgap && b.b.DConUp == false && b.b.DCwaiting == true) {
        b.b.DConUp = true;
      } else {
        b.b.DConUp = false;
      }
      b.b.DCwaiting = false;
    }
    // Button released
    else if (b.b.buttonVal == HIGH && b.b.buttonLast == LOW && (millis() - downTime) > debounce) {
      if (not b.b.ignoreUp) {
        upTime = millis();
        if (b.b.DConUp == false) {
          b.b.DCwaiting = true;
        } else {
          event = doubleClick;
          b.b.DConUp = false;
          b.b.DCwaiting = false;
          b.b.singleOK = false;
        }
      }
    }
    // Test for normal click event: DCgap expired
    if (b.b.buttonVal == HIGH && (millis() - upTime) >= DCgap && b.b.DCwaiting == true && b.b.DConUp == false && b.b.singleOK == true && event != 2) {
      event = click;
      b.b.DCwaiting = false;
    }
    // Test for hold
    if (b.b.buttonVal == LOW && (millis() - downTime) >= holdTime) {
      // Trigger "normal" hold
      if (not b.b.holdEventPast) {
        event = hold;
        b.b.waitForUp = true;
        b.b.ignoreUp = true;
        b.b.DConUp = false;
        b.b.DCwaiting = false;
        //downTime = millis();
        b.b.holdEventPast = true;
      }
      // Trigger "long" hold
      if ((millis() - downTime) >= longHoldTime) {
        if (not longHoldEventPast) {
          event = longHold;
          longHoldEventPast = true;
        }
      }
    }
    b.b.buttonLast = b.b.buttonVal;
    return event;
  }

  //-------------------------------------------------------------------------------------------
  baseButton() {
    b.b.buttonVal = HIGH;       // value read from button
    b.b.buttonLast = HIGH;      // buffered value of the button's previous state
    b.b.DCwaiting = false;      // whether we're waiting for a double click (down)
    b.b.DConUp = false;         // whether to register a double click on next release, or whether to wait and click
    b.b.singleOK = true;        // whether it's OK to do a single click
    b.b.ignoreUp = false;       // whether to ignore the button release because the click+hold was triggered
    b.b.waitForUp = false;      // when held, whether to wait for the up event
    b.b.holdEventPast = false;  // whether or not the hold event happened already
  }
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class pinButton : public baseButton {
private:
  //-------------------------------------------------------------------------------------------
  virtual uint8_t readit() override {
    return digitalRead(pin);
  }
public:
  //-------------------------------------------------------------------------------------------
  pinButton(uint8_t pin)
    : baseButton() {
    this->pin = pin;
    pinMode(pin, INPUT);
  }
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class expanderButton : public baseButton {
private:
  TCA9554* expander;
  //-------------------------------------------------------------------------------------------
  virtual uint8_t readit() override {  // power button is pulldown
    return !expander->read1(pin);
  }
public:
  //-------------------------------------------------------------------------------------------
  expanderButton(uint8_t pin, TCA9554* expander)
    : baseButton() {
    this->expander = expander;
    this->pin = pin;
    expander->pinMode1(pin, INPUT);
    /* verify, that reading is far away from millis()
    uint32_t ds = millis();
    expander->read1(pin);
    uint32_t de = millis();
    Serial.println(de-ds);
    */
  }
};
