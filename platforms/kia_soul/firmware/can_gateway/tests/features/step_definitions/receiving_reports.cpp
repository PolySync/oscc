WHEN("^a steering wheel angle report is sent from the vehicle with the steering wheel angle (.*)$")
{
    REGEX_PARAM(int, angle);

    kia_soul_obd_steering_wheel_angle_data_s * steering_wheel_angle_data =
        (kia_soul_obd_steering_wheel_angle_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_wheel_angle_data->steering_angle = angle;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 1 steering wheel angle field should be set to (.*)$")
{
    REGEX_PARAM(int, angle);

    assert_that(
        g_tx_chassis_state_1.data.steering_wheel_angle,
        is_equal_to(angle));
}


THEN("^the Chassis State 1 steering wheel angle rate field should be set to (.*)$")
{
    REGEX_PARAM(int, angle);

    assert_that(
        g_tx_chassis_state_1.data.steering_wheel_angle_rate,
        is_equal_to(angle));
}


THEN("^the last received steering wheel angle timestamp should be set$")
{
    assert_that(
        g_obd_steering_wheel_angle_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}




WHEN("^a wheel speed report is sent from the vehicle with the wheel speed (.*)$")
{
    REGEX_PARAM(int, speed);

    kia_soul_obd_wheel_speed_data_s * wheel_speed_data =
        (kia_soul_obd_wheel_speed_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    wheel_speed_data->wheel_speed_front_left = speed;
    wheel_speed_data->wheel_speed_front_right = speed;
    wheel_speed_data->wheel_speed_rear_left = speed;
    wheel_speed_data->wheel_speed_rear_right = speed;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 2 wheel speed fields should be set to (.*)$")
{
    REGEX_PARAM(int, speed);

    assert_that(
        g_tx_chassis_state_2.data.wheel_speed_front_left,
        is_equal_to(speed));

    assert_that(
        g_tx_chassis_state_2.data.wheel_speed_front_right,
        is_equal_to(speed));

    assert_that(
        g_tx_chassis_state_2.data.wheel_speed_rear_left,
        is_equal_to(speed));

    assert_that(
        g_tx_chassis_state_2.data.wheel_speed_rear_right,
        is_equal_to(speed));
}


THEN("^the last received wheel speed timestamp should be set$")
{
    assert_that(
        g_obd_wheel_speed_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}




WHEN("^a brake pressure report is sent from the vehicle with the brake pressure (.*)$")
{
    REGEX_PARAM(int, pressure);

    kia_soul_obd_brake_pressure_data_s * brake_pressure_data =
        (kia_soul_obd_brake_pressure_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_pressure_data->master_cylinder_pressure = pressure;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 1 brake pressure field should be set to (.*)$")
{
    REGEX_PARAM(int, pressure);

    assert_that(
        g_tx_chassis_state_1.data.brake_pressure,
        is_equal_to(pressure));
}


THEN("^the last received brake pressure timestamp should be set$")
{
    assert_that(
        g_obd_brake_pressure_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}




WHEN("^a turn signal report is sent from the vehicle with the turn signal (.*)$")
{
    REGEX_PARAM(std::string, turn_signal);

    kia_soul_obd_turn_signal_data_s * turn_signal_data =
        (kia_soul_obd_turn_signal_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    if(turn_signal == "left")
    {
        turn_signal_data->turn_signal_flags = KIA_SOUL_OBD_TURN_SIGNAL_FLAG_LEFT_TURN;
    }
    else if(turn_signal == "right")
    {
        turn_signal_data->turn_signal_flags = KIA_SOUL_OBD_TURN_SIGNAL_FLAG_RIGHT_TURN;
    }

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_TURN_SIGNAL_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the last received turn signal timestamp should be set$")
{
    assert_that(
        g_obd_turn_signal_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
