// variable needed to track number of times analog pins have been written
// to for the update_brake() tests which span multiple scenarios
int mock_arduino_analog_write_count;

// variables needed to preserve the state of the PID controller between scenarios
float mock_pid_int_error;
float mock_pid_prev_input;


GIVEN("^the left brake sensor reads (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return[14] = sensor_val;
}


GIVEN("^the right brake sensor reads (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return[13] = sensor_val;
}


WHEN("^an enable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_ENABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_enable_s * brake_enable =
        (oscc_brake_enable_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_enable->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_enable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a disable brake command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_DISABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_brake_disable_s * brake_disable =
        (oscc_brake_disable_s *) g_mock_mcp_can_read_msg_buf_buf;

    brake_disable->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_disable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a fault report is received$")
{
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;

    oscc_fault_report_s * fault_report =
        (oscc_fault_report_s *) g_mock_mcp_can_read_msg_buf_buf;

    fault_report->magic[0] = OSCC_MAGIC_BYTE_0;
    fault_report->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^the brake pedal command (.*) is received$")
{
    REGEX_PARAM(float, command);

    oscc_brake_command_s * brake_command =
        (oscc_brake_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    brake_command->magic[0] = OSCC_MAGIC_BYTE_0;
    brake_command->magic[1] = OSCC_MAGIC_BYTE_1;
    brake_command->pedal_command = command;

    pid_zeroize( &g_pid, BRAKE_PID_WINDUP_GUARD );

    g_pid.proportional_gain = BRAKE_PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain     = BRAKE_PID_INTEGRAL_GAIN;
    g_pid.derivative_gain   = BRAKE_PID_DERIVATIVE_GAIN;

    // restore PID params needed for next scenario
    g_pid.prev_input = mock_pid_prev_input;
    g_pid.int_error = mock_pid_int_error;

    // restore number of analog writes that have occurred so far so that
    // the analogWrite mocking stores them in their appropriate places
    g_mock_arduino_analog_write_count = mock_arduino_analog_write_count;

    check_for_incoming_message();

    g_mock_arduino_micros_return += 20000;

    update_brake();
}


THEN("^the brake pedal command should be parsed$")
{
    oscc_brake_command_s * brake_command =
        (oscc_brake_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    assert_that_double(
        g_brake_control_state.commanded_pedal_position,
        is_equal_to_double(brake_command->pedal_command));
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
