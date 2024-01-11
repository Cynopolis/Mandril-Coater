/**
 * @file SerialMessage.cpp
 * @brief This file contains the SerialMessage class
 * @details This file contains the SerialMessage class which is used to parse serial messages
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "SerialMessage.h"

SerialMessage::SerialMessage(HardwareSerial *serial) :
    serial(serial){}

void SerialMessage::Init(unsigned int baud_rate){
    serial->begin(baud_rate);
}

void SerialMessage::readSerial(){
    char c;

    // read the incoming serial data:
    while (serial->available() > 0 && data_recieved == false) {
        // get the neext character in the serial buffer
        c = serial->read();
        // only execute this if the startMarker has been received
        // if the incoming character is the endMarker clean up and set the flags
        if (recvInProgress == true) {
            if (c == endMarker) {
                data[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                data_recieved = true;
            }
            // if the incoming character is not the endMarker
            else {
                // add it to the data array
                data[ndx] = c;
                ndx++; // increment the data array index
                // if the index is greater than the maximum data array size,
                // keep overwriting the last element until the endMarker is received.
                if (ndx >= num_chars) {
                    ndx = num_chars - 1;
                }
            }
        }
        // if the incoming character is the startMarker, set the recvInProgress flag
        else if (c == startMarker) {
            recvInProgress = true;
        }
    }
}

void SerialMessage::parseData() {      // split the data into its parts
    this->populated_args = 0; // reset the populated args counter
    char * indx; // this is used by strtok() as an index
    int i = 0;
    indx = strtok(temp_data, ",");      // get the first part - the string
    while(indx != NULL){
        this->args[i] = atoi(indx);
        populated_args++;
        i++;
        indx = strtok(NULL, ","); // this continues where the previous call left off
    }
}

void SerialMessage::Update(){
    readSerial();
    if (data_recieved == true) {
        // for debug only:
        // Serial.print("Received:");
        // Serial.print(data);
        // Serial.println(":End");
        strcpy(temp_data, data);
        // this temporary copy is necessary to protect the original data
        //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        //PrintArgs();
        data_recieved = false;
        new_data = true;
    }
}

bool SerialMessage::IsNewData(){
    return new_data;
}

void SerialMessage::ClearNewData(){
    new_data = false;
}

int * SerialMessage::GetArgs(){
    return args;
}

int SerialMessage::GetArgsLength(){
    return args_length;
}

int SerialMessage::GetPopulatedArgs(){
    return populated_args;
}

void SerialMessage::PrintArgs(){
    serial->print("Current number of args: ");
    serial->println(populated_args);
    for (int i = 0; i < populated_args; i++) {
        serial->print(args[i]);
        serial->print(" ");
    }
    serial->println();
}