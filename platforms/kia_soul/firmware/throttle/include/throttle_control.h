#ifndef _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_
#define _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_


/**
 * @brief Accelerator spoof values.
 *
 * Contains the high and low accelerator spoof values.
 *
 */
typedef struct
{
    uint16_t low;
    uint16_t high;
} accelerator_spoof_t;


/**
 * @brief Current throttle state.
 *
 * Keeps track of the current state of the throttle system.
 *
 */
typedef struct
{
    uint16_t accel_pos_sensor_high; /* Value of high signal of accelerator position sensor */
    uint16_t accel_pos_sensor_low; /* Value of low signal of accelerator position sensor */
    float accel_pos_target; /* As specified by higher level controller */
} kia_soul_throttle_state_s;


/**
 * @brief Current throttle control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    bool operator_override; /* Flag indicating whether accelerator was manually pressed by operator */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_throttle_control_state_s;


// *****************************************************
// Function:    calculate_accelerator_spoof
//
// Purpose:     Container for hand-tuned empirically determined values
//
// Returns:     void
//
// Parameters:  [in] accelerator_target - floating point value that is the target accelerator position
//              [out] spoof - structure containing the spoofed accelerator position values
//
// *****************************************************
void calculate_accelerator_spoof(
    const float accelerator_target,
    accelerator_spoof_t * const spoof );


// *****************************************************
// Function:    check_accelerator_override
//
// Purpose:     This function checks to see if the vehicle's
//              operator has manually pressed the accelerator.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void check_accelerator_override( void );


// *****************************************************
// Function:    enable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void enable_control( void );


// *****************************************************
// Function:    disable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void disable_control( void );


#endif
