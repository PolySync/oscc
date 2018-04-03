WHEN("^the operator applies (.*) to the accelerator for (\\d+) ms$")
{
    REGEX_PARAM(int, throttle_sensor_val);
    REGEX_PARAM(int, duration);

    g_mock_arduino_analog_read_return[0] = throttle_sensor_val;
    g_mock_arduino_analog_read_return[1] = throttle_sensor_val;

    g_mock_arduino_millis_return = 1;
    check_for_faults();

    g_mock_arduino_millis_return += duration;
    check_for_faults();
}
