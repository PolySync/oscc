WHEN("^an enable brake command is received$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command_data->enabled = 1;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^a disable brake command is received$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command_data->enabled = 0;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^the brake pedal command (.*) is received$")
{
    REGEX_PARAM(int, command);

    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    brake_command_data->enabled = 1;
    brake_command_data->pedal_command = command;

    check_for_incoming_message();

    brake_update();
}


THEN("^the brake pedal command should be parsed$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that(
        g_brake_control_state.commanded_pedal_position,
        is_equal_to(brake_command_data->pedal_command));
}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_brake_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
