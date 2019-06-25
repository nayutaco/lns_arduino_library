#include <Servo.h>

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <LnShield.h>

#include "board.h"


namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield

  Servo sServo;
  uint64_t mAmount = LnShield::AMOUNT_INIT;
}


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t status)
{
  if (status == LnShield::STATUS_NORMAL) {
    for (int i = 0; i < 5; i++) {
      sServo.write(100);
      delay(500);
      sServo.write(150);
    }
  }
}


static void callbackChangeMsat(uint64_t amountMsat)
{
  if (mAmount != LnShield::AMOUNT_INIT) {
    if (amountMsat >= 1000) {
      for (int i=0; i<5 * amountMsat / 1000; i++) {
        sServo.write(90);
        delay(100);
        sServo.write(150);
      }
    }
  }
  mAmount = amountMsat;
}


static void callbackError(LnShield::Err_t Err)
{
  while (true) {
    delay(10000);
  }
}

//////////////////////////////////////////////////////////////

void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  sServo.attach(PIN_SERVO);
  sServo.write(150);
  delay(1000);
  sServo.write(10);
  delay(1000);
  sServo.write(150);

  sLn.init();
  sLn.eventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  if (digitalRead(PIN_BUTTON) == LOW) {
    sServo.write(155);
    sLn.cmdInvoice(1000, "");
    delay(2000);
    sServo.write(150);
  }
  sLn.eventPoll();

  delay(100);
}
