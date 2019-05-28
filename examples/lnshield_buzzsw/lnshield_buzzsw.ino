#include <LnShield.h>

//Rpi board
#define PIN_RX          (0)   //UART
#define PIN_TX          (1)   //UART
#define PIN_OE          (4)   //UART Output Enable(use in library)

static LnShield sLn(PIN_OE);   //LightningNetwork Shield
static uint64_t sAmount = LnShield::AMOUNT_INIT;

//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t Status)
{
  if (Status == LnShield::STATUS_NORMAL) {
    digitalWrite(8, HIGH);
    tone(5, 2000, 500);
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  tone(5, 2000, 2000);
}


static void callbackError(LnShield::Err_t Err)
{
  (void)Err;
  digitalWrite(6, HIGH);
}

//////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);

  sLn.init();
  sLn.easyEventInit(callbackChangeStatus, callbackChangeMsat, callbackError);

  digitalWrite(6, LOW);
  digitalWrite(8, LOW);
}

void loop() {
  if (digitalRead(2) == LOW) {
    bool ret = sLn.easyEventRequestInvoice(2000);
    if (ret) {
      for (int i=0; i<2; i++) {
        tone(5, 1000, 500);
        tone(5, 2000, 500);
      }
      delay(2000);
    } else {
      tone(5, 100, 2000);
    }
  }

  sLn.easyEventPoll();
}
