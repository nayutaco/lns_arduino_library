#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#include "board.h"
#include "neopixel.h"


#define NUM_NEOPIXEL    (16)


static Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

static uint32_t Wheel(byte WheelPos);
static uint32_t index2color(int Index);


void neopixel_setup()
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

void neopixel_clear()
{
  strip.clear();
  strip.show();
}


NeoPixelLedType_t neopixel_led(uint16_t Blink, NeoPixelLedType_t Type)
{
  bool ret;

  switch (Type) {
  case NEOPIXEL_LED_WAIT:
    neopixel_wipe(((Blink / NUM_NEOPIXEL) % 3), Blink);
    break;
  case NEOPIXEL_LED_INITED:
    neopixel_clear();
    Type = NEOPIXEL_LED_NORMAL;
    break;
  case NEOPIXEL_LED_NORMAL:
    //none
    neopixel_wipe(((Blink / NUM_NEOPIXEL) % 3) + 3, Blink);
    break;
  case NEOPIXEL_LED_GET:
    ret = neopixel_rainbow();
    if (ret) {
      neopixel_clear();
      Type = 2;
    }
    break;
  }
  return Type;
}


void neopixel_wipe(int ColIdx)
{
  uint32_t c = index2color(ColIdx);
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void neopixel_wipe(int ColIdx, uint16_t Index)
{
  uint32_t c = index2color(ColIdx);
  strip.setPixelColor(Index % strip.numPixels(), c);
  strip.show();
}

bool neopixel_rainbow()
{
  static uint16_t j;

  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i+j) & 255));
    strip.show();
  }
  bool ret = false;
  j += 8;
  if (j >= 256) {
    j = 0;
    ret = true;
  }
  return ret;
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
static uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


static uint32_t index2color(int Index)
{
  uint32_t color;

  switch (Index) {
  case 1:
    color = strip.Color(0, 255, 0);
    break;
  case 2:
    color = strip.Color(0, 0, 255);
    break;
  case 3:
    color = strip.Color(8, 0, 0);
    break;
  case 4:
    color = strip.Color(0, 8, 0);
    break;
  case 5:
    color = strip.Color(0, 0, 8);
    break;
  default:
    color = strip.Color(255, 0, 0);
    break;
  }
  return color;
}
