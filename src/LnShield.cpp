#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "LnShield.h"

#define UART_SPEED          (115200)


namespace
{
    const uint8_t CMD_GET_BALANCE = 0x01;        ///< 所有額取得
    const uint8_t CMD_GETNEWADDRESS = 0x02;      ///< アドレス発行
    const uint8_t CMD_SEND = 0x03;               ///< 送金
    const uint8_t CMD_FEERATE = 0x04;            ///< FEE設定

    const uint8_t CMD_POLL = 0x7e;               ///< 生存確認
    const uint8_t CMD_STOP = 0x7f;               ///< Raspi停止

    const uint8_t RES_FLAG = 0x80;               ///< レスポンスフラグ
}


namespace {
    uint16_t getBe16_(const uint8_t *pData) {
        return (uint16_t)pData[0] | (((uint16_t)pData[1]) << 8);
    }
    int setBe16_(uint8_t *pData, uint16_t Val) {
        pData[0] = Val >> 8;
        pData[1] = Val & 0xff;
        return sizeof(uint16_t);
    }

    uint64_t getBe64_(const uint8_t *pData) {
        return (uint64_t)pData[0] |
                    (((uint64_t)pData[1]) << 8)  |
                    (((uint64_t)pData[2]) << 16) |
                    (((uint64_t)pData[3]) << 24);
    }
}


LnShield::LnShield(int pinOutputEnable)
    : mPinOE(pinOutputEnable), mStatus(STAT_STARTUP)
{
}


LnShield::~LnShield()
{
}


LnShield::Err_t LnShield::init()
{
    if (mStatus != STAT_STARTUP) {
        return EALREADY_INIT;
    }

    pinMode(mPinOE, OUTPUT);

    //mPinOE
    digitalWrite(mPinOE, HIGH);
    //for Arduino program write
    digitalWrite(mPinOE, LOW);

    Serial.begin(UART_SPEED);

    //初期化開始
    mStatus = STAT_STARTING;

    return ENONE;
}


LnShield::Err_t LnShield::stop()
{
    Err_t err;
    uint16_t RecvLen;
    err = uartSendCmd(CMD_STOP, 0, 0, &RecvLen);
    delay(100);

    return err;
}


LnShield::Err_t LnShield::handshake()
{
    const uint8_t INITRD[] = { 0x12, 0x34, 0x56, 0x78, 0x9a };
    const uint8_t INITWT[] = { 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x9a, 0x78, 0x56, 0x34, 0x12 };

    Err_t err = ENONE;

    if (mStatus == STAT_STARTING) {
        if (Serial.available() > 0) {
            uint8_t rd = Serial.read();
            if (rd == 0x55) {
                mStatus = STAT_HANDSHAKE1;
                delay(100);
            }
        }
    }
    if (mStatus == STAT_HANDSHAKE1) {
        if (Serial.available() > 0) {
            uint8_t rd = Serial.read();
            if (rd != 0x55) {
                mStatus = STAT_HANDSHAKE2;
                delay(100);
            }
        }
    }
    if (mStatus == STAT_HANDSHAKE2) {
        if (Serial.available() >= sizeof(INITRD)) {
            mStatus = STAT_HANDSHAKE3;
            delay(100);
        }
    }
    if (mStatus == STAT_HANDSHAKE3) {
        int lp = 0;
        for (lp = 0; lp < sizeof(INITRD); lp++) {
            if (Serial.read() != INITRD[lp]) {
                break;
            }
        }
        if (lp == sizeof(INITRD)) {
            Serial.write(INITWT, sizeof(INITWT));
            mStatus = STAT_INITED;
        } else {
            mStatus = STAT_STARTING;
            err = EINVALID_RES;
        }
    }

    return err;
}


LnShield::Err_t LnShield::polling()
{
    Err_t err = ENONE;
    uint16_t recv_len;

    switch (mStatus) {
    case STAT_STARTING:
    case STAT_HANDSHAKE1:
    case STAT_HANDSHAKE2:
    case STAT_HANDSHAKE3:
        err = handshake();
        break;

    case STAT_INITED:
        //定常状態
        err = uartSendCmd(CMD_POLL, 0, 0, &recv_len);
        break;
    default:
        break;
    }

    return err;
}


LnShield::Err_t LnShield::getNewAddress(char address[])
{
    if (mStatus != STAT_INITED) {
        return EDISABLED;
    }

    Err_t err = ENONE;
    uint16_t recv_len;

    err = uartSendCmd(CMD_GETNEWADDRESS, 0, 0, &recv_len);
    if (err == ENONE) {
        memcpy(address, &mWorkBuf[1], recv_len);
    }
    return err;
}


