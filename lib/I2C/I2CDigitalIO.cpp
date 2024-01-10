/**
 * @file I2CDigitalOut.cpp
 * @brief This file contains the I2CDigitalOut implimentation which is used to toggle a digital output on an I2C port
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "Arduino.h"
#include "I2CDigitalIO.h"

I2CDigitalIO::I2CDigitalIO(const I2CPin& pin, bool initialState) : pin(pin) {
    this->Set(initialState);
}

void I2CDigitalIO::Set(bool value) {
    this->pin.i2cPort->write(this->pin.number, value);
}

bool I2CDigitalIO::Get() {
    return this->pin.i2cPort->read(this->pin.number);
}