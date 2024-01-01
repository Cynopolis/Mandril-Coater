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
    this->i2cPort->write(this->configuration.enablePin.pin, HIGH);
    this->i2cPort->write(this->configuration.directionPin.pin, LOW);
    this->i2cPort->write(this->configuration.stepPin.pin, HIGH);

    this->timeOfLastStep = micros();
}

void StepperMotor::SetSpeed(float speed) {
    if(speed > this->configuration.maxSpeed){
        speed = this->configuration.maxSpeed;
    }
    this->speed = speed;
}

void StepperMotor::SetTargetPosition(int32_t position) {
    this->targetPosition = position;
    if(this->targetPosition > this->currentPosition){
        this->direction = 1;
        this->i2cPort->write(this->configuration.directionPin.pin, LOW);
    } else {
        this->direction = -1;
        this->i2cPort->write(this->configuration.directionPin.pin, HIGH);
    }
}

void StepperMotor::Update() {
    // if we are within 0.1% of the target position, stop
    if(abs(this->targetPosition - this->currentPosition) < 0.001 * this->configuration.stepsPerUnit){
        return;
    }

    uint32_t timeSinceLastStep = micros() - this->timeOfLastStep;
    // do one step if it is time
    if(timeSinceLastStep >= (1000000.0 / this->speed)){
        this->currentPosition += this->direction / this->configuration.stepsPerUnit;
        this->i2cPort->write(this->configuration.stepPin.pin, LOW);
        this->i2cPort->write(this->configuration.stepPin.pin, HIGH);
        this->timeOfLastStep = micros();
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->write(this->configuration.enablePin.pin, enabled);
}