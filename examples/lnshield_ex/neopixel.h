#ifndef NEO_PIXEL_H__
#define NEO_PIXEL_H__

#include <stdint.h>


enum NeoPixelLedType_t {
    NEOPIXEL_LED_WAIT,
    NEOPIXEL_LED_INITED,
    NEOPIXEL_LED_NORMAL,
    NEOPIXEL_LED_GET,
    NEOPIXEL_LED_PAY,
    NEOPIXEL_LED_ERROR,
};


void neopixel_setup();
void neopixel_clear();
NeoPixelLedType_t neopixel_led(NeoPixelLedType_t Type);


void neopixel_wipe(int ColIdx);
void neopixel_wipe(int ColIdx, uint16_t index);
bool neopixel_rainbow();

#endif /* NEO_PIXEL_H__ */
