// Throttle control ECU firmware
// 2014 Kia Soul throttle system


#include <SPI.h>
#include "mcp_can.h"
#include "DAC_MCP49xx.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "throttle_module.h"
#include "init.h"
#include "communications.h"
#include "throttle_control.h"


void setup( )
{
    init_structs_to_zero( );

    init_pins( );

    init_interfaces( );

    publish_throttle_report( );

    control_state.enabled = false;
    control_state.emergency_stop = false;

    override_flags.accelerator_pressed = false;
    override_flags.voltage = 0;
    override_flags.voltage_spike_a = 0;
    override_flags.voltage_spike_b = 0;

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    DEBUG_PRINTLN( "init: pass" );
}


void loop()
{
    can_frame_s rx_frame;
    int ret = check_for_rx_frame( can, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
        handle_ready_rx_frame( &rx_frame );
    }

    publish_timed_report( );

    check_rx_timeouts( );

    throttle_state.accel_pos_sensor_low = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH ) << 2; //10 bit to 12 bit
    throttle_state.accel_pos_sensor_high = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW ) << 2;

    check_accelerator_override( );

    if ( control_state.enabled == true )
    {
        struct accel_spoof_t accel_spoof;

        calculate_accelerator_spoof(
                throttle_state.accel_pos_target,
                &accel_spoof );

        dac.outputA( accel_spoof.high );
        dac.outputB( accel_spoof.low );
    }
}
