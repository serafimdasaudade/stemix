#include "main.h"

/*
 * PWM Register Values
 * Oscillator Frequency Fosc = 4000000
 * Clock Frequency Fclk = 1000000
 * PWM Freq = 1000 desired...actual: 1000
 * Prescaler Value = 4
 * PR2 = 249
 * Maximum duty value = 1000
 * Requested Duty Value = 500
 *
 * Code Provided AS IS....Use at your own risk!
*/
/*
/// be sure to set PWM port as output
T2CON = 0b00000101; // prescaler + turn on TMR2;
PR2 = 0b11111001;
CCPR1L = 0b01111101;  // set duty MSB
CCP1CON = 0b00001100; // duty lowest bits + PWM mode
*/


/*
 * PWM Register Values
 * Oscillator Frequency Fosc = 4000000
 * Clock Frequency Fclk = 1000000
 * PWM Freq = 1000 desired...actual: 992
 * Prescaler Value = 16
 * PR2 = 62
 * Maximum duty value = 250
 * Requested Duty Value = 1250
 *
 * Code Provided AS IS....Use at your own risk!
*/
/*
// be sure to set PWM port as output
T2CON = 0b00000111; // prescaler + turn on TMR2;
PR2 = 0b00111110;
CCPR1L = 0b00111000;  // set duty MSB
CCP1CON = 0b00101100; // duty lowest bits + PWM mode
*/


void set_duty(unsigned int duty);

void init_dc_motor_encoder(void)
{
    unsigned int duty=0;
    TRISBbits.TRISB2 = 1;
    TRISC2=0;

    // be sure to set PWM port as output
    T2CON = 0b00000101; // prescaler + turn on TMR2;
    PR2 = 0b11111001;
    CCPR1L = 0b01111101;  // set duty MSB
    CCP1CON = 0b00001100; // duty lowest bits + PWM mode

    set_duty(duty);
    TMR2ON = 1;
}

void set_duty(unsigned int duty)
{
    CCPR1L=duty>>2;
    CCP1CON&=0xcf;
    CCP1CON|=(0x30&(duty<<4));
}
void dc_motor_control(void)
{
    if(PORTBbits.RB2)
    {
        dcmotor.encoder_counter++;
        //rpm = dcmotor.encoder_counter;
    }
    switch(dcmotor.status)
    {
        case ON:
            //PORTCbits.RC2 = 1;
            //TRISBbits.TRISB2 = 1;
            rpm =program.rpm;
            set_duty(program.rpm*20);
            //set_duty(0);
            break;
        case OFF:
            //PORTCbits.RC2 = 0;
            //TRISBbits.TRISB2 = 1;
            rpm = 0;
            set_duty(0);
            break;
    }
}
