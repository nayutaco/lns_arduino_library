#include <stdlib.h>
#include <LnShield.h>

#include "board.h"
#include "dbgboard.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield
}


//////////////////////////////////////////////////////////////
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static int ledCount;

static void colorWipe(uint32_t c, uint8_t wait);


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  if (Status == LnShield::STATUS_NORMAL) {
    dbgboard_buzzer(DBGBOARD_BUZZER_CHGSTAT);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  dbgboard_buzzer(DBGBOARD_BUZZER_GET);
  for (int i=0; i<5; i++) {
    neopixelBlink(strip.Color(191, 191, 0), 50); // Yellow
    neopixelBlink(strip.Color(0, 0, 63), 50); // Blue
  }
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;

  dbgboard_led(DBGBOARD_LED_ERROR);
  while (true) {
    colorWipe(strip.Color(64, 0, 0), 0);
    colorWipe(strip.Color(0, 0, 0), 0);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  dbgboard_setup();

  dbgboard_led(DBGBOARD_LED_INIT);
  dbgboard_buzzer(DBGBOARD_BUZZER_INIT);
  sLn.init();
  dbgboard_led(DBGBOARD_LED_WAIT);

    //neopixel
  strip.begin();
  strip.setBrightness(50);
  strip.show(); 

  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  //button
  bool on = dbgboard_button();
  if (on) {
    bool ret = sLn.easyEventRequestInvoice(2000);
    if (ret) {
      dbgboard_buzzer(DBGBOARD_BUZZER_INVOICE);
      colorWipe(strip.Color(0, 0, 192), 0);
      colorWipe(strip.Color(0, 0, 0), 0);
    } else {
      dbgboard_buzzer(DBGBOARD_BUZZER_ERROR);
    }
  }
  dbgboard_led(DBGBOARD_LED_NORMAL);
  
  ledCount++;
  if ((ledCount % 20) == 0) {
    colorWipe(strip.Color(0, 10, 0), 0);
    colorWipe(strip.Color(0, 0, 0), 0);
  }
  sLn.easyEventPoll();

  delay(100);
}


void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void neopixelBlink(uint32_t c, uint8_t wait) {
  for (int j=0; j<5; j++) {
      for (uint16_t i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);    //turn every pixel on
      }
      strip.show();
      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);        //turn every pixel off
      }
      strip.show();
      delay(wait);
  }
}
