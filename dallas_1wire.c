#include "main.h"

//unsigned char i = 0;
//float Temerature = 0;
void ds1820_init(void);

void read_dallas() {
    unsigned char Temp[9];
    unsigned int temp_f;
    float temp_c;
}

#define DS1820     RB0
#define DS1820_1     RB1

#define DS1820_DIR  TRISB0
#define DS1820_1DIR  TRISB1


unsigned char ds_data[9];

#define input       1

#define output      0

//bit read_ds1822(void);

bit ds1820_reset(void);
void tck_send(unsigned char tck_byte);
unsigned char tck_read(void);

void init_ds1820(void) {
    RB0 = 1;
    TRISB0 = 1; // pull up
    RB1 = 1;
    TRISB1 = 1; // pull up
}

char read_ds1820(void) {
    int aux;
    float out;
    unsigned char loop;

    INT16 maxpv = 350;
    INT16 minpv = 52;
    INT16 maxsp = 210;
    INT16 minsp = 40;

    if (ds1820_reset() == 0)return 0;

    tck_send(0xCC);

    tck_send(0x44); //start conversion

    //__delay_ms(1000);
    __delay_us(100);

    if (ds1820_reset() == 0)return 0;

    tck_send(0xCC);

    tck_send(0xBE); //read scratchpad

    loop = 9;

    do {

        ds_data[--loop] = tck_read();

    } while (loop != 0);

    aux = (ds_data[7] << 8) | ds_data[8];
    if (aux <= 1000 && aux >= 0)
        temp_int = aux;
    /*
            if(temp_int >= offset)
                temp_int = temp_int -offset;
     */
    /*
     * Vou estimar o valor que esta na cama a partir do que estou a medir no sensor
     */

    out = ((-maxpv) * (maxsp - minsp) + temp_int * (maxsp - minsp) + maxsp * (maxpv - minpv)) / (maxpv - minpv);
    temp_int = out;
    
    //program.rpm=out;
    /*
            LCDGotoXY(0, 1); //set
            sprintf(str, "%03dC",(temp_1int));
            //sprintf(str, "%X%X%X%X%X%X%X%X%X",Temp[0],Temp[1],Temp[2],Temp[3],Temp[4],Temp[5],Temp[6],Temp[7],Temp[8]);
            //sprintf(str, "%02X%02X%02X%02X%02X%02X%02X%02X", ds_data[0], ds_data[1], ds_data[2], ds_data[3], ds_data[4], ds_data[5], ds_data[6], ds_data[7]);
            LCDWriteString(str);
     */
    return 1;
}

bit ds1820_reset(void) {

    unsigned char detect;

    if (DS1820 == 0) return 0;

    __delay_us(100);

    DS1820 = 0;

    DS1820_DIR = output;

    __delay_us(480);

    DS1820_DIR = input;

    __delay_us(70);

    detect = 0;

    if (DS1820 == 0) detect = 1;

    __delay_us(410);

    if (detect == 1)return 1;

    else return 0;

}

unsigned char tck_read(void) {

    unsigned char byte = 0, loop = 8;

    do {

        DS1820 = 0;

        DS1820_DIR = output;

        __delay_us(6);

        DS1820_DIR = input;

        __delay_us(8);

        byte >>= 1;

        byte |= 0x80;

        if (DS1820 == 0) byte &= 0x7F;

        __delay_us(55);

    } while (--loop != 0);

    return byte;

}

void tck_send(unsigned char byte) {

    unsigned char loop = 8;

    do {

        DS1820 = 0;

        DS1820_DIR = output;

        __delay_us(6);

        if (byte & 0x01) DS1820_DIR = input;

        __delay_us(55);

        byte >>= 1;

        DS1820_DIR = input;

        __delay_us(10);

    } while (--loop != 0);

}

bit ds1820_1reset(void);
void tck_1send(unsigned char tck_byte);
unsigned char tck_1read(void);

char read_1ds1820(void) {
    int aux;
    UINT32 out;
    unsigned char loop;

    UINT16 maxpv = 330;
    UINT16 minpv = 60; //98;
    UINT16 maxsp = 210;
    UINT16 minsp = 40;

    if (ds1820_1reset() == 0)return 0;

    tck_1send(0xCC);

    tck_1send(0x44); //start conversion

    //__delay_ms(1000);
    __delay_us(100);

    if (ds1820_1reset() == 0)return 0;

    tck_1send(0xCC);

    tck_1send(0xBE); //read scratchpad

    loop = 9;

    do {

        ds_data[--loop] = tck_1read();

    } while (loop != 0);

    aux = (ds_data[7] << 8) | ds_data[8];
    if (aux <= 1000 && aux >= 0)
        temp_int = aux;
    /*
            if(temp_int >= offset)
                temp_int = temp_int -offset;
     */
    /*
     * Vou estimar o valor que esta na cama a partir do que estou a medir no sensor
     */
    out = (-maxpv * (maxsp - minsp) + temp_int * (maxsp - minsp) + maxsp * (maxpv - minpv)) / (maxpv - minpv);
    //program.temp_int=aux;
    temp_int = out;
    //program.rpm=out;
    /*
            LCDGotoXY(0, 1); //set
            sprintf(str, "%03dC",(temp_1int));
            //sprintf(str, "%X%X%X%X%X%X%X%X%X",Temp[0],Temp[1],Temp[2],Temp[3],Temp[4],Temp[5],Temp[6],Temp[7],Temp[8]);
            //sprintf(str, "%02X%02X%02X%02X%02X%02X%02X%02X", ds_data[0], ds_data[1], ds_data[2], ds_data[3], ds_data[4], ds_data[5], ds_data[6], ds_data[7]);
            LCDWriteString(str);
     */
    return 1;

}

bit ds1820_1reset(void) {

    unsigned char detect;

    if (DS1820_1 == 0) return 0;

    __delay_us(100);

    DS1820_1 = 0;

    DS1820_1DIR = output;

    __delay_us(480);

    DS1820_1DIR = input;

    __delay_us(70);

    detect = 0;

    if (DS1820_1 == 0) detect = 1;

    __delay_us(410);

    if (detect == 1)return 1;

    else return 0;

}

unsigned char tck_1read(void) {

    unsigned char byte = 0, loop = 8;

    do {

        DS1820_1 = 0;

        DS1820_1DIR = output;

        __delay_us(6);

        DS1820_1DIR = input;

        __delay_us(8);

        byte >>= 1;

        byte |= 0x80;

        if (DS1820_1 == 0) byte &= 0x7F;

        __delay_us(55);

    } while (--loop != 0);

    return byte;

}

void tck_1send(unsigned char byte) {

    unsigned char loop = 8;

    do {

        DS1820_1 = 0;

        DS1820_1DIR = output;

        __delay_us(6);

        if (byte & 0x01) DS1820_1DIR = input;

        __delay_us(55);

        byte >>= 1;

        DS1820_1DIR = input;

        __delay_us(10);

    } while (--loop != 0);

}
