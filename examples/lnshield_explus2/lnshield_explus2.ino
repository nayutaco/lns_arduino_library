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
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static int ledCount;

static void colorWipe(uint32_t c, uint8_t wait);


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  dbgboard_buzzer(DBGBOARD_BUZZER_CHGSTAT);
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  for (int i=0; i<3; i++) {
    neopixelBlink(strip.Color(255, 0, 0), 50); // Red
    neopixelBlink(strip.Color(0, 0, 0), 50); // turn off
  }
}


static void callbackError()
{
  dbgboard_led(DBGBOARD_LED_ERROR);
  colorWipe(strip.Color(80, 0, 40), 0);
  colorWipe(strip.Color(0, 0, 0), 0);
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
    dbgboard_buzzer(DBGBOARD_BUZZER_INVOICE);
    sLn.easyEventRequestInvoice(2000);
    colorWipe(strip.Color(80, 0, 255), 0); // Blue
    colorWipe(strip.Color(0, 0, 0), 0); // turn off
  }
  dbgboard_led(DBGBOARD_LED_NORMAL);
  
  ledCount++;
  if ((ledCount % 20) == 0) {
    colorWipe(strip.Color(20, 20, 0), 0);
    colorWipe(strip.Color(0, 0, 0), 0);
  }
  sLn.easyEventLoop();

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
