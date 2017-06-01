WHEN("^a Chassis State 1 report is received with brake pressure (.*)$")
{
    REGEX_PARAM(int, pressure);

    oscc_report_chassis_state_1_data_s * chassis_state_1_data =
        (oscc_report_chassis_state_1_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    chassis_state_1_data->brake_pressure = pressure;
    chassis_state_1_data->flags = OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID;

    g_mock_mcp_can_read_msg_buf_id = OSCC_REPORT_CHASSIS_STATE_1_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the control state's current_vehicle_brake_pressure field should be (.*)$")
{
    REGEX_PARAM(float, pressure);

    assert_that_double(
        g_brake_control_state.current_vehicle_brake_pressure,
        is_equal_to_double(pressure));
}
