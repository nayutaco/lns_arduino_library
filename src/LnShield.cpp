/** @file   LnShield.cpp
 *  @brief  Lightning Shield for Arduino API
 *  @author Nayuta inc.
 */
#include <Arduino.h>
#include <string.h>
#include "LnShield.h"

#define M_PIN_OE            (4)     //output enable

//#define M_USE_SERIALDBG
#ifdef M_USE_SERIALDBG
#include <SoftwareSerial.h>

#define DBG_SOFTSERIAL_RX   (2)
#define DBG_SOFTSERIAL_TX   (3)
static SoftwareSerial       sDebug(DBG_SOFTSERIAL_RX, DBG_SOFTSERIAL_TX);

#define DBG_INIT()          sDebug.begin(115200)
#define DBG_PRINT(val)      sDebug.print(val)
#define DBG_PRINTLN(val)    sDebug.println(val)

#else   //M_USE_SERIALDBG

#define DBG_INIT()          //none
#define DBG_PRINT(val)      //none
#define DBG_PRINTLN(val)    //none

#endif  //M_USE_SERIALDBG

/********************************************************************
 *
 ********************************************************************/

namespace
{
    const uint32_t UART_SPEED = 115200;         ///< bps

    const uint8_t CMD_GET_BALANCE = 0x01;       ///< get balance
    const uint8_t CMD_GETNEWADDRESS = 0x02;     ///< get new bitcoin address
    const uint8_t CMD_SEND = 0x03;              ///< send bitcoin
    const uint8_t CMD_FEERATE = 0x04;           ///< set feerate

    const uint8_t CMD_INVOICE = 0x40;           ///< create invoice
    const uint8_t CMD_GETLASTINVOICE = 0x41;    ///< get last invoice

    const uint8_t CMD_EPAPER = 0x7d;            ///< ePaper output
    const uint8_t CMD_POLL = 0x7e;              ///< ping/pong poll
    const uint8_t CMD_STOP = 0x7f;              ///< Raspi stop

    const uint8_t RES_FLAG = 0x80;              ///< response flag
}


/********************************************************************
 * local functions
 ********************************************************************/

namespace
{
    uint16_t getBe16_(const uint8_t *pData)
    {
        return ((uint16_t)pData[0] << 8) | (uint16_t)pData[1];
    }

    uint64_t getBe32_(const uint8_t *pData)
    {
        return ((uint32_t)pData[0] << 24) |
               ((uint32_t)pData[1] << 16) |
               ((uint32_t)pData[2] <<  8) |
               (uint32_t)pData[3];
    }

    uint64_t getBe64_(const uint8_t *pData)
    {
        return ((uint64_t)pData[0] << 56) |
               ((uint64_t)pData[1] << 48) |
               ((uint64_t)pData[2] << 40) |
               ((uint64_t)pData[3] << 32) |
               ((uint64_t)pData[4] << 24) |
               ((uint64_t)pData[5] << 16) |
               ((uint64_t)pData[6] <<  8) |
               (uint64_t)pData[7];
    }

    void setBe16_(uint8_t *pData, uint16_t Value)
    {
        uint16_t mask = 0xff;
        for (size_t lp = 0; lp < sizeof(Value); lp++) {
            pData[sizeof(Value) - lp - 1] = (uint8_t)((Value & mask) >> (8 * lp));
            mask <<= 8;
        }
    }

    void setBe32_(uint8_t *pData, uint32_t Value)
    {
        uint32_t mask = 0xff;
        for (size_t lp = 0; lp < sizeof(Value); lp++) {
            pData[sizeof(Value) - lp - 1] = (uint8_t)((Value & mask) >> (8 * lp));
            mask <<= 8;
        }
    }

    void setBe64_(uint8_t *pData, uint64_t Value)
    {
        uint64_t mask = 0xff;
        for (size_t lp = 0; lp < sizeof(Value); lp++) {
            pData[sizeof(Value) - lp - 1] = (uint8_t)((Value & mask) >> (8 * lp));
            mask <<= 8;
        }
    }
}


/********************************************************************
 * ctor/dtor
 ********************************************************************/

LnShield::LnShield()
    : mStatus(INSTAT_STARTUP), mLocalMsat(AMOUNT_INIT)
{
}


LnShield::~LnShield()
{
}


/********************************************************************
 * public methods
 ********************************************************************/

