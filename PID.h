/**********************************************************************************************
 *
 * File:   Port From Arduino PID
 * by Roberto Fonte <robertofonte@hotmail.com>
 *
 *
 *
 * Arduino PID Library - Version 1.0.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#define LIBRARY_VERSION	1.0.0

//Constants used in some of the functions below
#define AUTOMATIC	1
#define MANUAL	0
#define DIRECT  0
#define REVERSE  1

#define bool        BOOL
#define false       0
#define true        1

//commonly used functions **************************************************************************
PID_INIT(INT, INT, INT, // * constructor.  links the PID to the Input, Output, and
        INT, INT, INT, INT); //   Setpoint.  Initial tuning parameters are also set here

void SetMode(int Mode); // * sets PID to either Manual (0) or Auto (non-0)

bool Compute(); // * performs the PID calculation.  it should be
//   called every time loop() cycles. ON/OFF and
//   calculation frequency can be set using SetMode
//   SetSampleTime respectively

void SetOutputLimits(INT, INT); //clamps the output to a specific range. 0-255 by default, but
//it's likely the user will want to change this depending on
//the application



//available but not commonly used functions ********************************************************
void SetTunings(INT, INT, // * While most users will set the tunings once in the
        INT); //   constructor, this function gives the user the option
//   of changing tunings during runtime for Adaptive control
void SetControllerDirection(int); // * Sets the Direction, or "Action" of the controller. DIRECT
//   means the output will increase when error is positive. REVERSE
//   means the opposite.  it's very unlikely that this will be needed
//   once it is set in the constructor.
void SetSampleTime(int); // * sets the frequency, in Milliseconds, with which
//   the PID calculation is performed.  default is 100


//Display functions ****************************************************************
INT GetKp(); // These functions query the pid for interal values.
INT GetKi(); //  they were created mainly for the pid front-end,
INT GetKd(); // where it's important to know what is actually
int GetMode(); //  inside the PID.
int GetDirection(); //
void Initialize();

INT kp; // * (P)roportional Tuning Parameter
INT ki; // * (I)ntegral Tuning Parameter
INT kd; // * (D)erivative Tuning Parameter

int controllerDirection;

INT myInput; // * Pointers to the Input, Output, and Setpoint variables
INT myOutput; //   This creates a hard link between the variables and the
INT mySetpoint; //   PID, freeing the user from having to constantly tell us
//   what these values are.  with pointers we'll just know.

//unsigned long lastTime;

INT ITerm, lastInput;

unsigned long SampleTime;
INT outMin, outMax;
bool inAuto;
