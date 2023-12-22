/**
 * @file main.cpp
 * @brief This file contains the main start point and main loop for the code 
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

#include <Arduino.h>

#include "StepperMotor.h"
#include "SerialMessage.h"

// -------------------------------------------------
// ---------    GLOBAL OBJECTS    ------------------
// -------------------------------------------------
StepperMotor linearMotor(LINEAR_MOTOR_STEP_PIN, LINEAR_MOTOR_DIRECTION_PIN, LINEAR_MOTOR_ENABLE_PIN, STEPS_PER_MM);
StepperMotor rotationMotor(ROTATION_MOTOR_STEP_PIN, ROTATION_MOTOR_DIRECTION_PIN, ROTATION_MOTOR_ENABLE_PIN, STEPS_PER_REVOLUTION);
SerialMessage serialMessage(&Serial);

// -------------------------------------------------
// ---------    GLOBAL VARIABLES    ----------------
// -------------------------------------------------
bool isHoming = false; // Is true when the machine is returning to the home position

// -------------------------------------------------
// ---------    INTERRUPT HANDLERS    --------------
// -------------------------------------------------

/**
 * @brief The interrupt handler for the endstop 1
*/
static void endstop1TriggeredHandler(){
  Serial.println("Endstop 1 triggered");
  // oh no we hit the endstop and we need to stop now
  linearMotor.SetTargetPosition(ENDSTOP_1_POSITION);
  linearMotor.SetCurrentPosition(ENDSTOP_1_POSITION);
};

/**
 * @brief The interrupt handler for the endstop 2
*/
static void endstop2TriggeredHandler(){
  Serial.println("Endstop 2 triggered");
  // oh no we hit the endstop and we need to stop now
  linearMotor.SetTargetPosition(ENDSTOP_2_POSITION);
  linearMotor.SetCurrentPosition(ENDSTOP_2_POSITION);
};

/**
 * @brief The interrupt handler for the home switch
*/
static void homeSwitchTriggeredHandler(){
  Serial.println("Home switch triggered");
  // set the linear motor's position to the home position
  linearMotor.SetCurrentPosition(HOME_SWITCH_POSITION);
  if(isHoming){
    // stop the linear motor
    linearMotor.SetTargetPosition(HOME_SWITCH_POSITION);
    // set the is homing flag to false
    isHoming = false;
  }
};

// -------------------------------------------------
// ---------    SERIAL PARSING    ------------------
// -------------------------------------------------
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

// -------------------------------------------------
// ---------    SETUP AND LOOP    ------------------
// -------------------------------------------------

/**
 * @brief The setup function
*/
void setup() {
  serialMessage.Init(SERIAL_BAUD_RATE);
  Serial.println("Beginning Machine Setup");

  // <--------- interrupt setup ---------->
  pinMode(ENDSTOP_1_PIN, LIMIT_SWITCH_INPUT_MODE);
  pinMode(ENDSTOP_2_PIN, LIMIT_SWITCH_INPUT_MODE);
  pinMode(HOME_SWITCH_PIN, LIMIT_SWITCH_INPUT_MODE);
  attachInterrupt(digitalPinToInterrupt(ENDSTOP_1_PIN), endstop1TriggeredHandler, LIMIT_SWITCH_TRIGGERED_STATE);
  attachInterrupt(digitalPinToInterrupt(ENDSTOP_2_PIN), endstop2TriggeredHandler, LIMIT_SWITCH_TRIGGERED_STATE);
  attachInterrupt(digitalPinToInterrupt(HOME_SWITCH_PIN), homeSwitchTriggeredHandler, LIMIT_SWITCH_TRIGGERED_STATE);
  
  // <---------- motor setup ------------>
  linearMotor.Init();
  linearMotor.SetMaximums(LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN, LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN);
  linearMotor.SetEnabled(true);

  rotationMotor.Init();
  rotationMotor.SetMaximums(ROTATION_MOTOR_MAX_SPEED, ROTATION_MOTOR_MAX_ACCELERATION);
  rotationMotor.SetEnabled(true);

  Serial.println("Finished Machine Setup");
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