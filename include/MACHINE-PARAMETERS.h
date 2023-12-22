#include <Arduino.h>

// <------Motor parameters----->
// linear motor
#define STEPS_PER_MM 4.44444444444
#define LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN 1440
#define LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN 10000000

// rotation motor
#define STEPS_PER_REVOLUTION 4.44444444444
#define ROTATION_MOTOR_MAX_SPEED 1440 // degrees per minute
#define ROTATION_MOTOR_MAX_ACCELERATION 10000000 // degrees per minute per minute

// <------limit switch parameters------->
// define whether the limit switches are NPN or PNP
#define LIMIT_SWITCH_TYPE_NPN 0
#define LIMIT_SWITCH_TYPE_PNP 1

// define the limit switch type
#define LIMIT_SWITCH_TYPE NPN

// define the input mode and triggered state based on the limit switch type
// if nothing defined here then you probably did something wrong when defining limit_switch_type
#if LIMIT_SWITCH_TYPE == LIMIT_SWITCH_TYPE_NPN
    #define LIMIT_SWITCH_INPUT_MODE INPUT_PULLDOWN 
    #define LIMIT_SWITCH_TRIGGERED_STATE RISING
#elif LIMIT_SWITCH_TYPE == LIMIT_SWITCH_TYPE_PNP
    #define LIMIT_SWITCH_INPUT_MODE INPUT_PULLUP
    #define LIMIT_SWITCH_TRIGGERED_STATE FALLING
#endif

// TODO: limit switch positions
#define ENDSTOP_1_POSITION -1000
#define ENDSTOP_2_POSITION 1000
#define HOME_SWITCH_POSITION 0


// <------LCD parameters---------->
// <------button parameters------->
// <------relay parameters-------->
// <------other parameters-------->
#define SERIAL_BAUD_RATE 115200