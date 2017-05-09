GIVEN("^the current steering wheel angle is (.*)$")
{
    REGEX_PARAM(int, angle);

    g_steering_control_state.current_steering_wheel_angle = angle;
}


GIVEN("^the spoofed torque output was (.*)$")
{
    REGEX_PARAM(int, torque);

    g_spoofed_torque_output_sum = torque;
}


GIVEN("^the time since the last report publishing exceeds (.*)$")
{
    REGEX_PARAM(std::string, interval);

    g_steering_report_last_tx_timestamp = 0;

    g_mock_arduino_millis_return =
        OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a steering report should be published to the control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(OSCC_REPORT_STEERING_CAN_ID));

    assert_that(
        g_mock_mcp_can_send_msg_buf_ext,
        is_equal_to(CAN_STANDARD));

    assert_that(
        g_mock_mcp_can_send_msg_buf_len,
        is_equal_to(OSCC_REPORT_STEERING_CAN_DLC));


    oscc_report_steering_data_s * steering_report_data =
        (oscc_report_steering_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        steering_report_data->enabled,
        is_equal_to(g_steering_control_state.enabled));

    assert_that(
        steering_report_data->override,
        is_equal_to(g_steering_control_state.operator_override));

    assert_that(
        g_steering_report_last_tx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}


THEN("^the report's command field should be set to (.*)$")
{
    REGEX_PARAM(int, command);

    oscc_report_steering_data_s * steering_report_data =
        (oscc_report_steering_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        steering_report_data->commanded_steering_wheel_angle,
        is_equal_to(command));
}


THEN("^the report's steering wheel angle field should be set to (.*)$")
{
    REGEX_PARAM(int, scaled_angle);

    oscc_report_steering_data_s * steering_report_data =
        (oscc_report_steering_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        steering_report_data->current_steering_wheel_angle,
        is_equal_to(scaled_angle));
}


THEN("^the report's torque output field should be set to (.*)$")
{
    REGEX_PARAM(int, torque);

    oscc_report_steering_data_s * steering_report_data =
        (oscc_report_steering_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        steering_report_data->spoofed_torque_output,
        is_equal_to(torque));
}
