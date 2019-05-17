#ifndef DBGBOARD_H__
#define DBGBOARD_H__

enum DbgBoardLedType_t {
    DBGBOARD_LED_INIT,
    DBGBOARD_LED_WAIT,
    DBGBOARD_LED_NORMAL,
    DBGBOARD_LED_ERROR,
};

enum DbgBoardBuzzerType_t {
    DBGBOARD_BUZZER_INIT,
    DBGBOARD_BUZZER_CHGSTAT,
    DBGBOARD_BUZZER_INVOICE,
    DBGBOARD_BUZZER_GET,
    DBGBOARD_BUZZER_PAY,
    DBGBOARD_BUZZER_ERROR,
};


void dbgboard_setup();
void dbgboard_led(DbgBoardLedType_t Type);
void dbgboard_buzzer(DbgBoardBuzzerType_t Type);
bool dbgboard_button();

#endif /* DBGBOARD_H__ */
