/**
 * @file GCodeQueue.h
 * @brief This file contains the GCodeQueue class which will hold GCode commands until we are ready to execute them
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_QUEUE_H
#define GCODE_QUEUE_H

#include "GCODE-DEFINITIONS.h"

/*
    The maximum number of GCode commands that can be stored in the queue before additional commands are discarded
    Commands in the queue will try to be processed as fast as possible.
    Once the command is processed, the queue will remove the command from the queue and shift all other commands down one index
*/
#define GCODE_QUEUE_MAX_SIZE 20
class GCodeQueue{
    public:

        /**
         * @brief Construct a new GCodeQueue object
        */
        GCodeQueue() = default;

        /**
         * @brief Add a GCode command to the end of the queue
         * @param command the GCode command to add to the queue
         * @return true if the command was added to the queue. False if the queue is full and the command was not added
        */
        bool push(const GCodeDefinitions::GCode &command);

        /**
         * @brief Pop a GCode command from the front of the queue
         * @return GCode the GCode command that was popped from the queue
         * @note if the queue is empty then NULL is returned. Also, the GCode command popped from the queue must be used completetly before calling this function again.
         * Calling this function a second time will overwrite the GCode command that was popped from the queue
        */
        GCodeDefinitions::GCode * pop();

        GCodeDefinitions::GCode * peek(){
            return &commands[0];
        }

        /**
         * @brief Get the number of GCode commands in the queue
         * @return int the number of GCode commands in the queue
        */
        uint32_t size();

        /**
         * @brief Get the maximum number of GCode commands that can be stored in the queue
         * @return int the maximum number of GCode commands that can be stored in the queue
        */
        static uint16_t max_size(){return GCODE_QUEUE_MAX_SIZE;};
    private:
        uint16_t currentQueueSize = 0; // the number of GCode commands in the queue
        // create an array of GCode commands
        GCodeDefinitions::GCode commands[GCODE_QUEUE_MAX_SIZE];

        GCodeDefinitions::GCode currentCommand; // the current GCode command being executed

        /**
         * @brief shift all GCode commands in the queue down one index
        */
        void shiftDown();
};

#endif // GCODE_QUEUE_H