/**
 * @file Endstop.cpp
 * @brief This file contains the implimentation for the Endstop class
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Endstop.h"

void Endstop::Init(void (*triggeredHandler)()){
    this->triggeredHandler = triggeredHandler;
    this->pin.i2cPort->pinMode(pin.number, INPUT);
    bool pinState = pin.i2cPort->digitalRead(pin.number);
    switch(triggerType){
        case LOW:
            isTriggered = !pinState;
            break;
        case HIGH:
            isTriggered = pinState;
            break;
        default:
            isTriggered = false;
            break;
    }
    if(isTriggered && triggeredHandler != NULL){
        this->triggeredHandler();
    }

    this->lastTriggeredTime = millis();
}

void Endstop::Update(){
    bool pinState = pin.i2cPort->digitalRead(pin.number);
    bool stateChanged = false;
    switch(triggerType){
        case LOW:
            stateChanged = pinState != isTriggered;
            this->isTriggered = !pinState;
            break;
        case HIGH:
            stateChanged = pinState != isTriggered;
            this->isTriggered = pinState;
            break;
        default:
            isTriggered = false;
            break;
    }
    // we need to filter out repeated triggers
    if(stateChanged && isTriggered && triggeredHandler != NULL){
        uint32_t currentTime = millis();
        if(currentTime - lastTriggeredTime > 10){
            this->triggeredHandler();
        }
        this->lastTriggeredTime = millis();
    }
}

bool Endstop::IsTriggered(){
    return isTriggered;
}


