#include <stdio.h>
#include <stdlib.h>

#include <LnShield.h>
#include <SoftwareSerial.h>

#define PIN_LED         (5)   //LED
#define PIN_BTN         (6)   //button

#define BUTTON_LIMIT    (20)

#define SEND_MSAT       (10000)

//#define M_DEBUG_MODE
#ifdef M_DEBUG_MODE
#define DBG_SERIAL_RX   (2)   //not use
#define DBG_SERIAL_TX   (3)   //debug output
#define DBG_BEGIN()         sDebug.begin(115200)
#define DBG_PRINTLN(val)    sDebug.println(val)
#else
#define DBG_BEGIN()         //none
#define DBG_PRINTLN(val)    //none
#endif


namespace {
  LnShield sLn;       //Lightning Shield instance

  int sInvoiceId = 0;
  char sInvoiceDesc[LnShield::INVOICE_DESC_MAX + 1];
  int sButtonCnt;

#ifdef M_DEBUG_MODE
  SoftwareSerial sDebug(DBG_SERIAL_RX, DBG_SERIAL_TX);
#endif
}


static void ledBlink(int num, int delayMs)
{
  for (int i=0; i<num; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_LED, LOW);
    delay(delayMs);
  }
}

//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t status)
{
  if (status == LnShield::STATUS_NORMAL) {
    DBG_PRINTLN("status: NORMAL");
    digitalWrite(PIN_LED, LOW);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  DBG_PRINTLN("change amount");
  ledBlink(4, 500);
}


static void callbackError(LnShield::Err_t err)
{
  (void)err;
  DBG_PRINTLN("error");
  while (true) {
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  DBG_BEGIN();
  DBG_PRINTLN("start");

  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_LED, HIGH);
  sLn.init();
  sLn.eventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
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
    if (sLn.cmdInvoice(SEND_MSAT, sInvoiceDesc) == LnShield::ENONE) {
      DBG_PRINTLN("create invoice");
      sInvoiceId++;
    }
  }
  sLn.eventPoll();

  delay(100);
}