LnShield::Err_t LnShield::init()
{
    DBG_INIT();
    DBG_PRINTLN("[begin]");

    if (mStatus != INSTAT_STARTUP) {
        DBG_PRINTLN("EALREADY_INIT");
        return EALREADY_INIT;
    }

    pinMode(M_PIN_OE, OUTPUT);

    //output enable
    digitalWrite(M_PIN_OE, HIGH);
    //for Arduino program write
    digitalWrite(M_PIN_OE, LOW);

    Serial.begin(UART_SPEED);

    //初期化開始
    mStatus = INSTAT_STARTING;

    return ENONE;
}


/********************************************************************
 * command: Bitcoin
 ********************************************************************/

#if 0
/** 支払い可能Bitcoin値取得
 * 現在のウォレットで支払い可能なBitcoin値を取得する。
 *
 * @param[out]      balance     Bitcoin amount
 * @return  エラー
 */
LnShield::Err_t LnShield::cmdGetBalance(uint64_t balance[])
{
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("cmdGetBalance:EDISABLED");
        return EDISABLED;
    }

    Err_t err = ENONE;
    uint16_t recv_len;

    err = uartSendCmd(CMD_GET_BALANCE, 0, 0, &recv_len);
    if (err == ENONE) {
        if (recv_len == sizeof(uint64_t)) {
            balance[0] = getBe64_(mWorkBuf);
        } else {
            err = EINVALID_RES;
        }
    }
    return err;
}


/** アドレス発行
 * 受信するためのBitcoinアドレスを生成する。
 *
 * @param[out]      address     Bitcoin Address for receive
 * @return  エラー
 */
LnShield::Err_t LnShield::cmdGetNewAddress(char address[])
{
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("cmdGetNewAddress:EDISABLED");
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


/** トランザクション手数料設定
 * #sendBitcoin()時に支払われるトランザクション手数料を設定する。
 *
 * @param[in]       feerate     feerate/1000byte
 * @return  エラー
 * @note
 *      - デフォルト値は、0.5 mBTC(50000 satoshi)
 *      - #sendBitcoin()後に設定した場合は、次回の #setBitcoin()で有効になる。
 */
LnShield::Err_t LnShield::cmdSetFeeRate(uint32_t feerate)
{
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("cmdSetFeeRate:EDISABLED");
        return EDISABLED;
    }

    uint8_t send_buf[4];
    Err_t err = ENONE;
    uint16_t recv_len;

    setBe32_(send_buf, feerate);
    err = uartSendCmd(CMD_FEERATE, send_buf, sizeof(send_buf), &recv_len);
    return err;
}


/** Bitcoin支払い
 * Bitcoin送金のトランザクションを発行する。<br>
 * amount以外にfeeも支払う。
 *
 * @param[in]       sendAddr    送金するBitcoinアドレス
 * @param[in]       amount      送金するBitcoin
 * @return  エラー
 */
LnShield::Err_t LnShield::cmdSendBitcoin(const char sendAddr[], uint64_t amount)
{
    if (mStatus != INSTAT_NORMAL) {
        return EDISABLED;
    }

    uint8_t send_buf[64];
    Err_t err = ENONE;
    uint16_t recv_len;

    size_t len = strlen(sendAddr);
    setBe64_(send_buf, amount);
    strcpy((char *)&send_buf[4], sendAddr);

    err = uartSendCmd(CMD_SEND, send_buf, 4 + len, &recv_len);
    if (err == ENONE) {
    }
    return err;
}
#endif


/********************************************************************
 * command: Lightning Network
 ********************************************************************/

LnShield::Err_t LnShield::cmdInvoice(uint64_t amountMsat, const char *description)
{
    DBG_PRINTLN("cmdInvoice");
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("\tEDISABLED");
        return EDISABLED;
    }
    size_t len = 0;
    if (description) {
        len = strlen(description);
        if (len > INVOICE_DESC_MAX) {
            return EINVALID_PARAM;
        }
    }

    uint8_t send_buf[sizeof(uint64_t) + INVOICE_DESC_MAX];
    Err_t err = ENONE;
    uint16_t recv_len;

    int pos = 0;
    setBe64_(send_buf, amountMsat);
    pos += sizeof(uint64_t);
    if (len > 0) {
        memcpy(send_buf + pos, description, len);
        pos += len;
    }
    err = uartSendCmd(CMD_INVOICE, send_buf, pos, &recv_len);
    return err;
}


