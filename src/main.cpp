/**
 * @file main.cpp
 * @brief This file contains the main start point and main loop for the code 
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/
// definitions
#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

// external libraries
#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

// internal libraries
#include "Endstop.h"
#include "GCodeMessage.h"
#include "I2CDigitalIO.h"
#include "MachineState.h"
#include "StepperMotor.h"

// -------------------------------------------------
// ---------    GLOBAL OBJECTS    ------------------
// -------------------------------------------------
StepperMotor linearMotor(LINEAR_MOTOR_CONFIGURATION);
StepperMotor rotationMotor(ROTATION_MOTOR_CONFIGURATION);

// create Serial Object
GCodeMessage USBSerialMessage(&Serial);
GCodeMessage displaySerialMessage(&Serial2);

// create Endstop objects
Endstop homeEndstop(HOME_STOP_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop endstop1(ENDSTOP_1_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop endstop2(ENDSTOP_2_PIN, LIMIT_SWITCH_TRIGGERED_STATE);

// create digital output objects
I2CDigitalIO estop(ESTOP_PIN);
I2CDigitalIO sprayer(SPRAYER_PIN);
I2CDigitalIO heater(HEATER_PIN);

// -------------------------------------------------
// ---------    GLOBAL VARIABLES    ----------------
// -------------------------------------------------
using namespace MachineState;

// -------------------------------------------------
// -----------    ENDSTOP HANDLERS    --------------
// -------------------------------------------------

/**
 * @brief The handler for when we are homed
 */
void HOMED(){
  linearMotor.SetTargetPosition(HOME_SWITCH_POSITION);
  linearMotor.SetCurrentPosition(HOME_SWITCH_POSITION);
  rotationMotor.SetTargetPosition(HOME_SWITCH_POSITION);
  rotationMotor.SetCurrentPosition(HOME_SWITCH_POSITION);
  machineState.isHomed = true;
  SetMachineState(State::IDLE);
}

/**
 * @brief The handler for when the home endstop is triggered
*/
void HomeEndstopTriggered(){
  if (machineState.state == State::HOMING)
  {
    HOMED();
    Serial.println("Homing endstop triggered. Homing Complete.");
  }
}

/**
 * @brief The handler for when endstop 1 is triggered
*/
void Endstop1Triggered(){
  linearMotor.SetTargetPosition(ENDSTOP_1_POSITION);
  linearMotor.SetCurrentPosition(ENDSTOP_1_POSITION);
  Serial.println("Endstop 1 triggered");
}

/**
 * @brief The handler for when endstop 2 is triggered
*/
void Endstop2Triggered(){
  linearMotor.SetTargetPosition(ENDSTOP_2_POSITION);
  linearMotor.SetCurrentPosition(ENDSTOP_2_POSITION);
  Serial.println("Endstop 2 triggered");
}

// -------------------------------------------------
// ---------    MACHINE COMMANDS    ----------------
// -------------------------------------------------
/**
 * @brief Emergency stop
*/
void ESTOP(){
  linearMotor.SetEnabled(false);
  rotationMotor.SetEnabled(false);
  SetMachineState(State::EMERGENCY_STOP);
  Serial.println("ESTOPPED");
}

/**
 * @brief Move the motors to the specified positions at the specified speeds
 * @param args The arguments for the move command
 * @param argsLength The length of the args array
*/
void MOVE(int16_t linearMotorPosition, int16_t linearMotorSpeed, int16_t rotationMotorPosition, int16_t rotationMotorSpeed){
  if(machineState.coordinateSystem == CoordinateSystem::RELATIVE){
    linearMotorPosition += linearMotor.GetCurrentPosition();
    rotationMotorPosition += rotationMotor.GetCurrentPosition();
  }

  linearMotor.SetTargetPosition(linearMotorPosition);
  linearMotor.SetSpeed(linearMotorSpeed);
  rotationMotor.SetTargetPosition(rotationMotorPosition);
  rotationMotor.SetSpeed(rotationMotorSpeed);
}

/**
 * @brief Stop the motors from moving anymore
 * @note This function sets the motor's target position to their current position
*/
void STOP_MOVE(){
  linearMotor.SetTargetPosition(linearMotor.GetCurrentPosition());
  rotationMotor.SetTargetPosition(rotationMotor.GetCurrentPosition());
}

