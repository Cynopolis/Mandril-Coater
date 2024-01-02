/**
 * @file I2CPin.h
 * @brief This file contains the I2CPin struct
 * @details This file contains the I2CPin struct which is used to store the pin number and the I2C port that the pin is connected to
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef I2C_PIN_H
#define I2C_PIN_H

#include <cstdint>
#include <PCF8574.h>

struct I2CPin{
    // the pin number
    uint8_t number;
    // a pointer to the I2C port that the pin is connected to
    PCF8574* i2cPort;

    I2CPin(uint8_t pin, PCF8574* i2cPort) : number(pin), i2cPort(i2cPort){}
};

#endif // I2C_PIN_H