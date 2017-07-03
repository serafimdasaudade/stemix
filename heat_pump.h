/*
 * PIN 25 RB4 - pin 2 con 1 e a entrada da ponte H de mosfet
 * PIN 24 RB3 - PIN 3 CON 1 e a outra entrada
 */
#define HEAT    1
#define COOL    0
typedef struct{
    UINT8 status;
}sheatpump;

sheatpump heatpump;

void heat_pump_control(void);
void heat_pump_cool(void);