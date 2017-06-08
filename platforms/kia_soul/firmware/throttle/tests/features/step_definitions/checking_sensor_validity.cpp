WHEN("^the accelerator position sensor becomes temporarily disconnected$")
{
    // first check - error value - one fault
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return = ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_problems();

    // second check - error value - two faults
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return += ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_problems();

    // third check - valiud value - faults reset
    g_mock_arduino_analog_read_return = 500;
    g_mock_arduino_millis_return += ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;
    check_for_sensor_problems();
}


WHEN("^the accelerator position sensor becomes permanently disconnected$")
{
    g_mock_arduino_analog_read_return = 0;
    g_mock_arduino_millis_return = ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;

    // must call function enough times to exceed the fault limit
    for( int i = 0; i < ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_FAULT_COUNT; ++i )
    {
        // continue timing out
        g_mock_arduino_millis_return += ACCELERATOR_POSITION_SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC;

        check_for_sensor_problems();
    }
}

THEN("^control should remain enabled")
{
    assert_that(
        g_throttle_control_state.enabled,
        is_equal_to(1));
}
