#ifndef PINOUT_H
#define PINOUT_H

#include <stdint.h>
#include <Wire.h>
#include <PCF8574.h>

#include "I2CPin.h"


// <------- I2C Definitions ---------->
#define SDA_PIN 4
#define SCL_PIN 5
#define PCF8574_OUT_1_8_ADDRESS 0x24
#define PCF8574_OUT_9_16_ADDRESS 0x25
#define PCF8574_IN_1_8_ADDRESS 0x21
#define PCF8574_IN_9_16_ADDRESS 0x22

// Create I2C Objects
TwoWire I2C_BUS(0);

PCF8574 i2c_output_port_1(PCF8574_OUT_1_8_ADDRESS, &I2C_BUS);
PCF8574 i2c_output_port_2(PCF8574_OUT_9_16_ADDRESS, &I2C_BUS);
PCF8574 i2c_input_port_1(PCF8574_IN_1_8_ADDRESS, &I2C_BUS);
PCF8574 i2c_input_port_2(PCF8574_IN_9_16_ADDRESS, &I2C_BUS);

// <------- Ethernet Definitions ---------->
// Type: LAN8720
#define MDC_PIN 23
#define MDIO_PIN 18
#define ETH_CLK_MODE 17 // out
#define PHYSICAL_ADDRESS 0

// <------stepper motor pin definitions------->
I2CPin LINEAR_MOTOR_STEP_PIN(0, &i2c_output_port_1);
I2CPin LINEAR_MOTOR_DIRECTION_PIN(1, &i2c_output_port_1);
I2CPin LINEAR_MOTOR_ENABLE_PIN(2, &i2c_output_port_1);

I2CPin ROTATION_MOTOR_STEP_PIN(3, &i2c_output_port_1);
I2CPin ROTATION_MOTOR_DIRECTION_PIN(4, &i2c_output_port_1);
I2CPin ROTATION_MOTOR_ENABLE_PIN(5, &i2c_output_port_1);

// <------ Endstop pin definitions-------->
I2CPin ENDSTOP_1_PIN(0, &i2c_input_port_1);
I2CPin ENDSTOP_2_PIN(1, &i2c_input_port_1);
I2CPin HOME_STOP_PIN(2, &i2c_input_port_1);

// <------ Miscelaneous pin definitions-------->
I2CPin ESTOP_PIN(3, &i2c_input_port_1);
I2CPin SPRAYER_PIN(6, &i2c_output_port_1);
I2CPin HEATER_PIN(7, &i2c_output_port_1);

#endif // PINOUT_H