LnShield::Err_t LnShield::cmdGetLastInvoice(char *pInvoice, size_t *pLen)
{
    DBG_PRINTLN("cmdGetLastInvoice");
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("\tEDISABLED");
        return EDISABLED;
    }
    if (*pLen < (5 + 7 + 104 + 6)) {
        // HRP: 5
        // DP:
        //  timestamp: 7
        //  signature: 104
        //  checksum : 6
        DBG_PRINTLN("\tEINVALID_PARAM");
        return EINVALID_PARAM;
    }

    Err_t err = ENONE;
    uint16_t recv_len = *pLen - 1;

    err = uartSendCmd(CMD_GETLASTINVOICE, 0, 0, pInvoice, &recv_len);
    if (err == ENONE) {
        if (recv_len < *pLen) {
            pInvoice[recv_len] = '\0';
            *pLen = recv_len;
            DBG_PRINTLN(pInvoice);
        } else {
            DBG_PRINTLN("\tELESS_BUFFER");
            err = ELESS_BUFFER;
        }
    } else {
        DBG_PRINT("\t");
        DBG_PRINTLN(err);
    }
    return err;
}


/********************************************************************
 * command: control
 ********************************************************************/

LnShield::Err_t LnShield::cmdStop()
{
    Err_t err;
    uint16_t RecvLen;
    err = uartSendCmd(CMD_STOP, 0, 0, &RecvLen);
    delay(100);

    return err;
}


LnShield::Err_t LnShield::cmdPolling()
{
    Err_t err = ENONE;
    uint16_t recv_len;

    switch (mStatus) {
    case INSTAT_STARTING:
    case INSTAT_HANDSHAKE1:
    case INSTAT_HANDSHAKE2:
    case INSTAT_HANDSHAKE3:
        err = handshake();
        break;

    case INSTAT_NORMAL:
        //定常状態
        err = uartSendCmd(CMD_POLL, 0, 0, &recv_len);
        if (err == ENONE) {
            mLocalMsat = getBe64_(mWorkBuf);
        } else {
            DBG_PRINTLN(err);
        }
        break;
    default:
        break;
    }

    return err;
}


#if 0
/** ePaper出力
 *
 * @return  エラー
 */
LnShield::Err_t LnShield::cmdEpaper(const char str[])
{
    DBG_PRINTLN("cmdEpaper");
    if (mStatus != INSTAT_NORMAL) {
        DBG_PRINTLN("\tEDISABLED");
        return EDISABLED;
    }

    size_t len = strlen(str);
    if (len > 384) {
        DBG_PRINTLN("\tEINVALID_PARAM");
        return EINVALID_PARAM;
    }

    Err_t err;
    uint16_t RecvLen;
    err = uartSendCmd(CMD_EPAPER, str, (uint16_t)len, &RecvLen);
    delay(100);

    return err;
}
#endif


void LnShield::eventInit(
    LnShieldFuncChangeStatus_t cbChangeStatus,
    LnShieldFuncChangeMsat_t cbChangeMsat,
    LnShieldFuncError_t cbError)
{
    mPrevStatus = (InStatus_t)-1;
    mEvtLocalMsat = AMOUNT_INIT;

    mEvtCbChangeStatus = cbChangeStatus;
    mEvtCbChangeMsat = cbChangeMsat;
    mEvtCbError = cbError;

    pinMode(7, OUTPUT);
}


void LnShield::eventPoll()
{
    Err_t ret = cmdPolling();
    if (ret != ENONE) {
        DBG_PRINT("error=");
        DBG_PRINTLN(ret);
        DBG_PRINT("STATUS=");
        DBG_PRINTLN(mStatus);
        Serial.write(ret);
        if (mEvtCbError != 0) {
            (*mEvtCbError)(ret);
        }
    }

    if (mStatus != mPrevStatus) {
        if (mEvtCbChangeStatus != 0) {
            DBG_PRINT("status change:");
            DBG_PRINT(mPrevStatus);
            DBG_PRINT(" ==> ");
            DBG_PRINTLN(mStatus);

            Status_t user_stat;
            switch (mStatus) {
            case INSTAT_STARTUP:
                user_stat = STATUS_INIT;
                break;
            case INSTAT_STARTING:
                user_stat = STATUS_STARTING;
                break;
            case INSTAT_NORMAL:
                user_stat = STATUS_NORMAL;
                break;
            case INSTAT_HANDSHAKE1:
            case INSTAT_HANDSHAKE2:
            case INSTAT_HANDSHAKE3:
            default:
                user_stat = STATUS_UNKNOWN;
            }
            if (user_stat != STATUS_UNKNOWN) {
                (*mEvtCbChangeStatus)(user_stat);
            }
        }
        mPrevStatus = mStatus;
    }

    if (mStatus == INSTAT_NORMAL) {
        //amount_msat
        uint64_t msat = getLastMsat();
        if ((msat != AMOUNT_INIT) && (mEvtLocalMsat != msat)) {
            DBG_PRINTLN("amount change");
            if (mEvtCbChangeMsat != 0) {
                (*mEvtCbChangeMsat)(msat);
            }
        }
        mEvtLocalMsat = msat;
    }
}


