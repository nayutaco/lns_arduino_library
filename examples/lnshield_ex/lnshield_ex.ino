#include <LnShield.h>
#include "board.h"

#include "dbgboard.h"
#include "neopixel.h"


namespace {
  LnShield sLn(PIN_OE);
  int sPrevStat = -1;
  uint64_t local_msat = UINT64_MAX;

  void op_normal(int stat)
  {
    if (stat != sPrevStat) {
      dbgboard_buzzer(DBGBOARD_BUZZER_CHGSTAT);
      sPrevStat = stat;
    }
  }

  void op_halt_error(int err)
  {
    dbgboard_buzzer(DBGBOARD_BUZZER_ERROR);
    while (true) {
      Serial.write(err);
      dbgboard_led(DBGBOARD_LED_ERROR);
      delay(200);
    }
  }
}

void setup() {
  dbgboard_setup();
  neopixel_setup();

  dbgboard_led(DBGBOARD_LED_INIT);
  dbgboard_buzzer(DBGBOARD_BUZZER_INIT);
  sLn.init();
  dbgboard_led(DBGBOARD_LED_WAIT);
}


void loop() {
  static uint16_t led_blink = 0;
  static LnShield::Status_t ptarm_stat = LnShield::STAT_STARTUP;
  static NeoPixelLedType_t neo_stat = NEOPIXEL_LED_WAIT;

  int ret = sLn.cmdPolling();
  if (ret != LnShield::ENONE) {
    op_halt_error(ret);
  }

  int stat = sLn.getStatus();
  op_normal(stat);

  uint64_t msat = sLn.getLastMsat();
  float flocal_msat = (float)local_msat;
  float fmsat = (float)msat;
  
  if ((ptarm_stat == LnShield::STAT_NORMAL) && (msat != UINT64_MAX) && (local_msat != msat)) {
    neo_stat = NEOPIXEL_LED_GET;
    dbgboard_buzzer(DBGBOARD_BUZZER_GET);
  }
  local_msat = msat;

  bool on = dbgboard_button();
  if (on) {
    dbgboard_buzzer(DBGBOARD_BUZZER_INVOICE);
    sLn.cmdInvoice(2000);
  }

  //heartbeat
  dbgboard_heartbeat(led_blink);

  if (sLn.getStatus() != LnShield::STAT_NORMAL) {
    neo_stat = NEOPIXEL_LED_WAIT;
  } else if (ptarm_stat != LnShield::STAT_NORMAL) {
    neo_stat = NEOPIXEL_LED_INITED;
  }
  neo_stat = neopixel_led(led_blink, neo_stat);
  led_blink++;

  ptarm_stat = sLn.getStatus();

  delay(100);
}

