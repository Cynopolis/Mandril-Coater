/**
 * @file Endstop.h
 * @brief This file contains the Endstop class
 * @details This file contains the Endstop class which is used to control and read the endstop states
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef ENDSTOP_H
#define ENDSTOP_H

#include "I2CPin.h"
#include <PCF8574.h>

class Endstop{
    public:
        /**
         * @brief Construct a new Endstop object
         * @param pin The pin that the endstop is connected to via an I2C MUX
         * @param triggerType The type of trigger for the endstop (HIGH or LOW)
        */
        Endstop(I2CPin pin, uint8_t triggerType) : 
            pin(pin),
            triggerType(triggerType),
            triggeredHandler(triggeredHandler){
                isTriggered = false;
            }
        
        /**
         * @brief Initialize the endstop
         * @param triggeredHandler A pointer to a function to call when the endstop is triggered
         * @note This function must be called before the endstop can be used
        */
        void Init(void (*triggeredHandler)());

        /**
         * @brief Update the endstop so it knows its state
         * @note This function must be called in the main loop
        */
        void Update();

        /**
         * @brief Get the state of the endstop
         * @return True if the endstop is triggered, false if the endstop is not triggered
        */
        bool IsTriggered();

    private:
        const I2CPin pin;
        const uint8_t triggerType{LOW};
        bool isTriggered{false};
        // save a pointer to a function to callback when the endstop is triggered
        void (*triggeredHandler)(){NULL};
        uint32_t lastTriggeredTime{0};

};

#endif // ENDSTOP_H