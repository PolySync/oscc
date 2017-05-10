GIVEN("^the current vehicle reported brake pressure is (.*)$")
{
    REGEX_PARAM(int, pressure);

    g_brake_control_state.current_vehicle_brake_pressure = pressure;
}


GIVEN("^the current sensor reported brake pressure is (.*)$")
{
    REGEX_PARAM(int, pressure);

    g_brake_control_state.current_sensor_brake_pressure = pressure;
}


WHEN("^the time since the last report publishing exceeds the interval$")
{
    g_brake_report_last_tx_timestamp = 0;

    g_mock_arduino_millis_return =
        OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a brake report should be published to the control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(OSCC_REPORT_BRAKE_CAN_ID));

    assert_that(
        g_mock_mcp_can_send_msg_buf_ext,
        is_equal_to(CAN_STANDARD));

    assert_that(
        g_mock_mcp_can_send_msg_buf_len,
        is_equal_to(OSCC_REPORT_BRAKE_CAN_DLC));

    oscc_report_brake_data_s * brake_report_data =
        (oscc_report_brake_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report_data->enabled,
        is_equal_to(g_brake_control_state.enabled));

    assert_that(
        brake_report_data->override,
        is_equal_to(g_brake_control_state.operator_override));

    assert_that(
        g_brake_report_last_tx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}


THEN("^the report's command field should be set to (.*)$")
{
    REGEX_PARAM(int, command);

    oscc_report_brake_data_s * brake_report_data =
        (oscc_report_brake_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report_data->pedal_command,
        is_equal_to(command));
}


THEN("^the report's current vehicle reported brake pressure field should be set to (.*)$")
{
    REGEX_PARAM(int, pressure);

    oscc_report_brake_data_s * brake_report_data =
        (oscc_report_brake_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report_data->pedal_input,
        is_equal_to(pressure));
}


THEN("^the report's sensor reported brake pressure should be set to (.*)$")
{
    REGEX_PARAM(int, pressure);

    oscc_report_brake_data_s * brake_report_data =
        (oscc_report_brake_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report_data->pedal_output,
        is_equal_to(pressure));
}
