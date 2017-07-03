//****************************************************************************
//                            Stemix
//  Author: Luis Santos
//
//  Started: V001 - 01/10/2014
//
//****************************************************************************
//****************************************************************************


#include <xc.h>

#include "lcd_hd44780_pic16.h"
//#include "../lcd_hd44780_pic16.h"
//#define pic16f882

// CONFIG
#ifdef pic16f882
#pragma config FOSC = HS //INTRC_CLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF

#else

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = ALL         // FLASH Program Memory Code Protection bits (0000h to 1FFFh code protected)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low Voltage In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = ON         // Data EE Memory Code Protection (Data EEPROM memory code-protected)
#pragma config WRT = OFF         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
/*
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config CP = ALL         // FLASH Program Memory Code Protection bits (0000h to 1FFFh code protected)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = ON         // Data EE Memory Code Protection (Data EEPROM memory code-protected)
#pragma config WRT = OFF        // FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)
*/
#endif

#include "main.h"

UINT8 PID_flag = 0, sec_flag = 0;

#define LOOP_SEC          2               //-- Approx Main Loops per second
#define ERROR_COUNT_MAX   38 * LOOP_SEC   //-- Max time For Temp Increase
#define ERROR_START_DELAY 20 * LOOP_SEC   //-- An additional delay Pwr Up
#define ERROR_TRIGGER_PWR 70              //-- 70% power is area for error chk

#define CRC_ERR_ADD  4         //-- Amount to add to Error counter if CRC Error

#define P_GAIN 2
#define D_GAIN 0
#define I_GAIN 10

#define I_COUNT_MAX   2            //-- # of intervals before increasing I
#define I_COUNT_DEC   4            //-- # times longer to wait b4 decrementing
#define I_VALUE_MAX   40           //-- Max Integral Value - Limits overshoot


#define CONTROLED_BAND 10          //-- Temperature band around Target that
//   is controlled (2 units per degree!!)

#define MAX_TEMP    (500)       //-- Max Temperature of 50 Degrees


#define BURST_TOTAL_RELOAD 6000         //-- Number of cycles per Run
#define BURST_PERCENT_DIVIDER 1        //-- Amount to div POWER to get count
#define BURST_TIMER_RELOAD  (-2)       //-- 2 counts per Full Wave
#define BURST_POWER_ON 6000             //-- MAX POWER SETTING
#define BURST_POWER_OFF  60             //-- MIN POWER SETTING

#define OUTPUT_ON 1
#define OUTPUT_OFF 0


//*********************** Ports ******************************
#define OUTPUT_PIN    PORTBbits.RB4  // OUTPUT to the TRIAC
#define T_OUTPUT_PIN  TRISBbits.TRISB4   // The tris for above

// VARIABLES
float lastInput;
float ITerm;
float outMax;
float outMin;
float Ki,Kp,Kd;
float POWER;
bit OverFlow;

unsigned int BURST_ON_NEXT; //-- Reload value for _COUNT
unsigned int BURST_ON_COUNT; //-- Number of counts to be ON
unsigned int BURST_TOTAL_COUNT; //-- Total number of Counts to do
unsigned int BURST_PREVIOUS_SETTING; //-- the previous power setting
/*
unsigned int Intergral;
unsigned int I_Counter;

unsigned int ErrorCounter;
unsigned int LastTemperature;
*/

//******* FUNCTIONS ***********
unsigned int Calculate(unsigned int TARGET, unsigned int CURRENT);
void Burst_Output(unsigned int Percentage);
void CheckTimeOut(unsigned int target, unsigned int current, unsigned int power);
void MyInterruptRoutine(void);

void interrupt isr(void) {
    ///////////timer interrupt///////////////
    if (TMR1IF) {
        MyInterruptRoutine(); //PID        
        //if (time_250ms == 8) //every second
        if (time_250ms >= 5240) //every second
        {
            PID_flag = 1;
            if (sec_flag >= 1)
                sec_flag = 0;
            else
                sec_flag++;
            time_250ms = 0;
            clock.second--; //increment second timer
            bnt.key_press_timer ++;

        } else {
            time_250ms++;
        }

        if (clock.second < 0) //every minutes
        {            
            if(clock.minute == 0)
            {
                clock.minute = 0;                
            }else
            {                
                clock.minute--; //increment minute timer
                clock.second = 59; //clear second timer
            }
        }
        if (clock.minute <= 0) //every hour
        {
            clock.minute = 0; //clear minute timer
            clock.hour++; //increment hour timer
            if (clock.hour == 24) {
                clock.day++;
                clock.hour = 0;
            }
        }
        /*
        TMR1H = 0x1F;
        TMR1L = 0x00;
        */
        TMR1H = 0xFF;
        TMR1L = 0x9B;
        TMR1IF = 0;
    }
}

