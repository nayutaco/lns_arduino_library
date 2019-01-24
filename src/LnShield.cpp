#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "LnShield.h"

//#define DEBUG_OUT       Serial.println
#define DEBUG_OUT(...)
#define UART_SPEED          (115200)

namespace
{
const uint8_t CMD_INIT = 0x00;               ///< 起動
const uint8_t CMD_GET_BALANCE = 0x01;        ///< 所有額取得
const uint8_t CMD_ISSUE = 0x02;              ///< アドレス発行
const uint8_t CMD_SEND = 0x03;               ///< 送金
const uint8_t CMD_FEE = 0x04;                ///< FEE設定
const uint8_t CMD_POLL = 0x70;               ///< 着金確認
const uint8_t CMD_STOP = 0x7f;               ///< Raspi停止

const uint8_t RES_FLAG = 0x80;               ///< レスポンスフラグ
}

LnShield::Status_t LnShield::sStatus = LnShield::STAT_STARTUP;


LnShield::LnShield(int pinOutputEnable) : mPinOE(pinOutputEnable), mFee(50000)
{
}


LnShield::~LnShield()
{
}


int LnShield::init(byte confs)
{
    if(sStatus != STAT_STARTUP) {
        DEBUG_OUT("already init1");
        return LNERR_ALREADY_INIT;
    }

    pinMode(mPinOE, OUTPUT);

    //mPinOE
    digitalWrite(mPinOE, HIGH);
    //for Arduino program write
    digitalWrite(mPinOE, LOW);

    Serial.begin(UART_SPEED);
    mConfs = confs;

    //初期化開始
    sStatus = STAT_STARTING;

    return 0;
}


void LnShield::stop()
{
    send(CMD_STOP, 0, 0);
    delay(100);
}


int LnShield::handshake()
{
    const byte INITRD[] = { 0x12, 0x34, 0x56, 0x78, 0x9a };
    const byte INITWT[] = { 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x9a, 0x78, 0x56, 0x34, 0x12 };

    int err = 0;

    if(sStatus == STAT_STARTING) {
        if(Serial.available() > 0) {
            byte rd = Serial.read();
            if(rd == 0x55) {
                sStatus = STAT_HANDSHAKE1;
                delay(100);
            }
        }
    }
    if(sStatus == STAT_HANDSHAKE1) {
        if(Serial.available() > 0) {
            byte rd = Serial.read();
            if(rd != 0x55) {
                sStatus = STAT_HANDSHAKE2;
                delay(100);
            }
        }
    }
    if(sStatus == STAT_HANDSHAKE2) {
        if(Serial.available() >= sizeof(INITRD)) {
            sStatus = STAT_HANDSHAKE3;
            delay(100);
        }
    }
    if(sStatus == STAT_HANDSHAKE3) {
        int lp = 0;
        for(lp = 0; lp < sizeof(INITRD); lp++) {
            if(Serial.read() != INITRD[lp]) {
                break;
            }
        }
        if(lp == sizeof(INITRD)) {
            Serial.write(INITWT, sizeof(INITWT));
            sStatus = STAT_INITED;
        } else {
            sStatus = STAT_STARTING;
            err = LNERR_INVALID_RES;
        }
    }

    return err;
}


int LnShield::polling(unsigned long amount[], char status[])
{
    int err = 0;
    int res;

    amount[0] = LNAMOUNT_INVALID_VAL;       //no receive
    status[0] = LNPAYSTAT_NONE;

    switch(sStatus) {
    case STAT_STARTING:
    case STAT_HANDSHAKE1:
    case STAT_HANDSHAKE2:
    case STAT_HANDSHAKE3:
        err = handshake();
        break;

    case STAT_INITED:
        //定常状態
        send(CMD_POLL, 0, 0);
        res = recv(CMD_POLL);
        if(res == 4) {
            amount[0] = mRecvBuf[1] | (((unsigned long)mRecvBuf[2]) << 8) | (((unsigned long)mRecvBuf[3]) << 16) | (((unsigned long)mRecvBuf[4]) << 24);
            DEBUG_OUT(amount[0]);
        } else if(res == 0) {
            //no receive
        } else if(res == 1) {
            if(status != NULL) {
                status[0] = mRecvBuf[1];
            }
            DEBUG_OUT(mRecvBuf[1]);
        } else {
            DEBUG_OUT("poll: invalid res");
            err = LNERR_INVALID_RES;
        }
        break;

    default:
        break;
    }

    //呼び出し側がある程度のdelayを持たせる前提
    //delay(1000);

    return err;
}


