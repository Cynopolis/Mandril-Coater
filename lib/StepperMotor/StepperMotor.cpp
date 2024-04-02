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
        // at some point it would be nice to be able to use a pulse counter to get higher accuacy counts
        // this->stepper->attachToPulseCounter(this->configuration.pulseCounterUnit);
        // convert mm per minute per minute to steps per second squared
        int32_t accelStepsPerUSSquared = static_cast<int32_t>(this->configuration.acceleration * this->configuration.stepsPerUnit) / (60);
        int8_t setAccelStatus = this->stepper->setAcceleration(accelStepsPerUSSquared);
        if(setAccelStatus != 0){
            Serial.println("Failed to set acceleration: " + String(accelStepsPerUSSquared));
        }
    }

    this->i2cPort->write(this->configuration.directionPin.number, HIGH);

    this->timeOfLastStep = micros();
}

void StepperMotor::MoveToPosition(int32_t position, float speed) {
    if(!this->isInitialized()){
        return;
    }

    if(abs(speed) < 0.001){
        Serial.println("Speed cannot be zero! Speed: " + String(speed));
        return;
    }

    // check that the speed is within parameters
    float abs_speed = static_cast<uint32_t>(abs(speed));
    if(abs_speed > this->configuration.maxSpeed){
        Serial.println("Speed is too high. Speed: " + String(speed) + " Max Speed: " + String(this->configuration.maxSpeed));
        abs_speed = this->configuration.maxSpeed;
    }

    // convert units per minute to microseconds per step
    this->period = static_cast<uint32_t>(60.0f * 1000000.0f / (abs_speed * this->configuration.stepsPerUnit));
    if(this->stepper->setSpeedInUs(this->period) == -1){
        Serial.println("Failed to set speed. The feedrate is too high.");
        return;
    }

    // start the move
    this->targetSteps = static_cast<int32_t>(static_cast<float>(position) * this->configuration.stepsPerUnit);
    int8_t move_status = this->stepper->moveTo(this->targetSteps, false);
    if(move_status != MOVE_OK){
        Serial.println("Failed to start move: " + String(move_status));
    }
}

void StepperMotor::Stop() {
    if(!this->isInitialized()){
        return;
    }
    this->i2cPort->write(this->configuration.enablePin.number, HIGH);
    this->stepper->forceStop();
    this->stepper->stopMove();
}

void StepperMotor::SetCurrentPosition(int32_t position) {
    if(!this->isInitialized()){
        return;
    }

    this->currentSteps = static_cast<int32_t>(static_cast<float>(position) * this->configuration.stepsPerUnit);
    Serial.println("New position: " + String(this->currentSteps));
    this->stepper->setCurrentPosition(this->currentSteps);
}

void StepperMotor::SetEnabled(bool enabled) {
    this->i2cPort->write(this->configuration.enablePin.number, enabled);
}


int32_t StepperMotor::GetCurrentPosition(){
    return static_cast<int32_t>(static_cast<float>(this->stepper->getCurrentPosition()) / this->configuration.stepsPerUnit);
}

int32_t StepperMotor::GetTargetPosition(){
    return this->targetSteps / this->configuration.stepsPerUnit;
}

uint32_t StepperMotor::GetSpeed(){
    if(this->period == 0){
        return 0;
    }
    float floatPeriod = static_cast<float>(period);
    return static_cast<uint32_t>(60.0f * 1000000.0f / (floatPeriod * configuration.stepsPerUnit));
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

void StepperMotor::SetAcceleration(int32_t acceleration){
    int32_t decelStepsPerUSSquared = static_cast<int32_t>(static_cast<float>(acceleration) * this->configuration.stepsPerUnit) / (60);
    int8_t setDecelStatus = this->stepper->setAcceleration(decelStepsPerUSSquared);
    if(setDecelStatus != 0){
        Serial.println("Failed to set deceleration: " + String(decelStepsPerUSSquared));
    }
}