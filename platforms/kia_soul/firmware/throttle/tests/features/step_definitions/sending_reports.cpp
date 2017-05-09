GIVEN("^the time since the last report publishing exceeds (.*)$")
{
    REGEX_PARAM(std::string, interval);

    g_throttle_report_last_tx_timestamp = 0;

    g_mock_arduino_millis_return =
        OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a throttle report should be published to the control CAN bus$")
{
    assert_that(g_mock_mcp_can_send_msg_buf_id, is_equal_to(OSCC_REPORT_THROTTLE_CAN_ID));
    assert_that(g_mock_mcp_can_send_msg_buf_ext, is_equal_to(CAN_STANDARD));
    assert_that(g_mock_mcp_can_send_msg_buf_len, is_equal_to(OSCC_REPORT_THROTTLE_CAN_DLC));

    oscc_report_throttle_data_s * throttle_report_data =
    (oscc_report_throttle_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(throttle_report_data->enabled, is_equal_to(g_throttle_control_state.enabled));
    assert_that(throttle_report_data->override, is_equal_to(g_throttle_control_state.operator_override));

    assert_that(g_throttle_report_last_tx_timestamp, is_equal_to(g_mock_arduino_millis_return));
}


THEN("^the report's command field should be set to (.*)$")
{
    REGEX_PARAM(int, commanded_accelerator_position);

    oscc_report_throttle_data_s * throttle_report_data =
    (oscc_report_throttle_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(throttle_report_data->commanded_accelerator_position, is_equal_to(commanded_accelerator_position));
}


THEN("^the report's current_accelerator_position field should be set to (.*)$")
{
    REGEX_PARAM(int, current_accelerator_position);

    oscc_report_throttle_data_s * throttle_report_data =
    (oscc_report_throttle_data_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(throttle_report_data->current_accelerator_position, is_equal_to(current_accelerator_position));
}
