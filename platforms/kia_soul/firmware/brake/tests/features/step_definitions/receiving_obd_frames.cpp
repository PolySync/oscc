WHEN("^a brake pressure OBD frame is received with brake pressure (.*)$")
{
    REGEX_PARAM(int, pressure);

    kia_soul_obd_brake_pressure_data_s * brake_pressure_data =
        (kia_soul_obd_brake_pressure_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_pressure_data->master_cylinder_pressure = pressure;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_can_frame();
}


THEN("^the control state's current_vehicle_brake_pressure field should be (.*)$")
{
    REGEX_PARAM(float, pressure);

    assert_that_double(
        g_brake_control_state.current_vehicle_brake_pressure,
        is_equal_to_double(pressure));
}
