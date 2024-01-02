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

StepperMotorConfiguration LINEAR_MOTOR_CONFIGURATION(
    LINEAR_MOTOR_STEP_PIN,
    LINEAR_MOTOR_DIRECTION_PIN,
    LINEAR_MOTOR_ENABLE_PIN,
    STEPS_PER_MM,
    LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN,
    LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN
);

// rotation motor
#define STEPS_PER_REVOLUTION 200
// 825 Hz is the maximum frequency the ESP32 can generate with the MUX
#define ROTATION_MOTOR_MAX_SPEED 825*60*STEPS_PER_REVOLUTION // degrees per minute
#define ROTATION_MOTOR_MAX_ACCELERATION 10000000 // degrees per minute per minute

StepperMotorConfiguration ROTATION_MOTOR_CONFIGURATION(
    ROTATION_MOTOR_STEP_PIN,
    ROTATION_MOTOR_DIRECTION_PIN,
    ROTATION_MOTOR_ENABLE_PIN,
    STEPS_PER_REVOLUTION,
    ROTATION_MOTOR_MAX_SPEED,
    ROTATION_MOTOR_MAX_ACCELERATION
);

// <------LCD parameters---------->
// <------button parameters------->
// <------relay parameters-------->
// <------other parameters-------->
#define SERIAL_BAUD_RATE 115200

#endif // MACHINE_PARAMETERS_H