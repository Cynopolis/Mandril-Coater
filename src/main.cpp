#include "PINOUT.h"
#include "MACHINE-PARAMETERS.h"

#include <Arduino.h>
#include "StepperMotor.h"


StepperMotor linearMotor(LINEAR_MOTOR_STEP_PIN, LINEAR_MOTOR_DIRECTION_PIN, LINEAR_MOTOR_ENABLE_PIN, STEPS_PER_MM);
StepperMotor rotationMotor(ROTATION_MOTOR_STEP_PIN, ROTATION_MOTOR_DIRECTION_PIN, ROTATION_MOTOR_ENABLE_PIN, STEPS_PER_REVOLUTION);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Beginning Machine Setup");
  
  // motor setup
  linearMotor.Init();
  linearMotor.setMaximums(LINEAR_MOTOR_MAX_SPEED_MM_PER_MIN, LINEAR_MOTOR_MAX_ACCELERATION_MM_PER_MIN_PER_MIN);
  
  rotationMotor.Init();
  rotationMotor.setMaximums(ROTATION_MOTOR_MAX_SPEED_DEGREES_PER_MIN, ROTATION_MOTOR_MAX_ACCELERATION_DEGREES_PER_MIN_PER_MIN);
}

void loop() {
  // put your main code here, to run repeatedly:
}