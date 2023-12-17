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
    pinMode(this->enablePin, OUTPUT);
}

void StepperMotor::SetMaximums(uint32_t maxSpeed, uint32_t acceleration) {
    this->stepper.setMaxSpeed(this->unitsToSteps(maxSpeed));
    this->stepper.setAcceleration(this->unitsToSteps(acceleration));
}

void StepperMotor::SetSpeed(uint32_t speed) {
    this->stepper.setMaxSpeed(this->unitsToSteps(speed));
}

void StepperMotor::SetTargetPosition(int32_t position) {
    this->stepper.moveTo(this->unitsToSteps(position));
}

void StepperMotor::Update() {
    if(this->stepper.distanceToGo() != 0){
        this->SetEnabled(true);
        this->stepper.run();
    }
    else{
        this->SetEnabled(false);
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    digitalWrite(this->enablePin, !enabled);
}

uint32_t StepperMotor::unitsToSteps(uint32_t units){
    return (uint32_t)(float(units)*(this->stepsPerUnit));
}