int LnShield::issueAddress()
{
    if(sStatus != STAT_INITED) {
        DEBUG_OUT("not initialied");
        return LNERR_DISABLED;
    }

    int err = 0;

    send(CMD_ISSUE, 0, 0);
    int res = recv(CMD_ISSUE);
    if(res > 0) {
        memcpy(mRecvAddr, &mRecvBuf[1], res);
        mRecvAddr[res] = '\0';
        DEBUG_OUT(mRecvAddr);
    } else {
        DEBUG_OUT("issue: invalid res");
        err = LNERR_INVALID_RES;
    }
    return err;
}


int LnShield::requestBitcoin(unsigned long amount, int unit, const char label[], const char message[])
{
    if(sStatus != STAT_INITED) {
        DEBUG_OUT("not initialied");
        return LNERR_DISABLED;
    }

    return LNERR_ERROR;
}


int LnShield::setFee(unsigned long fee, int unit)
{
    if(sStatus != STAT_INITED) {
        DEBUG_OUT("not initialied");
        return LNERR_DISABLED;
    }
    if((unit < LNUNIT_SATOSHI) || (LNUNIT_BTC < unit)) {
        return LNERR_INVALID_PARAM;
    }

    mFee = changeSatoshi(fee, unit);

    int err = 0;
    uint8_t sendBuf[4];

    sendBuf[0] = (byte)(mFee & 0xff);
    sendBuf[1] = (byte)((mFee & 0xff00) >> 8);
    sendBuf[2] = (byte)((mFee & 0xff0000) >> 16);
    sendBuf[3] = (byte)((mFee & 0xff000000) >> 24);

    DEBUG_OUT("fee");
    send(CMD_FEE, sendBuf, 4);
    int res = recv(CMD_FEE);
    if(res > 0) {
        DEBUG_OUT(mRecvBuf[1]);
    } else {
        DEBUG_OUT("fee: invalid res");
        err = LNERR_INVALID_RES;
    }
    return err;
}


int LnShield::sendBitcoin(const char sendAddr[], unsigned long amount, int unit)
{
    if(sStatus != STAT_INITED) {
        DEBUG_OUT("not initialied");
        return LNERR_DISABLED;
    }

    int err = 0;
    uint8_t sendBuf[64];

    int len = strlen(sendAddr);
    unsigned long val = changeSatoshi(amount, unit);
    sendBuf[0] = (byte)(val & 0xff);
    sendBuf[1] = (byte)((val & 0xff00) >> 8);
    sendBuf[2] = (byte)((val & 0xff0000) >> 16);
    sendBuf[3] = (byte)((val & 0xff000000) >> 24);
    strcpy((char *)&sendBuf[4], sendAddr);

    DEBUG_OUT("send");
    send(CMD_SEND, sendBuf, 4 + len);
    int res = recv(CMD_SEND);
    if(res > 0) {
        DEBUG_OUT(mRecvBuf[1]);
    } else {
        DEBUG_OUT("send: invalid res");
        err = LNERR_INVALID_RES;
    }
    return err;
}


unsigned long LnShield::getBalance()
{
    if(sStatus != STAT_INITED) {
        DEBUG_OUT("not initialied");
        return 0;
    }

    unsigned long ret = LNAMOUNT_INVALID_VAL;

    send(CMD_GET_BALANCE, 0, 0);
    int res = recv(CMD_GET_BALANCE);
    if(res == 4) {
        ret = mRecvBuf[1] | (((unsigned long)mRecvBuf[2]) << 8) | (((unsigned long)mRecvBuf[3]) << 16) | (((unsigned long)mRecvBuf[4]) << 24);
    } else {
        DEBUG_OUT("balance: invalid res");
    }
    return ret;
}


////////////////////////////////////////////////

/** BitcoinShieldへの送信
 *
 * @param[in]   cmd     送信コマンド(cmdXxx)
 * @param[in]   pData   送信データ
 * @param[in]   len     pData長
 */
