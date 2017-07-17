WHEN("^the time since the last received controller command exceeds the timeout$")
{
    g_throttle_command_last_rx_timestamp = 0;

    g_mock_arduino_millis_return =
        COMMAND_TIMEOUT_IN_MSEC;

    check_for_controller_command_timeout();
}


WHEN("^the operator applies (.*) to the accelerator$")
{

    REGEX_PARAM(int, throttle_sensor_val);

    g_mock_arduino_analog_read_return = throttle_sensor_val;

    check_for_operator_override();
}
