#include "main.h"

/*
void OpenPWM1 ( char period);
void SetDCPWM1 ( unsigned int duty_cycle);
void SetOutputPWM1 ( unsigned char output_config,
                     unsigned char pwm_mode);
void ClosePWM1 (void);

 */
void init_stepper(void) {
    /*
     * PWM registers configuration
     * Fosc = 8000000 Hz
     * Fpwm = 1000.00 Hz (Requested : 1000 Hz)
     * Duty Cycle = 50 %
     * Resolution is 10 bits
     * Prescaler is 16
     */
    CCP2CON = 0b00001100;                           //PWM mode single output
    //CCPTMRSbits.C2TSEL = 0b00;                      //select timer2 as PWM source
    PR2 = 0b01111100;
    T2CON = 0b00000111;
    CCPR1L = 0b00111110;
    CCP1CON = 0b00011100;
}

