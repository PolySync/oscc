WHEN("^an enable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_command_s * brake_command =
        (oscc_brake_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command->enable = 1;

    check_for_incoming_message();
}


WHEN("^a disable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_command_s * brake_command =
        (oscc_brake_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command->enable = 0;

    check_for_incoming_message();
}


WHEN("^a fault report is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;

    check_for_incoming_message();
}
