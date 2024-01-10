#include "GCodeStringParser.h"

GCodeDefinitions::GCode * ParseGCodeString(char *message, uint16_t length){
    // the string will be organized as follows:
    //!COMMAND,X###,R###,F###,S###,P###;
    // where everything after the command is optional

    // make all of the characters uppercase to remove any imput variation.
    GCodeParser::capitalize(message);

    // reset last command
    GCodeParser::lastCommand = {
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

    // the maximum length of one of any of the values is 8 characters
    char temp[8];
    uint8_t tempLength = 0;
    // get the command
    while(message[tempLength] != ',' && tempLength < length){
        temp[tempLength] = message[tempLength];
        tempLength++;
    }
    // parse the command
    GCodeParser::lastCommand.command = matchToCommand(temp, tempLength);

    // reset the temp array
    memset(temp, 0, 8);
    uint8_t dataStart = tempLength + 1;
    tempLength = 0;

    // get the rest of the values
    for(uint16_t i = dataStart; i < length; i++){
        char c = message[i];
        if(c == ','){
            // parse the value
            GCodeParser::populateLastCommandWithData(temp, tempLength);
            // reset the temp array
            memset(temp, 0, 8);
            tempLength = 0;
        }
        else if (c == ';'){
            GCodeParser::populateLastCommandWithData(temp, tempLength);
            break;
        }
        else{
            temp[tempLength] = c;
            tempLength++;
        }
    }

    return &GCodeParser::lastCommand;
}

GCodeDefinitions::Command matchToCommand(char *str, uint8_t length){
    GCodeDefinitions::Command command = GCodeDefinitions::Command::INVALID;

    // go through each command string and see if it matches
    for(int i = 0; i < GCodeDefinitions::commandStringLength; i++){
        // check if the strings match
        for(int j = 0; j < length; j++){
            // if the strings don't match then break
            if(str[j] != GCodeDefinitions::commandStrings[i][j]){
                break;
            }
            // if the strings match and we are at the end of the string then we have a match
            if(j == length - 1){
                command = (GCodeDefinitions::Command)i;
            }
        }
    }

    return command;
}

void populateLastCommandWithData(char *str, uint8_t length){
    // the first character of the string will be the value type
    char valueType = str[0];
    // the rest of the string will be the value
    char value[8];
    for(uint8_t i = 1; i < length; i++){
        value[i - 1] = str[i];
    }
    // parse the value
    uint16_t parsedValue = atoi(value);

    // populate the last command with the data
    switch(valueType){
        case 'X':
            GCodeParser::lastCommand.X = parsedValue;
            GCodeParser::lastCommand.hasX = true;
            break;
        case 'R':
            GCodeParser::lastCommand.R = parsedValue;
            GCodeParser::lastCommand.hasR = true;
            break;
        case 'F':
            GCodeParser::lastCommand.F = parsedValue;
            GCodeParser::lastCommand.hasF = true;
            break;
        case 'S':
            GCodeParser::lastCommand.S = parsedValue;
            GCodeParser::lastCommand.hasS = true;
            break;
        case 'P':
            GCodeParser::lastCommand.P = parsedValue;
            GCodeParser::lastCommand.hasP = true;
            break;
        default:
            GCodeParser::lastCommand.command = GCodeDefinitions::Command::INVALID;
            break;
    }
}

void capitalize(char *str){
    for(uint8_t i = 0; i < strlen(str); i++){
        str[i] = toupper(str[i]);
    }
}