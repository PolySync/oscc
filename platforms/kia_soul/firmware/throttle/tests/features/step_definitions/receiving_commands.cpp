WHEN("^an enable throttle command is received$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command_data->enabled = 1;

    check_for_incoming_message();
}


WHEN("^a disable throttle command is received$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command_data->enabled = 0;

    check_for_incoming_message();
}


WHEN("^the accelerator position command (.*) is received$")
{
    REGEX_PARAM(int, commanded_accelerator_position);

    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    throttle_command_data->enabled = 1;
    throttle_command_data->commanded_accelerator_position = commanded_accelerator_position;

    check_for_incoming_message();

    update_throttle();
}


THEN("^the accelerator position command should be parsed$")
{
    oscc_command_throttle_data_s * throttle_command_data =
        (oscc_command_throttle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that(
        g_throttle_control_state.commanded_accelerator_position,
        is_equal_to(throttle_command_data->commanded_accelerator_position));
}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_throttle_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
