/**
 * @file GCODE-DEFINITIONS.h
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
            M42, // set pin  
            M112, // cancel current command
            G92, // Set current position
            M204, // Set decceleration
            COUNT // not a command, but the number of commands. Always have this as the last element
    };

    // create an array to hold a list of char[] that correspond to the commands
    static const char * const commandStrings[] = {
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
        "M42",
        "M112",
        "G92",
        "M204"
    };

    constexpr uint8_t commandStringLength = static_cast<uint8_t>(Command::COUNT); // note: this needs to be updated if commandStrings is changed

    // struct to hold the parsed command
    struct GCode{
        Command command = Command::INVALID;
        int32_t X = 0;
        bool hasX = false;

        int32_t R = 0;
        bool hasR = false;

        int32_t F = 0;
        bool hasF = false;

        int32_t S = 0;
        bool hasS = false;

        int32_t P = 0;
        bool hasP = false;

        int32_t T = 0;
        bool hasT = false;

        // create a deep copy fucntion
        GCode copy() const{
            GCode copyGCode;
            copyGCode.command = this->command;
            copyGCode.X = this->X;
            copyGCode.hasX = this->hasX;
            copyGCode.R = this->R;
            copyGCode.hasR = this->hasR;
            copyGCode.F = this->F;
            copyGCode.hasF = this->hasF;
            copyGCode.S = this->S;
            copyGCode.hasS = this->hasS;
            copyGCode.P = this->P;
            copyGCode.hasP = this->hasP;
            copyGCode.T = this->T;
            copyGCode.hasT = this->hasT;
            return copyGCode;
        }
    };
};

#endif // GCODE_DEFINITIONS_H