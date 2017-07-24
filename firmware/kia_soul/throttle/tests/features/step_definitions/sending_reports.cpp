WHEN("^a throttle report is published$")
{
    g_throttle_control_state.enabled = true;
    g_throttle_control_state.operator_override = true;
    g_throttle_control_state.dtcs = 0x55;

    publish_throttle_report();
}


THEN("^a throttle report should be put on the control CAN bus$")
{
    assert_that(g_mock_mcp_can_send_msg_buf_id, is_equal_to(OSCC_THROTTLE_REPORT_CAN_ID));
    assert_that(g_mock_mcp_can_send_msg_buf_ext, is_equal_to(CAN_STANDARD));
    assert_that(g_mock_mcp_can_send_msg_buf_len, is_equal_to(OSCC_THROTTLE_REPORT_CAN_DLC));
}


THEN("^the throttle report's enabled field should be set$")
{
    oscc_throttle_report_s * throttle_report =
        (oscc_throttle_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        throttle_report->enabled,
        is_equal_to(g_throttle_control_state.enabled));
}


THEN("^the throttle report's override field should be set$")
{
    oscc_throttle_report_s * throttle_report =
        (oscc_throttle_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        throttle_report->operator_override,
        is_equal_to(g_throttle_control_state.operator_override));
}


THEN("^the throttle report's DTCs field should be set$")
{
    oscc_throttle_report_s * throttle_report =
        (oscc_throttle_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        throttle_report->dtcs,
        is_equal_to(g_throttle_control_state.dtcs));
}
