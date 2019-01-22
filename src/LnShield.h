#ifndef LN_SHIELD_H_
#define LN_SHIELD_H_

#include <stdint.h>


#define LNERR_ALREADY_INIT      (-1)    ///< 初期化済みでinit()を呼び出した
#define LNERR_DISABLED          (-2)    ///< インスタンス使用不可
#define LNERR_INVALID_PARAM     (-3)    ///< 引数不正
#define LNERR_INVALID_RES       (-4)    ///< シールドからの応答が不正
#define LNERR_PROCESSING        (-5)    ///< 不正応答
#define LNERR_ERROR             (-6)    ///< 処理中


#define LNUNIT_SATOSHI          (1)     ///< 1/10^8 bitcoin
#define LNUNIT_MBTC             (2)     ///< 1/10^3 bitcoin
#define LNUNIT_BTC              (3)     ///< 1 bitcoin


#define LNAMOUNT_INVALID_VAL    (0xFFFFFFFFUL)      ///< Bitcoin額として無効値


#define LNPAYSTAT_NONE          ((char)0)
#define LNPAYSTAT_RECV          ((char)1)

//FORWARD definition
class Stream;


/** @class  LnShield
 */
class LnShield {
public:
    enum Status_t {
        STAT_STARTUP,       //起動直後
        STAT_STARTING,      //init()呼び出し後
        STAT_WAITING,       //初期化中
        STAT_STARTED,       //通信直後はもたつくので、pollを待つ
        STAT_INITED,        //定常状態
    };

public:
    LnShield();
    virtual ~LnShield();

public:
    /** 初期化
     *
     * @param[in]   confs       着金通知するConfirmation数
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     */
    int init(byte confs);

    /** Raspberry Pi停止
     *
     */
    void stop();

    /** 着金確認
     *
     * @param[out]      amount      着金した値[単位:satoshi]
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     * @note
     *      - amount[0]が #LNAMOUNT_INVALID_VAL の場合、着金無し
     */
    int polling(unsigned long amount[], char status[]);

    /** アドレス発行
     * 受信するためのBitcoinアドレスを生成する。
     *
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     */
    int issueAddress();

    /** Bitcoin支払い要求
     * Bitcoinをほかの人に支払ってもらえるよう、BIP0021に従ったURLを生成する。
     *
     * @param[in]       amount      要求するBitcoin。0のときは省略。
     * @param[in]       unit        amountの単位(LNUNIT_xxx)
     * @param[in]       label       ラベル
     * @param[in]       message     メッセージ
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     * @note
     *      - #issueAddress()するまでは同じBitcoinアドレスを使用する。
     */
    int requestBitcoin(unsigned long amount, int unit, const char label[], const char message[]);

    /** トランザクション手数料設定
     * #sendBitcoin()時に支払われるトランザクション手数料を設定する。
     *
     * @param[in]       fee         手数料
     * @param[in]       unit        feeの単位(LNUNIT_xxx)
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     * @note
     *      - デフォルト値は、0.5 mBTC(50000 satoshi)
     *      - #sendBitcoin()後に設定した場合は、次回の #setBitcoin()で有効になる。
     */
    int setFee(unsigned long fee, int unit);

    /** Bitcoin支払い
     * Bitcoin送金のトランザクションを発行する。<br>
     * amount以外にfeeも支払う。
     *
     * @param[in]       sendAddr    送金するBitcoinアドレス
     * @param[in]       amount      送金するBitcoin(単位は引数unit次第)
     * @param[in]       unit        amountの単位(LNUNIT_xxx)
     * @return          0:正常終了, 0以外:エラー種別(LNERR_xxx)
     */
    int sendBitcoin(const char sendAddr[], unsigned long amount, int unit);

    /** 支払い可能Bitcoin値取得
     * 現在のウォレットで支払い可能なBitcoin値を取得する。
     *
     * @return          支払い可能なBitcoin値(単位:satoshi)
     * @attention
     *      - 値は符号なし32bit値で返すため、最大で4,294,967,294 satoshiまでしか返さない。
     *      - インスタンスが使用不可の場合や32bitで表せない値の場合、#LNAMOUNT_INVALID_VAL を返す。
     */
    unsigned long getBalance();


    Status_t getStatus() { return sStatus; }


    void sendTest(uint8_t cmd, const uint8_t *pData, uint8_t len) { send(cmd, pData, len); }

private:
    void send(uint8_t cmd, const uint8_t *pData, uint8_t len);
    int recv(uint8_t cmd);
    unsigned long changeSatoshi(unsigned long val, int unit);
    bool checkRecvTimeout(int rdCnt);


private:
    static Status_t     sStatus;

private:
    uint8_t             mSendBuf[64];       ///< 送信バッファ
    uint8_t             mRecvBuf[64];       ///< 受信バッファ
    char                mRecvAddr[40];      ///< 着金アドレス
    Stream*             mpSerial;           ///< BitcoinShieldとの通信. #init()で設定するが、0の場合はインスタンスとして使用できない。
    uint32_t            mFee;               ///< 手数料[単位:satoshi]
    byte                mConfs;             ///< CONFIRMATION数(kingfisher1号のみ有効)
};

#endif  //LN_SHIELD_H_
