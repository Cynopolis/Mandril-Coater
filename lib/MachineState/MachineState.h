/**
 * @file MachineState.h
 * @brief This file contains a state machine which is used to control the machine
 * @version 1.0.0
 * @author Quinn Henthorne. Contact: quinn.henthorne@gmail.com
*/

#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <Arduino.h>

namespace MachineState{
    // all possible states of the machine which could interfere with executing another command
    enum State : uint8_t{
        IDLE, // default machine state. Machine can process new commands in this state
        HOMING, // machine is attempting to travel towards home. No new movement commands can be processed in this state
        MOVING, // machine is moving. No new movement commands can be processed in this state
        PAUSED, // machine is paused. No actuation commands can be processed in this state
        EMERGENCY_STOP, // machine is in emergency stop. No commands can be processed in this state besides emergency stop release
        ERROR, // machine is in error state. No commands can be processed in this state. Machine must be power cycled
        PING, // machine is in ping state. will continue moving/actuating, but the only command that will be processed is a ping
        WAITING // machine is waiting for a given amount of time. No commands can be processed in this state
    };

    enum CoordinateSystem : uint8_t{
        ABSOLUTE,
        RELATIVE
    };

    struct MachineStateTracker{
        State state;
        CoordinateSystem coordinateSystem;
        unsigned long timeEnteredState;
        unsigned long waitTime;
        bool isHomed; // true if the machine has been homed at least once
    };

    // this instance will be used to track the machine state
    MachineStateTracker machineState = {
        .state = MachineState::State::IDLE,
        .coordinateSystem = MachineState::CoordinateSystem::ABSOLUTE,
        .timeEnteredState = 0,
        .waitTime = 0,
        .isHomed = false
    };

    /**
     * @brief This function is used to set the machine state
     * @param state The state to set the machine to
     * @note this will update the timeEnteredState variable
    */
    void SetMachineState(State state){
        machineState.state = state;
        machineState.timeEnteredState = millis();
        // if we're in the IDLE state we probably don't want a wait time
        if(State::IDLE){
            machineState.waitTime = 0;
        }
    }

    /**
     * @brief Update the machine state
    */
    void UpdateMachineState(){
        // if we are in the waiting state, check if we have waited long enough
        if(machineState.state == State::WAITING){
            if(millis() - machineState.timeEnteredState >= machineState.waitTime){
                SetMachineState(State::IDLE);
                Serial.println("Wait time complete");
            }
        }
    }
    

    /**
     * @brief Check if a command should be run in the current machine state
     * @param command The command to check
     * @param state The current machine state
     * @return true if the command can be run in the current state, false otherwise
    */
    bool IsCommandParsableInState(GCodeDefinitions::Command command, State state){
        // for each state there is a list of commands that can be executed in that state
        // if the command is in that list, return true
        // otherwise return false

        // for the idle state, all commands are valid
        if(state == State::IDLE){
            return true;
        }

        // for the homing and moving state, only move commands are invalid
        if(state == State::HOMING || state == State::MOVING){
            switch(command){
            case GCodeDefinitions::Command::G0:
            case GCodeDefinitions::Command::G1:
                return false;
            default:
                return true;
            }
        }

        // for the paused state, only pause/resume commands are valid
        if(state == State::PAUSED){
            switch(command){
            case GCodeDefinitions::Command::M24:
                return true;
            default:
                return false;
            }
        }

        // for the emergency stop state, only emergency stop release commands are valid
        if(state == State::EMERGENCY_STOP){
            switch(command){
            case GCodeDefinitions::Command::M1:
                return true;
            default:
                return false;
            }
        }

        // for the error state, no commands are valid
        if(state == State::ERROR){
            return false;
        }

        // for the ping state, only ping and G0 commands are valid
        if(state == State::PING){
            switch(command){
            case GCodeDefinitions::Command::M2:
            case GCodeDefinitions::Command::G0:
                return true;
            default:
                return false;
            }
        }

        // for the waiting state, only ESTOP is valid, but that is covered above so return false
        if(state == State::WAITING){
            return false;
        }

        // if we get here we don't know what's going on
        // by default if we don't know what's going on, we don't want to execute any commands
        return false;
    }
};

#endif // MACHINE_STATE_H