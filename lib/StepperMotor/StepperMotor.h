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
        StepperMotor(StepperMotorConfiguration *configuration) :
            i2cPort(configuration->directionPin.i2cPort),
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

        /**
         * @brief Returns the current position of the motor
         * @return The current position of the motor
        */
        int32_t GetCurrentPosition();

        /**
         * @brief Returns the target position of the motor
         * @return The target position of the motor
        */
        int32_t GetTargetPosition();

        /**
         * @brief Returns the speed of the motor
         * @return The speed of the motor
         */
        uint32_t GetSpeed();

        /**
         * @brief Set the max travel of the motor
         * @param maxTravel The max travel of the motor in units
        */
        void SetMaxTravel(int32_t maxTravel);



    
    private:
        PCF8574* i2cPort;
        StepperMotorConfiguration *configuration;

        /**
         * @brief Updates the direction pin
         * @note Before you call this function, you should take the updateInProgressMutex
        */
        void updateDirectionPin();
    
    protected:
        int32_t currentSteps = 0; // (Used by update)
        int32_t targetSteps = 0; // (Used by update)
        int8_t direction = 1; // The direction of the motor. 1 for forward, -1 for backward (used by update)
        uint32_t period = 0; // The period of the square wave to generate in us/step (used by update)
        uint32_t timeOfLastStep = 0; // The time of the last step in microseconds (used by update)
        int32_t maxTravel = 0; // If this is 0, there is no max travel.
        // This mutex shoudl be taken when changing variables that the update function uses
        SemaphoreHandle_t updateInProgressMutex = xSemaphoreCreateMutex();
};

#endif // STEPPER_MOTOR_H