void main(void) {
    //UINT8 i;
    unsigned int target, current, power;
    unsigned int PwrUpDly;

    init();

    OverFlow = 0;
    PwrUpDly = ERROR_START_DELAY; //- Only Do on POWER UP -
//////////////////////////////
    POWER = 5000;
    lastInput=200;    
    ITerm=5000;
    outMax=BURST_TOTAL_RELOAD;
    outMin=0;
    Ki=1.5*0.1;
    Kd=0.1/0.1;
    Kp=50;
    Ki=(0-Ki);
    Kd=(0-Kd);
    Kp=(0-Kp);

////////////////////////7

    CLRWDT();

    //****** Initialise ports **********
    OUTPUT_PIN = 1;
    T_OUTPUT_PIN = 0;

    //****** Initialise Variables ***********
    BURST_ON_NEXT = 0; //-- Reload value for _COUNT
    BURST_ON_COUNT = 0; //-- Number of counts to be ON
    BURST_TOTAL_COUNT = BURST_TOTAL_RELOAD; //-- Total number of Counts to do
    BURST_PREVIOUS_SETTING = 0; //-- the previous power setting
//    Intergral = 0;

//    ErrorCounter = 0;

    BUZZER_OFF;
    /*
        mySetpoint = program.temp_int;
        myInput = temp_int;
        myOutput = 0;
        PID_INIT(myInput,myOutput,mySetpoint,2, 5, 1, DIRECT);
        SetMode(AUTOMATIC);
     */
    program.status = IDLE;
    dcmotor.status = OFF;
    //Initialize the LCD Module
    BUZZER_ON;
    __delay_ms(10);
    BUZZER_OFF;
    init_ds1820();
    init_dc_motor_encoder();
    heat_pump_cool();
    while (1) {
        //Do nothing, just loop indefinitely
        CLRWDT();
        program_control();
        bnt_control();
        //heat_pump_control();
        //heat_pump_cool();
        dc_motor_control();
        CLRWDT();
        target = program.temp_int; //ReadADC();
        current = temp_int; //ReadProbe(current);
        power = Calculate(target, current);
        //CheckTimeOut(target,current,power);  //-- Check After successful power up, check error's
        //program.rpm = power;
        Burst_Output(power);
        //myInput = temp_int;
        if (PID_flag) {
            PID_flag = 0;
            //Compute();  //nao esta boa a funcao. pic fica pressa
            //MyInterruptRoutine(); //PID
        }
        //MyInterruptRoutine(); //PID
        TMR1IE = 0;
        sensor_1_ok = read_ds1820();
        //sensor_1_ok = read_1ds1820();
        TMR1IE = 1;
    }
}

