WHEN("^a Chassis State 1 report is received with steering wheel angle (.*)$")
{
    REGEX_PARAM(int, raw_angle);

    oscc_report_chassis_state_1_data_s * chassis_state_1_data =
        (oscc_report_chassis_state_1_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    chassis_state_1_data->steering_wheel_angle = raw_angle;
    chassis_state_1_data->flags = OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID;

    g_mock_mcp_can_read_msg_buf_id = OSCC_REPORT_CHASSIS_STATE_1_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the control state's current_steering_wheel_angle field should be (.*)$")
{
    REGEX_PARAM(float, scaled_angle);

    significant_figures_for_assert_double_are(6);
    assert_that_double(
        g_steering_control_state.current_steering_wheel_angle,
        is_equal_to_double(scaled_angle));
}
