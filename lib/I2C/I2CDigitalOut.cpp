/**
 * @file I2CDigitalOut.cpp
 * @brief This file contains the I2CDigitalOut implimentation which is used to toggle a digital output on an I2C port
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Arduino.h"
#include "I2CDigitalOut.h"

void I2CDigitalOut::Set(bool value) {
    this->pinState = value;
    this->pin.i2cPort->write(this->pin.number, value);
}

bool I2CDigitalOut::Get() {
    return this->pinState;
}