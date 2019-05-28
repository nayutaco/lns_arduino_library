#include <stdlib.h>

#include <LnShield.h>

#include "board.h"
#include "dbgboard.h"


namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield
}


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  digitalWrite(PIN_LED_RED, LOW);
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  for (int i=0; i<4; i++) {
    dbgboard_led(DBGBOARD_LED_ERROR);
    delay(200);
  }
  digitalWrite(PIN_LED_RED, LOW);
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;
  dbgboard_led(DBGBOARD_LED_ERROR);
  while (true) {
    delay(10000);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  dbgboard_setup();

  dbgboard_led(DBGBOARD_LED_INIT);
  sLn.init();
  dbgboard_led(DBGBOARD_LED_WAIT);

  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  if (dbgboard_button()) {
    sLn.easyEventRequestInvoice(1000);
  }
  sLn.easyEventPoll();

  delay(100);
}
