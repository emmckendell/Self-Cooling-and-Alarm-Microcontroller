#define _XTAL_FREQ      8000000

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02

#define ALARM_EN        PORTBbits.RB2           // <-- Need to change the assignments here
#define RTC_ALARM_NOT   PORTBbits.RB1           // <-- Need to change the assignments here
#define KEY_PRESSED     PORTAbits.RA4           // <-- Need to change the assignments here

#define Ch_Minus        0
#define Channel         1
#define Ch_Plus         2
#define Prev            3
#define Next            4
#define Play_Pause      5
#define Minus           6
#define Plus            7
#define EQ              8
#define Zero_Button     9