void init(void) {
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
#ifdef pic16f882
    WPUB = 0x00;
    IOCB = 0x00;
    ANSEL = 0x00;
    ANSELH = 0x00;
    //OSCCON = 0b01110111; // Internal OSC @ 8MHz
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.SCS = 0;
    //OPTION_REG = 0b11010111; // WPU disabled, INT on rising edge, FOSC/4
    ///add LS - 28_5_2014
    //while (!OSCCONbits.HTS) //wait for ... became stable
    {
        NOP();
    }

#else
    //Set RA# aos digital I/O  pag 128

    PCFG0 = LOW;
    PCFG1 = HIGH;
    PCFG2 = HIGH;
    PCFG3 = LOW;

    /*
    PCFG0 = LOW;
    PCFG1 = HIGH;
    PCFG2 = LOW;
    PCFG3 = LOW;
     */
#endif
    //    WDTCON = 0b00000001;        // Prescaler 1:65536
    PID_flag = 0;
    time_250ms = 0;
    clock.hour = 00;
    clock.minute = 00;
    rpm = 00;
    temp_int = 0;
    program.sec = 00;
    program.minute = 10;
    program.rpm = RPM_MAX;
    program.temp_int = 40; //TEMP_MAX - 2;
    bnt.key_press_timer = 0;
    bnt_init();

#ifdef USE_LCD
    __delay_ms(30);
    LCDInit(LS_NONE);
    //LCDInit(LS_BLINK);
    //Clear the display
    LCDClear();
    //    __delay_ms(2000);
    //lcd_init();
#ifdef USE_LCD
    LCDGotoXY(0, 0); //set
    sprintf(str, "   Concessus   ");
    LCDWriteString(str);
    LCDGotoXY(0, 1); //set
    sprintf(str, "     Stemix     ");
    LCDWriteString(str);
    CLRWDT();
    __delay_ms(2000);
    CLRWDT();
#endif

#endif
    if (read_eep(EE_DEFAULT_ADD) == DEFAULT_ADD) //
    {
        program.sec = read_eep(EE_TIME_SEC);
        program.minute = read_eep(EE_TIME_MINUTE);
        program.rpm = read_eep(EE_RPM);
        program.temp_int = (read_eep(EE_TEMP_MSB) << 8) | read_eep(EE_TEMP_LSB);

    } else {
        write_eep(EE_DEFAULT_ADD, DEFAULT_ADD);
        write_eep(EE_TIME_SEC, program.sec);
        write_eep(EE_TIME_MINUTE, program.minute);
        write_eep(EE_RPM, program.rpm);
        write_eep(EE_TEMP_MSB, (program.temp_int >> 8));
        write_eep(EE_TEMP_LSB, program.temp_int);
    }
    //// TIMER 1 ////////////////////////////////
    TMR1IF = 0; // Clear the timr 1 interrupt flag
    //125ms
    //TMR1H = 0x1F;
    //TMR1L = 0x00;
    //
    //0,1ms0
    TMR1H = 0xFF;
    TMR1L = 0x9B;
    
    T1CONbits.T1CKPS = 0b00; //prescale 1/1 (1MHz -> 0,001ms)
    //(50ms = 50000counts -> 65535-50000=15535 (0xC3AF))
    T1CONbits.TMR1CS = 0; //use internal clock(FOSC/4)
    TMR1IE = 1; //Enable timer 1 interrupt
    TMR1ON = 1; //Enable timer1
    TXIE = 0; //Enable transmit interrupts
    RCIE = 0; //Enable receive interrupts
    BCLIE = 0; // Enable bus collision interrupts
    SSPIE = 0; // Enable serial port interrupts
    PEIE = 1; // Enable peripheral interrupts
    GIE = 1; // Enable global interrupts
}

void program_control(void) {
    UINT temp_int_aux;
    //return;
#ifdef USE_LCD
    //LCDClear();
    LCDGotoXY(0, 0); //set

    if (program.temp_int < 100)
        sprintf(str, "%02d:%02d  %03d  0%2.1f  ", program.minute, program.sec, program.rpm, (float) (program.temp_int / 10.0));
    else
        sprintf(str, "%02d:%02d  %03d  %2.1f  ", program.minute, program.sec, program.rpm, (float) (program.temp_int / 10.0));

    //sprintf(str, "%02d:%02d %ld", program.hour, program.minute, temperature_raw);
    LCDWriteString(str);
#endif

    switch (program.status) {
        case START:
#ifdef USE_LCD
            LCDGotoXY(0, 1); //actual
            //if (sec_flag)
            {
                if (temp_int < 100) {
                    if(temp_int<=36)
                        temp_int_aux = 36;
                    else
                        temp_int_aux = temp_int;
                    
                    if (sensor_1_ok)
                        sprintf(str, "%02d:%02d  %03d  0%2.1f", clock.minute, clock.second, rpm, (float) (temp_int_aux / 10.0));
                    else
                        sprintf(str, "%02d:%02d  %03d      ", clock.minute, clock.second, rpm);
                } else {
                    if (sensor_1_ok)
                        sprintf(str, "%02d:%02d  %03d  %2.1f", clock.minute, clock.second, rpm, (float) (temp_int / 10.0));
                    else
                        sprintf(str, "%02d:%02d  %03d      ", clock.minute, clock.second, rpm);
                }
                //sprintf(str, "%02d:%02d %04d %d", clock.minute, clock.second, rpm, temp_int);
            }
            LCDWriteString(str);
#endif
            if (clock.minute == 0 && clock.second == 0) //time off program finish
            {
                program.status = FINISH;
                //dcmotor.status = OFF;
            }
            //dcmotor.status = ON;

            break;
        case FINISH:
        case IDLE:
            //break;
        case STOP:
#ifdef USE_LCD

            LCDGotoXY(0, 1); //actual
            if (temp_int < 100) {
                if(temp_int<=36)
                        temp_int_aux = 36;
                    else
                        temp_int_aux = temp_int;

                if (sensor_1_ok)
                    sprintf(str, "STOP   %03d  0%2.1f", rpm, (float) (temp_int_aux / 10.0));
                else
                    sprintf(str, "STOP   %03d      ", rpm);
            } else {
                if (sensor_1_ok)
                    sprintf(str, "STOP   %03d  %2.1f", rpm, (float) (temp_int / 10.0));
                else
                    sprintf(str, "STOP   %03d      ", rpm);
            }
            //sprintf(str, " STOP %04d %d", rpm, temp_int);
            LCDWriteString(str);

#endif
            if (program.status == FINISH) {
                BUZZER_ON;
                TMR1IE = 0;
                __delay_ms(1500);
                TMR1IE = 1;
            }
            program.status = STOP;
            break;
    }
}


