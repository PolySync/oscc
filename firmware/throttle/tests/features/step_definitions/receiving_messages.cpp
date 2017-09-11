WHEN("^an enable throttle command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_THROTTLE_ENABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_throttle_enable_s * throttle_enable =
        (oscc_throttle_enable_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_enable->magic[0] = OSCC_MAGIC_BYTE_0;
    throttle_enable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a disable throttle command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_THROTTLE_DISABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_throttle_disable_s * throttle_disable =
        (oscc_throttle_disable_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_disable->magic[0] = OSCC_MAGIC_BYTE_0;
    throttle_disable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a fault report is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;

    oscc_fault_report_s * fault_report =
        (oscc_fault_report_s *) g_mock_mcp_can_read_msg_buf_buf;

    fault_report->magic[0] = OSCC_MAGIC_BYTE_0;
    fault_report->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a command is received with spoof values (.*) and (.*)$")
{
    REGEX_PARAM(uint16_t, high);
    REGEX_PARAM(uint16_t, low);

    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_THROTTLE_COMMAND_CAN_ID;

    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->magic[0] = OSCC_MAGIC_BYTE_0;
    throttle_command->magic[1] = OSCC_MAGIC_BYTE_1;
    throttle_command->spoof_value_high = high;
    throttle_command->spoof_value_low = low;

    check_for_incoming_message();

    update_throttle(
        throttle_command->spoof_value_high,
        throttle_command->spoof_value_low);
}
