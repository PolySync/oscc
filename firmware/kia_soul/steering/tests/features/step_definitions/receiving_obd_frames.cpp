WHEN("^a steering wheel angle OBD frame is received with steering wheel angle (.*)$")
{
    REGEX_PARAM(int, raw_angle);

    kia_soul_obd_steering_wheel_angle_data_s * steering_wheel_angle_data =
        (kia_soul_obd_steering_wheel_angle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_wheel_angle_data->steering_wheel_angle = raw_angle;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_can_frame();
}


THEN("^the control state's current_steering_wheel_angle field should be (.*)$")
{
    REGEX_PARAM(float, scaled_angle);

    significant_figures_for_assert_double_are(6);
    assert_that_double(
        g_steering_control_state.current_steering_wheel_angle,
        is_equal_to_double(scaled_angle));
}
