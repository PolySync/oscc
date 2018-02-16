WHEN("^a sensor becomes permanently disconnected$")
{
    g_mock_arduino_analog_read_return[0] = 0;
    g_mock_arduino_analog_read_return[1] = 0;

    check_for_faults();

    // must call function enough times to exceed the fault limit
    g_mock_arduino_millis_return = 105;

    check_for_faults();

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
        check_for_faults();
    }


    // set an elapsed time to account for hystoresis compensation
    g_mock_arduino_millis_return = 105;

    check_for_faults();
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
