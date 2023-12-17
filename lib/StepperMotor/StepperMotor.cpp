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

void StepperMotor::SetMaximums(float maxSpeed, float acceleration) {
    this->stepper.setMaxSpeed(this->stepsPerUnit*maxSpeed);
    this->stepper.setAcceleration(this->stepsPerUnit*acceleration);
}

void StepperMotor::SetSpeed(float speed) {
    this->stepper.setMaxSpeed(this->stepsPerUnit*speed);
}

void StepperMotor::SetTargetPosition(int32_t position) {
    this->stepper.moveTo(long(this->stepsPerUnit*position));
}

void StepperMotor::Update() {
    if(this->stepper.distanceToGo() != 0){
        this->stepper.run();
    }
}

void StepperMotor::SetEnabled(bool enabled) {
    digitalWrite(this->enablePin, !enabled);
}