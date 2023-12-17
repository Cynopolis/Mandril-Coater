#include "StepperMotor.h"
#include <Arduino.h>

void StepperMotor::Init(){
    pinMode(this->enablePin, OUTPUT);
}

void StepperMotor::setMaximums(uint32_t maxSpeed, uint32_t acceleration) {
    this->stepper.setMaxSpeed(maxSpeed);
    this->stepper.setAcceleration(acceleration);
}

void StepperMotor::setTargetPosition(int32_t position) {
    this->stepper.moveTo(position);
}

void StepperMotor::Update() {
    if(this->stepper.distanceToGo() != 0){
        this->stepper.run();
    }
}

void StepperMotor::setEnabled(bool enabled) {
    digitalWrite(this->enablePin, enabled);
}