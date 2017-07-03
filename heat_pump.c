#include "main.h"

void heat_pump_off(void)
{
    //HEAT OFF
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB4 = 0;
    PORTBbits.RB4 = 1;
    PORTBbits.RB3 = 1;
    heatpump.status = OFF;
/*
            LCDGotoXY(0, 1); //actual
        sprintf(str, " STOP %04d %3.1f", rpm, (float)(temp_int/10));
        //sprintf(str, " STOP %04d %d", rpm, temp_int);
        LCDWriteString(str);
*/

}

void heat_pump_heat(void)
{
    //HEAT OFF
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB4 = 0;
    PORTBbits.RB4 = 0;
    PORTBbits.RB3 = 0;
    heatpump.status = HEAT;
/*
            LCDGotoXY(0, 1); //actual
        sprintf(str, "calor %d", (temp_int));
        //sprintf(str, " STOP %04d %d", rpm, temp_int);
        LCDWriteString(str);
*/
}

void heat_pump_cool(void)
{
    //HEAT OFF
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB4 = 0;
    PORTBbits.RB4 = 1;
    PORTBbits.RB3 = 0;
    heatpump.status = COOL;

/*
            LCDGotoXY(0, 1); //actual
        sprintf(str, "frio %d", (temp_int));
        //sprintf(str, " STOP %04d %d", rpm, temp_int);
        LCDWriteString(str);
*/
}
void heat_pump_control(void)
{
    int erro;
    //HEAT OFF
    //heat_pump_cool();

    //if (temp_int == program.temp_int)
    //    heat_pump_off();
            //real       //set point
    temp_int = temp_int;
    erro = program.temp_int - temp_int;

    if(erro > 20)
    {
        heat_pump_heat();
    }else
    {
    if (temp_int < program.temp_int-10)
    {
        if(heatpump.status == COOL)
            heat_pump_off();
        else
            heat_pump_heat();
    }
    else if(temp_int > program.temp_int)
        heat_pump_cool();
    else
    {
/*
        if(heatpump.status == COOL)
            heat_pump_cool();
        else
*/
            heat_pump_off();
    }
    }
/*
    switch (program.status) {
        case START:

            break;
        default:
            heat_pump_off();
            break;
    }
*/
}