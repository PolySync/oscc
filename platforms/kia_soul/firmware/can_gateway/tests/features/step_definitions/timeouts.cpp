WHEN("^the time since the last received steering wheel angle report exceeds the timeout$")
{
    g_obd_steering_wheel_angle_rx_timestamp = 0;

    g_mock_arduino_millis_return =
        KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_RX_WARN_TIMEOUT_IN_MSEC;

    check_for_obd_timeout();
}


WHEN("^the time since the last received wheel speed report exceeds the timeout$")
{
    g_obd_wheel_speed_rx_timestamp = 0;

    g_mock_arduino_millis_return =
    KIA_SOUL_OBD_WHEEL_SPEED_RX_WARN_TIMEOUT_IN_MSEC;

    check_for_obd_timeout();
}


WHEN("^the time since the last received brake pressure report exceeds the timeout$")
{
    g_obd_brake_pressure_rx_timestamp = 0;

    g_mock_arduino_millis_return =
    KIA_SOUL_OBD_BRAKE_PRESSURE_RX_WARN_TIMEOUT_IN_MSEC;

    check_for_obd_timeout();
}


WHEN("^the time since the last received turn signal report exceeds the timeout$")
{
    g_obd_turn_signal_rx_timestamp = 0;

    g_mock_arduino_millis_return =
    KIA_SOUL_OBD_TURN_SIGNAL_RX_WARN_TIMEOUT_IN_MSEC;

    check_for_obd_timeout();
}

