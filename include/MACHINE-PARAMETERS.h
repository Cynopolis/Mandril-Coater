#ifndef MACHINE_PARAMETERS_H
#define MACHINE_PARAMETERS_H
#include "PINOUT.h"
#include "StepperMotorConfiguration.h"
// <------Motor parameters----->

// this callback handler is used by the stepper motor to control the direction and enable pins
static bool DefaultPinCallbackHandler(uint8_t pin, uint8_t state){
    // The pin has 0b10000000 or'd with it to indicate that it is an external pin
    // We need to remove that flag to get the actual pin number
    uint8_t pinMasked = pin & ~PIN_EXTERNAL_FLAG;

    bool pinStatus = i2c_output_port_1.read(pinMasked);
    i2c_output_port_1.write(pinMasked, state);
    return pinStatus;
}

// linear motor
#define STEPS_PER_MM 5 // TODO: just an estimate
// 825 Hz is the maximum frequency the ESP32 can generate with the MUX
#define LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN 10000*STEPS_PER_MM // Max speed is 10000 mm per minute

// currently acceleration is not used, but it could potentially be added in the future
#define LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN 10000 // mm per minute per minute
#define IS_LINEAR_MOTOR_INVERTED true
// this is not currently used, but it could be used to count the number of pulses during a move
#define LINEAR_MOTOR_PULSE_COUNT_UNIT_NUMBER 0

StepperMotorConfiguration LINEAR_MOTOR_CONFIGURATION(
    LINEAR_MOTOR_STEP_PIN_NUMBER,
    LINEAR_MOTOR_DIRECTION_PIN,
    LINEAR_MOTOR_ENABLE_PIN,
    LINEAR_MOTOR_PULSE_COUNT_UNIT_NUMBER,
    STEPS_PER_MM,
    LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN,
    LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN,
    IS_LINEAR_MOTOR_INVERTED,
    DefaultPinCallbackHandler
);

// rotation motor
#define STEPS_PER_DEGREE 400/360 // 400 steps per revolution divided by 360 degrees
// 825 Hz is the maximum frequency the ESP32 can generate with the MUX
#define ROTATION_MOTOR_MAX_SPEED 60*360*STEPS_PER_DEGREE // Max speed is 60 RPM
#define ROTATION_MOTOR_MAX_ACCELERATION 10000*360 // rotations per minute per minute
#define IS_ROTATION_MOTOR_INVERTED false
// this is not currently used, but it could be used to count the number of pulses during a move
#define ROTATION_MOTOR_PULSE_COUNT_UNIT_NUMBER 1

StepperMotorConfiguration ROTATION_MOTOR_CONFIGURATION(
    ROTATION_MOTOR_STEP_PIN_NUMBER,
    ROTATION_MOTOR_DIRECTION_PIN,
    ROTATION_MOTOR_ENABLE_PIN,
    ROTATION_MOTOR_PULSE_COUNT_UNIT_NUMBER,
    STEPS_PER_DEGREE,
    ROTATION_MOTOR_MAX_SPEED,
    ROTATION_MOTOR_MAX_ACCELERATION,
    IS_ROTATION_MOTOR_INVERTED,
    DefaultPinCallbackHandler
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

// <------other parameters-------->
// serial definitions
#define SERIAL_BAUD_RATE 115200

// IP Definitions
#define STATIC_IP 10, 0, 0, 2
#define GATEWAY_IP 10, 0, 0, 1
#define SUBNET_MASK 255, 255, 255, 0

#endif // MACHINE_PARAMETERS_H