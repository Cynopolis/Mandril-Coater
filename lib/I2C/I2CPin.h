#pragma once
#include <cstdint>

struct I2CPin{
    uint8_t pin;
    uint8_t address;

    I2CPin(uint8_t pin, uint8_t address) : pin(pin), address(address){}
};