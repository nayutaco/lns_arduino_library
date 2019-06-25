/** @file   LnShield.h
 *  @brief  Lightning Shield for Arduino API
 *  @author Nayuta inc.
 */
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
     * @return  error
     */
    Err_t init();


    /********************************************************************
     * event loop
     ********************************************************************/

    /** initialize event loop
     *
     * @param[in]   cbChangeStatus  callback function on change status
     * @param[in]   cbChangeMsat    callback function on change local_msat
     * @param[in]   cbError         callback function on error
     */
    void LnShield::eventInit(
            LnShieldFuncChangeStatus_t cbChangeStatus,
            LnShieldFuncChangeMsat_t cbChangeMsat,
            LnShieldFuncError_t cbError);


    /** eventloop process
     *
     * @attention
     *  - call this function within 60sec for Raspberry Pi
     *      - Raspberry Pi check polling UART command elapse
     */
    void eventPoll();


    /********************************************************************
     * request
     ********************************************************************/


    /** latest lightning amount_msat
     *
     * @retval  UINT64_MAX      yet updated on startup
     */
    uint64_t getLastMsat() const { return mLocalMsat; }


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

    /** stop Raspberry Pi
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


private:
    enum InStatus_t {
        INSTAT_STARTUP,
        INSTAT_STARTING,
        INSTAT_HANDSHAKE1,
        INSTAT_HANDSHAKE2,
        INSTAT_HANDSHAKE3,
        INSTAT_NORMAL,
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
    uint64_t            mLocalMsat;         ///< local msat


    uint64_t            mEvtLocalMsat;
    LnShieldFuncChangeStatus_t  mEvtCbChangeStatus;
    LnShieldFuncChangeMsat_t    mEvtCbChangeMsat;
    LnShieldFuncError_t         mEvtCbError;
};

#endif  //LN_SHIELD_H_
