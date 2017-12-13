WHEN("^an OBD CAN frame is received on the OBD CAN bus$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;

    republish_obd_frames_to_control_can_bus();
}


THEN("^an OBD CAN frame should be published to the Control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID));
}
