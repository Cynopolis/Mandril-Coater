/**
 * @file StepperMotor.cpp
 * @brief This file contains the StepperMotor class
 * @details This file contains the StepperMotor class which is used to control a stepper motor
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "StepperMotor.h"
#include <Arduino.h>

void StepperMotor::Init(){
    this->i2cPort->write(this->configuration.enablePin.number, HIGH);
    this->i2cPort->write(this->configuration.directionPin.number, LOW);
    this->i2cPort->write(this->configuration.stepPin.number, HIGH);

    this->timeOfLastStep = micros();
}

void StepperMotor::SetSpeed(float speed) {
    speed = abs(speed);
    // convert units per minute to steps per microsecond
    this->period = 60 * 1000000 / (speed * this->configuration.stepsPerUnit);
}

void StepperMotor::updateDirectionPin(){
    if(this->targetSteps > this->currentSteps){
        this->direction = 1;
        this->i2cPort->write(this->configuration.directionPin.number, HIGH);
    } else {
        this->direction = -1;
        this->i2cPort->write(this->configuration.directionPin.number, LOW);
    }

}

void StepperMotor::SetTargetPosition(int32_t position) {
    this->targetSteps = position * this->configuration.stepsPerUnit;
    this->updateDirectionPin();
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    this->currentSteps = position * this->configuration.stepsPerUnit;
    this->updateDirectionPin();
}

void StepperMotor::Update() {
    // if we are within 5 steps of the target position, stop
    if(abs(this->targetSteps - this->currentSteps) < 5){
        return;
    }

    uint32_t timeSinceLastStep = micros() - this->timeOfLastStep;
    // do one step if it is time
    if(timeSinceLastStep >= this->period){
        this->currentSteps += this->direction;
        this->i2cPort->write(this->configuration.stepPin.number, LOW);
        this->i2cPort->write(this->configuration.stepPin.number, HIGH);
        this->timeOfLastStep = micros();
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->write(this->configuration.enablePin.number, enabled);
}


int32_t StepperMotor::GetCurrentPosition(){
    return this->currentSteps / this->configuration.stepsPerUnit;
}

int32_t StepperMotor::GetTargetPosition(){
    return this->targetSteps / this->configuration.stepsPerUnit;
}

uint32_t StepperMotor::GetSpeed(){
    if(this->period == 0){
        return 0;
    }
    float floatPeriod = (float)period;
    float floatStepsPerUnit = (float)configuration.stepsPerUnit;
    return (uint32_t) (60.0f * 1000000.0f / (floatPeriod * floatStepsPerUnit));
}