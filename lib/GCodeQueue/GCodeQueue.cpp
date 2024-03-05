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
        Serial.println("!1;");
        Serial2.println("!1;");
        return false;
    }

    // add the command to the queue
    commands[currentQueueSize] = command.copy();
    currentQueueSize++;

    return true;
}

GCodeDefinitions::GCode * GCodeQueue::pop(uint16_t index){
    // check if the queue is empty
    if(currentQueueSize == 0){
        return NULL;
    }

    // get the command at the index
    currentCommand = commands[index];

    // shift all commands down one index
    shiftDown(index);

    // return the command at the front of the queue
    return &currentCommand;
}

uint32_t GCodeQueue::size(){
    return currentQueueSize;
}

void GCodeQueue::shiftDown(uint16_t startIndex){
    // shift all commands down one index
    for(uint16_t i = startIndex; i < currentQueueSize - 1; i++){
        commands[i] = commands[i + 1];
    }

    commands[currentQueueSize - 1] = GCodeDefinitions::GCode(); // clear the last command

    // decrement the queue size
    currentQueueSize--;
}
