WHEN("^the time since the last received controller command exceeds the timeout$")
{
    g_brake_command_last_rx_timestamp = 0;

    g_mock_arduino_millis_return =
        COMMAND_TIMEOUT_IN_MSEC;

    check_for_controller_command_timeout();
}


WHEN("^the operator applies (.*) to the brake pedal$")
{
    REGEX_PARAM(int, pedal_val);

    g_mock_arduino_analog_read_return = pedal_val;

    // The exponential filter used requires multiple passes for it to recognize
    // an override
    for( int i = 0; i < 3; ++i )
    {
        check_for_operator_override();
    }
}


THEN("^override flag should be set$")
{
    assert_that(
        g_brake_control_state.operator_override,
        is_equal_to(true));
}


