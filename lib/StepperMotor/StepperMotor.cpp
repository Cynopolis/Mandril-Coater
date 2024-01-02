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

void StepperMotor::SetTargetPosition(int32_t position) {
    this->targetPosition = position;
    if(this->targetPosition > this->currentPosition){
        this->direction = 1;
        this->i2cPort->write(this->configuration.directionPin.number, LOW);
    } else {
        this->direction = -1;
        this->i2cPort->write(this->configuration.directionPin.number, HIGH);
    }
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    this->stepper.setCurrentPosition(long(this->stepsPerUnit*position));
}

void StepperMotor::Update() {
    // if we are within 0.5% of the target position, stop
    if(abs(this->targetPosition - this->currentPosition) < 0.005 * this->configuration.stepsPerUnit){
        return;
    }

    uint32_t timeSinceLastStep = micros() - this->timeOfLastStep;
    // do one step if it is time
    if(timeSinceLastStep >= this->period){
        this->currentPosition += this->direction / this->configuration.stepsPerUnit;
        this->i2cPort->write(this->configuration.stepPin.number, LOW);
        this->i2cPort->write(this->configuration.stepPin.number, HIGH);
        this->timeOfLastStep = micros();
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->write(this->configuration.enablePin.number, enabled);
}