/**
 * @brief Move the motors to their home positions
*/
void HOME(){
  Serial.println("HOME");
  // TODO: Have the motors move until the home limit switch is hit
  linearMotor.SetTargetPosition(-4000);
  rotationMotor.SetTargetPosition(0);
  MOVE(-4000, 1000, 0, 1000);
  SetMachineState(State::HOMING);
  machineState.isHomed = false;
}

void SET_PIN(uint8_t pin_number, bool value){
  // invert the value here because the relay board is active low
  value = !value;
  
  if(pin_number < 8){
    Serial.print("Port 1, pin ");
    Serial.print(pin_number);
    Serial.print(" value ");
    Serial.println(value);
    i2c_output_port_1.write(pin_number, value);
  }
  else if(pin_number < 16){
    Serial.print("Port 2, pin ");
    Serial.print(pin_number - 8);
    Serial.print(" value ");
    Serial.println(value);
    i2c_output_port_2.write(pin_number - 8, value);
  }
  else{
    Serial.println("Invalid pin number");
  }
}

// -------------------------------------------------
// ---------    SERIAL PARSING    ------------------
// -------------------------------------------------

/**
 * @brief Parse the serial message
 * @note Check that there is new data before calling this function
 * @return true if the message should be consumed
*/
bool parseSerial(const GCodeDefinitions::GCode &gcode){
    using namespace GCodeDefinitions;

    // check if we're in a state to parse this serial command
    if(!IsCommandParsableInState(gcode.command, machineState.state)){
      // if we're not in a state to parse this command, ignore it and don't pop it from the queue
      return false;
    }

    switch(gcode.command){
      // Invalid command so do nothing
      case Command::INVALID:
        Serial.println("Invalid command! Ignoring.");
                break;
      
      // M2: Ping
      case Command::M2:
        // if we recieved a ping, log the time and send a ping back
        machineState.timeEnteredState = millis();
        Serial.println("!M2;");
        break;
      
      // G4: Wait a specified amount of time in ms
      case Command::G4:
        Serial.println("!G4;");
        SetMachineState(State::WAITING);
        machineState.waitTime = gcode.T;
        break;
      
      // M0: Emergency stop
      case Command::M0:
        Serial.println("!M0;");
        ESTOP();
        break;
      
      // M1: Release the emergency stop
      case Command::M1:
        Serial.println("!M1;");
        STOP_MOVE();
        linearMotor.SetEnabled(true);
        rotationMotor.SetEnabled(true);
        break;
      
      // M24: Pause/Resume
      case Command::M24:
        Serial.println("!M24;");
        if(gcode.S == 0){
          SetMachineState(State::PAUSED);
        }
        else if(gcode.S == 1){
          SetMachineState(State::IDLE);
        }
        break;
      
      // M114: Get the current position of the motors
      case Command::M114:
        Serial.print("!M114,X");
        Serial.print(linearMotor.GetCurrentPosition());
        Serial.print(",R");
        Serial.print(rotationMotor.GetTargetPosition());
        Serial.print(",F");
        Serial.print(linearMotor.GetSpeed());
        Serial.print(",S");
        Serial.print(rotationMotor.GetSpeed());
        Serial.println(";");
        break;
      
      // G91: Relative positioning 
      case Command::G91:
        Serial.println("!G91;");
        machineState.coordinateSystem = CoordinateSystem::RELATIVE;
        break;
      
      // G90: Absolute positioning
      case Command::G90:
        Serial.println("!G90;");
        machineState.coordinateSystem = CoordinateSystem::ABSOLUTE;
        break;
      
      // M208: Set max travel
      case Command::M208:
        Serial.println("!M208;");
        linearMotor.SetMaxTravel(gcode.X);
        break;

      // M92: Set steps per unit
      case Command::M92:
        Serial.println("M92 IS UNIMPLIMENTED AND HAS NO PLAN TO BE IMPLIMENTED");
        break;
      
      // G1: Controlled move
      case Command::G1:{
        Serial.println("!G1;");
        // calculate the rotational motor speed to the rotation finished at the same time as the linear motor
        // r_speed = (x_speed * r_change) / x_change
        float x_change = static_cast<float>(gcode.X - linearMotor.GetCurrentPosition());
        float r_change = static_cast<float>(gcode.R - rotationMotor.GetCurrentPosition());
        // if we are in relative mode, the given values are already our changes
        if(machineState.coordinateSystem == CoordinateSystem::RELATIVE){
          x_change = static_cast<float>(gcode.X);
          r_change = static_cast<float>(gcode.R);
        }

        // no division by 0 on my watch
        if(x_change == 0){
          x_change = 1;
        }

        float rotationalFeedRate = static_cast<float>(gcode.F) * r_change / x_change;
        uint16_t rotationalMotorSpeed = 0;
        MOVE(gcode.X, gcode.F, gcode.R, static_cast<uint16_t>(rotationalFeedRate));
        break;
      }
      
      // G0: Move with a ping timeout
      case Command::G0:
        Serial.println("!G0;");
        // if we recieve S0, stop the motors
        if(gcode.S == 0){
          STOP_MOVE();
          SetMachineState(State::IDLE);
        }

        MOVE(gcode.X, gcode.F, gcode.R, gcode.P);
        SetMachineState(State::PING);
        // if the user doesn't ping us every 500ms, stop moving
        machineState.waitTime = 500;
        break;
    
      // G28: Home
      case Command::G28:
        Serial.println("!G28;");
        HOME();
        break;
      
      // M42: Set pin
      case Command::M42:{
        Serial.println("!M42;");
        // the pin will remain low unless the S parameter is 1
        bool pinState = false;
        if(gcode.S == 1){
          pinState = true;
        }
        SET_PIN(gcode.P, pinState);
        break;
      }
      
      default:
        Serial.println("Something went wrong parsing the command");
        break;
    }

    return true;
}

