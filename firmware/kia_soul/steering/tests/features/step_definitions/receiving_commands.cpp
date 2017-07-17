// variables needed to preserve the state of the PID controller between scenarios
float mock_pid_int_error;
float mock_pid_prev_input;


WHEN("^an enable steering command is received$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command_data->enabled = 1;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_can_frame();
}


WHEN("^a disable steering command is received$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command_data->enabled = 0;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_can_frame();
}


WHEN("^the steering wheel angle command (.*) with angle rate (.*) is received$")
{
    REGEX_PARAM(int, command);
    REGEX_PARAM(int, rate);

    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    steering_command_data->enabled = 1;
    steering_command_data->commanded_steering_wheel_angle = command;
    steering_command_data->commanded_steering_wheel_angle_rate = rate;

    pid_zeroize( &g_pid, PID_WINDUP_GUARD );

    g_pid.proportional_gain = PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain     = PID_INTEGRAL_GAIN;
    g_pid.derivative_gain   = PID_DERIVATIVE_GAIN;

    // restore PID params needed for next scenario
    g_pid.prev_input = mock_pid_prev_input;
    g_pid.int_error = mock_pid_int_error;

    check_for_can_frame();

    update_steering();
}


THEN("^the steering wheel angle command should be parsed$")
{
    oscc_command_steering_data_s * steering_command_data =
        (oscc_command_steering_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    // save PID params from last scenario needed for the next scenario
    mock_pid_prev_input = g_pid.prev_input;
    mock_pid_int_error = g_pid.int_error;

    significant_figures_for_assert_double_are(4);
    assert_that_double(
        g_steering_control_state.commanded_steering_wheel_angle,
        is_equal_to_double(steering_command_data->commanded_steering_wheel_angle/9.0));
}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_steering_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
