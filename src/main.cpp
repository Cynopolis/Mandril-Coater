#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

#include <Arduino.h>

#include "StepperMotor.h"
#include "SerialMessage.h"


StepperMotor linearMotor(LINEAR_MOTOR_STEP_PIN, LINEAR_MOTOR_DIRECTION_PIN, LINEAR_MOTOR_ENABLE_PIN, STEPS_PER_MM);
StepperMotor rotationMotor(ROTATION_MOTOR_STEP_PIN, ROTATION_MOTOR_DIRECTION_PIN, ROTATION_MOTOR_ENABLE_PIN, STEPS_PER_REVOLUTION);
SerialMessage serialMessage(&Serial);

/**
 * @brief The setup function
*/
void setup() {
  serialMessage.Init(SERIAL_BAUD_RATE);
  Serial.println("Beginning Machine Setup");
  
  // motor setup
  linearMotor.Init();
  linearMotor.SetMaximums(LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN, LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN);
  linearMotor.SetEnabled(true);

  rotationMotor.Init();
  rotationMotor.SetMaximums(ROTATION_MOTOR_MAX_SPEED, ROTATION_MOTOR_MAX_ACCELERATION);
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