//****************************************************************
//                    Calculate
//****************************************************************
unsigned int Calculate(unsigned int TARGET, unsigned int CURRENT) {
    
    /*Compute all the working error variables*/
      float input;
      float error;
      float dInput;
      float output;

      input = CURRENT;
      error = TARGET - input;

      ITerm+= (Ki * error);
      if(ITerm > outMax)
          ITerm= outMax;
      else if(ITerm < outMin)
          ITerm= outMin;
      dInput = (input - lastInput);

      /*Compute PID Output*/
      output = Kp * error + ITerm - Kd * dInput;

      //rpm = output;
      
	  if(output > outMax) output = outMax;
      else if(output < outMin) output = outMin;

      POWER = output;

/*
      if (CURRENT > (TARGET + CONTROLED_BAND)) {
          POWER = BURST_POWER_ON;
      }
*/

      /*Remember some variables for next time*/
      lastInput = input;
      
      
/*
    CLRWDT();
    if (CURRENT >= TARGET-2) {
        if (CURRENT > (TARGET + CONTROLED_BAND)) {
            POWER = BURST_POWER_ON;
            Intergral = 0;
        } else {
            if (CURRENT != TARGET) {
                if (I_Counter++ > I_COUNT_MAX) {
                    I_Counter = 0; //-- Counter slows the response of I
                    Intergral++; //-- Only Increase Integral if less than target
                    if (Intergral > I_VALUE_MAX) Intergral = I_VALUE_MAX;
                }
            }
            //-- P.I.D     Proportional                Integral                 Derivative
            //POWER = ((CURRENT /TARGET) * P_GAIN) + (Intergral * I_GAIN) + ((CURRENT-TARGET) * D_GAIN);
            POWER = ((TARGET-CURRENT) * P_GAIN) + ( 1* I_GAIN) + ((CURRENT-TARGET) * D_GAIN);
        }
    } else {
        POWER = BURST_POWER_OFF;
        if (I_Counter++ > (I_COUNT_MAX * I_COUNT_DEC)) //-- Slower decrease of value
        {
            I_Counter = 0; //-- Counter slows the response of I
            if (Intergral != 0)Intergral--;
        }
    }
    rpm = POWER;
    if (POWER > BURST_POWER_ON) POWER = BURST_POWER_ON;

    if (CURRENT > MAX_TEMP) POWER = BURST_POWER_ON;
*/
    //POWER = 50;
    return (POWER);
}

//**********************************************************
void Burst_Output(unsigned int Percentage) {
    CLRWDT();
    if (Percentage != BURST_PREVIOUS_SETTING) //-- Check if need to change
    {
        BURST_PREVIOUS_SETTING = Percentage; //-- Need to change so store
        BURST_ON_NEXT = Percentage / BURST_PERCENT_DIVIDER; // All of the new settings

        if (BURST_ON_NEXT > BURST_TOTAL_RELOAD) //-- Check if in limit
        {
            BURST_ON_NEXT = BURST_TOTAL_RELOAD; //-- else set to the limit
        }
    }
}

//**********************************************************
void MyInterruptRoutine(void) {
    //-- Timer 0 Overflow -- Used for zero cross detection
    //if(T0IF)
    {
        //  T0IF=0;
        //  TMR0=BURST_TIMER_RELOAD;          //-- Reload timer for Net Wave Trigger

        if (BURST_ON_COUNT > 0) //-- Check if need to Enable Output
        {
            BURST_ON_COUNT--; //-- If so Decrement count by one
            T_OUTPUT_PIN = 0; //-- Ensure TRIS set to OUTPUT
            OUTPUT_PIN = OUTPUT_ON; //-- and enable the output
        } else {
            OUTPUT_PIN = OUTPUT_OFF; //-- Else disable the output
        }

        BURST_TOTAL_COUNT--; //-- Decrement the total count
        if (BURST_TOTAL_COUNT <= 0) //-- Check if time is up
        {
            BURST_TOTAL_COUNT = BURST_TOTAL_RELOAD; //-- if so relead the total count
            BURST_ON_COUNT = BURST_ON_NEXT; //-- and reload the NEXT ON count
        }

    }
    // GIE=1;
}

