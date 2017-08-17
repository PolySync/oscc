WHEN("^a brake report is published$")
{
    g_brake_control_state.enabled = true;
    g_brake_control_state.operator_override = true;
    g_brake_control_state.dtcs = 0x55;

    publish_brake_report();
}


THEN("^a brake report should be put on the control CAN bus$")
{
    assert_that(g_mock_mcp_can_send_msg_buf_id, is_equal_to(OSCC_BRAKE_REPORT_CAN_ID));
    assert_that(g_mock_mcp_can_send_msg_buf_ext, is_equal_to(CAN_STANDARD));
    assert_that(g_mock_mcp_can_send_msg_buf_len, is_equal_to(OSCC_BRAKE_REPORT_CAN_DLC));
}


THEN("^the brake report's fields should be set$")
{
    oscc_brake_report_s * brake_report =
        (oscc_brake_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report->magic[0],
        is_equal_to(OSCC_MAGIC_BYTE_0));

    assert_that(
        brake_report->magic[1],
        is_equal_to(OSCC_MAGIC_BYTE_1));

    assert_that(
        brake_report->enabled,
        is_equal_to(g_brake_control_state.enabled));

    assert_that(
        brake_report->operator_override,
        is_equal_to(g_brake_control_state.operator_override));

    assert_that(
        brake_report->dtcs,
        is_equal_to(g_brake_control_state.dtcs));
}
