WHEN("^an enable throttle command is received$")
{
    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->enable = 1;

    check_for_incoming_message();
}


WHEN("^a disable throttle command is received$")
{
    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->enable = 0;

    check_for_incoming_message();
}


WHEN("^a command is received with spoof values (.*) and (.*)$")
{
    REGEX_PARAM(uint16_t, high);
    REGEX_PARAM(uint16_t, low);

    oscc_throttle_command_s * throttle_command =
        (oscc_throttle_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command->enable = 1;
    throttle_command->spoof_value_high = high;
    throttle_command->spoof_value_low = low;

    check_for_incoming_message();

    update_throttle(
        throttle_command->spoof_value_high,
        throttle_command->spoof_value_low);
}
