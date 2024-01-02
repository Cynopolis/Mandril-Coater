/**
 * @file main.cpp
 * @brief This file contains the main start point and main loop for the code 
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>

#include "StepperMotor.h"
#include "SerialMessage.h"

// -------------------------------------------------
// ---------    GLOBAL OBJECTS    ------------------
// -------------------------------------------------
StepperMotor linearMotor(&i2c_output_port_1, LINEAR_MOTOR_CONFIGURATION);
StepperMotor rotationMotor(&i2c_output_port_1, ROTATION_MOTOR_CONFIGURATION);

// create Serial Object
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
  
  
  // <---------- motor setup ------------>
  // Begin I2C Setup
  I2C_BUS.begin(SDA_PIN, SCL_PIN, 100000);
  i2c_output_port_1.begin();
  i2c_output_port_2.begin();
  i2c_input_port_1.begin();
  i2c_input_port_2.begin();

  linearMotor.Init();
  linearMotor.SetEnabled(true);

  rotationMotor.Init();
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