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

WHEN("^an engine report is sent from the vehicle with the rpm (\\d+) and temperature (\\d+)$")
{
    REGEX_PARAM(int, rpm);
    REGEX_PARAM(int, temp);

    kia_soul_obd_engine_rpm_temp_data_s * engine_data =
        (kia_soul_obd_engine_rpm_temp_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    engine_data->engine_rpm = rpm;
    engine_data->engine_temp = temp;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_ENGINE_RPM_TEMP_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 3 engine rpm is (\\d+) and temperature is (\\d+)$")
{
    REGEX_PARAM(int, rpm);
    REGEX_PARAM(int, temp);

    assert_that(
        g_tx_chassis_state_3.data.engine_rpm,
        is_equal_to(rpm));

    assert_that(
        g_tx_chassis_state_3.data.engine_temp,
        is_equal_to(temp));
}


THEN("^the last received engine report timestamp should be set$")
{
    assert_that(
        g_obd_engine_rpm_temp_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}

WHEN("^a vehicle speed report is sent from the vehicle with the vehicle speed (.*)$")
{
    REGEX_PARAM(int, speed);

    kia_soul_obd_vehicle_speed_data_s * vehicle_speed_data =
        (kia_soul_obd_vehicle_speed_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    vehicle_speed_data->vehicle_speed = speed;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_VEHICLE_SPEED_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}

THEN("^the Chassis State 3 vehicle speed field should be set to (.*)$")
{
    REGEX_PARAM(int, speed);

    assert_that(
        g_tx_chassis_state_3.data.vehicle_speed,
        is_equal_to(speed));
}

THEN("^the last received vehicle speed timestamp should be set$")
{
    assert_that(
        g_obd_vehicle_speed_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}

WHEN("^a gear position report is sent from the vehicle with the gear position (\\d+)$")
{
    REGEX_PARAM(int, pos);

    kia_soul_obd_gear_position_data_s * gear_pos_data =
        (kia_soul_obd_gear_position_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    gear_pos_data->gear_position = pos;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_GEAR_POSITION_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 3 gear position field should be set to (\\d+)$")
{
    REGEX_PARAM(int, pos);

    assert_that(
        g_tx_chassis_state_3.data.gear_position,
        is_equal_to(pos));
}


THEN("^the last received gear position timestamp should be set$")
{
    assert_that(
        g_obd_gear_position_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}

WHEN("^an accelerator pedal position report is sent from the vehicle with the position (.*)$")
{
    REGEX_PARAM(int, pos);

    kia_soul_obd_accelerator_pedal_position_data_s * accelerator_pedal_position_data =
        (kia_soul_obd_accelerator_pedal_position_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    accelerator_pedal_position_data->accelerator_pedal_position = pos;

    g_mock_mcp_can_read_msg_buf_id = KIA_SOUL_OBD_ACCELERATOR_POSITION_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


THEN("^the Chassis State 3 accelerator pedal position field should be set to (.*)$")
{
    REGEX_PARAM(int, pos);

    assert_that(
        g_tx_chassis_state_3.data.accelerator_pedal_position,
        is_equal_to(pos));
}


THEN("^the last received accelerator pedal position timestamp should be set$")
{
    assert_that(
        g_obd_accelerator_pedal_position_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
