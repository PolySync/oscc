GIVEN("^the actuator and pump motor are in a (.*) state$")
{
    REGEX_PARAM(std::string, state);

    if( state == "bad" )
    {
        g_mock_arduino_analog_read_return = 0;
    }
    else if( state == "good" )
    {
        g_mock_arduino_analog_read_return = BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN;
    }
}


WHEN("^the startup checks are run$")
{
    brake_init();
}


THEN("^the actuator error should be set to (.*)")
{
    REGEX_PARAM(std::string, error_val);

    if( error_val == "false" )
    {
        assert_that(
            g_brake_control_state.startup_pressure_check_error,
            is_equal_to(0));
    }
    else if( error_val == "true" )
    {
        assert_that(
            g_brake_control_state.startup_pressure_check_error,
            is_equal_to(1));
    }
}


THEN("^the pump motor error should be set to (.*)")
{
    REGEX_PARAM(std::string, error_val);

    if( error_val == "false" )
    {
        assert_that(
            g_brake_control_state.startup_pressure_check_error,
            is_equal_to(0));
    }
    else if( error_val == "true" )
    {
        assert_that(
            g_brake_control_state.startup_pressure_check_error,
            is_equal_to(1));
    }
}
