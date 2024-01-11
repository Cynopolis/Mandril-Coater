/**
 * @file GCodeDefinitions.h
 * @brief This file contains the GCodeParser interface which is used to parse strings down into its constituent components
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef GCODE_DEFINITIONS_H
#define GCODE_DEFINITIONS_H

#include <Arduino.h>

namespace GCodeDefinitions{
    enum Command : uint8_t{
            INVALID, // invalid command
            M2, // ping
            G4, // wait
            M0, // estop
            M1, // release estop
            M24, // pause/resume
            M114, // get position
            G91, // relative positioning
            G90, // absolute positioning
            M208, // set max travel
            M92, // set steps per unit
            G1, // controlled move
            G0, // coast move
            G28, // home
            M42 // set pin  
    };

    // create an array to hold a list of char[] that correspond to the commands
    const char * const commandStrings[] = {
        "INVALID",
        "M2",
        "G4",
        "M0",
        "M1",
        "M24",
        "M114",
        "G91",
        "G90",
        "M208",
        "M92",
        "G1",
        "G0",
        "G28",
        "M42"
    };

    const uint8_t commandStringLength = 15; // note: this needs to be updated if commandStrings is changed

    // struct to hold the parsed command
    struct GCode{
        Command command;
        uint16_t X;
        bool hasX;

        uint16_t R;
        bool hasR;

        uint16_t F;
        bool hasF;

        uint16_t S;
        bool hasS;

        uint16_t P;
        bool hasP;

        uint16_t T;
        bool hasT;
    };
};

#endif // GCODE_DEFINITIONS_H