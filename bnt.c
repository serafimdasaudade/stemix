#include "main.h"

void bnt_init(void) {
    UINT8 i;

    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 1;
    TRISAbits.TRISA5 = 1;
    
    for(i=0;i<=9;i++){
        bnt.key_is_press_flag[i] = 0;
        bnt.key_prev_press_flag[i] = 0;
    }
}

UINT8 wait_for_press_bnt(void) {
    const UINT delay = 0;
    const UINT delay_1 = 0;
    PIN1 = 0;
    PIN2 = 1;
    PIN9 = 1;
    PIN10 = 1;
//    __delay_ms(delay_1);

    if (!PIN8) {
//        __delay_ms(delay);
        if(!PIN8){
            bnt.key_is_press_flag[RPM_SET] = 1;
            return RPM_SET;
        }
    }
    bnt.key_is_press_flag[RPM_SET] = 0;

    if (!PIN3) {
//        __delay_ms(delay);
        if(!PIN3){
            bnt.key_is_press_flag[TIME] = 1;
            return TIME;
        }
    }
    bnt.key_is_press_flag[TIME] = 0;
    PIN1 = 1;
    PIN2 = 0;
    PIN9 = 1;
    PIN10 = 1;
//    __delay_ms(delay_1);
    if (!PIN8) {
//        __delay_ms(delay);
        if(!PIN8){
            bnt.key_is_press_flag[T_PLUS] ++;
            return T_PLUS;
        }
    }
    bnt.key_is_press_flag[T_PLUS] = 0;
    if (!PIN3) {
//        __delay_ms(delay);
        if(!PIN3){
            bnt.key_is_press_flag[T_MENOS] ++;
            return T_MENOS;
        }
    }
    bnt.key_is_press_flag[T_MENOS] = 0;
    PIN1 = 1;
    PIN2 = 1;
    PIN9 = 0;
    PIN10 = 1;
//    __delay_ms(delay_1);
    if (!PIN8) {
//        __delay_ms(delay);
        if(!PIN8){
            bnt.key_is_press_flag[RPM_PLUS] ++;
            return RPM_PLUS;
        }
    }
    bnt.key_is_press_flag[RPM_PLUS] = 0;
    if (!PIN3) {
//        __delay_ms(delay);
        if(!PIN3){
            bnt.key_is_press_flag[RPM_MENOS] ++;
            return RPM_MENOS;
        }
    }
    bnt.key_is_press_flag[RPM_MENOS] = 0;
    PIN1 = 1;
    PIN2 = 1;
    PIN9 = 1;
    PIN10 = 0;
//    __delay_ms(delay_1);
    if (!PIN8) {
//        __delay_ms(delay);
        if(!PIN8){
            bnt.key_is_press_flag[TIME_PLUS] ++;
            return TIME_PLUS;
        }
    }
    bnt.key_is_press_flag[TIME_PLUS] = 0;
    if (!PIN3) {
//        __delay_ms(delay);
        if(!PIN3){
            bnt.key_is_press_flag[TIME_MENOS] ++;
            return TIME_MENOS;
        }
    }
    bnt.key_is_press_flag[TIME_MENOS] = 0;

    bnt.key_press_timer = 0;
    
    return 0;
}

UINT8 bnt_control(void) {
    UINT i;
    //LCDdisable();
    //bnt_init();

    key = wait_for_press_bnt();
    if (key) {

        if(bnt.key_press_timer < 2)
        {
            for(i=1;i<=9;i++)
            {
                if(bnt.key_is_press_flag[i] && !bnt.key_prev_press_flag[i])
                {
                    bnt.key_prev_press_flag[i] = 1;
                    goto test_key;
                }else if(bnt.key_is_press_flag[i] > 3)
                {
                    bnt.key_is_press_flag[i] = 1;
                    goto test_key;
                }
            }
            return 0;
        }
        
test_key:

/*
        if(bnt.key_press_timer%2 == 0 && bnt.key_press_timer <= 15 )
            NOP();
        else if(bnt.key_press_timer > 15)
            NOP();
        else
            return 0;
*/
        switch (key) {
            case TIME_PLUS:
                if (program.sec < SEC_MAX) {
                    program.sec = program.sec + TIME_STEP;
                } else {
                    program.sec = 0;
                    if (program.minute < MIN_MAX)
                        program.minute++;
                    else
                        program.minute = 0;
                }
                break;
            case TIME_MENOS:
                if (program.sec > 0) {
                    program.sec = program.sec - TIME_STEP;
                } else {
                    program.sec = SEC_MAX;
                    if (program.minute > 0)
                        program.minute--;
                    else
                        program.minute = MIN_MAX;
                }
                break;
            case RPM_PLUS:
                //dc_motor_control();
                if (program.rpm < RPM_MAX)
                    program.rpm++;
                else
                    program.rpm = 40;
                break;
            case T_PLUS:
                if (program.temp_int < TEMP_MAX)
                        program.temp_int++;
                    else
                        program.temp_int = TEMP_MAX;
                break;

            case RPM_SET:
                //program.rpm = rpm;
                //myInput = program.rpm;
                //start motor a encoder
                if(dcmotor.status == ON)
                    dcmotor.status = OFF;
                else
                    dcmotor.status = ON;
                
                break;
            case RPM_MENOS:
                if (program.rpm > 40)
                    program.rpm--;
                else
                    program.rpm = RPM_MAX;
                break;
            case T_MENOS:
                if (program.temp_int > TEMP_MIN)
                    program.temp_int--;
                else
                   program.temp_int = TEMP_MIN;
                break;
            case TIME:
                clock.hour = 0;    //program.hour;
                clock.minute = program.minute;
                clock.second = program.sec;
                switch (program.status) {
                    case IDLE:
                        program.status = START;
                        //start program control (heater, DC motor)
                        dcmotor.status = ON;
                        break;
                    case START:
                        program.status = STOP;
                        //stop program control
                        dcmotor.status = OFF;
                        break;
                    case STOP:
                        program.status = START;
                        //start program control
                        dcmotor.status = ON;
                        break;
                }
                break;

            default:
                //key_is_press_flag = 0;
                break;
        }
        write_eep(EE_DEFAULT_ADD,DEFAULT_ADD);
        write_eep(EE_TIME_SEC,program.sec);
        write_eep(EE_TIME_MINUTE,program.minute);
        write_eep(EE_RPM,program.rpm);
        write_eep(EE_TEMP_MSB,(program.temp_int >> 8));
        write_eep(EE_TEMP_LSB,program.temp_int);
    } else {
        NOP();
        //__delay_ms(50);
        for(i=1;i<=9;i++)
        {
            bnt.key_prev_press_flag[i] = 0;
        }
    }
    return 0;
}

