/**
 * @file StepperMotor.cpp
 * @brief This file contains the StepperMotor class
 * @details This file contains the StepperMotor class which is used to control a stepper motor
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "StepperMotor.h"
#include <Arduino.h>
#include <freertos/semphr.h>

void StepperMotor::Init(){
    this->i2cPort->write(this->configuration->enablePin.number, HIGH);
    this->i2cPort->write(this->configuration->directionPin.number, LOW);
    pinMode(this->configuration->stepPin, OUTPUT);

    this->updatesSinceLastStep = micros();
}

void StepperMotor::SetSpeed(float speed) {
    speed = static_cast<uint32_t>(abs(speed));
    // convert units per minute to steps per microsecond
    this->period = 60 * 1000000 / (speed * this->configuration->stepsPerUnit);
}

void StepperMotor::updateDirectionPin(){
    uint8_t motorIsReversed = this->configuration->invertDirection ? 1 : -1;
    // set direction to move forward
    if(this->targetSteps > this->currentSteps){
        this->direction = motorIsReversed;
        this->i2cPort->write(this->configuration->directionPin.number, !(this->configuration->invertDirection));
    // set direction to move backward
    } else {
        this->direction = -motorIsReversed;
        this->i2cPort->write(this->configuration->directionPin.number, (this->configuration->invertDirection));
    }
}

void StepperMotor::SetTargetPosition(int32_t position) {
    // check if a maximum travel has been set
    if(this->maxTravel != 0){
        // minimum travel is always 0, so we only care about the maximum travel
        // if the target position is greater than the maximum travel, set it to the maximum travel
        if(position > this->maxTravel){
            position = this->maxTravel;
        }
    }

    // We take the mutex here because we are updating internal variables the update function uses
    this->targetSteps = position * this->configuration->stepsPerUnit;
    this->updateDirectionPin();
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    // We take the mutex here because we are updating internal variables the update function uses
    this->currentSteps = position * this->configuration->stepsPerUnit;
    this->updateDirectionPin();
}

void IRAM_ATTR StepperMotor::StepUpdate(uint16_t dt) {
    // if we are within 5 steps of the target position, stop
    if(abs(this->targetSteps - this->currentSteps) > 5){
        this->updatesSinceLastStep += 1;
        // do one step if it is time
        if(dt*(this->updatesSinceLastStep) >= this->period){
            this->currentSteps += this->direction;
            digitalWrite(this->configuration->stepPin, LOW);
            digitalWrite(this->configuration->stepPin, HIGH);
            this->updatesSinceLastStep = 1;
        }
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->write(this->configuration->enablePin.number, enabled);
}


int32_t StepperMotor::GetCurrentPosition(){
    return this->currentSteps / this->configuration->stepsPerUnit;
}

int32_t StepperMotor::GetTargetPosition(){
    return this->targetSteps / this->configuration->stepsPerUnit;
}

uint32_t StepperMotor::GetSpeed(){
    if(this->period == 0){
        return 0;
    }
    float floatPeriod = static_cast<float>(period);
    float floatStepsPerUnit = static_cast<float>(configuration->stepsPerUnit);
    return static_cast<uint32_t>(60.0f * 1000000.0f / (floatPeriod * floatStepsPerUnit));
}

void StepperMotor::SetMaxTravel(int32_t maxTravel){
    this->maxTravel = maxTravel;
}