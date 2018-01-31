/**
 * @file oscc_check.cpp
 *
 */


#include <Arduino.h>

#include "oscc_check.h"
#include "vehicles.h"

bool fault_exceeded_duration(
        bool fault_active,
        unsigned long max_fault_duration,
        fault_state_s *state)
{
    bool faulted = false;

    if( fault_active == false )
    {
        /*
         * If a fault is not active, update the state to clear the fault active
         * flag and clear the last fault time.
         */
        state->fault_active = false;
        state->fault_start_time = 0;
    }
    else
    {
        unsigned long now = millis();

        if( state->fault_active == false )
        {
            /* We detected that a fault has just become active, update the state
             * to indicate the fault is active and track the fault start time
             * so we can determine if the fault has been active for longer than
             * the maximum fault duration.
             */
            state->fault_active = true;
            state->fault_start_time = now;
        }

        unsigned long fault_duration = now - state->fault_start_time;

        if( fault_duration >= max_fault_duration )
        {
            /* The fault has been active for longer than the maximum
             * acceptable duration.
             */
            faulted = true;
        }
    }

    return faulted;
}

bool check_voltage_grounded(
        uint16_t high,
        uint16_t low,
        unsigned long max_fault_duration,
        fault_state_s *fault_state )
{
    bool fault_active = (high == 0) || (low == 0);
    return fault_exceeded_duration(fault_active, max_fault_duration, fault_state);
}
