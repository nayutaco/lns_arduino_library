#ifndef BOARD_H__
#define BOARD_H__

//Rpi board
#define PIN_RX          (0)   //UART
#define PIN_TX          (1)   //UART
#define PIN_OE          (4)   //UART Output Enable(use in library)

//NeoPixel
#define PIN_NEOPIXEL    (2)

//debug board
#define PIN_LED_RED     (5)   //赤LED
#define PIN_BTN         (6)   //button

//!!DO NOT USE!!
#define PIN_D10         (10)  //SPI:SS
#define PIN_D11         (11)  //SPI:MOSI
#define PIN_D12         (12)  //SPI:MISO
#define PIN_D13         (13)  //SPI:SCK

#endif /* BOARD_H__ */
