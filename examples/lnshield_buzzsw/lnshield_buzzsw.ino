#include <stdio.h>
#include <stdlib.h>
#include <LnShield.h>

//Rpi board
#define PIN_RX          (0)   //UART
#define PIN_TX          (1)   //UART
#define PIN_OE          (4)   //UART Output Enable(use in library)

static LnShield sLn(PIN_OE);   //LightningNetwork Shield
static uint64_t sAmount = LnShield::AMOUNT_INIT;
static int sInvoiceId = 0;
static char sInvoiceDesc[LnShield::INVOICE_DESC_MAX + 1];

//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  if (Status == LnShield::STATUS_NORMAL) {
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    tone(5, 2000, 500);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  for (int lp = 0; lp < 3; lp++) {
    tone(5, 2000, 200);
    delay(200);
  }
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;
  digitalWrite(6, HIGH);
  while (true);
}

//////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);

  digitalWrite(6, LOW);
  digitalWrite(8, LOW);

  for (int lp = 0; lp < 7; lp++) {
    tone(5, 500 * lp, 300);
    //noTone(5);
    delay(200);
  }

  sLn.init();
  sLn.eventInit(callbackChangeStatus, callbackChangeMsat, callbackError);

  digitalWrite(6, HIGH);
  digitalWrite(8, HIGH);
}

void loop() {
  if (digitalRead(2) == LOW) {
    sprintf(sInvoiceDesc, "payid#%d", sInvoiceId);
    LnShield::Err_t ret = sLn.cmdInvoice(1, sInvoiceDesc);
    if (ret == LnShield::ENONE) {
      sInvoiceId++;
      for (int i=0; i<2; i++) {
        tone(5, 1000, 500);
        tone(5, 2000, 500);
      }
      delay(2000);
    } else {
      tone(5, 100, 2000);
    }
  }

  sLn.eventPoll();
}
