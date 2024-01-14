#pragma once
#include "I2CPin.h"

struct StepperMotorConfiguration{
    const I2CPin stepPin;
    const I2CPin directionPin;
    const I2CPin enablePin;
    const float stepsPerUnit;
    const float maxSpeed;
    const float acceleration;
    const bool invertDirection = false;

    StepperMotorConfiguration(I2CPin &stepPin, I2CPin &directionPin, I2CPin &enablePin, float stepsPerUnit, float maxSpeed, float acceleration, bool invertDirection) : 
        stepPin(stepPin), 
        directionPin(directionPin), 
        enablePin(enablePin), 
        stepsPerUnit(stepsPerUnit), 
        maxSpeed(maxSpeed), 
        acceleration(acceleration),
        invertDirection(invertDirection){}
};