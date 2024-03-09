/**
 * @file Endstop.cpp
 * @brief This file contains the implimentation for the Endstop class
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Endstop.h"

void Endstop::Init(void (*triggeredHandler)()){
    this->triggeredHandler = triggeredHandler;

    // check to see if the endstop is triggered on startup
    bool pinState = pin.i2cPort->read(pin.number);
    switch(triggerType){
        case LOW:
            this->isTriggered = !pinState;
            break;
        case HIGH:
            this->isTriggered = pinState;
            break;
        default:
            isTriggered = false;
            break;
    }
    if(isTriggered && triggeredHandler != NULL){
        this->triggeredHandler();
        this->lastTriggeredTime = millis();
    }
}

void Endstop::Update(){
    bool pinState = pin.i2cPort->read(pin.number);
    bool stateChanged = false;
    // check if the endstop has been triggered
    switch(triggerType){
        // if the trigger type is LOW then the endstop is triggered when the pin is low
        case LOW:
            stateChanged = pinState != isTriggered;
            this->isTriggered = !pinState;
            break;
        // if the trigger type is HIGH then the endstop is triggered when the pin is high
        case HIGH:
            stateChanged = pinState != isTriggered;
            this->isTriggered = pinState;
            break;
        // if the trigger type is not LOW or HIGH then the endstop is never
        default:
            isTriggered = false;
            break;
    }
    // we need to filter out repeated triggers
    if(stateChanged && isTriggered && triggeredHandler != NULL){
        // only trigger the handler if it has been more than 10ms since the last trigger
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


