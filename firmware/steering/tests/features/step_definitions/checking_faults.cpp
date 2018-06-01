WHEN("^the operator applies (.*) to the steering wheel$")
{
    REGEX_PARAM(int, steering_sensor_val);

    g_mock_arduino_analog_read_return[0] = steering_sensor_val;
    g_mock_arduino_analog_read_return[1] = 0;

    // need to call since newest read only contributes 1% for smoothing
    // and the average will start at 0.0 for the tests
    for( int i = 0; i < 200; ++i )
    {
        check_for_faults();
    }


    // set an elapsed time to account for hystoresis compensation
    g_mock_arduino_millis_return = Hysteresis_Time + 5;

    check_for_faults();
}
