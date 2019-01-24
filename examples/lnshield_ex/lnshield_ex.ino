#include <LnShield.h>

#define PIN_OE          (4)
#define PIN_LED_RED     (5)   //赤
#define PIN_BTN         (6)   //
#define PIN_LED_GRN     (7)   //緑
#define PIN_BUZZ        (12)  // 圧電スピーカを接続したピン番号
#define PIN_LED_ERR     (13)  //Arduino UNO上のLED(異常検知)

namespace {
  LnShield sLn(PIN_OE);
  int sPrevStat = -1;

  enum Stat_t {
    ST_INIT,
    ST_NORMAL,
    ST_STOP
  } sStat;

  void beep(int num)
  {
    const int BEAT = 300;
    const int HZ[] = {
      262, 294, 330, 349, 392, 440, 494, 523,
    };
    tone(PIN_BUZZ, HZ[num], BEAT);
    delay(BEAT);
  }

  void op_normal()
  {
    int stat = sLn.getStatus();
    if (stat != sPrevStat) {
      digitalWrite(PIN_LED_RED, (stat == LnShield::STAT_INITED) ? LOW : HIGH);
      beep(7);
      sPrevStat = stat;
    }
  }

  void op_halt_error()
  {
    beep(2);
    int led = 0;
    while (true) {
      digitalWrite(PIN_LED_RED, led);
      digitalWrite(PIN_LED_GRN, led ^ 1);
      delay(200);
      led ^= 1;
    }
  }

  void op_halt_stop()
  {
    sStat = ST_STOP;
    sLn.stop();
    beep(3);
    beep(4);
    beep(3);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GRN, LOW);
    while (true) {
      ;
    }
  }
}

void setup() {
  sStat = ST_INIT;
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GRN, LOW);

  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GRN, HIGH);
  sLn.init(0);
  
  beep(0);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GRN, LOW);
  sStat = ST_NORMAL;
}

void loop() {
  static int led_onoff = 0;

  unsigned long amount;
  char status;

  int ret = sLn.polling(&amount, &status);
  if (ret == 0) {
    op_normal();
  } else {
    op_halt_error();
  }

  if (digitalRead(PIN_BTN) != 0) {
    op_halt_stop();
  }

  //heartbeat
  digitalWrite(PIN_LED_GRN, (led_onoff < 10) ? LOW : HIGH);
  led_onoff++;
  if (led_onoff > 20) {
    led_onoff = 0;
  }
  delay(100);
}
