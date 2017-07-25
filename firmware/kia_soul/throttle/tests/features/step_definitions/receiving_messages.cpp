WHEN("^an enable throttle command is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_THROTTLE_COMMAND_CAN_ID;

    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->magic = OSCC_MAGIC;
    throttle_command->enable = 1;

    check_for_incoming_message();
}


WHEN("^a disable throttle command is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_THROTTLE_COMMAND_CAN_ID;

    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->magic = OSCC_MAGIC;
    throttle_command->enable = 0;

    check_for_incoming_message();
}


WHEN("^a fault report is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;

    oscc_fault_report_s * fault_report =
        (oscc_fault_report_s *) g_mock_mcp_can_read_msg_buf_buf;

    fault_report->magic = OSCC_MAGIC;

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

    throttle_command->magic = OSCC_MAGIC;
    throttle_command->enable = 1;
    throttle_command->spoof_value_high = high;
    throttle_command->spoof_value_low = low;

    check_for_incoming_message();

    update_throttle(
        throttle_command->spoof_value_high,
        throttle_command->spoof_value_low);
}
