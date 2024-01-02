/**
 * @file StepperMotor.h
 * @brief This file contains the StepperMotor class
 * @details This file contains the StepperMotor class which is used to control a stepper motor
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <PCF8574.h>
#include "StepperMotorConfiguration.h"

class StepperMotor {
    public:
        /**
         * @brief Construct a new Stepper Motor object
         * @param I2CPort A pointer to the I2C handler
         * @param configuration The configuration of the motor
        */
        StepperMotor(StepperMotorConfiguration &configuration) :
            i2cPort(configuration.stepPin.i2cPort),
            configuration(configuration){}

        /**
         * @brief Initialize the stepper motor
         */
        void Init();
        
        /**
         * @brief Set the speed of the motor
         * @param speed The speed of the motor in unites per minute
         * @note This function currently just changes the maximum speed, so if the 
         * acceleration is low, you will see slow speed changes
        */
        void SetSpeed(float speed);

        /**
         * @brief Set the target position of the motor
         * @param position The target position of the motor
        */
        void SetTargetPosition(int32_t position);

        /**
         * @brief Set the current position of the motor
         * @param position The current position of the motor
        */
        void SetCurrentPosition(int32_t position);

        /**
         * @brief Incriments the motor given the current position and the target position
        */
        void Update();

        /**
         * @brief disable/enable the motor
         * @param enabled True to enable the motor, false to disable the motor
        */
        void SetEnabled(bool enabled);

    
    private:
        PCF8574* i2cPort;
        StepperMotorConfiguration configuration;
    
    protected:
        float stepsPerUnit = 1; // The number of steps per unit of the motor. The unit is defined by the user like steps per mm or steps per degree
        float maxSpeed = 1; // The maximum speed of the motor in units per minute
        float targetPosition = 0; // The target position of the motor in units
        float currentPosition = 0; // The current position of the motor in units
        float direction = 1; // The direction of the motor. 1 for forward, -1 for backward
        uint32_t period = 0; // The period of the square wave to generate in us/step
        uint32_t timeOfLastStep = 0; // The time of the last step in microseconds
};

#endif // STEPPER_MOTOR_H