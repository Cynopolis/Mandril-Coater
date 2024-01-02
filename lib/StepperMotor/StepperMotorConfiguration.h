#pragma once
#include "I2CPin.h"

struct StepperMotorConfiguration{
    I2CPin stepPin;
    I2CPin directionPin;
    I2CPin enablePin;
    float stepsPerUnit;
    float maxSpeed;
    float acceleration;

    StepperMotorConfiguration(I2CPin &stepPin, I2CPin &directionPin, I2CPin &enablePin, float stepsPerUnit, float maxSpeed, float acceleration) : 
        stepPin(stepPin), 
        directionPin(directionPin), 
        enablePin(enablePin), 
        stepsPerUnit(stepsPerUnit), 
        maxSpeed(maxSpeed), 
        acceleration(acceleration){}
};