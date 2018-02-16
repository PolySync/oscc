/**
 * @file communications.cpp
 *
 */


#include "communications.h"
#include "dtc.h"
#include "globals.h"
#include "mcp_can.h"
#include "oscc_can.h"
#include "vehicles.h"


static void parse_brake_report( uint8_t *data );
static void parse_steering_report( uint8_t *data );
static void parse_throttle_report( uint8_t *data );


void check_for_module_reports( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        if ( rx_frame.id == OSCC_BRAKE_REPORT_CAN_ID )
        {
            parse_brake_report( rx_frame.data );
        }
        else if ( rx_frame.id == OSCC_STEERING_REPORT_CAN_ID )
        {
            parse_steering_report( rx_frame.data );
        }
        else if ( rx_frame.id == OSCC_THROTTLE_REPORT_CAN_ID )
        {
            parse_throttle_report( rx_frame.data );
        }
    }
}


void republish_obd_frames_to_control_can_bus( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_obd_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        if( (rx_frame.id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
            || (rx_frame.id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
            || (rx_frame.id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID) )
        {
            cli();
            g_control_can.sendMsgBuf(
                rx_frame.id,
                CAN_STANDARD,
                sizeof(rx_frame),
                (uint8_t *) &rx_frame.data );
            sei();
        }
    }
}


static void parse_brake_report( uint8_t *data )
{
    oscc_brake_report_s *report = (oscc_brake_report_s *) data;


    if ( report->enabled == 1 )
    {
        g_display_state.status_screen.brakes = MODULE_STATUS_ENABLED;
    }
    else
    {
        g_display_state.status_screen.brakes = MODULE_STATUS_DISABLED;
    }


    if ( report->dtcs != 0 )
    {
        g_display_state.status_screen.brakes = MODULE_STATUS_ERROR;
    }


    if ( DTC_CHECK(report->dtcs, OSCC_BRAKE_DTC_INVALID_SENSOR_VAL) != 0 )
    {
        g_display_state.dtc_screen.brakes[OSCC_BRAKE_DTC_INVALID_SENSOR_VAL] = true;
    }
    else
    {
        g_display_state.dtc_screen.brakes[OSCC_BRAKE_DTC_INVALID_SENSOR_VAL] = false;
    }

    if ( DTC_CHECK(report->dtcs, OSCC_BRAKE_DTC_OPERATOR_OVERRIDE) != 0 )
    {
        g_display_state.dtc_screen.brakes[OSCC_BRAKE_DTC_OPERATOR_OVERRIDE] = true;
    }
    else
    {
        g_display_state.dtc_screen.brakes[OSCC_BRAKE_DTC_OPERATOR_OVERRIDE] = false;
    }
}


static void parse_steering_report( uint8_t *data )
{
    oscc_steering_report_s *report = (oscc_steering_report_s *) data;


    if ( report->enabled == 1 )
    {
        g_display_state.status_screen.steering = MODULE_STATUS_ENABLED;
    }
    else
    {
        g_display_state.status_screen.steering = MODULE_STATUS_DISABLED;
    }


    if ( report->dtcs != 0 )
    {
        g_display_state.status_screen.steering = MODULE_STATUS_ERROR;
    }


    if ( DTC_CHECK(report->dtcs, OSCC_STEERING_DTC_INVALID_SENSOR_VAL) != 0 )
    {
        g_display_state.dtc_screen.steering[OSCC_STEERING_DTC_INVALID_SENSOR_VAL] = true;
    }
    else
    {
        g_display_state.dtc_screen.steering[OSCC_STEERING_DTC_INVALID_SENSOR_VAL] = false;
    }

    if ( DTC_CHECK(report->dtcs, OSCC_STEERING_DTC_OPERATOR_OVERRIDE) != 0 )
    {
        g_display_state.dtc_screen.steering[OSCC_STEERING_DTC_OPERATOR_OVERRIDE] = true;
    }
    else
    {
        g_display_state.dtc_screen.steering[OSCC_STEERING_DTC_OPERATOR_OVERRIDE] = false;
    }
}


static void parse_throttle_report( uint8_t *data )
{
    oscc_throttle_report_s *report = (oscc_throttle_report_s *) data;


    if ( report->enabled == 1 )
    {
        g_display_state.status_screen.throttle = MODULE_STATUS_ENABLED;
    }
    else
    {
        g_display_state.status_screen.throttle = MODULE_STATUS_DISABLED;
    }


    if ( report->dtcs != 0 )
    {
        g_display_state.status_screen.throttle = MODULE_STATUS_ERROR;
    }


    if ( DTC_CHECK(report->dtcs, OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL) != 0 )
    {
        g_display_state.dtc_screen.throttle[OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL] = true;
    }
    else
    {
        g_display_state.dtc_screen.throttle[OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL] = false;
    }

    if ( DTC_CHECK(report->dtcs, OSCC_THROTTLE_DTC_OPERATOR_OVERRIDE) != 0 )
    {
        g_display_state.dtc_screen.throttle[OSCC_THROTTLE_DTC_OPERATOR_OVERRIDE] = true;
    }
    else
    {
        g_display_state.dtc_screen.throttle[OSCC_THROTTLE_DTC_OPERATOR_OVERRIDE] = false;
    }
}
