/**
 * @file Endstop.cpp
 * @brief This file contains the implimentation for the Endstop class
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Endstop.h"

void Endstop::Init(void (*triggeredHandler)()){
    this->triggeredHandler = triggeredHandler;
    this->Update();
}

void Endstop::Update(){
    bool pinState = pin.i2cPort->read(pin.number);
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

    if(isTriggered){
        triggeredHandler();
    }
}

bool Endstop::IsTriggered(){
    return isTriggered;
}


