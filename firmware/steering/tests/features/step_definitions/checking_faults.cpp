WHEN("^a sensor becomes temporarily disconnected$")
{
    g_mock_arduino_analog_read_return[0] = 0;
    g_mock_arduino_analog_read_return[1] = 0;

    check_for_sensor_faults();

    check_for_sensor_faults();

    check_for_sensor_faults();
}


WHEN("^a sensor becomes permanently disconnected$")
{
    g_mock_arduino_analog_read_return[0] = 0;
    g_mock_arduino_analog_read_return[1] = 0;

    // must call function enough times to exceed the fault limit
    for( int i = 0; i < 100; ++i )
    {
        check_for_sensor_faults();
    }
}


WHEN("^the time since the last received controller command exceeds the timeout$")
{
    g_steering_command_timeout = true;

    check_for_controller_command_timeout();
}


WHEN("^the operator applies (.*) to the steering wheel$")
{
    REGEX_PARAM(int, steering_sensor_val);

    g_mock_arduino_analog_read_return[0] = steering_sensor_val;
    g_mock_arduino_analog_read_return[1] = 0;

    // need to call since newest read only contributes 1% for smoothing
    // and the average will start at 0.0 for the tests
    for( int i = 0; i < 200; ++i )
    {
        check_for_operator_override();
    }
}


THEN("^control should remain enabled")
{
    assert_that(
        g_steering_control_state.enabled,
        is_equal_to(1));
}


THEN("^a fault report should be published$")
{
    assert_that(g_mock_mcp_can_send_msg_buf_id, is_equal_to(OSCC_FAULT_REPORT_CAN_ID));
    assert_that(g_mock_mcp_can_send_msg_buf_ext, is_equal_to(CAN_STANDARD));
    assert_that(g_mock_mcp_can_send_msg_buf_len, is_equal_to(OSCC_FAULT_REPORT_CAN_DLC));

    oscc_fault_report_s * fault_report =
        (oscc_fault_report_s *) g_mock_mcp_can_send_msg_buf_buf;

    assert_that(
        fault_report->magic[0],
        is_equal_to(OSCC_MAGIC_BYTE_0));

    assert_that(
        fault_report->magic[1],
        is_equal_to(OSCC_MAGIC_BYTE_1));

    assert_that(
        fault_report->fault_origin_id,
        is_equal_to(FAULT_ORIGIN_STEERING));
}
