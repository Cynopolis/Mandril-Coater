/**
 * @file I2CDigitalOut.h
 * @brief This file contains the I2CDigitalOut class which is used to toggle a digital output on an I2C port
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef I2C_DIGITAL_OUT_H
#define I2C_DIGITAL_OUT_H

#include "I2CPin.h"

class I2CDigitalOut {
    public:
    I2CDigitalOut(const I2CPin& pin) : pin(pin) {}
    void Set(bool value);
    bool Get();

    private:
    I2CPin pin;
    bool pinState;
};

#endif // I2C_DIGITAL_OUT_H