#pragma once
#include <cstdint>

struct I2CPin{
    // the pin number
    uint8_t number;
    // a pointer to the I2C port that the pin is connected to
    PCF8574* i2cPort;

    I2CPin(uint8_t pin, PCF8574* i2cPort) : number(pin), i2cPort(i2cPort){}
};