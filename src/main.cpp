#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>

#include "StepperMotor.h"
#include "SerialMessage.h"

// Create I2C Objects
TwoWire I2C_BUS(0);

PCF8574 i2c_output_port_1(PCF8574_OUT_1_8_ADDRESS, &I2C_BUS);
PCF8574 i2c_output_port_2(PCF8574_OUT_9_16_ADDRESS, &I2C_BUS);
PCF8574 i2c_input_port_1(PCF8574_IN_1_8_ADDRESS, &I2C_BUS);
PCF8574 i2c_input_port_2(PCF8574_IN_9_16_ADDRESS, &I2C_BUS);

StepperMotor linearMotor(&i2c_output_port_1, LINEAR_MOTOR_CONFIGURATION);
StepperMotor rotationMotor(&i2c_output_port_1, ROTATION_MOTOR_CONFIGURATION);

// create Serial Object
SerialMessage serialMessage(&Serial);

/**
 * @brief The setup function
*/
void setup() {
  // serialMessage.Init(SERIAL_BAUD_RATE);
  Serial.begin(SERIAL_BAUD_RATE); 
  Serial.println("Beginning Machine Setup");
  
  // Begin I2C Setup
  I2C_BUS.begin(SDA_PIN, SCL_PIN, 100000);
  i2c_output_port_1.begin();
  i2c_output_port_2.begin();
  i2c_input_port_1.begin();
  i2c_input_port_2.begin();

  // motor setup
  linearMotor.Init();
  linearMotor.SetEnabled(true);

  rotationMotor.Init();
  rotationMotor.SetEnabled(true);

  Serial.println("Finished Machine Setup");
}

/**
 * @brief Emergency stop
*/
void ESTOP(){
  linearMotor.SetEnabled(false);
  rotationMotor.SetEnabled(false);
  Serial.println("ESTOP");
}

/**
 * @brief Move the motors to the specified positions at the specified speeds
 * @param args The arguments for the move command
 * @param argsLength The length of the args array
*/
void MOVE(int * args, int argsLength){
  Serial.println("MOVE");
  if(argsLength != 5){
    Serial.println("Invalid number of arguments");
    return;
  }
  int linearMotorPosition = args[1];
  int linearMotorSpeed = args[2];
  int rotationMotorPosition = args[3];
  int rotationMotorSpeed = args[4];
  linearMotor.SetTargetPosition(linearMotorPosition);
  linearMotor.SetSpeed(linearMotorSpeed);
  rotationMotor.SetTargetPosition(rotationMotorPosition);
  rotationMotor.SetSpeed(rotationMotorSpeed);
}

/**
 * @brief Move the motors to their home positions
*/
void HOME(){
  Serial.println("HOME");
  // TODO: Have the motors move until the home limit switch is hit
  linearMotor.SetTargetPosition(0);
  rotationMotor.SetTargetPosition(0);
}

/**
 * @brief Parse the serial message
 * @note Check that there is new data before calling this function
*/
void parseSerial(){
    int * args = serialMessage.GetArgs();
    int argsLength = serialMessage.GetPopulatedArgs();
    int populatedArgs = serialMessage.GetPopulatedArgs();
    MessageTypes::MessageType messageType = MessageTypes::MessageType(args[0]);
    switch(messageType){
      case MessageTypes::MessageType::ESTOP:
        ESTOP();
        break;
      case MessageTypes::MessageType::MOVE:
        MOVE(args, argsLength);
        break;
      case MessageTypes::MessageType::HOME:
        HOME();
        break;
      default:
        Serial.println("Invalid Message Type");
        break;
    }
    serialMessage.ClearNewData();
}

/**
 * @brief The main loop
*/
void loop() {
  // check for new serial data
  serialMessage.Update();
  if(serialMessage.IsNewData()){
    parseSerial();
  }

  // update the motors
  linearMotor.Update();
  rotationMotor.Update();
}