/********************************************************************
 * private methods
 ********************************************************************/

LnShield::Err_t LnShield::handshake()
{
    const uint8_t INITRD[] = { 0x12, 0x34, 0x56, 0x78, 0x9a };
    const uint8_t INITWT[] = { 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x9a, 0x78, 0x56, 0x34, 0x12 };
    const uint8_t ERR[] = { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a };

    Err_t err = ENONE;

    //0x55が現れるまで読み捨て
    if (mStatus == INSTAT_STARTING) {
        if (Serial.available() > 0) {
            uint8_t rd = Serial.read();
            if (rd == 0x55) {
                DBG_PRINTLN("HANDSHAKE1");
                mStatus = INSTAT_HANDSHAKE1;
                //delay(100);
            }
        }
    }
    //0xaaが現れるまで読み捨て
    if (mStatus == INSTAT_HANDSHAKE1) {
        if (Serial.available() > 0) {
            uint8_t rd = Serial.read();
            if (rd == 0x00) {
                DBG_PRINTLN("HANDSHAKE2");
                mStatus = INSTAT_HANDSHAKE2;
                //delay(100);
            } else if (rd == 0x55) {
                //read out
            } else {
                mStatus = INSTAT_STARTING;
                err = 0;
                DBG_PRINTLN("fail INSTAT_HANDSHAKE2");
                Serial.write(ERR, sizeof(ERR));
                delay(2000);
            }
        }
    }
    if (mStatus == INSTAT_HANDSHAKE2) {
        if (Serial.available() >= sizeof(INITRD)) {
            DBG_PRINTLN("HANDSHAKE3");
            mStatus = INSTAT_HANDSHAKE3;
            //delay(100);
        }
    }
    if (mStatus == INSTAT_HANDSHAKE3) {
        int lp;
        uint8_t rd;
        for (lp = 0; lp < sizeof(INITRD); lp++) {
            rd = Serial.read();
            if (rd != INITRD[lp]) {
                break;
            }
        }
        if (lp == sizeof(INITRD)) {
            Serial.write(INITWT, sizeof(INITWT));
            DBG_PRINTLN("INSTAT_NORMAL");
            mStatus = INSTAT_NORMAL;
            delay(100);
            
            // local_msatの更新
            err = cmdPolling();
        } else {
            //retry
            mStatus = INSTAT_STARTING;
            err = 0;
            DBG_PRINTLN("fail INSTAT_HANDSHAKE3");
            DBG_PRINT("lp=");
            DBG_PRINTLN(lp);
            DBG_PRINT("rd=");
            DBG_PRINTLN(rd);
            Serial.write(ERR, sizeof(ERR));
            delay(2000);
        }
    }

    return err;
}


/** send to LnShield
 *
 * @param[in]   Cmd     command
 * @param[in]   pData   data
 * @param[in]   Len     pData length
 * @return      error
 */
void LnShield::uartSend(uint8_t Cmd, const uint8_t *pData, uint16_t Len)
{
    mWorkBuf[0] = 0x00;
    mWorkBuf[1] = 0xff;
    mWorkBuf[2] = Len >> 8;
    mWorkBuf[3] = Len & 0xff;
    mWorkBuf[4] = (uint8_t)(0 - mWorkBuf[2] - mWorkBuf[3]);
    mWorkBuf[5] = Cmd;
    uint8_t dcs = 0;
    for (uint8_t lp = 0; lp < Len; lp++) {
        mWorkBuf[6 + lp] = pData[lp];
        dcs += pData[lp];
    }
    mWorkBuf[6 + Len] = (uint8_t)(0 - dcs);     //DCS
    mWorkBuf[6 + Len + 1] = 0xef;

    Serial.write(mWorkBuf, Len + 8);
}


