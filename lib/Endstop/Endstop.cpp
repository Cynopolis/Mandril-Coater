/**
 * @file Endstop.cpp
 * @brief This file contains the implimentation for the Endstop class
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Endstop.h"

void Endstop::Init(void (*triggeredHandler)(), void (*untriggeredHandler)()){
    this->triggeredHandler = triggeredHandler;
    this->untriggeredHandler = untriggeredHandler;
    delay(1); // delay to allow the i2c port to initialize and settle
    // this->isTriggered = this->triggerType == HIGH ? pin.i2cPort->read(pin.number) : !pin.i2cPort->read(pin.number);
    this->lastTriggeredTime = millis();
    this->lastStateChangeTime = millis();
}

void Endstop::Update(){
    bool pinState = pin.i2cPort->read(pin.number);
    // if the trigger type is low then invert the pin state
    if(this->triggerType == LOW){
        pinState = !pinState;
    }

    // the state pin state has changed if the new pin state doesn't equal the last isTriggered state
    bool stateChanged = pinState != this->isTriggered;
    this->isTriggered = pinState;
    
    // if our state has changed then reset our state change timer
    if(stateChanged){
        this->lastStateChangeTime = millis();
        // we can retrigger on a new state change
        this->canTrigger = true;
    }
    
    // check if the state hasn't changed in a while and we haven't called a handler for a while, 
    // and there has been a state change since we last triggered
    if(millis() - this->lastStateChangeTime > 100 && millis() - this->lastTriggeredTime > 100 && this->canTrigger){
        if(isTriggered){
            this->triggeredHandler();
            this->canTrigger = false;
        }
        // check to make sure we actually have an untriggered handler
        else if(untriggeredHandler != NULL){
            this->untriggeredHandler();
            this->canTrigger = false;
        }
        this->lastTriggeredTime = millis();
    }
}

bool Endstop::IsTriggered(){
    return isTriggered;
}


