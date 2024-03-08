/**
 * @file StepperMotor.cpp
 * @brief This file contains the StepperMotor class
 * @details This file contains the StepperMotor class which is used to control a stepper motor
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "StepperMotor.h"
#include <Arduino.h>

void StepperMotor::Init(FastAccelStepperEngine &engine) {
    this->stepper = engine.stepperConnectToPin(this->configuration.stepPin);

    // as ugly as this looks, it is the only way to pass a member function to the external call for pin
    engine.setExternalCallForPin(this->configuration.callBackHandler);

    if(this->stepper == nullptr){
        Serial.println("Failed to connect stepper to pin! Something is very wrong!");
    }
    else{
        this->stepper->setDirectionPin(this->configuration.directionPin.number | PIN_EXTERNAL_FLAG);
        this->stepper->setEnablePin(this->configuration.enablePin.number | PIN_EXTERNAL_FLAG);
        this->stepper->setAutoEnable(true);
        int32_t accelStepsPerUSSquared = 60 * 1000000 / static_cast<int32_t>(this->configuration.acceleration * this->configuration.stepsPerUnit);
        this->stepper->setAcceleration(accelStepsPerUSSquared);
    }

    this->i2cPort->pinMode(this->configuration.directionPin.number, OUTPUT);
    this->i2cPort->pinMode(this->configuration.enablePin.number, OUTPUT);
    this->i2cPort->digitalWrite(this->configuration.directionPin.number, LOW);

    this->timeOfLastStep = micros();
}

void StepperMotor::SetSpeed(float speed) {
    speed = static_cast<uint32_t>(abs(speed));
    // convert units per minute to microseconds per step
    this->period = 60 * 1000000 / (speed * this->configuration.stepsPerUnit);
    this->stepper->setSpeedInUs(this->period);
}

void StepperMotor::SetTargetPosition(int32_t position) {
    this->targetSteps = position * this->configuration.stepsPerUnit;
    this->stepper->moveTo(this->targetSteps, false);
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    this->currentSteps = position * this->configuration.stepsPerUnit;
    this->stepper->setCurrentPosition(this->currentSteps);
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->digitalWrite(this->configuration.enablePin.number, enabled);
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
    float floatPeriod = static_cast<float>(period);
    float floatStepsPerUnit = static_cast<float>(configuration.stepsPerUnit);
    return static_cast<uint32_t>(60.0f * 1000000.0f / (floatPeriod * floatStepsPerUnit));
}

void StepperMotor::SetMaxTravel(int32_t maxTravel){
    this->maxTravel = maxTravel;
}