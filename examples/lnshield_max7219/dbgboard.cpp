#include <Arduino.h>

#include "board.h"
#include "dbgboard.h"

#define BUTTON_LIMIT    (20)


static int mButtonCnt;
static int mBlink;


void dbgboard_setup()
{
    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(PIN_LED_RED, OUTPUT);
}


void dbgboard_led(DbgBoardLedType_t Type)
{
    switch (Type) {
    case DBGBOARD_LED_INIT:
        digitalWrite(PIN_LED_RED, HIGH);
        break;

    case DBGBOARD_LED_WAIT:
        digitalWrite(PIN_LED_RED, LOW);
        break;

    case DBGBOARD_LED_NORMAL:
        if ((mBlink / 10) % 2) {
            digitalWrite(PIN_LED_RED, LOW);
        } else {
            digitalWrite(PIN_LED_RED, HIGH);
        }
        break;

    case DBGBOARD_LED_ERROR:
        digitalWrite(PIN_LED_RED, mBlink & 1);
        break;
    }

    mBlink++;
}


bool dbgboard_button()
{
    bool ret = false;
    if (digitalRead(PIN_BTN) == 0) {
        if (mButtonCnt == BUTTON_LIMIT) {
            ret = true;
            mButtonCnt++;
        } else if (mButtonCnt < BUTTON_LIMIT) {
            mButtonCnt++;
        }
    } else {
        mButtonCnt = 0;
    }

    return ret;
}
