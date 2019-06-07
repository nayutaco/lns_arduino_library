#include <stdio.h>
#include <stdlib.h>

#include <LnShield.h>

#include "board.h"
#include "dbgboard.h"


namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield

  int sInvoiceId = 0;
  char sInvoiceDesc[LnShield::INVOICE_DESC_MAX + 1];
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
    sprintf(sInvoiceDesc, "payid#%d", sInvoiceId);
    if (sLn.easyEventRequestInvoice(1, sInvoiceDesc)) {
      sInvoiceId++;
    } else {
      
    }
  }
  sLn.easyEventPoll();

  delay(100);
}
