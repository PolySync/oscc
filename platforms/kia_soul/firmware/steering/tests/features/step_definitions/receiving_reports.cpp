WHEN("^a Chassis State 1 report is received with steering wheel angle (.*)$")
{
    REGEX_PARAM(int, raw_angle);

    oscc_report_chassis_state_1_data_s * chassis_state_1_data =
        (oscc_report_chassis_state_1_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    chassis_state_1_data->steering_wheel_angle = raw_angle;

    g_mock_mcp_can_read_msg_buf_id = OSCC_REPORT_CHASSIS_STATE_1_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the control state's current_steering_wheel_angle field should be (.*)$")
{
    REGEX_PARAM(int, scaled_angle);

    assert_that(
        g_steering_control_state.current_steering_wheel_angle,
        is_equal_to(scaled_angle));
}