void LnShield::send(uint8_t cmd, const uint8_t *pData, uint8_t len)
{
    mSendBuf[0] = 0x00;
    mSendBuf[1] = 0xff;
    mSendBuf[2] = len + 1;
    mSendBuf[3] = (uint8_t)(0 - mSendBuf[2]);
    mSendBuf[4] = cmd;
    for(uint8_t lp = 0; lp < len; lp++) {
        mSendBuf[5 + lp] = pData[lp];
        cmd += pData[lp];
    }
    mSendBuf[5 + len] = (uint8_t)(0 - cmd);     //DCS
    mSendBuf[5 + len + 1] = 0xef;

    //Serial.println("----");
    //for (int lp = 0; lp < len + 7; lp++) {
    //    Serial.println(mSendBuf[lp], HEX);
    //}
    //Serial.println("----");

    Serial.write(mSendBuf, len + 7);
}


/** BitcoinShieldからの受信
 *
 * @param[in]   cmd     受信待ちコマンド
 * @retval      >=0     受信データ長(レスポンスコマンドを含まない)
 * @retval      <0      エラー
 * @note
 *          - 受信データはmRecvBuf[]に入っている.
 *          - mRecvBuf[0]にレスポンスコマンド、mRecvBuf[1～]にデータが入る.
 */
int LnShield::recv(uint8_t cmd)
{
    int rd;
    uint16_t timeout = 0;
    const uint16_t TIMEOUT_CNT = 1000;
    const int DELAY = 50;

    if(!checkRecvTimeout(4)) {
        return -1;
    }
    while(true) {
        if((mRecvBuf[0] == 0x00) && (mRecvBuf[1] == 0xff) && ((uint8_t)(mRecvBuf[2] + mRecvBuf[3]) == 0x00)) {
            break;
        } else {
            mRecvBuf[0] = mRecvBuf[1];
            mRecvBuf[1] = mRecvBuf[2];
            mRecvBuf[2] = mRecvBuf[3];
            timeout = 0;
            while((Serial.available() < 1) && (timeout < TIMEOUT_CNT)) {
                delay(DELAY);
                timeout++;
            }
            if(timeout >= TIMEOUT_CNT) {
                DEBUG_OUT("recv timeout1a");
                return -1;
            }
            mRecvBuf[3] = Serial.read();
        }
    }

    uint8_t len = mRecvBuf[2];
    uint8_t readLen = 0;
    uint8_t dcs = 0;
    timeout = 0;
    while((readLen != len) && (timeout < TIMEOUT_CNT)) {
        if(Serial.available() > 0) {
            mRecvBuf[readLen] = Serial.read();
            dcs += mRecvBuf[readLen];
            readLen++;
        } else {
            delay(DELAY);
            timeout++;
        }
    }
    if(timeout >= TIMEOUT_CNT) {
        DEBUG_OUT("recv timeout2");
        return -5;
    }
    while((Serial.available() < 2) && (timeout < TIMEOUT_CNT)) {
        delay(DELAY);
        timeout++;
    }
    if(timeout >= TIMEOUT_CNT) {
        DEBUG_OUT("recv timeout3");
        return -6;
    }
    rd = Serial.read();
    if((uint8_t)(dcs + rd) != 0x00) {
        DEBUG_OUT("recv [DCS]");
        DEBUG_OUT(rd);
        return -7;
    }
    rd = Serial.read();
    if(rd != 0xef) {
        DEBUG_OUT("recv POST");
        DEBUG_OUT(rd);
        return -8;
    }
    if((cmd | RES_FLAG) != mRecvBuf[0]) {
        DEBUG_OUT("recv CMD");
        DEBUG_OUT(cmd);
        DEBUG_OUT(mRecvBuf[0]);
        return -9;
    }

    return len - 1;     //データの部分だけ
}


/** 単位をsatoshiに変換
 *
 * @param[in]   val     変換対象の値
 * @param[in]   unit    valの単位
 * @retval      satoshiに変換した値
 */
unsigned long LnShield::changeSatoshi(unsigned long val, int unit)
{
    switch(unit) {
    case LNUNIT_MBTC:
        val *= 10000UL;
        break;
    case LNUNIT_BTC:
        val *= 100000000UL;
        break;
    case LNUNIT_SATOSHI:
    default:
        break;
    }

    return val;
}


bool LnShield::checkRecvTimeout(int rdCnt)
{
    uint16_t timeout = 0;
    const uint16_t TIMEOUT_SHORT_CNT = 100;
    const int DELAY = 50;

    while((Serial.available() >= rdCnt) && (timeout < TIMEOUT_SHORT_CNT)) {
        delay(DELAY);
        timeout++;
    }
    if(timeout >= TIMEOUT_SHORT_CNT) {
        DEBUG_OUT("recv timeout1");
        return false;
    }

    return true;
}
