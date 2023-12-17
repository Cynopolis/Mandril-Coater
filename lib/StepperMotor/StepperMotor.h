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
        void setMaximums(uint32_t maxSpeed, uint32_t acceleration);

        /**
         * @brief Set the target position of the motor
         * @param position The target position of the motor
        */
        void setTargetPosition(int32_t position);

        /**
         * @brief Incriments the motor given the current position and the target position
        */
        void Update();

        /**
         * @brief disable/enable the motor
         * @param enabled True to enable the motor, false to disable the motor
        */
        void setEnabled(bool enabled);

    
    private:
        const uint8_t stepPin; // The pin that will incriment the stepper motor
        const uint8_t directionPin; // The pin that will change the direction of the stepper motor
        const uint8_t enablePin; // The pin that will enable the stepper motor
        AccelStepper stepper; // The stepper motor
    
    protected:
        float stepsPerUnit = 1; // The number of steps per unit of the motor. The unit is defined by the user like steps per mm or steps per degree
};