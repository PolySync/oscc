WHEN("^a sensor becomes temporarily disconnected$")
{
    g_mock_arduino_analog_read_return = 0;

    check_for_sensor_faults();

    check_for_sensor_faults();

    check_for_sensor_faults();
}


WHEN("^a sensor becomes permanently disconnected$")
{
    g_mock_arduino_analog_read_return = 0;

    // must call function enough times to exceed the fault limit
    for( int i = 0; i < 100; ++i )
    {
        check_for_sensor_faults();
    }
}

THEN("^control should remain enabled")
{
    assert_that(
        g_steering_control_state.enabled,
        is_equal_to(1));
}
