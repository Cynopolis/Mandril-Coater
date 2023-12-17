/**
 * @file StepperMotor.h
 * @brief This file contains the StepperMotor class
 * @details This file contains the StepperMotor class which is used to control a stepper motor
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <AccelStepper.h>

class StepperMotor {
    public:
        /**
         * @brief Construct a new Stepper Motor object
         * @param stepPin The pin that will incriment the stepper motor
         * @param directionPin The pin that will change the direction of the stepper motor
         * @param enablePin The pin that will enable the stepper motor
        */
        StepperMotor(uint8_t stepPin, uint8_t directionPin, uint8_t enablePin, float stepsPerUnit) :
            stepPin(stepPin),
            directionPin(directionPin),
            enablePin(enablePin),
            stepsPerUnit(stepsPerUnit),
            stepper(AccelStepper::DRIVER, stepPin, directionPin) {}

        /**
         * @brief Initialize the stepper motor
         */
        void Init();
        
        /**
         * @brief Set the motor maximums
         * @param maxSpeed The maximum speed of the motor
         * @param acceleration The acceleration of the motor
        */
        void SetMaximums(uint32_t maxSpeed, uint32_t acceleration);
        
        /**
         * @brief Set the speed of the motor
         * @param speed The speed of the motor
         * @note This function currently just changes the maximum speed, so if the 
         * acceleration is low, you will see slow speed changes
        */
        void SetSpeed(uint32_t speed);

        /**
         * @brief Set the target position of the motor
         * @param position The target position of the motor
        */
        void SetTargetPosition(int32_t position);

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
        /**
         * @brief Converts units to steps
         * @param units The units to convert to steps
         * @return The number of steps
        */
        uint32_t unitsToSteps(uint32_t units);

        const uint8_t stepPin; // The pin that will incriment the stepper motor
        const uint8_t directionPin; // The pin that will change the direction of the stepper motor
        const uint8_t enablePin; // The pin that will enable the stepper motor
        AccelStepper stepper; // The stepper motor
    
    protected:
        float stepsPerUnit = 1; // The number of steps per unit of the motor. The unit is defined by the user like steps per mm or steps per degree
};

#endif // STEPPER_MOTOR_H