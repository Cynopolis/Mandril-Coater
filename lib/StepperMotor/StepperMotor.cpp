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
        this->stepper->setDirectionPin(this->configuration.directionPin.number | PIN_EXTERNAL_FLAG, false);
        this->stepper->setEnablePin(this->configuration.enablePin.number | PIN_EXTERNAL_FLAG, false);
        this->stepper->setAutoEnable(true);
        // convert mm per minute per minute to steps per second squared
        int32_t accelStepsPerUSSquared = static_cast<int32_t>(this->configuration.acceleration * this->configuration.stepsPerUnit) / (60);
        int8_t setAccelStatus = this->stepper->setAcceleration(accelStepsPerUSSquared);
        if(setAccelStatus != 0){
            Serial.println("Failed to set acceleration: " + String(accelStepsPerUSSquared));
        }
    }

    this->i2cPort->write(this->configuration.directionPin.number, LOW);

    this->timeOfLastStep = micros();
}

void StepperMotor::MoveToPosition(int32_t position, float speed) {
    if(!this->isInitialized() || speed == 0){
        return;
    }

    // set the speed
    uint32_t abs_speed = static_cast<uint32_t>(abs(speed));
    // convert units per minute to microseconds per step
    this->period = 60 * 1000000 / (abs_speed * this->configuration.stepsPerUnit);
    if(this->stepper->setSpeedInUs(this->period) == -1){
        Serial.println("Failed to set speed! The feedrate is too high!");
        return;
    }

    // start the move
    this->targetSteps = position * this->configuration.stepsPerUnit;
    int8_t move_status = this->stepper->moveTo(this->targetSteps, false);
    if(move_status != MOVE_OK){
        Serial.println("Failed to start move!" + String(move_status));
    }
}

void StepperMotor::Stop() {
    if(!this->isInitialized()){
        return;
    }
    this->stepper->stopMove();
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    if(!this->isInitialized()){
        return;
    }
    
    this->currentSteps = position * this->configuration.stepsPerUnit;
    this->stepper->setCurrentPosition(this->currentSteps);
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
    float floatPeriod = static_cast<float>(period);
    float floatStepsPerUnit = static_cast<float>(configuration.stepsPerUnit);
    return static_cast<uint32_t>(60.0f * 1000000.0f / (floatPeriod * floatStepsPerUnit));
}

void StepperMotor::SetMaxTravel(int32_t maxTravel){
    this->maxTravel = maxTravel;
}

bool StepperMotor::isInitialized(){
    if(this->stepper == nullptr){
        Serial.println("Stepper is not initialized! Cannot perform action");
        return false;
    }
    return true;
}