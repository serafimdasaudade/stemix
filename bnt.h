//pinout conector2
/* LAYout conector teclado
 * 1        10
 * __________
 * |         |
 * |__       |
 *    |      |
 *  __|      |
 * |         |
 * |_________|
 * 5        6
 */
    //Conector teclado                      PIN PIC
/*
#define PIN2    RC0 //T+,T-                 PIN 11
#define PIN1    RC3 //RPM, TIME(RUN/STOP)   PIN 14
#define PIN10   RC6 //TIME+, TIME-          PIN 17
#define PIN9    RC7 //RPM+, RPM-            PIN 2
*/

#define PIN9    RA0
#define PIN10   RA1
#define PIN1    RA2
#define PIN2    RA3

#define PIN3    RA5 //RPM,            T+, RPM+, TIME+,  PIN6
#define PIN8    RA4 //TIME(RUN/STOP), T-, RPM-, TIME-,   PIN7

#define TIME_PLUS   1
#define RPM_PLUS    2
#define T_PLUS      3
#define RPM_SET     4
#define TIME_MENOS  5
#define RPM_MENOS   6
#define T_MENOS     7
#define TIME        8

typedef struct{
    UINT8 key_is_press_flag[9];
    UINT8 key_prev_press_flag[9];
    UINT key_press_timer;
}Sbnt;

Sbnt bnt;

void bnt_init(void);
UINT8 wait_for_press_bnt(void);
UINT8 bnt_control(void);


