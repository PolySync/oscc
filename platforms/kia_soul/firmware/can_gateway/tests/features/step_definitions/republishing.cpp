#include "kia_soul.h"


WHEN("^a (.*) OBD CAN frame is received on the OBD CAN bus$")
{
    REGEX_PARAM( std::string, obd_frame_type );

    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    if( obd_frame_type == "steering wheel angle" )
    {
        g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;
    }
    else if( obd_frame_type == "wheel speed" )
    {
        g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID;
    }
    else if( obd_frame_type == "brake pressure" )
    {
        g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID;
    }

    republish_obd_frames_to_control_can_bus();
}


THEN("^a (.*) OBD CAN frame should be published to the Control CAN bus$")
{
    REGEX_PARAM( std::string, obd_frame_type );

    if( obd_frame_type == "steering wheel angle" )
    {
        assert_that(
            g_mock_mcp_can_send_msg_buf_id,
            is_equal_to(KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID));
    }
    else if( obd_frame_type == "wheel speed" )
    {
        assert_that(
            g_mock_mcp_can_send_msg_buf_id,
            is_equal_to(KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID));
    }
    else if( obd_frame_type == "brake pressure" )
    {
        assert_that(
            g_mock_mcp_can_send_msg_buf_id,
            is_equal_to(KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID));
    }
}
