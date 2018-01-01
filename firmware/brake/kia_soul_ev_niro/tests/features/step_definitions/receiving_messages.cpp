WHEN("^an enable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_ENABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_enable_s * brake_enable =
        (oscc_brake_enable_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_enable->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_enable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a disable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_DISABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_disable_s * brake_disable =
        (oscc_brake_disable_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_disable->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_disable->magic[1] = OSCC_MAGIC_BYTE_1;

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
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;

    oscc_brake_command_s * brake_command =
        (oscc_brake_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_command->magic[1] = OSCC_MAGIC_BYTE_1;
    brake_command->spoof_value_high = high;
    brake_command->spoof_value_low = low;

    check_for_incoming_message();

    update_brake(
        brake_command->spoof_value_high,
        brake_command->spoof_value_low);
}
