//****************************************************************************
//                            Stemix
//  Author: Luis Santos
//
//  Started: V001 - 01/10/2014
//                  18/10/2014 - start eeprom support and dallas_1wire
//****************************************************************************
//****************************************************************************


#include <xc.h>
#include <GenericTypeDefs.h>
#include <string.h>
#include <stdio.h>

#include "lcd_hd44780_pic16.h"
//#include "PID.h"
#include "bnt.h"
#include "stepper.h"
#include "dallas_1wire.h"
#include "eeprom.h"
#include "heat_pump.h"
#include "dc_motor.h"

#define VERSION         002

#define USE_LCD

#define BUZZER_OFF  PORTBbits.RB7 = 0;
#define BUZZER_ON   PORTBbits.RB7 = 1;

#define HIGH            1
#define LOW             0

#define RPM_MAX         50
#define MIN_MAX         59
#define SEC_MAX         60
#define TIME_STEP       10
#define TEMP_MAX        500
#define TEMP_MIN        40

UINT time_250ms;

typedef struct{
    INT8 second;
    INT8 minute;
    INT8 hour;
    UINT day;
}sclock;

sclock clock;

#define IDLE    1
#define START   2
#define STOP    3
#define FINISH  4
#define FAULT   5

typedef struct{
    UINT8 status;
    INT temp_int;
    //INT temp_dec;
    //float temp;
    UINT rpm;
    UINT8 minute;
    //UINT8 hour;
    UINT8 sec;
}sprogram;

sprogram program;

UINT8 str[16];
UINT8 key;
UINT32 temp_int;
INT rpm;
//float temp_c_f;

void init(void);
void delay_250us(BYTE time);
void program_control(void);
