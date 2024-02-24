/**
 * @file GCodeMessage.h
 * @brief This file contains the GCodeMessage class which is a child of the SerialMessage class and is used to parse GCode messages
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_MESSAGE_H
#define GCODE_MESSAGE_H

#include "SerialMessage.h"
#include "GCODE-DEFINITIONS.h"
#include "GCodeQueue.h"

class GCodeMessage : public SerialMessage{
    public:
    /**
     * @brief Construct a new GCode Message object
     */
    GCodeMessage(HardwareSerial *serial = &Serial) : SerialMessage(serial){};

    /**
     * @brief Returns the parsed GCode message
     * @return the parsed GCode message
     * @note If nothing has been parsed, or if the CLearNewData() function has been called, then the returned GCode will be invalid
     */
    GCodeDefinitions::GCode * PopGCode();

    GCodeDefinitions::GCode * PeekGCode(){
        return this->queue.peek();
    }

    uint32_t GetQueueSize(){
        return this->queue.size();
    }

    /**
     * @brief Clears the new data flag
    */
    void ClearNewData() override;

    /**
     * @brief Returns true if an estop command has been received
     * @return true if an estop command has been received
     * @post The estopCommandReceived flag will be set to false
    */
    bool EStopCommandReceived(){
        bool tempVal = this->estopCommandReceived;
        this->estopCommandReceived = false;
        return tempVal;
    }

    private:
    GCodeQueue queue; // the queue of GCode commands
    bool estopCommandReceived = false; // immediately true if an estop command has been received

    /**
     * @brief Parse the message into a GCode struct
     */
    void parseData() override;

    /**
     * @brief Parse a string into its constituent components
     * @param message The string to parse
     * @param length The length of the string
     * @return The parsed GCode
    */
    GCodeDefinitions::GCode parseGCodeString(char *message, uint16_t length);

    /**
     * @brief Check if a string matches a command
     * @param str The string to check
     * @return The command that the string matches
    */
    GCodeDefinitions::Command matchToCommand(char *str, uint8_t length);

    /**
     * @brief Populate a single value in the given command with the data from the string
     * @param str The string to parse
     * @param length The length of the string
    */
    void populateCommandWithData(GCodeDefinitions::GCode *command, char *str, uint8_t length);

    /**
     * @brief Capitalize a string
     * @param str A pointer to the string to capitalize
    */
    void capitalize(char *str);
};

#endif // GCODE_MESSAGE_H