#include <LnShield.h>

#define PIN_BTN         (2)   //
#define PIN_OE          (4)
#define PIN_LED_RED     (5)   //赤
#define PIN_LED_GRN     (7)   //緑
#define PIN_BUZZ        (12)  // 圧電スピーカを接続したピン番号
#define PIN_LED_ERR     (13)  //Arduino UNO上のLED(異常検知)

namespace {
  LnShield ln(PIN_OE);
  int Led = 0;
  int prevStat = -1;

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
    int stat = ln.getStatus();
    if (stat != prevStat) {
      digitalWrite(PIN_LED_RED, (stat == LnShield::STAT_INITED) ? LOW : HIGH);
      beep(7);
      prevStat = stat;
    }
  }

  void op_error()
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
}

void setup() {
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GRN, LOW);

  beep(0);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GRN, HIGH);
  ln.init(0);
  
  beep(1);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GRN, LOW);
}

void loop() {
  unsigned long amount;
  char status;
  int ret = ln.polling(&amount, &status);
  if (ret == 0) {
    op_normal();
  } else {
    op_error();
  }
  digitalWrite(PIN_LED_GRN, Led);
  Led ^= 1;
  delay(1000);
}
