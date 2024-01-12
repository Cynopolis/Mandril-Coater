#include "GCodeMessage.h"

void GCodeMessage::ClearNewData(){
    this->new_data = false;
    this->lastCommand = {
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
}

GCodeDefinitions::GCode * GCodeMessage::GetGCode(){
    return &(this->lastCommand);
}

void GCodeMessage::parseData(){
    uint16_t messageLength = strlen(this->data);
    // parse the message
    this->parseGCodeString(this->data, messageLength);
}


void GCodeMessage::parseGCodeString(char *message, uint16_t length){
    // the string will be organized as follows:
    //!COMMAND,X###,R###,F###,S###,P###;
    // where everything after the command is optional
    // Resetting everything
    this->ClearNewData();
    // make all of the characters uppercase to remove any imput variation.
    this->capitalize(message);

    // reset last command
    this->lastCommand = {
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
    char temp[15];
    uint8_t tempLength = 0;
    // get the command
    while(message[tempLength] != ',' && tempLength < length){
        temp[tempLength] = message[tempLength];
        tempLength++;
    }
    // parse the command
    this->lastCommand.command = matchToCommand(temp, tempLength);

    // reset the temp array
    memset(temp, 0, 8);
    uint8_t dataStart = tempLength + 1;
    tempLength = 0;

    // get the rest of the values
    for(uint16_t i = dataStart; i < length; i++){
        char c = message[i];
        if(c == ','){
            // parse the value
            this->populateLastCommandWithData(temp, tempLength);
            // reset the temp array
            memset(temp, 0, 15);
            tempLength = 0;
        }
        // we don't append the ';' at the end of the string, so we check if this is our last character by using i
        else if (i == length - 1){
            temp[tempLength] = c;
            tempLength++;
            this->populateLastCommandWithData(temp, tempLength);
            break;
        }
        else{
            temp[tempLength] = c;
            tempLength++;
        }
    }

    // set the new data flag to true if our message isn't invalid
    this->new_data = (this->lastCommand.command != GCodeDefinitions::Command::INVALID);
}

GCodeDefinitions::Command GCodeMessage::matchToCommand(char *str, uint8_t length){
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
                return (GCodeDefinitions::Command)i;
            }
        }
    }

    return GCodeDefinitions::Command::INVALID;
}

void GCodeMessage::populateLastCommandWithData(char *str, uint8_t length){
    // the first character of the string will be the value type
    char valueType = str[0];
    // the rest of the string will be the value
    char value[15];
    for(uint8_t i = 1; i < length; i++){
        value[i - 1] = str[i];
    }
    value[length - 1] = '\0';
    // parse the value
    uint16_t parsedValue = atoi(value);

    // populate the last command with the data
    switch(valueType){
        case 'X':
            this->lastCommand.X = parsedValue;
            this->lastCommand.hasX = true;
            break;
        case 'R':
            this->lastCommand.R = parsedValue;
            this->lastCommand.hasR = true;
            break;
        case 'F':
            this->lastCommand.F = parsedValue;
            this->lastCommand.hasF = true;
            break;
        case 'S':
            this->lastCommand.S = parsedValue;
            this->lastCommand.hasS = true;
            break;
        case 'P':
            this->lastCommand.P = parsedValue;
            this->lastCommand.hasP = true;
            break;
        case 'T':
            this->lastCommand.T = parsedValue;
            this->lastCommand.hasT = true;
            break;
        default:
            this->lastCommand.command = GCodeDefinitions::Command::INVALID;
            break;
    }
}

void GCodeMessage::capitalize(char *str){
    for(uint8_t i = 0; i < strlen(str); i++){
        str[i] = toupper(str[i]);
    }
}