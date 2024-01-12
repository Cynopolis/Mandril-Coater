#ifndef MACHINE_PARAMETERS_H
#define MACHINE_PARAMETERS_H
#include "PINOUT.h"
#include "StepperMotorConfiguration.h"
// <------Motor parameters----->
// linear motor
#define STEPS_PER_MM 5 // TODO: just an estimate
// 825 Hz is the maximum frequency the ESP32 can generate with the MUX
#define LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN 825*60*STEPS_PER_MM // mm per minute
#define LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN 10000000 // mm per minute per minute
#define IS_LINEAR_MOTOR_INVERTED true

StepperMotorConfiguration LINEAR_MOTOR_CONFIGURATION(
    LINEAR_MOTOR_STEP_PIN,
    LINEAR_MOTOR_DIRECTION_PIN,
    LINEAR_MOTOR_ENABLE_PIN,
    STEPS_PER_MM,
    LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN,
    LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN,
    IS_LINEAR_MOTOR_INVERTED
);

// rotation motor
#define STEPS_PER_REVOLUTION 200
// 825 Hz is the maximum frequency the ESP32 can generate with the MUX
#define ROTATION_MOTOR_MAX_SPEED 825*60*STEPS_PER_REVOLUTION // degrees per minute
#define ROTATION_MOTOR_MAX_ACCELERATION 10000000 // degrees per minute per minute
#define IS_ROTATION_MOTOR_INVERTED false

StepperMotorConfiguration ROTATION_MOTOR_CONFIGURATION(
    ROTATION_MOTOR_STEP_PIN,
    ROTATION_MOTOR_DIRECTION_PIN,
    ROTATION_MOTOR_ENABLE_PIN,
    STEPS_PER_REVOLUTION,
    ROTATION_MOTOR_MAX_SPEED,
    ROTATION_MOTOR_MAX_ACCELERATION,
    IS_ROTATION_MOTOR_INVERTED
);

// <------Endstop parameters------->
// define whether the limit switches are NPN or PNP
#define LIMIT_SWITCH_TYPE_NPN 0
#define LIMIT_SWITCH_TYPE_PNP 1

// define the limit switch type
#define LIMIT_SWITCH_TYPE NPN

// define the input mode and triggered state based on the limit switch type
// if nothing defined here then you probably did something wrong when defining limit_switch_type
#if LIMIT_SWITCH_TYPE == LIMIT_SWITCH_TYPE_NPN
    #define LIMIT_SWITCH_TRIGGERED_STATE LOW
#elif LIMIT_SWITCH_TYPE == LIMIT_SWITCH_TYPE_PNP
    #define LIMIT_SWITCH_TRIGGERED_STATE HIGH
#endif

// TODO: limit switch positions
#define ENDSTOP_1_POSITION -1000
#define ENDSTOP_2_POSITION 1000
#define HOME_SWITCH_POSITION 0


// <------LCD parameters---------->
// <------button parameters------->
// <------relay parameters-------->
// <------other parameters-------->
// serial definitions
#define SERIAL_BAUD_RATE 115200

// IP Definitions
#define STATIC_IP 10, 0, 0, 2
#define GATEWAY_IP 10, 0, 0, 1
#define SUBNET_MASK 255, 255, 255, 0

#endif // MACHINE_PARAMETERS_H