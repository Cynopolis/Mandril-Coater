/**
 * @file SerialMessage.h
 * @brief This file contains the SerialMessage class
 * @details This file contains the SerialMessage class which is used to parse serial messages
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef SERIALMESSAGE_H
#define SERIALMESSAGE_H

#include "Arduino.h"

#define num_chars 500

class SerialMessage{
    public:
        /**
         * @brief Construct a new Serial Message object
         */
        SerialMessage(HardwareSerial *serial = &Serial);

        /**
         * @brief Initialize the SerialMessage object
         */
        virtual void Init(unsigned int baud_rate = 115200);

        /**
         * @brief Update the SerialMessage object and parse any data that's available
         */
        void Update();

        /**
         * @brief Returns true if there is new data available
         * @return true if there is new data available
         */
        bool IsNewData();

        /**
         * @brief Clears the new data flag
         */
        virtual void ClearNewData();

        /**
         * @brief Return a pointer to the args array
         * @return a pointer to the args array
         */
        int * GetArgs();

        /**
         * @brief Returns the number of args that have been populated for the current message
         * @return the number of args that have been populated for the current message
         */
        int GetArgsLength();

        /**
         * @brief Returns the number of args that have been populated for the current message
         * @return the number of args that have been populated for the current message
         */
        int GetPopulatedArgs();

        /**
         * @brief Prints the args array to the serial monitor
         */
        virtual void PrintArgs();

    protected:
        virtual void readSerial();
        virtual void parseData();

        bool new_data = false;
        bool data_recieved = false;
        bool recvInProgress = false;
        char data[num_chars]; // an array to store the received data
        char temp_data[num_chars]; // an array that will be used with strtok()
        uint16_t ndx = 0;
        const static int args_length = 30;
        int populated_args = 0; // the number of args that have been populated for the current message
        int args[args_length];
        const char startMarker = '!';
        const char endMarker = ';';
    
    private:
        HardwareSerial *serial;
};

#endif