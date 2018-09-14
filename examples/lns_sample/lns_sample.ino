#include <LnShield.h>

//4ピンはOEのため、使用不可！

//#define PIN_BTN_ADDR    (6)   //アドレス発行ボタン
#define PIN_BTN_STOP    (6)   //Raspi停止ボタン
//#define PIN_BTN_SEND    (9)   //FEE設定ボタン
#define PIN_LED_ERR     (13)  //Arduino UNO上のLED(異常検知)
#define PIN_LED_RUN     (7)   //運用中LED
#define PIN_LED_STS     (5)   //起動待ち・着金ありLED
#define PIN_PWM         (12)  //圧電スピーカ

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#if 1
#define DEBUG_INIT()
#define DEBUG_LOG(...)
#define DEBUG_LOGLN(...)
#else
#define DEBUG_INIT()    Serial.begin(115200)
#define DEBUG_LOG       Serial.print
#define DEBUG_LOGLN     Serial.println
#endif

LnShield   myShield;
int Led7 = 1;
char m_pay_stat = LNPAYSTAT_NONE;

///////////////////////////////////////////////////
struct sound_t {
  int   hz;
  int   len;
};

const int MM_C = 818;
const int MM_D = 918;
const int MM_E = 1030;
const int MM_A = 1375;
const int MM_B = 1543;
const int MM_E2 = 2060;
const int MM_P = 1;
const int DURATION = 80;
const sound_t SOUND1[] = {
  { MM_C, 1 },
  { MM_D, 1 },
  { MM_E, 5 },
//  { MM_D, 1 },
//  { MM_C, 1 },
//  { MM_P, 5 },
//  { MM_C, 1 },
//  { MM_D, 1 },
//  { MM_E, 1 },
//  { MM_D, 1 },
//  { MM_C, 1 },
//  { MM_D, 10 },
};

const sound_t SOUND2[] = {
  { MM_B, 1 },
  { MM_E2, 3 },
};


const sound_t SOUND3[] = {
  { MM_E2, 1 },
  { MM_B, 3 },
};

///////////////////////////////////////////////////
void halt(const char* pMsg, int err) {
  myShield.stop();

  //以降はSerialが使用可能
  Serial.begin(115200);
  Serial.print(pMsg);
  Serial.println(err);

  while (1) {
    digitalWrite(PIN_LED_ERR, HIGH);
    digitalWrite(PIN_LED_RUN, HIGH);
    digitalWrite(PIN_LED_STS, HIGH);
    delay(100);
    digitalWrite(PIN_LED_ERR, LOW);
    digitalWrite(PIN_LED_RUN, LOW);
    digitalWrite(PIN_LED_STS, LOW);
    delay(100);
  }
}


///////////////////////////////////////////////////
void buzzer(const sound_t *pSnd, int len) {
  for (int lp = 0; lp < len; lp++) {
    if (pSnd[lp].hz != MM_P) {
      tone(PIN_PWM, pSnd[lp].hz);
    } else {
      noTone(PIN_PWM);
    }
    delay(DURATION * pSnd[lp].len);
  }
  noTone(PIN_PWM);
}

///////////////////////////////////////////////////
void setup() {
  DEBUG_INIT();

  //pinMode(PIN_BTN_ADDR, INPUT);
  pinMode(PIN_BTN_STOP, INPUT);
  //pinMode(PIN_BTN_SEND, INPUT);
  pinMode(PIN_LED_ERR, OUTPUT);
  pinMode(PIN_LED_RUN, OUTPUT);
  pinMode(PIN_LED_STS, OUTPUT);
  digitalWrite(PIN_LED_ERR, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_RUN, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_STS, HIGH);
  delay(1000);
  buzzer(SOUND3, ARRAY_SIZE(SOUND3));
  digitalWrite(PIN_LED_ERR, LOW);
  delay(1000);
  digitalWrite(PIN_LED_RUN, LOW);
  delay(1000);
  digitalWrite(PIN_LED_STS, LOW);
  delay(1000);

  DEBUG_LOGLN("Arduino Start");
  //CONFIRMATION=0(即時)
  int err = myShield.init(0);
  if (err == 0) {
    DEBUG_LOGLN("init");
  } else {
    halt("fail: init", err);
  }
}

///////////////////////////////////////////////////
void loop() {
  int err;

  LnShield::Status_t stat = myShield.getStatus();
  int led;
  switch (stat) {
    case LnShield::STAT_STARTUP:
    case LnShield::STAT_STARTING:
      digitalWrite(PIN_LED_STS, HIGH);
      digitalWrite(PIN_LED_RUN, LOW);
      break;
    case LnShield::STAT_WAITING:
      digitalWrite(PIN_LED_STS, (Led7) ? HIGH : LOW);
      digitalWrite(PIN_LED_RUN, LOW);
      break;
    default:
      digitalWrite(PIN_LED_STS, LOW);
      digitalWrite(PIN_LED_RUN, (Led7) ? HIGH : LOW);
      break;
  }
  Led7 ^= 1;

  //アドレス発行
//  if (digitalRead(PIN_BTN_ADDR) == LOW) {
//    DEBUG_LOGLN("issue addr");
//    err = myShield.issueAddress();
//    if (err) {
//      halt("fail: issue", err);
//    }
//  }

  //停止
  if (digitalRead(PIN_BTN_STOP) == HIGH) {
    halt("stop", 0);
  }

  //FEE設定(動作未確認)
//  if (digitalRead(PIN_BTN_SEND) == LOW) {
//    DEBUG_LOGLN("send");
//    err = myShield.setFee(5000, LNUNIT_SATOSHI);
//    if (err) {
//      halt("fail: send", err);
//    }
//    
//    err = myShield.sendBitcoin("abc", 10, LNUNIT_MBTC);
//    if (err) {
//      halt("fail: send", err);
//    }
//  }

  //Raspberry Piのチェック(応答しない場合、UARTの受信T/Oは1秒)
  unsigned long getMoney[1];
  char pay_stat[1];
  err = myShield.polling(getMoney, pay_stat);
  if (err == 0) {
    //チェック自体は成功
    if (getMoney[0] != LNAMOUNT_INVALID_VAL) {
      //チェックして、着金があった
      DEBUG_LOG("get: ");
      DEBUG_LOGLN(getMoney[0]);

      //着金通知
      buzzer(SOUND2, ARRAY_SIZE(SOUND2));
      digitalWrite(PIN_LED_STS, HIGH);
      delay(2000);
      digitalWrite(PIN_LED_STS, LOW);
      m_pay_stat = LNPAYSTAT_NONE;
    } else if ((pay_stat[0] != m_pay_stat) && (pay_stat[0] == LNPAYSTAT_RECV)) {
      buzzer(SOUND1, ARRAY_SIZE(SOUND1));
      m_pay_stat = LNPAYSTAT_RECV;
    } else {
      //着金はなかった
    }

    //
    delay(1000);
  } else {
    //APIエラー
    halt("poll get err: ", err);
  }
}

