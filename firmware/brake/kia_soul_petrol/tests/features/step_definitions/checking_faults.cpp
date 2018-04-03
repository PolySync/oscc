WHEN("^the operator applies (.*) to the brake pedal for (\\d+) ms$")
{
    REGEX_PARAM(int, pedal_val);
    REGEX_PARAM(int, duration);

    g_mock_arduino_analog_read_return[10] = pedal_val;
    g_mock_arduino_analog_read_return[11] = pedal_val;

    g_mock_arduino_millis_return = 1;
    check_for_operator_override();

    g_mock_arduino_millis_return += duration;
    check_for_operator_override();
}


THEN("^control should remain enabled")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(1));
}
