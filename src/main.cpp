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
#include <FastAccelStepper.h>
// freeRTOS libraries
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// internal libraries
#include "Endstop.h"
#include "GCodeMessage.h"
#include "I2CDigitalIO.h"
#include "MachineState.h"
#include "StepperMotor.h"

// -------------------------------------------------
// ---------    GLOBAL OBJECTS    ------------------
// -------------------------------------------------
FastAccelStepperEngine stepperEngine = FastAccelStepperEngine();
StepperMotor linearMotor(LINEAR_MOTOR_CONFIGURATION);
StepperMotor rotationMotor(ROTATION_MOTOR_CONFIGURATION);

// create Serial Object
GCodeQueue gcodeQueue;
GCodeMessage USBSerialMessage(&Serial, &gcodeQueue);
GCodeMessage displaySerialMessage(&Serial2, &gcodeQueue);

// create Endstop objects
Endstop homeEndstop(HOME_STOP_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop endstop1(ENDSTOP_1_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop endstop2(ENDSTOP_2_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop estop(ESTOP_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
// wtaches for the user to press the home button to trigger the homing sequence
Endstop homeButton(HOME_BUTTON_PIN, LIMIT_SWITCH_TRIGGERED_STATE);
Endstop resumeButton(RESUME_BUTTON_PIN, LIMIT_SWITCH_TRIGGERED_STATE);

// create digital output objects
I2CDigitalIO sprayer(SPRAYER_PIN);
I2CDigitalIO heater(HEATER_PIN);

// -------------------------------------------------
// ---------    GLOBAL VARIABLES    ----------------
// -------------------------------------------------
using namespace MachineState;

// -------------------------------------------------
// ----------    PRE DECLARATIONS    ---------------
// -------------------------------------------------
// Put functions here that need to be declared before their definitions

/**
 * @brief Move the motors to the specified positions at the specified speeds
 * @param args The arguments for the move command
 * @param argsLength The length of the args array
*/
void MOVE(int16_t linearMotorPosition, int16_t linearMotorSpeed, int16_t rotationMotorPosition, int16_t rotationMotorSpeed);

/**
 * @brief Move the motors to their home positions
*/
void HOME();

void STOP_MOVE();

// -------------------------------------------------
// -----------    ENDSTOP HANDLERS    --------------
// -------------------------------------------------

/**
 * @brief The handler for when the home endstop is triggered
*/
void HomeEndstopTriggered(){
  if(machineState.state == State::HOMING_INITIAL){
    Serial.println("Initial homing complete. Starting final homing.");
    STOP_MOVE();
    linearMotor.SetCurrentPosition(HOME_SWITCH_POSITION);
    rotationMotor.SetCurrentPosition(0);
    // move the motors back a little bit and then rehome but slower
    float backDistance = 50;
    float backSpeed = 300;
    Serial.println("Moving back a little bit");
    MOVE(backDistance, backSpeed, 0, 0);
    // wait for the motors to move backwards a little
    delay(static_cast<uint32_t>((backDistance/backSpeed) * 1000 * 60));
    Serial.println("Moving back to home position");
    SetMachineState(State::HOMING_FINAL);
    // move the motors back to the home position
    MOVE(static_cast<int16_t>(HOME_SWITCH_POSITION-backDistance), 150, 0, 30);
    SetMachineState(State::HOMING_FINAL);

  }
  else if(machineState.state == State::HOMING_FINAL){
    Serial.println("Finished homing.");
    SetMachineState(State::IDLE);
    STOP_MOVE();
    linearMotor.SetCurrentPosition(HOME_SWITCH_POSITION);
    rotationMotor.SetCurrentPosition(0);
    machineState.isHomed = true;
  }
  else{
    Serial.println("Home endstop triggered: " + String(machineState.state));
  }
}

/**
 * @brief The handler for when endstop 1 is triggered
*/
void Endstop1Triggered(){
  STOP_MOVE();
  linearMotor.SetCurrentPosition(ENDSTOP_1_POSITION);
  if(machineState.state == State::HOMING_INITIAL || machineState.state == State::HOMING_FINAL){
    Serial.println("Endstop 1 was triggered during homing. This is not good.");
    SetMachineState(State::IDLE);
  }
  Serial.println("Endstop 1 triggered");
}

/**
 * @brief The handler for when endstop 2 is triggered
*/
void Endstop2Triggered(){
  STOP_MOVE();
  linearMotor.SetCurrentPosition(ENDSTOP_2_POSITION);
  if(machineState.state == State::HOMING_INITIAL || machineState.state == State::HOMING_FINAL){
    Serial.println("Endstop 2 was triggered during homing. This is not good.");
    SetMachineState(State::IDLE);
  }
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
  linearMotor.Stop();
  rotationMotor.Stop();
  SetMachineState(State::EMERGENCY_STOP);
  Serial.println("ESTOPPED");
}

void RELEASE_ESTOP(){
  linearMotor.SetCurrentPosition(0);
  rotationMotor.SetCurrentPosition(0);
  linearMotor.Stop();
  rotationMotor.Stop();
  linearMotor.SetEnabled(true);
  rotationMotor.SetEnabled(true);
  machineState.isHomed = false;
  linearMotor.SetEnabled(true);
  rotationMotor.SetEnabled(true);
  SetMachineState(State::IDLE);
  Serial.println("ESTOP RELEASED");
}

/**
 * @brief Move the motors to the specified positions at the specified speeds
 * @param args The arguments for the move command
 * @param argsLength The length of the args array
*/
void MOVE(int16_t linearMotorPosition, int16_t linearMotorSpeed, int16_t rotationMotorPosition, int16_t rotationMotorSpeed){
  if(machineState.coordinateSystem == CoordinateSystem::RELATIVE){
    if(linearMotorSpeed != 0){
      linearMotorPosition += linearMotor.GetCurrentPosition();
    }
    if(rotationMotorSpeed != 0){
      rotationMotorPosition += rotationMotor.GetCurrentPosition();
    }
  }

  // TODO: These lines are good for debugging but do not let them reach main
  // Serial.println("Current position: X" + String(linearMotor.GetCurrentPosition()) + ",R" + String(rotationMotor.GetCurrentPosition()));
  // Serial.println("Target position: X" + String(linearMotorPosition) + ",R" + String(rotationMotorPosition));
  // Serial.println("Speed: F" + String(linearMotorSpeed) + ",S" + String(rotationMotorSpeed));
  
  if(linearMotorSpeed != 0){
    linearMotor.Stop();
    linearMotor.MoveToPosition(linearMotorPosition, linearMotorSpeed);
  }
  if(rotationMotorSpeed != 0){
    rotationMotor.Stop();
    rotationMotor.MoveToPosition(rotationMotorPosition, rotationMotorSpeed);
  }
}

/**
 * @brief Stop the motors from moving anymore
 * @note This function sets the motor's target position to their current position
*/
void STOP_MOVE(){
  linearMotor.Stop();
  rotationMotor.Stop();
  if(machineState.state == State::MOVING || machineState.state == State::NO_BLOCK_MOVING){
    SetMachineState(State::IDLE);
    Serial.println("Move stopped");
  }
}

/**
 * @brief Begin the homing process
*/
void HOME(){
  if(machineState.state != State::HOMING_INITIAL || machineState.state != State::HOMING_FINAL){
    Serial.println("Begin Homing.");
    SetMachineState(State::HOMING_INITIAL);
    MOVE(ENDSTOP_1_POSITION-10000, 2000, 0, 0);
  }
}

void SET_PIN(uint8_t pin_number, bool value){
  // invert the value here because the relay board is active low
  value = !value;
  
  if(pin_number < 8){
    // Serial.print("Port 1, pin ");
    // Serial.print(pin_number);
    // Serial.print(" value ");
    // Serial.println(value);
    i2c_output_port_1.write(pin_number, value);
  }
  else if(pin_number < 16){
    // Serial.print("Port 2, pin ");
    // Serial.print(pin_number - 8);
    // Serial.print(" value ");
    // Serial.println(value);
    i2c_output_port_2.write(pin_number - 8, value);
  }
  else{
    Serial.println("Invalid pin number");
  }
}

void RESUME(){
  if(machineState.state == State::PAUSED){
    Serial.println("Resume button pressed. Resuming Program.");
    SetMachineState(State::IDLE);
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
void parseSerial(GCodeDefinitions::GCode &gcode){
    using namespace GCodeDefinitions;

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
        Serial2.println("!M2;");
        break;
      
      // G4: Wait a specified amount of time in ms
      case Command::G4:
        Serial.println("!G4;");
        Serial2.println("!G4;");
        SetMachineState(State::WAITING);
        // STOP_MOVE();
        machineState.waitTime = gcode.T;
        break;
      
      // M0: Emergency stop
      case Command::M0:
        Serial.println("!M0;");
        Serial2.println("!M0;");
        ESTOP();
        break;
      
      // M1: Release the emergency stop
      case Command::M1:
        Serial.println("!M1;");
        Serial2.println("!M1;");
        RELEASE_ESTOP();
        break;
      
      // M24: Pause/Resume
      case Command::M24:
        Serial.println("!M24;");
        Serial2.println("!M24;");
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
        Serial.print(rotationMotor.GetCurrentPosition());
        Serial.println(";");

        Serial2.print("!M114,X");
        Serial2.print(linearMotor.GetCurrentPosition());
        Serial2.print(",R");
        Serial2.print(rotationMotor.GetCurrentPosition());
        Serial2.println(";");
        break;
      
      // G91: Relative positioning 
      case Command::G91:
        Serial.println("!G91;");
        Serial2.println("!G91;");
        machineState.coordinateSystem = CoordinateSystem::RELATIVE;
        break;
      
      // G90: Absolute positioning
      case Command::G90:
        Serial.println("!G90;");
        Serial2.println("!G90;");
        machineState.coordinateSystem = CoordinateSystem::ABSOLUTE;
        break;
      
      // M208: Set max travel
      case Command::M208:
        Serial.println("!M208;");
        Serial2.println("!M208;");
        linearMotor.SetMaxTravel(gcode.X);
        break;

      // M92: Set steps per unit
      case Command::M92:
        Serial.println("M92 IS UNIMPLIMENTED AND HAS NO PLAN TO BE IMPLIMENTED");
        break;
      
      // G1: Controlled move
      case Command::G1:{
        Serial.println("!G1;");
        Serial2.println("!G1;");

        float rotationalFeedRate = gcode.P;
        if(!gcode.hasP){
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
          rotationalFeedRate = static_cast<float>(gcode.F) * r_change / x_change;
        }

        MOVE(gcode.X, gcode.F, gcode.R, static_cast<int16_t>(rotationalFeedRate));

        SetMachineState(State::MOVING);
        break;
      }
      
      // G0: Move but non-blocking for other commands
      case Command::G0:
        Serial.println("!G0;");
        Serial2.println("!G0;");
        MOVE(gcode.X, gcode.F, gcode.R, gcode.P);
        SetMachineState(State::NO_BLOCK_MOVING);
        break;
    
      // G28: Home
      case Command::G28:
        Serial.println("!G28;");
        Serial2.println("!G28;");
        HOME();
        break;
      
      // M42: Set pin
      case Command::M42:{
        Serial.println("!M42;");
        Serial2.println("!M42;");
        // the pin will remain low unless the S parameter is 1
        bool pinState = false;
        if(gcode.S == 1){
          pinState = true;
        }
        SET_PIN(gcode.P, pinState);
        break;
      }
      // M112: Non-emergency cancel current operation
      case Command::M112:
        Serial.println("!M112;");
        Serial2.println("!M112;");
        STOP_MOVE();
        SetMachineState(State::IDLE);
        break;
      
      // G92: Set current position
      case Command::G92:
        Serial.println("!G92;");
        Serial2.println("!G92;");
        linearMotor.SetCurrentPosition(gcode.X);
        rotationMotor.SetCurrentPosition(gcode.R);
        break;
      
      // M204: Set decceleration
      case Command::M204:
        Serial.println("!M204;");
        Serial2.println("!M204;");
        if(gcode.hasX){
          linearMotor.SetAcceleration(gcode.X);
        }
        if(gcode.hasR){
          rotationMotor.SetAcceleration(gcode.R);
        }

        if(!gcode.hasR && !gcode.hasX){
          linearMotor.SetAcceleration(LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN);
          rotationMotor.SetAcceleration(ROTATION_MOTOR_MAX_ACCELERATION);
        }
        break;

      // M999: Jump to command (relative)
      case Command::M999:
        Serial.println("!M999;");
        Serial2.println("!M999;");
        // If the number of times to jump is greater than 0...
        if(gcode.T != 0){
          // try to set the queue index back by the given offset. The +1 is to account for the M999 command itself
          if(!gcodeQueue.MoveBack(gcode.P + 1)){
            Serial.println("Invalid jump command. Index out of range.");
          }
          // decrement the number of times to jump
          gcode.T--;
        }
        break;
      
      default:
        Serial.println("Something went wrong parsing the command");
        break;
    }
}

void CheckGCodeInbox(GCodeMessage &messageHandler){
  if(gcodeQueue.Size() > 0){
    // check if we're in a state to parse this serial command
    if(IsCommandParsableInState(gcodeQueue.Peek()->command, machineState.state)){
      parseSerial(*(gcodeQueue.Peek()));
      // if we parsed the data, pop it from the queue
      gcodeQueue.Pop();
      // clear the new data flag
      messageHandler.ClearNewData();
    }
    else{
      // if we're estopped let's check our queue and see if there is a command to release the estop
      if(machineState.state == State::EMERGENCY_STOP){
        // go through all of the commands in the queue and see if one of them is an M1 command
        for(uint16_t i = 0; i < messageHandler.GetQueueSize(); i++){
          if(messageHandler.PeekGCode(i)->command == GCodeDefinitions::Command::M1){
            RELEASE_ESTOP();
            messageHandler.PopGCode(i);
            if(messageHandler.GetQueueSize() == 0){
              messageHandler.ClearNewData();
            }
            break;
          }
        }
      }
    }

    if((USBSerialMessage.GetQueueSize() + displaySerialMessage.GetQueueSize()) == 0 && machineState.state == State::IDLE){
      Serial.println("!0;");
      Serial2.println("!0;");
    }
  }
}

// -------------------------------------------------
// ------------    FreeRTOS Tasks    ---------------
// -------------------------------------------------
/**
 * @brief This task updates the stepper motors
*/
void EndstopUpdateTask(void * pvParameters){
  Serial.println("Endstop Update Task Started");
  // update the motors
  for(;;){
    // update the endstops
    homeEndstop.Update();
    endstop1.Update();
    endstop2.Update();
    estop.Update();
    homeButton.Update();
    resumeButton.Update();
    vTaskDelay(1);
  }
  ESTOP();
  Serial.println("Endstop Update Task Ended! This is NOT good!");
}

/**
 * @brief This task updates the machine state
*/
void MachineStateUpdateTask(void * pvParameters){
  Serial.println("Machine State Update Task Started");
  for(;;){
    // update the machine state
    UpdateMachineState();

    // if we are in the moving state and the motors are not moving, stop moving
    if(machineState.state == State::MOVING && !(linearMotor.IsMoving() || rotationMotor.IsMoving())){
      SetMachineState(State::IDLE);
    }

    // if we have finished our homing sequence, but the machine is still not homed, set the alarm pin
    if((machineState.state == State::HOMING_INITIAL || machineState.state == State::HOMING_FINAL) && !linearMotor.IsMoving()){
        // set ALARM_PIN_NUMBER to high
        Serial.println("Homing failed");
        SET_PIN(ALARM_PIN_NUMBER, true);
        delay(5000);
        SET_PIN(ALARM_PIN_NUMBER, false);
        SetMachineState(State::IDLE);
    }

    // if we are in the ping state and don't hear back from the controller in the time promised, stop moving
    if(machineState.state == State::PING){
      if(machineState.timeEnteredState - millis() >= machineState.waitTime){
        Serial.println("Ping timeout");
        STOP_MOVE();
        SetMachineState(State::IDLE);
      }
    }
    vTaskDelay(5);
  }
  ESTOP();
  Serial.println("Machine State Update Task Ended! This is NOT good!");
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

  // <---------- motor setup ------------>
  // IMPORTANT: The stepper motor driver is connected to the I2C bus, so we need to initialize the I2C bus first
  stepperEngine.init();
  linearMotor.Init(stepperEngine);
  linearMotor.SetEnabled(true);

  rotationMotor.Init(stepperEngine);
  rotationMotor.SetEnabled(true);

  // <---------- endstop setup ------------>
  // IMPORTANT: The endstops are connected to the I2C bus, so we need to initialize the I2C bus first
  // You also need to initialize the stepper motor driver before the endstops
  homeEndstop.Init(HomeEndstopTriggered);
  endstop1.Init(Endstop1Triggered);
  endstop2.Init(Endstop2Triggered);
  estop.Init(ESTOP, RELEASE_ESTOP);
  homeButton.Init(HOME);
  resumeButton.Init(RESUME);


  // This is pinned to core 0, so keep in mind 
  // WiFi and Bluetooth will be on this core if that's being used
  xTaskCreatePinnedToCore(
    EndstopUpdateTask,   /* Function to implement the task */
    "EndstopUpdateTask", /* Name of the task */
    2048,             /* Stack size in words */
    NULL,              /* Task input parameter */
    3,                 /* Priority of the task */
    NULL,              /* Task handle. */
    0  /* Core where the task should run */
  );

  xTaskCreate(
    MachineStateUpdateTask,   /* Function to implement the task */
    "MachineStateUpdateTask", /* Name of the task */
    2048,             /* Stack size in words */
    NULL,              /* Task input parameter */
    tskIDLE_PRIORITY,  /* Priority of the task */
    NULL               /* Task handle. */
  );

  Serial.println("Finished Machine Setup");
}

/**
 * @brief The main loop
 * @note this function uses core 1
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
  
  // Process any commands we have in the queue
  CheckGCodeInbox(USBSerialMessage);
  CheckGCodeInbox(displaySerialMessage);   
}