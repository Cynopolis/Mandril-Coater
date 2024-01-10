/**
 * @file GCodeStringParser.h
 * @brief This file contains the GCodeParser interface which is used to parse strings down into its constituent components
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_STRING_PARSER_H
#define GCODE_STRING_PARSER_H

#include <Arduino.h>
#include "GCodeDefinitions.h"

namespace GCodeParser{
    /**
     * @brief Parse a string into its constituent components
     * @param message The string to parse
     * @param length The length of the string
     * @return A pointer to the array of parsed values
    */
    GCodeDefinitions::GCode * ParseGCodeString(char *message, uint16_t length);

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

    // the most recently parsed command
    extern GCodeDefinitions::GCode lastCommand = {
        .command = INVALID,
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

#endif // GCODE_STRING_PARSER_H