WHEN("^an enable steering command is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID;

    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->enable = 1;

    check_for_incoming_message();
}


WHEN("^a disable steering command is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID;

    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->enable = 0;

    check_for_incoming_message();
}


WHEN("^a fault report is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;

    check_for_incoming_message();
}


WHEN("^a command is received with spoof values (.*) and (.*)$")
{
    REGEX_PARAM(uint16_t, high);
    REGEX_PARAM(uint16_t, low);

    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID;


    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->enable = 1;
    steering_command->spoof_value_high = high;
    steering_command->spoof_value_low = low;

    check_for_incoming_message();

    update_steering(
        steering_command->spoof_value_high,
        steering_command->spoof_value_low);
}
