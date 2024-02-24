/**
 * @file GCodeQueue.cpp
 * @brief This file contains the GCodeQueue class implimentation which will hold GCode commands until we are ready to execute them
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "GCodeQueue.h"
#include <Arduino.h>

bool GCodeQueue::push(const GCodeDefinitions::GCode &command){
    // check if the queue is full
    if(currentQueueSize == GCODE_QUEUE_MAX_SIZE){
        Serial.println("GCodeQueue::push(): queue is full. This command will be discarded");
        return false;
    }

    // add the command to the queue
    commands[currentQueueSize] = command.copy();
    currentQueueSize++;

    return true;
}

GCodeDefinitions::GCode * GCodeQueue::pop(){
    // check if the queue is empty
    if(currentQueueSize == 0){
        return NULL;
    }

    // get the command at the front of the queue
    currentCommand = commands[0];

    // shift all commands down one index
    shiftDown();

    // return the command at the front of the queue
    return &currentCommand;
}

uint32_t GCodeQueue::size(){
    return currentQueueSize;
}

void GCodeQueue::shiftDown(){
    // shift all commands down one index
    for(uint16_t i = 0; i < currentQueueSize - 1; i++){
        commands[i] = commands[i + 1];
    }

    commands[currentQueueSize - 1] = GCodeDefinitions::GCode(); // clear the last command

    // decrement the queue size
    currentQueueSize--;
}
