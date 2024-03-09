/**
 * @file I2CDigitalIO.h
 * @brief This file contains the I2CDigitalOut class which is used to toggle a digital output on an I2C port
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef I2C_DIGITAL_IO_H
#define I2C_DIGITAL_IO_H

#include "I2CPin.h"

class I2CDigitalIO {
    public:
    /**
     * @brief Construct a new I2C Digital IO object
     * @param pin The pin to toggle
     * @param initialState The initial state of the pin
    */
    I2CDigitalIO(const I2CPin& pin, bool initialState = false);
    
    /**
     * @brief Initialize the pin
    */
    void Init();

    /**
     * @brief Set the state of the pin
     * @param value The value to set the pin to
    */
    void Set(bool value);

    /**
     * @brief Get the state of the pin
     * @return The state of the pin
    */
    bool Get();

    private:
    I2CPin pin;
};

#endif // I2C_DIGITAL_IO_H