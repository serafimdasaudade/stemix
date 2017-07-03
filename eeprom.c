#include "main.h"

UINT8 read_eep(UINT badd) {
    char GIE_BIT_VAL = 0;
    //save geral interrupt state
    GIE_BIT_VAL = INTCONbits.GIE;
    INTCONbits.GIE = 0;        ///disable geral interrupt
    EEADR = (badd & 0x07f);
//    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    NOP(); //Nop may be required for latency at high frequencies
    NOP(); //Nop may be required for latency at high frequencies
    ///geral inerrupt last state
    INTCONbits.GIE=GIE_BIT_VAL;
    return ( EEDATA); // return with read byte
}

void write_eep(UINT badd, UINT8 bdat)
{
    char GIE_BIT_VAL = 0;

    GIE_BIT_VAL = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    EEADR = (badd & 0x0ff);
    EEDATA = bdat;
    EECON1bits.EEPGD = 0;
//    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while (EECON1bits.WR); //Wait till the write completion
    EECON1bits.WREN = 0;

    INTCONbits.GIE = GIE_BIT_VAL;
}