#include <stdlib.h>
#include <LnShield.h>

#include "board.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield
  uint64_t mAmount = LnShield::AMOUNT_INIT;
}


//////////////////////////////////////////////////////////////
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static int ledCount;

static void colorWipe(uint32_t c, uint8_t wait);


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  for (int i=0; i<3; i++) {
    neopixelBlink(strip.Color(63, 0, 0), 50);
    neopixelBlink(strip.Color(0, 63, 0), 50);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  if (mAmount != LnShield::AMOUNT_INIT) {
    for (int i=0; i<5; i++) {
      neopixelBlink(strip.Color(191, 191, 0), 50); // Yellow
      neopixelBlink(strip.Color(0, 0, 63), 50); // Blue
    }
  }
  mAmount = amountMsat;
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;

  while (true) {
    colorWipe(strip.Color(64, 0, 0), 50);
    colorWipe(strip.Color(0, 0, 0), 50);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  sLn.init();

  pinMode(PIN_BTN, INPUT_PULLUP);

  //neopixel
  strip.begin();
  strip.setBrightness(30);
  strip.show(); 

  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  bool ledEvent = false;

  if (digitalRead(PIN_BTN) == 0) {
    LnShield::Err_t ret = sLn.easyEventRequestInvoice(2000, "ringled");
    if (ret == LnShield::ENONE) {
      colorWipe(strip.Color(0, 0, 192), 50);
      colorWipe(strip.Color(0, 0, 0), 50);
    } else {
      colorWipe(strip.Color(128, 0, 192), 10);
    }
    ledEvent = true;
  }

  
  if (!ledEvent) {
    ledCount++;
    if ((ledCount % 20) == 0) {
      colorWipe(strip.Color(0, 10, 0), 50);
      colorWipe(strip.Color(0, 0, 0), 50);
    }
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