/** receive from LnShield
 *
 * @param[in]       Cmd         受信待ちコマンド
 * @param[in,out]   pRecvLen    [in]pResult length, [out]received length
 * @return      エラー
 * @note
 *          - 受信データはmWorkBuf[]に入っている.
 */
LnShield::Err_t LnShield::uartRecv(uint8_t Cmd, uint8_t *pResult, uint16_t *pRecvLen)
{
    int rd;

    rd = Serial.readBytes(mWorkBuf, 6);
    if (rd != 6) {
        DBG_PRINT("uartRecv\tEUART_RD_HEAD_LEN");
        DBG_PRINT("\t");
        DBG_PRINTLN(rd);
        return EUART_RD_HEAD_LEN;
    }

    if (mWorkBuf[0] != 0x00) {
        DBG_PRINT("uartRecv\tEUART_RD_HEAD_PREAMBLE0");
        DBG_PRINT("\t");
        DBG_PRINTLN(mWorkBuf[0]);
        return EUART_RD_HEAD_PREAMBLE;
    }
    if (mWorkBuf[1] != 0xff) {
        DBG_PRINT("uartRecv\tEUART_RD_HEAD_PREAMBLE1");
        DBG_PRINT("\t");
        DBG_PRINTLN(mWorkBuf[1]);
        return EUART_RD_HEAD_PREAMBLE;
    }
    if (((mWorkBuf[2] + mWorkBuf[3] + mWorkBuf[4]) & 0xff) != 0x00) {
        DBG_PRINTLN("uartRecv\tEUART_RD_HEAD_LCS");
        return EUART_RD_HEAD_LCS;
    }

    uint16_t len = getBe16_(mWorkBuf + 2);
    if (len > *pRecvLen) {
        // read out
        for (int lp = 0; lp < 1 + len + 2; lp++) {
            uint8_t ro;
            rd = Serial.readBytes(&ro, 1);
            if (rd != 1) {
                break;
            }
        }
        DBG_PRINTLN("uartRecv\tELESS_BUFFER");
        return ELESS_BUFFER;
    }

    uint8_t reply = mWorkBuf[5];
    if ((Cmd | RES_FLAG) != reply) {
        DBG_PRINTLN("uartRecv\tEUART_RD_REPLY");
        return EUART_RD_REPLY;
    }

    //data
    rd = Serial.readBytes(pResult, len);
    if (rd != len) {
        DBG_PRINTLN("uartRecv\tEUART_RD_TAIL_LEN1");
        return EUART_RD_TAIL_LEN;
    }
    uint8_t sum = 0;
    for (size_t lp = 0; lp < len; lp++) {
        sum += pResult[lp];
    }

    uint8_t dcs;
    rd = Serial.readBytes(&dcs, 1);
    if (rd != 1) {
        DBG_PRINTLN("uartRecv\tEUART_RD_TAIL_DCS1");
        return EUART_RD_TAIL_DCS;
    }
    if ((sum + dcs) & 0xff != 0) {
        DBG_PRINTLN("uartRecv\tEUART_RD_TAIL_DCS2");
        return EUART_RD_TAIL_DCS;
    }

    uint8_t postamble;
    rd = Serial.readBytes(&postamble, 1);
    if (rd != 1) {
        DBG_PRINTLN("uartRecv\tEUART_RD_TAIL_POSTAMBLE1");
        return EUART_RD_TAIL_POSTAMBLE;
    }
    if (postamble != 0xef) {
        DBG_PRINTLN("uartRecv\tEUART_RD_TAIL_POSTAMBLE2");
        return EUART_RD_TAIL_POSTAMBLE;
    }

    *pRecvLen = len;
    return ENONE;
}


LnShield::Err_t LnShield::uartSendCmd(uint8_t Cmd, const uint8_t *pData, uint16_t Len, uint16_t *pRecvLen)
{
    *pRecvLen = sizeof(mWorkBuf);
    return uartSendCmd(Cmd, pData, Len, mWorkBuf, pRecvLen);
}


LnShield::Err_t LnShield::uartSendCmd(
            uint8_t Cmd, const uint8_t *pData, uint16_t Len,
            uint8_t *pResult, uint16_t *pRecvLen)
{
    Err_t err;

    uartSend(Cmd, pData, Len);
    err = uartRecv(Cmd, pResult, pRecvLen);
    return err;
}
