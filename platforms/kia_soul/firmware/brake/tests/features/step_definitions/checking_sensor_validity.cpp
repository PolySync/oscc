WHEN("^a sensor becomes temporarily disconnected$")
{
    // first check - error value - one fault
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return = SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_faults();

    // second check - error value - two faults
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return += SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_faults();

    // third check - valid value - faults reset
    g_mock_arduino_analog_read_return = 500;
    g_mock_arduino_millis_return += SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_faults();
}


WHEN("^a sensor becomes permanently disconnected$")
{
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return = SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;

    // must call function enough times to exceed the fault limit
    for( int i = 0; i < SENSOR_VALIDITY_CHECK_FAULT_COUNT; ++i )
    {
        // continue timing out
        g_mock_arduino_millis_return += SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;

        check_for_sensor_faults();
    }
}


THEN("^control should remain enabled")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(1));
}
