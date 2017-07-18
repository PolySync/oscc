WHEN("^an enable steering command is received$")
{
    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->enable = 1;

    check_for_incoming_message();
}


WHEN("^a disable steering command is received$")
{
    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->enable = 0;

    check_for_incoming_message();
}


WHEN("^a command is received with spoof values (.*) and (.*)$")
{
    REGEX_PARAM(uint16_t, high);
    REGEX_PARAM(uint16_t, low);

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
