#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <LnShield.h>

#include "board.h"
#include "dbgboard.h"
#include "max7219.h"


namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield
  uint64_t sAmount = LnShield::AMOUNT_INIT;
}


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  switch (Status) {
  case LnShield::STATUS_INIT:
    max7219_scroll_string("INIT");
    break;
  case LnShield::STATUS_STARTING:
    max7219_scroll_string("STARTING");
    break;
  case LnShield::STATUS_NORMAL:
    sAmount = sLn.getLastMsat();
    char str[30];
    sprintf(str, "NORMAL %ld", (unsigned long)sAmount);
    max7219_scroll_string(str);

    for (int i=0; i<2; i++) {
      dbgboard_led(DBGBOARD_LED_ERROR);
      delay(200);
    }
    digitalWrite(PIN_LED_RED, LOW);
    break;
  default:
    break;
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  if ((sAmount != LnShield::AMOUNT_INIT) && (sAmount - amountMsat != 0)) {
    for (int i=0; i<4; i++) {
      dbgboard_led(DBGBOARD_LED_ERROR);
      delay(200);
    }
    digitalWrite(PIN_LED_RED, LOW);
    char str[30];
    sprintf(str, "RECEIVE %ld", (unsigned long)(amountMsat - sAmount));
    max7219_scroll_string(str);
  }
  sAmount = amountMsat;
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;

  dbgboard_led(DBGBOARD_LED_ERROR);
  while (true) {
    max7219_scroll_string("ERROR");
    delay(10000);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  dbgboard_setup();

  dbgboard_led(DBGBOARD_LED_INIT);
  sLn.init();
  dbgboard_led(DBGBOARD_LED_WAIT);

  max7219_setup();

  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
  max7219_scroll_string("HELLO");
}


void loop() {
  if (dbgboard_button()) {
    int ret = sLn.cmdInvoice(1000);
    if (ret == LnShield::ENONE) {
      max7219_scroll_string("INVOICE 1000");
    } else {
      max7219_scroll_string("FAIL INVOICE");
    }
  }
  sLn.easyEventPoll();

  delay(100);
}
