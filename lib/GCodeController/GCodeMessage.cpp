#include "GCodeMessage.h"

void GCodeMessage::ClearNewData(){
    // only set the data flag to false if the queue is empty
    if(this->queue.size() == 0){
        this->new_data = false;
        Serial.println("GCodeMessage::ClearNewData(): Cleared new data flag");
    }
}

GCodeDefinitions::GCode * GCodeMessage::PopGCode(){
    GCodeDefinitions::GCode * command = this->queue.pop();
    if(this->queue.size() == 0){
        this->new_data = false;
    }
    return command;
}

void GCodeMessage::parseData(){
    uint16_t messageLength = strlen(this->data);
    GCodeDefinitions::GCode newCommand = this->parseGCodeString(this->data, messageLength);
    // if the command is an estop command then set the estop command received flag to true and stop parsing
    if(newCommand.command == GCodeDefinitions::Command::M0){
        this->estopCommandReceived = true;
    }
    // otherwise just push the command to the queue to be used later
    else{
        this->queue.push(newCommand);
    }
}


GCodeDefinitions::GCode GCodeMessage::parseGCodeString(char *message, uint16_t length){

    GCodeDefinitions::GCode newCommand = GCodeDefinitions::GCode();

    // the string will be organized as follows:
    //!COMMAND,X###,R###,F###,S###,P###;
    // where everything after the command is optional
    // Resetting everything
    this->ClearNewData();
    // make all of the characters uppercase to remove any imput variation.
    this->capitalize(message);

    // reset last command
    newCommand = GCodeDefinitions::GCode();

    // the maximum length of one of any of the values is 8 characters
    char temp[15];
    uint8_t tempLength = 0;
    // get the command
    while(message[tempLength] != ',' && tempLength < length){
        temp[tempLength] = message[tempLength];
        tempLength++;
    }
    // parse the command
    newCommand.command = matchToCommand(temp, tempLength);

    // reset the temp array
    memset(temp, 0, 8);
    uint8_t dataStart = tempLength + 1;
    tempLength = 0;

    // get the rest of the values
    for(uint16_t i = dataStart; i < length; i++){
        char c = message[i];
        if(c == ','){
            // parse the value
            this->populateCommandWithData(&newCommand, temp, tempLength);
            // reset the temp array
            memset(temp, 0, 15);
            tempLength = 0;
        }
        // we don't append the ';' at the end of the string, so we check if this is our last character by using i
        else if (i == length - 1){
            temp[tempLength] = c;
            tempLength++;
            this->populateCommandWithData(&newCommand, temp, tempLength);
            break;
        }
        else{
            temp[tempLength] = c;
            tempLength++;
        }
    }

    // set the new data flag to true if our message isn't invalid
    this->new_data = (newCommand.command != GCodeDefinitions::Command::INVALID);

    // return the parsed command
    return newCommand;
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

void GCodeMessage::populateCommandWithData(GCodeDefinitions::GCode *command, char *str, uint8_t length){
    // the first character of the string will be the value type
    char valueType = str[0];
    // the rest of the string will be the value
    char value[15];
    for(uint8_t i = 1; i < length; i++){
        value[i - 1] = str[i];
    }
    value[length - 1] = '\0';
    // parse the value
    int32_t parsedValue = atoi(value);

    // populate the last command with the data
    switch(valueType){
        case 'X':
            command->X = parsedValue;
            command->hasX = true;
            break;
        case 'R':
            command->R = parsedValue;
            command->hasR = true;
            break;
        case 'F':
            command->F = parsedValue;
            command->hasF = true;
            break;
        case 'S':
            command->S = parsedValue;
            command->hasS = true;
            break;
        case 'P':
            command->P = parsedValue;
            command->hasP = true;
            break;
        case 'T':
            command->T = parsedValue;
            command->hasT = true;
            break;
        default:
            command->command = GCodeDefinitions::Command::INVALID;
            break;
    }
}

void GCodeMessage::capitalize(char *str){
    for(uint8_t i = 0; i < strlen(str); i++){
        str[i] = toupper(str[i]);
    }
}