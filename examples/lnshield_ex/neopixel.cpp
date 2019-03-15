#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#include "board.h"
#include "neopixel.h"


#define NUM_NEOPIXEL    (16)
//#define NUM_NEOPIXEL    (144)
#define LED_DARK        (3)


static Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static int mBlink;

static uint32_t Wheel(byte WheelPos);
static uint32_t index2color(int Index);


void neopixel_setup()
{
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) {
        clock_prescale_set(clock_div_1);
    }
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


NeoPixelLedType_t neopixel_led(NeoPixelLedType_t Type)
{
    bool ret;

    switch (Type) {
    case NEOPIXEL_LED_START:
        for (int lp = 0; lp < 30; lp++) {
            for (int led = 0; led < 3; led++) {
                neopixel_wipe(led);
                delay(100);
            }
        }
        break;
    case NEOPIXEL_LED_WAIT:
        neopixel_wipe((mBlink / NUM_NEOPIXEL) % 3, mBlink);
        break;
    case NEOPIXEL_LED_INITED:
        neopixel_clear();
        Type = NEOPIXEL_LED_NORMAL;
        break;
    case NEOPIXEL_LED_NORMAL:
        //none
#if 1
        //neopixel_rainbow_cycle();
        if ((mBlink % 160) == 0) {
            neopixel_clear();
        }
        neopixel_wipe(((mBlink / NUM_NEOPIXEL) % 4) + LED_DARK, mBlink);
#else
        neopixel_wipe_noshow(((mBlink / NUM_NEOPIXEL) % 3) + LED_DARK, mBlink);
        mBlink++;
        neopixel_wipe_noshow(((mBlink / NUM_NEOPIXEL) % 3) + LED_DARK, mBlink);
        strip.show();
#endif
        break;
    case NEOPIXEL_LED_GET:
        ret = neopixel_rainbow();
        if (ret) {
            neopixel_clear();
            Type = NEOPIXEL_LED_NORMAL;
        }
        break;
    case NEOPIXEL_LED_PAY:
        ret = neopixel_rainbow_cycle();
        if (ret) {
            neopixel_clear();
            Type = NEOPIXEL_LED_NORMAL;
        }
        break;
    case NEOPIXEL_LED_ERROR:
        neopixel_wipe(mBlink % 2);
        break;
    }
    mBlink++;
    return Type;
}


void neopixel_wipe(int ColIdx)
{
    uint32_t c = index2color(ColIdx);
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    strip.show();
}

void neopixel_wipe(int ColIdx, uint16_t Index)
{
    neopixel_wipe_noshow(ColIdx, Index);
    strip.show();
}

void neopixel_wipe_noshow(int ColIdx, uint16_t Index)
{
    uint32_t c = index2color(ColIdx);
    strip.setPixelColor(Index % strip.numPixels(), c);
}

bool neopixel_rainbow()
{
    static uint16_t j;

    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    bool ret = false;
    j += 2;
    if (j >= 256) {
        j = 0;
        ret = true;
    }
    return ret;
}

bool neopixel_rainbow_cycle()
{
    static uint16_t j;

    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        strip.show();
    }
    bool ret = false;
    j += 4;
    if (j >= 256*20) {
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
    if (WheelPos < 85) {
        return strip.Color((255 - WheelPos * 3), 0, (WheelPos * 3) >> 1);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, (WheelPos * 3), (255 - WheelPos * 3) >> 1);
    }
    WheelPos -= 170;
    return strip.Color((WheelPos * 3) >> 1, (255 - WheelPos * 3), 0);
}


static uint32_t index2color(int Index)
{
    uint8_t r = 0, g = 0, b = 0;

    switch (Index) {
    case 0:
        r = 127;
        break;
    case 1:
        g = 127;
        break;
    case 2:
        b = 127;
        break;
    case 3:
        r = 7;
        break;
    case 4:
        g = 7;
        break;
    case 5:
        b = 7;
        break;
    default:
        break;
    }
    return strip.Color(r, g, b);
}
