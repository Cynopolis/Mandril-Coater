#pragma once
#include "I2CPin.h"

struct StepperMotorConfiguration{
    const uint8_t stepPin;
    const I2CPin directionPin;
    const I2CPin enablePin;
    const float stepsPerUnit;
    const float maxSpeed;
    const float acceleration;
    const bool invertDirection = false;
    bool (*callBackHandler)(uint8_t pin, uint8_t value);

    StepperMotorConfiguration(
        uint8_t stepPin, 
        I2CPin &directionPin, 
        I2CPin &enablePin, 
        float stepsPerUnit, 
        float maxSpeed, 
        float acceleration, 
        bool invertDirection,
        bool (*callBackHandler)(uint8_t pin, uint8_t value)
        ) : 
        stepPin(stepPin), 
        directionPin(directionPin), 
        enablePin(enablePin), 
        stepsPerUnit(stepsPerUnit), 
        maxSpeed(maxSpeed), 
        acceleration(acceleration),
        invertDirection(invertDirection),
        callBackHandler(callBackHandler){}
};