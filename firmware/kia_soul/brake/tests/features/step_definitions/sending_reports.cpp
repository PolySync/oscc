WHEN("^a brake report is published$")
{
    g_brake_control_state.enabled = true;
    g_brake_control_state.operator_override = true;
    g_brake_control_state.dtcs = 0x55;
    g_brake_control_state.brake_pressure_front_left = 0x56;
    g_brake_control_state.brake_pressure_front_right = 0x57;

    publish_brake_report();
}


THEN("^a brake report should be put on the control CAN bus$")
{
    assert_that(g_mock_mcp_can_send_msg_buf_id, is_equal_to(OSCC_BRAKE_REPORT_CAN_ID));
    assert_that(g_mock_mcp_can_send_msg_buf_ext, is_equal_to(CAN_STANDARD));
    assert_that(g_mock_mcp_can_send_msg_buf_len, is_equal_to(OSCC_BRAKE_REPORT_CAN_DLC));
}


THEN("^the brake report's enabled field should be set$")
{
    oscc_brake_report_s * brake_report =
        (oscc_brake_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report->enabled,
        is_equal_to(g_brake_control_state.enabled));
}


THEN("^the brake report's override field should be set$")
{
    oscc_brake_report_s * brake_report =
        (oscc_brake_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report->operator_override,
        is_equal_to(g_brake_control_state.operator_override));
}


THEN("^the brake report's DTCs field should be set$")
{
    oscc_brake_report_s * brake_report =
        (oscc_brake_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report->dtcs,
        is_equal_to(g_brake_control_state.dtcs));
}


THEN("^the brake report's front pressure sensor fields should be set$")
{
    oscc_brake_report_s * brake_report =
        (oscc_brake_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        brake_report->brake_pressure_front_left,
        is_equal_to(g_brake_control_state.brake_pressure_front_left));

    assert_that(
        brake_report->brake_pressure_front_right,
        is_equal_to(g_brake_control_state.brake_pressure_front_right));
}
