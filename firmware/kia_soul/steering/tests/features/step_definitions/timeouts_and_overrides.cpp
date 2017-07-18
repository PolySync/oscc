WHEN("^the time since the last received controller command exceeds the timeout$")
{
    g_steering_command_timeout = true;

    check_for_controller_command_timeout();
}


WHEN("^the operator applies (.*) to the steering wheel$")
{
    REGEX_PARAM(int, steering_sensor_val);

    g_mock_arduino_analog_read_return = steering_sensor_val;

    check_for_operator_override();
}