// -------------------------------------------------
// ---------    SETUP AND LOOP    ------------------
// -------------------------------------------------

/**
 * @brief The setup function
*/
void setup() {
  // <---------- Serial setup ------------>
  USBSerialMessage.Init(SERIAL_BAUD_RATE);
  // we initialize the display serial message differently because it's using different pins
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN);
  Serial.println("Beginning Machine Setup");
  
  // <---------- I2C setup ------------>
  I2C_BUS.begin(SDA_PIN, SCL_PIN, 100000);
  i2c_output_port_1.begin();
  i2c_output_port_2.begin();
  i2c_input_port_1.begin();
  i2c_input_port_2.begin();

  // <---------- endstop setup ------------>
  homeEndstop.Init(HomeEndstopTriggered);
  endstop1.Init(Endstop1Triggered);
  endstop2.Init(Endstop2Triggered);

  // <---------- motor setup ------------>
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
  USBSerialMessage.Update();
  displaySerialMessage.Update();
  // check to see if we recieved an ESTOP
  if(USBSerialMessage.EStopCommandReceived() || displaySerialMessage.EStopCommandReceived()){
    ESTOP();
    USBSerialMessage.ClearNewData();
    displaySerialMessage.ClearNewData();
  }
  
  if(USBSerialMessage.IsNewData()){
    // try to parse the new data
    if(parseSerial(*(USBSerialMessage.PeekGCode()))){
      // if we parsed the data, pop it from the queue
      USBSerialMessage.PopGCode();
      // clear the new data flag
      USBSerialMessage.ClearNewData();
    }
  }

  if(displaySerialMessage.IsNewData()){
    // try to parse the new data
    if(parseSerial(*(displaySerialMessage.PeekGCode()))){
      // if we parsed the data, pop it from the queue
      displaySerialMessage.PopGCode();
      // clear the new data flag
      displaySerialMessage.ClearNewData();
    }
  }

  // update the motors
  if(machineState.state != State::PAUSED){
    linearMotor.Update();
    rotationMotor.Update();
  }

  // update the endstops
  homeEndstop.Update();
  endstop1.Update();
  endstop2.Update();

  // poll our input pins
  if(estop.Get()){
    ESTOP();
  }

  UpdateMachineState();

  // if we are in the ping state and don't hear back from the controller in the time promised, stop moving
  if(machineState.state == State::PING){
    if(machineState.timeEnteredState - millis() >= machineState.waitTime){
      Serial.println("Ping timeout");
      STOP_MOVE();
      SetMachineState(State::IDLE);
    }
  }
}