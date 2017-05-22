// variable needed to track number of times analog pins have been written
// to for the brake_update() tests which span multiple scenarios
int mock_arduino_analog_write_count;

// variables needed to preserve the state of the PID controller between scenarios
float mock_pid_int_error;
float mock_pid_prev_input;


GIVEN("^the left brake sensor reads (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return = sensor_val;
}


GIVEN("^the right brake sensor reads (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return = sensor_val;
}


WHEN("^an enable brake command is received$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command_data->enabled = 1;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^a disable brake command is received$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_command_data->enabled = 0;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    check_for_incoming_message();
}


WHEN("^the brake pedal command (.*) is received$")
{
    REGEX_PARAM(int, command);

    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    brake_command_data->enabled = 1;
    brake_command_data->pedal_command = command;

    pid_zeroize( &g_pid, PID_WINDUP_GUARD );

    g_pid.proportional_gain = PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain     = PID_INTEGRAL_GAIN;
    g_pid.derivative_gain   = PID_DERIVATIVE_GAIN;

    // restore PID params needed for next scenario
    g_pid.prev_input = mock_pid_prev_input;
    g_pid.int_error = mock_pid_int_error;

    // restore number of analog writes that have occurred so far so that
    // the analogWrite mocking stores them in their appropriate places
    g_mock_arduino_analog_write_count = mock_arduino_analog_write_count;

    check_for_incoming_message();

    g_mock_arduino_micros_return += 20000;

    brake_update();
}


THEN("^the brake pedal command should be parsed$")
{
    oscc_command_brake_data_s * brake_command_data =
        (oscc_command_brake_data_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that(
        g_brake_control_state.commanded_pedal_position,
        is_equal_to(brake_command_data->pedal_command));
}


THEN("^the (.*) solenoid should be activated with duty cycle (.*)$")
{
    REGEX_PARAM(std::string, solenoid);
    REGEX_PARAM(int, duty_cycle);

    static unsigned long timestamp = 20000;

    // micros() mock must return 20000 more than last time to always get a
    // timestamp delta of 20000 microseconds so brake update is deterministic
    g_mock_arduino_micros_return = timestamp;
    timestamp += 20000;

    // save number of analog writes that have occurred so far to be restored later
    mock_arduino_analog_write_count = g_mock_arduino_analog_write_count;

    // save PID params from last scenario needed for the next scenario
    mock_pid_prev_input = g_pid.prev_input;
    mock_pid_int_error = g_pid.int_error;

    static int i = 0;

    if(solenoid == "ACCUMULATOR")
    {
        // disable release solenoids
        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(SOLENOID_PWM_OFF));

        ++i;

        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(SOLENOID_PWM_OFF));

        ++i;

        // enable accumulator solenoids
        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(duty_cycle));

        ++i;

        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(duty_cycle));

        ++i;
    }
    else if(solenoid == "RELEASE")
    {
        // disable accumulator solenoids
        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(SOLENOID_PWM_OFF));

        ++i;

        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(SOLENOID_PWM_OFF));

        ++i;

        // enable release solenoids
        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(duty_cycle));

        ++i;

        assert_that(
            g_mock_arduino_analog_write_pins[i],
            is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

        assert_that(
            g_mock_arduino_analog_write_val[i],
            is_equal_to(duty_cycle));

        ++i;
    }

}


THEN("^the last command timestamp should be set$")
{
    assert_that(
        g_brake_command_last_rx_timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}
