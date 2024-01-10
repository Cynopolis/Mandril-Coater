/**
 * @file GCodeMessage.h
 * @brief This file contains the GCodeMessage class which is a child of the SerialMessage class and is used to parse GCode messages
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_MESSAGE_H
#define GCODE_MESSAGE_H

#include "SerialMessage.h"
#include "GCodeDefinitions.h"

class GCodeMessage : public SerialMessage{
    public:
    /**
     * @brief Construct a new GCode Message object
     */
    GCodeMessage(HardwareSerial *serial = &Serial) : SerialMessage(serial){};

    /**
     * @brief Clears the new data flag
     */
    void ClearNewData() override;

    /**
     * @brief Returns the parsed GCode message
     * @return the parsed GCode message
     * @note If nothing has been parsed, or if the CLearNewData() function has been called, then the returned GCode will be invalid
     */
    GCodeDefinitions::GCode GetGCode();

    private:
    /**
     * @brief Parse the message into a GCode struct
     */
    void parseData() override;

    /**
     * @brief Parse a string into its constituent components
     * @param message The string to parse
     * @param length The length of the string
     * @return A pointer to the array of parsed values
    */
    GCodeDefinitions::GCode * parseGCodeString(char *message, uint16_t length);

    /**
     * @brief Check if a string matches a command
     * @param str The string to check
     * @return The command that the string matches
    */
    GCodeDefinitions::Command matchToCommand(char *str, uint8_t length);

    /**
     * @brief Populate a single value in lastCommand with the data from a string
     * @param str The string to parse
     * @param length The length of the string
    */
    void populateLastCommandWithData(char *str, uint8_t length);

    /**
     * @brief Capitalize a string
     * @param str A pointer to the string to capitalize
    */
    void capitalize(char *str);

    GCodeDefinitions::GCode lastCommand = {
        .command = GCodeDefinitions::Command::INVALID,
        .X = 0,
        .hasX = false,
        .R = 0,
        .hasR = false,
        .F = 0,
        .hasF = false,
        .S = 0,
        .hasS = false,
        .P = 0,
        .hasP = false
    };
};

#endif // GCODE_MESSAGE_H