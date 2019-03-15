//#define USE_SOFTWARESERIAL
//#define USE_NEOPIXEL

#include <LnShield.h>
#include "board.h"

#include "dbgboard.h"
#include "neopixel.h"

#ifdef USE_SOFTWARESERIAL
#include <SoftwareSerial.h>
#endif  //USE_SOFTWARESERIAL

namespace {
  LnShield sLn(PIN_OE);   //LightningNetwork Shield

  int prevStat = -1;
  uint64_t localMsat = UINT64_MAX;
  NeoPixelLedType_t neoStat = NEOPIXEL_LED_WAIT;

#ifdef USE_SOFTWARESERIAL
  SoftwareSerial dbgSerial(10, 11); //RX, TX
  #define DBG_BEGIN()     dbgSerial.begin(115200)
  #define DBG_PRINTLN(a)  dbgSerial.println(a)
#else
  #define DBG_BEGIN()     //none
  #define DBG_PRINTLN(a)  //none
#endif  //USE_SOFTWARESERIAL
}

void setup() {
  DBG_BEGIN();
  DBG_PRINTLN("begin");
  dbgboard_setup();
#ifdef USE_NEOPIXEL
  neopixel_setup();
  neopixel_led(NEOPIXEL_LED_START);
#endif

  dbgboard_led(DBGBOARD_LED_INIT);
  dbgboard_buzzer(DBGBOARD_BUZZER_INIT);
  sLn.init();
  dbgboard_led(DBGBOARD_LED_WAIT);
}


void loop() {
  int ret = sLn.cmdPolling();
  if (ret != LnShield::ENONE) {
    dbgboard_buzzer(DBGBOARD_BUZZER_ERROR);
    DBG_PRINTLN("error");
    DBG_PRINTLN(ret);
    while (true) {
      Serial.write(ret);
      dbgboard_led(DBGBOARD_LED_ERROR);
#ifdef USE_NEOPIXEL
      neopixel_led(NEOPIXEL_LED_ERROR);
#endif
      delay(200);
    }
    //no return
  }

  int stat = sLn.getStatus();
  if (stat != prevStat) {
    dbgboard_buzzer(DBGBOARD_BUZZER_CHGSTAT);
  }
  if (stat != LnShield::STAT_NORMAL) {
    //wait normal status
    neoStat = NEOPIXEL_LED_WAIT;
    DBG_PRINTLN("wait");
  } else if (prevStat != LnShield::STAT_NORMAL) {
    //changed to normal status
    neoStat = NEOPIXEL_LED_INITED;
    localMsat = sLn.getLastMsat();   //can receive localMsat
    DBG_PRINTLN("to normal");
  }
  prevStat = stat;

  if (stat == LnShield::STAT_NORMAL) {
    //amount_msat
    uint64_t msat = sLn.getLastMsat();
    if ((msat != UINT64_MAX) && (localMsat != msat)) {
      DBG_PRINTLN("change msat");
      //change localMsat
      if (msat > localMsat) {
        neoStat = NEOPIXEL_LED_GET;
        dbgboard_buzzer(DBGBOARD_BUZZER_GET);
      } else {
        neoStat = NEOPIXEL_LED_PAY;
        dbgboard_buzzer(DBGBOARD_BUZZER_PAY);
      }
    }
    localMsat = msat;

    //button
    bool on = dbgboard_button();
    if (on) {
      dbgboard_buzzer(DBGBOARD_BUZZER_INVOICE);
      sLn.cmdInvoice(2000);
    }
  }

  dbgboard_led(DBGBOARD_LED_NORMAL);
  if (neoStat != NEOPIXEL_LED_NORMAL) {
    DBG_PRINTLN("led!");
  }
#ifdef USE_NEOPIXEL
  neoStat = neopixel_led(neoStat);
#endif

  delay(100);
}
