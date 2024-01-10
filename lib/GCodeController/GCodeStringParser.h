/**
 * @file GCodeStringParser.h
 * @brief This file contains the GCodeParser interface which is used to parse strings down into its constituent components
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_STRING_PARSER_H
#define GCODE_STRING_PARSER_H

#include <Arduino.h>

namespace GCodeParser{
    /**
     * @brief Parse a string into its constituent components
     * @param message The string to parse
     * @param length The length of the string
     * @return A pointer to the array of parsed values
    */
    GCode * ParseGCodeString(char *message, int length);

    /**
     * @brief match two strings
     * @param str1 The first string
     * @param str2 The second string
    */
    bool match(char *str1, char *str2);

    // the most recently parsed command
    extern GCode lastCommand = {
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