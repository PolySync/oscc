GIVEN("^an enable throttle command is received$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command_data->enabled = 1;

    check_for_incoming_message();
}


GIVEN("^a disable throttle command is received$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command_data->enabled = 0;

    check_for_incoming_message();
}


THEN("^the accelerator target command should be parsed$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that_double(
        g_throttle_control_state.commanded_accelerator_position,
        is_equal_to_double(throttle_command_data->accelerator_command));
}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_throttle_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
