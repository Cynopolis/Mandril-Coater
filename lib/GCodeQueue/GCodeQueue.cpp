/**
 * @file GCodeQueue.cpp
 * @brief This file contains the GCodeQueue class implimentation which will hold GCode commands until we are ready to execute them
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "GCodeQueue.h"
#include <Arduino.h>

bool GCodeQueue::Push(const GCodeDefinitions::GCode &command){
    // check if the queue is full
    if(currentQueueSize == GCODE_QUEUE_MAX_SIZE){
        return false;
    }

    // add the command to the queue
    this->commands[this->queueWriteIndex] = command.copy();
    // incriment the queue size and the write index
    currentQueueSize++;
    queueWriteIndex++;

    // wrap around the write index if it is greater than the max size
    if(queueWriteIndex == GCODE_QUEUE_MAX_SIZE){
        queueWriteIndex = 0;
    }

    return true;
}

GCodeDefinitions::GCode * GCodeQueue::Pop(uint16_t index){
    // check if the queue is empty
    if(currentQueueSize == 0){
        return nullptr;
    }
    
    // get the command at the index offset by our read index
    uint16_t offsetIndex = this->queueReadIndex + index;
    // if our offset index is greater than the max size, then wrap around
    if(offsetIndex >= GCODE_QUEUE_MAX_SIZE){
        offsetIndex -= GCODE_QUEUE_MAX_SIZE;
    }

    this->currentCommand = commands[offsetIndex];

    // incriment the read index and decriment the queue size
    this->queueReadIndex++;
    currentQueueSize--;
    // wrap around the read index if it is greater than the max size
    if(queueReadIndex == GCODE_QUEUE_MAX_SIZE){
        queueReadIndex = 0;
    }

    // return the command at the front of the queue
    return &this->currentCommand;
}

GCodeDefinitions::GCode * GCodeQueue::Peek(uint16_t index){
    if(index >= currentQueueSize){
        return nullptr;
    }

    // get the command at the index offset by our read index
    uint16_t offsetIndex = this->queueReadIndex + index;
    // if our offset index is greater than the max size, then wrap around
    if(offsetIndex >= GCODE_QUEUE_MAX_SIZE){
        offsetIndex -= GCODE_QUEUE_MAX_SIZE;
    }

    return &this->commands[offsetIndex];
}

uint16_t GCodeQueue::Size(){
    return this->currentQueueSize;
}

bool GCodeQueue::MoveBack(uint16_t offset){

    // move the read index back by the offset and allow it to be negative
    int32_t newReadIndexWrapChecker = static_cast<int32_t>(this->queueReadIndex - offset);
    bool wrapAround = false;

    // if the new read index is less than 0, then wrap around
    if(newReadIndexWrapChecker < 0){
        newReadIndexWrapChecker += GCODE_QUEUE_MAX_SIZE;
        wrapAround = true;
    }

    // convert the new read index back to a uint16_t
    uint16_t newReadIndex = static_cast<uint16_t>(newReadIndexWrapChecker);

    // if we pass the write pointer, then return false
    bool startsAndEndsGreaterThanWritePointer = newReadIndex > this->queueWriteIndex && this->queueReadIndex > this->queueWriteIndex;
    bool startsAndEndsLessThanWritePointer = newReadIndex < this->queueWriteIndex && this->queueReadIndex < this->queueWriteIndex;
    bool startsLessThanAndEndsGreaterThanWritePointer = newReadIndex < this->queueWriteIndex && this->queueReadIndex > this->queueWriteIndex && wrapAround;
    bool wrappedPastWritePointer = !(startsAndEndsGreaterThanWritePointer || startsAndEndsLessThanWritePointer || startsLessThanAndEndsGreaterThanWritePointer);
    // if you're trying to move the read index back by more than the queue size, then you're doing something wrong
    bool offsetTooBig = offset >= GCODE_QUEUE_MAX_SIZE;
    bool readIndexEqualsWriteIndex = this->queueReadIndex == this->queueWriteIndex;

    

    if(offsetTooBig || wrappedPastWritePointer || readIndexEqualsWriteIndex){
        Serial.print("MoveBack offset: " + String(offset));
        Serial.print(" queueReadIndex: " + String(this->queueReadIndex));
        Serial.print(" queueWriteIndex: " + String(this->queueWriteIndex));
        Serial.println(" newReadIndex: " + String(newReadIndex));
        Serial.print(" wrapAround: " + String(wrapAround));
        Serial.print(" startsAndEndsGreaterThanWritePointer: " + String(startsAndEndsGreaterThanWritePointer));
        Serial.print(" startsAndEndsLessThanWritePointer: " + String(startsAndEndsLessThanWritePointer));
        Serial.print(" startsLessThanAndEndsGreaterThanWritePointer: " + String(startsLessThanAndEndsGreaterThanWritePointer));
        Serial.print(" wrappedPastWritePointer: " + String(wrappedPastWritePointer));
        Serial.print(" offsetTooBig: " + String(offsetTooBig));
        Serial.println(" readIndexEqualsWriteIndex: " + String(readIndexEqualsWriteIndex));
        Serial.println("Command at invalid index: " + String(this->commands[newReadIndex].command));
        Serial.println("Command at 0: " + String(this->commands[0].command));
        return false;
    }

    // set the new read index
    this->queueReadIndex = newReadIndex;
    this->currentQueueSize += offset;
    return true;
}