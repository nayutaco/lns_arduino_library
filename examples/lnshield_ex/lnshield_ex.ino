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

  void beep(int num, int beat)
  {
    const int HZ[] = {
      262, 294, 330, 349, 392, 440, 494, 523,
    };
    tone(PIN_BUZZ, HZ[num], beat);
    delay(beat);
  }
  void beep(int num)
  {
    const int BEAT = 300;
    beep(num, BEAT);
  }

  void op_normal()
  {
    int stat = sLn.getStatus();
    if (stat != sPrevStat) {
      digitalWrite(PIN_LED_RED, (stat == LnShield::STAT_INITED) ? LOW : HIGH);
      beep(0);
      sPrevStat = stat;
    }
  }

  void op_halt_error(int err, int snd)
  {
    int led = 0;

    beep(snd, 2000);
    while (true) {
      Serial.write(err);
      digitalWrite(PIN_LED_RED, led);
      digitalWrite(PIN_LED_GRN, led ^ 1);
      delay(200);
      led ^= 1;
    }
  }

  void op_halt_stop()
  {
    sStat = ST_STOP;
    beep(3);
    sLn.cmdStop();
    beep(4);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GRN, LOW);
    beep(3);
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
  sLn.init();
  
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GRN, LOW);
  sStat = ST_NORMAL;
}

void loop() {
  static int led_onoff = 0;

  int ret = sLn.cmdPolling();
  if (ret == 0) {
    op_normal();
  } else {
    op_halt_error(ret, 0);
  }

  if (digitalRead(PIN_BTN) != 0) {
    //op_halt_stop();
    uint64_t balance;
    ret = sLn.cmdGetBalance(&balance);
    if (ret == LnShield::ENONE) {
      sLn.cmdEpaper("Arduino");
    } else {
      op_halt_error(ret, 5);
    }
  }

  //heartbeat
  digitalWrite(PIN_LED_GRN, (led_onoff < 10) ? LOW : HIGH);
  led_onoff++;
  if (led_onoff > 20) {
    led_onoff = 0;
  }
  delay(100);
}
