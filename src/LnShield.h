#ifndef LN_SHIELD_H_
#define LN_SHIELD_H_

#include <stdint.h>


/** @class  LnShield
 */
class LnShield {
public:
    enum Status_t {
        STAT_STARTUP,           //起動直後
        STAT_STARTING,          //init()呼び出し後
        STAT_HANDSHAKE1,
        STAT_HANDSHAKE2,
        STAT_HANDSHAKE3,
        STAT_NORMAL,            //定常状態
    };

    enum Err_t {
        ENONE,
        ERROR,                  ///< エラー

        EUART_RD_HEAD_LEN,      ///< uart packet Header不正
        EUART_RD_HEAD_PREAMBLE, ///< uart packet Header不正
        EUART_RD_HEAD_LCS,      ///< uart packet Header不正
        EUART_RD_TAIL_LEN,      ///< uart packet Data不正
        EUART_RD_TAIL_POSTAMBLE,///< uart packet Data不正
        EUART_RD_TAIL_DCS,      ///< uart packet Data不正
        EUART_RD_REPLY,         ///< uart packet response不正

        EALREADY_INIT,          ///< 初期化済みでinit()を呼び出した
        EDISABLED,              ///< インスタンス使用不可
        EINVALID_PARAM,         ///< 引数不正
        EINVALID_RES,           ///< シールドからの応答が不正
        EPROCESSING,            ///< 処理中
    };

public:
    LnShield(int pinOutputEnable);
    virtual ~LnShield();


public:
    /** 初期化
     *
     * @return  エラー
     */
    Err_t init();


    Status_t getStatus() const { return mStatus; }


    /** latest lightning amount_msat
     *
     * @retval  UINT64_MAX      yet updated on startup
     */
    uint64_t getLastMsat() const { return mLocalMsat; }


    /********************************************************************
     * command: Bitcoin
     ********************************************************************/

    /** 支払い可能Bitcoin値取得
     * 現在のウォレットで支払い可能なBitcoin値を取得する。
     *
     * @param[out]      balance     Bitcoin amount
     * @return  エラー
     */
    Err_t cmdGetBalance(uint64_t balance[]);


    /** アドレス発行
     * 受信するためのBitcoinアドレスを生成する。
     *
     * @param[out]      address     Bitcoin Address for receive
     * @return  エラー
     */
    Err_t cmdGetNewAddress(char address[]);


    /** トランザクション手数料設定
     * #sendBitcoin()時に支払われるトランザクション手数料を設定する。
     *
     * @param[in]       feerate     feerate/1000byte
     * @return  エラー
     * @note
     *      - デフォルト値は、0.5 mBTC(50000 satoshi)
     *      - #sendBitcoin()後に設定した場合は、次回の #setBitcoin()で有効になる。
     */
    Err_t cmdSetFeeRate(uint32_t feerate);


    /** Bitcoin支払い
     * Bitcoin送金のトランザクションを発行する。<br>
     * amount以外にfeeも支払う。
     *
     * @param[in]       sendAddr    送金するBitcoinアドレス
     * @param[in]       amount      送金するBitcoin
     * @return  エラー
     */
    Err_t cmdSendBitcoin(const char sendAddr[], uint64_t amount);


    /********************************************************************
     * command: Lightning Network
     ********************************************************************/

    Err_t cmdInvoice(uint64_t amountMsat);


    /********************************************************************
     * command: control
     ********************************************************************/

    /** Raspberry Pi停止
     *
     */
    Err_t cmdStop();


    /** 定常確認
     *
     * @return  エラー
     */
    Err_t cmdPolling();


    /** ePaper出力
     *
     * @return  エラー
     */
    Err_t cmdEpaper(const char str[]);


private:
    Err_t handshake();
    void uartSend(uint8_t Cmd, const uint8_t *pData, uint16_t Len);
    Err_t uartRecv(uint8_t Cmd, uint16_t *pRecvLen);
    Err_t uartSendCmd(uint8_t Cmd, const uint8_t *pData, uint16_t Len, uint16_t *pRecvLen);


private:
    Status_t            mStatus;
    int                 mPinOE;             ///< OutputEnable
    uint8_t             mWorkBuf[64];       ///< 作業バッファ
    uint64_t            mLocalMsat;         ///< pollingで取得したmsat
};

#endif  //LN_SHIELD_H_
