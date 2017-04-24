#ifndef _OSCC_KIA_SOUL_BRAKE_CONTROL_H_
#define _OSCC_KIA_SOUL_BRAKE_CONTROL_H_


// *****************************************************
// Function:    brake_lights_off
//
// Purpose:     Turn the brake lights off
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_lights_off( void );


// *****************************************************
// Function:    brake_lights_on
//
// Purpose:     Turn the brake lights on
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_lights_on( void );


// *****************************************************
// Function:    brake_command_actuator_solenoids
//
// Purpose:     Sets the PWM that controls the "actuator" solenoids to the
//              the specified value
//
// Returns:     void
//
// Parameters:  uint16_t - duty_cycle - value to send to the PWM
//
// *****************************************************
void brake_command_actuator_solenoids( uint16_t duty_cycle );


// *****************************************************
// Function:    brake_command_release_solenoids
//
// Purpose:     Sets the PWM that controls the "release" solenoids to the
//              the specified value
//
// Returns:     void
//
// Parameters:  uint16_t - duty_cycle - value to send to the PWM
//
// *****************************************************
void brake_command_release_solenoids( uint16_t duty_cycle );


// *****************************************************
// Function:    brake_enable
//
// Purpose:     Enable brakes
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_enable( void );


// *****************************************************
// Function:    brake_disable
//
// Purpose:     Disable the brakes
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_disable( void );


// *****************************************************
// Function:    brake_check_driver_override
//
// Purpose:     This function checks the voltage input from the brake pedal
//              sensors to determine if the driver is attempting to brake
//              the vehicle.  This must be done over time by taking
//              periodic samples of the input voltage, calculating the
//              difference between the two and then passing that difference
//              through a basic exponential filter to smooth the input.
//
//              The required response time for the filter is 250 ms, which at
//              50ms per sample is 5 samples.  As such, the alpha for the
//              exponential filter is 0.5 to make the input go "close to" zero
//              in 5 samples.
//
//              The implementation is:
//                  s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )
//
//              If the filtered input exceeds the max voltage, it is an
//              indicator that the driver is pressing on the brake pedal
//              and the control should be disabled.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_check_driver_override( void );


// *****************************************************
// Function:    brake_update_pressure
//
// Purpose:     Update brake pressure
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_update_pressure( void );


// *****************************************************
// Function:    brake_init
//
// Purpose:     Initializes the brakes
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_init( void );


// *****************************************************
// Function:    brake_update
//
// Purpose:     Update function for the brake module
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_update( void );


#endif