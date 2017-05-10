WHEN("^an enable steering command is received$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command_data->enabled = 1;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^a disable steering command is received$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command_data->enabled = 0;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^the steering wheel angle command (.*) is received$")
{
    REGEX_PARAM(int, command);

    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    steering_command_data->enabled = 1;
    steering_command_data->commanded_steering_wheel_angle = command;

    check_for_incoming_message();

    update_steering();
}


THEN("^the steering wheel angle command should be parsed$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that(
        g_steering_control_state.commanded_steering_wheel_angle,
        is_equal_to(steering_command_data->commanded_steering_wheel_angle/9.0));
}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_steering_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
