#include <LnShield.h>

//Rpi board
#define PIN_RX          (0)   //UART
#define PIN_TX          (1)   //UART
#define PIN_OE          (4)   //UART Output Enable(use in library)

//debug board
#define PIN_LED_RED     (5)   //赤LED
#define PIN_BTN         (6)   //button
#define PIN_LED_GRN     (7)   //緑LED
#define PIN_BUZZ        (9)   // 圧電スピーカを接続したピン番号

//unused
#define PIN_D10         (10)  //SPI:SS
#define PIN_D11         (11)  //SPI:MOSI
#define PIN_D12         (12)  //SPI:MISO
#define PIN_D13         (13)  //SPI:SCK

#define BUTTON_LIMIT    (20)

namespace {
  LnShield sLn(PIN_OE);
  int sPrevStat = -1;
  int button = 0;
  uint64_t local_msat = UINT64_MAX;

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
    const int BEAT = 500;
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
  for (int i=0; i<3; i++) {
    beep(4, 100);
    beep(5, 100);
    beep(6, 100);
  }
  sStat = ST_INIT;
  pinMode(PIN_BTN, INPUT_PULLUP);
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
  if (ret == LnShield::ENONE) {
    op_normal();
    uint64_t msat = sLn.getLastMsat();
    if (msat != UINT64_MAX) {
      if (local_msat != msat) {
        local_msat = msat;
        beep(1);
        beep(7);
        beep(1);
      }
    }
  } else {
    op_halt_error(ret, 0);
  }

  if (digitalRead(PIN_BTN) == 0) {
    if (button < BUTTON_LIMIT) {
      button++;
    }
  } else {
    button = 0;
  }
  if (button == BUTTON_LIMIT) {
    beep(4);
//    op_halt_stop();
//    uint64_t balance;
//    ret = sLn.cmdGetBalance(&balance);
//    if (ret == LnShield::ENONE) {
//      sLn.cmdEpaper("Arduino");
//    } else {
//      op_halt_error(ret, 5);
//    }
    sLn.cmdInvoice(2000);
    button++;
  }

  //heartbeat
  digitalWrite(PIN_LED_GRN, (led_onoff < 10) ? LOW : HIGH);
  led_onoff++;
  if (led_onoff > 20) {
    led_onoff = 0;
  }
  delay(100);
}
