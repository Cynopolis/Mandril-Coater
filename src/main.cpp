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
 * @brief Parse the serial message
 * @note Check that there is new data before calling this function
*/
void parseSerial(){
    int * args = serialMessage.GetArgs();
    int argsLength = serialMessage.GetArgsLength();
    int populatedArgs = serialMessage.GetPopulatedArgs();
    MessageType messageType = MessageType(args[0]);
    switch(messageType){
      case ESTOP:
        Serial.println("ESTOP");
        break;
      case MOVE:
        Serial.println("MOVE");
        break;
      case HOME:
        Serial.println("HOME");
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
  serialMessage.Update();
  if(serialMessage.IsNewData()){
    parseSerial();
  }
}