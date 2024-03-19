#ifndef PINOUT_H
#define PINOUT_H

#include <stdint.h>
#include <Wire.h>
#include <PCF8574.h>
#include <FastAccelStepper.h>

#include "I2CPin.h"


// <------- I2C Definitions ---------->
#define SDA_PIN 4
#define SCL_PIN 5
#define PCF8574_OUT_1_8_ADDRESS 0x24
#define PCF8574_OUT_9_16_ADDRESS 0x25
#define PCF8574_IN_1_8_ADDRESS 0x22
#define PCF8574_IN_9_16_ADDRESS 0x21

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

// <------- Serial Definitions ---------->
#define RX2_PIN 16
#define TX2_PIN 13

// <------stepper motor pin definitions------->
#define LINEAR_MOTOR_STEP_PIN_NUMBER 32 // This is HT1 on the board
#define LINEAR_MOTOR_DIRECTION_PIN_NUMBER 1
#define LINEAR_MOTOR_ENABLE_PIN_NUMBER 2
I2CPin LINEAR_MOTOR_DIRECTION_PIN(LINEAR_MOTOR_DIRECTION_PIN_NUMBER, &i2c_output_port_1);
I2CPin LINEAR_MOTOR_ENABLE_PIN(LINEAR_MOTOR_ENABLE_PIN_NUMBER, &i2c_output_port_1);

#define ROTATION_MOTOR_STEP_PIN_NUMBER 33 // This is HT2 on the board
#define ROTATION_MOTOR_DIRECTION_PIN_NUMBER 4
#define ROTATION_MOTOR_ENABLE_PIN_NUMBER 5
I2CPin ROTATION_MOTOR_DIRECTION_PIN(ROTATION_MOTOR_DIRECTION_PIN_NUMBER, &i2c_output_port_1);
I2CPin ROTATION_MOTOR_ENABLE_PIN(ROTATION_MOTOR_ENABLE_PIN_NUMBER, &i2c_output_port_1);

// <------ Endstop pin definitions-------->
#define ENDSTOP_1_PIN_NUMBER 0
#define ENDSTOP_2_PIN_NUMBER 1
#define HOME_STOP_PIN_NUMBER 2

I2CPin ENDSTOP_1_PIN(ENDSTOP_1_PIN_NUMBER, &i2c_input_port_1);
I2CPin ENDSTOP_2_PIN(ENDSTOP_2_PIN_NUMBER, &i2c_input_port_1);
I2CPin HOME_STOP_PIN(HOME_STOP_PIN_NUMBER, &i2c_input_port_1);

// <------ Miscelaneous pin definitions-------->
#define ESTOP_PIN_NUMBER 3
#define HOME_BUTTON_PIN_NUMBER 4
#define SPRAYER_PIN_NUMBER 6
#define HEATER_PIN_NUMBER 7
I2CPin ESTOP_PIN(ESTOP_PIN_NUMBER, &i2c_input_port_1);
I2CPin HOME_BUTTON_PIN(HOME_BUTTON_PIN_NUMBER, &i2c_input_port_1);
I2CPin SPRAYER_PIN(SPRAYER_PIN_NUMBER, &i2c_output_port_1);
I2CPin HEATER_PIN(HEATER_PIN_NUMBER, &i2c_output_port_1);

#endif // PINOUT_H