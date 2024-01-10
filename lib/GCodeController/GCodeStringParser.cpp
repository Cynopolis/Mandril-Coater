#include "GCodeStringParser.h"

GCodeParser::GCode * ParseGCodeString(char *message, int length){
    // the string will be organized as follows:
    //!COMMAND,X###,R###,F###,S###,P###;
    // where everything after the command is optional

    // the maximum length of one of any of the values is 8 characters
    char temp[8];

    // get the command
    int i = 0;
    while(message[i] != ',' && i < length){
        temp[i] = message[i];
        i++;
    }
}