LnShield::Err_t LnShield::setFeeRate(uint32_t feerate)
{
    if (mStatus != STAT_INITED) {
        return EDISABLED;
    }

    uint8_t send_buf[4];
    Err_t err = ENONE;
    uint16_t recv_len;

    send_buf[0] = (uint8_t)(feerate & 0xff);
    send_buf[1] = (uint8_t)((feerate & 0xff00) >> 8);
    send_buf[2] = (uint8_t)((feerate & 0xff0000) >> 16);
    send_buf[3] = (uint8_t)((feerate & 0xff000000) >> 24);

    err = uartSendCmd(CMD_FEERATE, send_buf, sizeof(send_buf), &recv_len);
    if (err == ENONE) {
    }
    return err;
}


LnShield::Err_t LnShield::sendBitcoin(const char sendAddr[], uint64_t amount)
{
    if (mStatus != STAT_INITED) {
        return EDISABLED;
    }

    uint8_t send_buf[64];
    Err_t err = ENONE;
    uint16_t recv_len;

    size_t len = strlen(sendAddr);
    uint64_t mask = 0xff;
    for (int lp = 0; lp < sizeof(amount); lp++) {
        send_buf[lp] = (uint8_t)((amount & mask) >> (8 * lp));
        mask <<= 8;
    }
    strcpy((char *)&send_buf[4], sendAddr);

    err = uartSendCmd(CMD_SEND, send_buf, 4 + len, &recv_len);
    if (err == ENONE) {
    }
    return err;
}


LnShield::Err_t LnShield::getBalance(uint64_t balance[])
{
    Err_t err = ENONE;
    uint16_t recv_len;

    err = uartSendCmd(CMD_GET_BALANCE, 0, 0, &recv_len);
    if (err == ENONE) {
        balance[0] = getBe64_(mWorkBuf);
    }
    return err;
}


////////////////////////////////////////////////

/** BitcoinShieldへの送信
 *
 * @param[in]   Cmd     送信コマンド(cmdXxx)
 * @param[in]   pData   送信データ
 * @param[in]   Len     pData長
 * @return      エラー
 */
void LnShield::uartSend(uint8_t Cmd, const uint8_t *pData, uint16_t Len)
{
    mWorkBuf[0] = 0x00;
    mWorkBuf[1] = 0xff;
    mWorkBuf[2] = Len >> 8;
    mWorkBuf[3] = Len & 0xff;
    mWorkBuf[4] = (uint8_t)(0 - mWorkBuf[2] - mWorkBuf[3]);
    mWorkBuf[5] = Cmd;
    uint8_t dcs;
    for (uint8_t lp = 0; lp < Len; lp++) {
        mWorkBuf[6 + lp] = pData[lp];
        dcs += pData[lp];
    }
    mWorkBuf[6 + Len] = (uint8_t)(0 - dcs);     //DCS
    mWorkBuf[6 + Len + 1] = 0xef;

    Serial.write(mWorkBuf, Len + 8);
}


/** BitcoinShieldからの受信
 *
 * @param[in]   Cmd     受信待ちコマンド
 * @return      エラー
 * @note
 *          - 受信データはmWorkBuf[]に入っている.
 */
LnShield::Err_t LnShield::uartRecv(uint8_t Cmd, uint16_t *pLen)
{
    int rd;

    rd = Serial.readBytes(mWorkBuf, 6);
    if ( (rd != 6) ||
         (mWorkBuf[0] != 0x00) ||
         (mWorkBuf[1] != 0xff) ||
         (((mWorkBuf[2] + mWorkBuf[3] + mWorkBuf[4]) & 0xff) != 0x00) ) {
        return EUART_RD_HEAD;
    }

    uint16_t len = getBe16_(mWorkBuf + 2);
    uint8_t reply = mWorkBuf[5];

    rd = Serial.readBytes(mWorkBuf, len + 2);
    uint8_t dcs = 0;
    for (size_t lp = 0; lp < len; lp++) {
        dcs += mWorkBuf[lp];
    }
    if ( (rd != len + 2) ||
         (((dcs + mWorkBuf[len]) & 0xff) != 0) ||
         (mWorkBuf[len + 1] != 0xef) ) {
        return EUART_RD_DATA;
    }
    if ((Cmd | RES_FLAG) != reply) {
        return EUART_RD_REPLY;
    }

    *pLen = len - 1;   //データ部分のサイズ
    return ENONE;
}


LnShield::Err_t LnShield::uartSendCmd(uint8_t Cmd, const uint8_t *pData, uint16_t Len, uint16_t *pRecvLen)
{
    Err_t err;

    *pRecvLen = 0;
    uartSend(Cmd, pData, Len);
    err = uartRecv(Cmd, pRecvLen);
    return err;
}
