#ifndef LN_SHIELD_H_
#define LN_SHIELD_H_

#include <stdint.h>


/** @class  LnShield
 */
class LnShield {
public:
    static const uint64_t       AMOUNT_INIT = UINT64_MAX;
    static const int            INVOICE_DESC_MAX = 20;

public:
    enum Status_t {
        STATUS_INIT,
        STATUS_STARTING,
        STATUS_NORMAL,
        //
        STATUS_UNKNOWN,
    };

    enum Err_t {
        ENONE,
        ERROR,                  ///< something error

        EUART_RD_HEAD_LEN,      ///< invalid uart packet Header
        EUART_RD_HEAD_PREAMBLE, ///< invalid uart packet Header
        EUART_RD_HEAD_LCS,      ///< invalid uart packet Header
        EUART_RD_TAIL_LEN,      ///< invalid uart packet Data
        EUART_RD_TAIL_POSTAMBLE,///< invalid uart packet Data
        EUART_RD_TAIL_DCS,      ///< invalid uart packet Data
        EUART_RD_REPLY,         ///< invalid uart packet response

        EALREADY_INIT,          ///< already init() called
        EDISABLED,              ///< instance cannot use
        EINVALID_PARAM,         ///< invalid parameter
        EINVALID_RES,           ///< invalid response
        ELESS_BUFFER,           ///< response too large
        EPROCESSING,            ///< processing
    };
    typedef void (*LnShieldFuncChangeStatus_t)(Status_t);
    typedef void (*LnShieldFuncChangeMsat_t)(uint64_t);
    typedef void (*LnShieldFuncError_t)(Err_t);


public:
    LnShield(int pinOutputEnable);
    virtual ~LnShield();


public:
    /** 初期化
     *
     * @return  エラー
     */
    Err_t init();


    /** latest lightning amount_msat
     *
     * @retval  UINT64_MAX      yet updated on startup
     */
    uint64_t getLastMsat() const { return mLocalMsat; }


    /********************************************************************
     * event loop for easy to use
     ********************************************************************/

    /** initialize event loop
     *
     * @param[in]   cbChangeStatus  callback function on change status
     * @param[in]   cbChangeMsat    callback function on change local_msat
     * @param[in]   cbError         callback function on error
     */
    void LnShield::easyEventInit(
            LnShieldFuncChangeStatus_t cbChangeStatus,
            LnShieldFuncChangeMsat_t cbChangeMsat,
            LnShieldFuncError_t cbError);


    /** eventloop process
     *
     * @attention
     *  - call this function within 60sec for Raspberry Pi
     *      - Raspberry Pi check polling UART command elapse
     */
    void easyEventPoll();


    /** request display invoice
     *
     * @param[in]   amountMsat      request msat
     * @param[in]   description     (optional)description
     * @retval  true    
     */
    bool easyEventRequestInvoice(uint64_t amountMsat, const char *description = NULL);


    /********************************************************************
     * command: Bitcoin
     ********************************************************************/

    /** 支払い可能Bitcoin値取得
     * 現在のウォレットで支払い可能なBitcoin値を取得する。
     *
     * @param[out]      balance     Bitcoin amount
     * @return  エラー
     */
    //Err_t cmdGetBalance(uint64_t balance[]);


    /** アドレス発行
     * 受信するためのBitcoinアドレスを生成する。
     *
     * @param[out]      address     Bitcoin Address for receive
     * @return  エラー
     */
    //Err_t cmdGetNewAddress(char address[]);


    /** トランザクション手数料設定
     * #sendBitcoin()時に支払われるトランザクション手数料を設定する。
     *
     * @param[in]       feerate     feerate/1000byte
     * @return  エラー
     * @note
     *      - デフォルト値は、0.5 mBTC(50000 satoshi)
     *      - #sendBitcoin()後に設定した場合は、次回の #setBitcoin()で有効になる。
     */
    //Err_t cmdSetFeeRate(uint32_t feerate);


    /** Bitcoin支払い
     * Bitcoin送金のトランザクションを発行する。<br>
     * amount以外にfeeも支払う。
     *
     * @param[in]       sendAddr    送金するBitcoinアドレス
     * @param[in]       amount      送金するBitcoin
     * @return  エラー
     */
    //Err_t cmdSendBitcoin(const char sendAddr[], uint64_t amount);


    /********************************************************************
     * command: Lightning Network
     ********************************************************************/

    /** request create invoice
     *
     * @param[in]   amountMsat      request msat
     * @param[in]   description     (not NULL)description
     * @return  error
     */
    Err_t cmdInvoice(uint64_t amountMsat, const char *description);


    /** request create invoice
     *
     * @param[out]      pInvoice    invoice string
     * @param[in,out]   pLen        [in]invoice buffer length, [out]invoice result length
     * @return  error
     */
    Err_t cmdGetLastInvoice(char *pInvoice, size_t *pLen);


    /********************************************************************
     * command: control
     ********************************************************************/

    /** Raspberry Pi停止
     *
     */
    Err_t cmdStop();


    /** send polling UART command
     *
     * @return  error
     * @attention
     *  - call this function within 60sec for Raspberry Pi
     *      - Raspberry Pi check polling UART command elapse
     */
    Err_t cmdPolling();


    /** ePaper出力
     *
     * @return  エラー
     */
    //Err_t cmdEpaper(const char str[]);


private:
    enum InStatus_t {
        INSTAT_STARTUP,         //起動直後
        INSTAT_STARTING,        //init()呼び出し後
        INSTAT_HANDSHAKE1,
        INSTAT_HANDSHAKE2,
        INSTAT_HANDSHAKE3,
        INSTAT_NORMAL,          //定常状態
    };


private:
    Err_t handshake();
    void uartSend(uint8_t Cmd, const uint8_t *pData, uint16_t Len);
    Err_t uartRecv(uint8_t Cmd, uint8_t *pResult, uint16_t *pRecvLen);
    Err_t uartSendCmd(uint8_t Cmd, const uint8_t *pData, uint16_t Len, uint16_t *pRecvLen);
    Err_t uartSendCmd(
                uint8_t Cmd, const uint8_t *pData, uint16_t Len,
                uint8_t *pResult, uint16_t *pRecvLen);


private:
    InStatus_t          mStatus;            ///< internal status
    InStatus_t          mPrevStatus;        ///< internal previous status
    int                 mPinOE;             ///< OutputEnable
    uint8_t             mWorkBuf[64];       ///< work buffer
    uint64_t            mLocalMsat;         ///< pollingで取得したmsat


    uint64_t            mEvtLocalMsat;
    LnShieldFuncChangeStatus_t  mEvtCbChangeStatus;
    LnShieldFuncChangeMsat_t    mEvtCbChangeMsat;
    LnShieldFuncError_t         mEvtCbError;
};

#endif  //LN_SHIELD_H_
