/*
 * PIN 13 RC2 - pin 8 con 1 - Gate MOSFET do motor
 * PIN 23 RB2 - pin 4 con 1 - MOTOR ENCODER   //PIN23
 * 15 pulse 1 RPM
 * 15 ---- 60
 * x  ---- y
 * x -> pulsos
 * y -> rpm
 * 15*y = x*60 <=> x=(15*y)/60
 */

#define ON      1
#define OFF     0
typedef struct
{
    UINT8 status;
    UINT16 encoder_counter;
}mMotor;

mMotor dcmotor;

void dc_motor_control(void);
void init_dc_motor_encoder(void);