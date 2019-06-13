#include <stdio.h>
#include <stdlib.h>

#include <LnShield.h>

//Rpi board
#define PIN_RX          (0)   //UART
#define PIN_TX          (1)   //UART
#define PIN_OE          (4)   //UART Output Enable(use in library)

//debug board
#define PIN_LED_RED     (5)   //赤LED
#define PIN_BTN         (6)   //button

#define BUTTON_LIMIT    (20)


namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield

  int sInvoiceId = 0;
  char sInvoiceDesc[LnShield::INVOICE_DESC_MAX + 1];
  int sButtonCnt;
}


static void ledBlink(int num, int delayMs)
{
  for (int i=0; i<num; i++) {
    digitalWrite(PIN_LED_RED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_LED_RED, LOW);
    delay(delayMs);
  }
}

//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t status)
{
  if (status == LnShield::STATUS_NORMAL) {
    digitalWrite(PIN_LED_RED, LOW);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  ledBlink(4, 500);
}


static void callbackError(LnShield::Err_t err)
{
  (void)err;
  while (true) {
    digitalWrite(PIN_LED_RED, HIGH);
    delay(100);
    digitalWrite(PIN_LED_RED, LOW);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED_RED, OUTPUT);

  digitalWrite(PIN_LED_RED, HIGH);
  sLn.init();
  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  //button
  bool pushed = false;
  if (digitalRead(PIN_BTN) == 0) {
      if (sButtonCnt == BUTTON_LIMIT) {
          pushed = true;
          sButtonCnt++;
      } else if (sButtonCnt < BUTTON_LIMIT) {
          sButtonCnt++;
      }
  } else {
      sButtonCnt = 0;
  }

  if (pushed) {
    ledBlink(2, 100);
    sprintf(sInvoiceDesc, "payid#%d", sInvoiceId);
    if (sLn.easyEventRequestInvoice(1, sInvoiceDesc)) {
      sInvoiceId++;
    }
  }
  sLn.easyEventPoll();

  delay(100);
}
