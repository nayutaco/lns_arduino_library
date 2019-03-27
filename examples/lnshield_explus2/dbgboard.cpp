#include <Arduino.h>

#include "board.h"
#include "dbgboard.h"

#define BUTTON_LIMIT    (20)


static void beep(int num, int beat);
static void beep(int num);


static int mButtonCnt;
static int mBlink;


void dbgboard_setup()
{
    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GRN, OUTPUT);
}


void dbgboard_led(DbgBoardLedType_t Type)
{
    switch (Type) {
    case DBGBOARD_LED_INIT:
        digitalWrite(PIN_LED_RED, HIGH);
        digitalWrite(PIN_LED_GRN, HIGH);
        break;

    case DBGBOARD_LED_WAIT:
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_GRN, LOW);
        break;

    case DBGBOARD_LED_NORMAL:
        if ((mBlink / 10) % 2) {
            digitalWrite(PIN_LED_GRN, LOW);
        } else {
            digitalWrite(PIN_LED_GRN, HIGH);
        }
        break;

    case DBGBOARD_LED_ERROR:
        digitalWrite(PIN_LED_RED, mBlink & 1);
        digitalWrite(PIN_LED_GRN, !(mBlink & 1));
        break;
    }

    mBlink++;
}


void dbgboard_buzzer(DbgBoardBuzzerType_t Type)
{
    switch (Type) {
    case DBGBOARD_BUZZER_INIT:
        for (int i = 0; i < 2; i++) {
            beep(4, 100);
            beep(5, 100);
            beep(6, 100);
        }
        break;

    case DBGBOARD_BUZZER_CHGSTAT:
        beep(0);
        break;

    case DBGBOARD_BUZZER_INVOICE:
        beep(4);
        break;

    case DBGBOARD_BUZZER_GET:
        beep(4, 100);
        beep(7, 200);
        break;

    case DBGBOARD_BUZZER_PAY:
        beep(1, 100);
        beep(2, 200);
        beep(1, 200);
        break;

    case DBGBOARD_BUZZER_ERROR:
        beep(0, 2000);
        break;
    }
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


static void beep(int num, int beat)
{
    const int HZ[] = {
        262, 294, 330, 349, 392, 440, 494, 523,
    };
    tone(PIN_BUZZ, HZ[num], beat);
    delay(beat);
}


static void beep(int num)
{
    const int BEAT = 500;
    beep(num, BEAT);
}
