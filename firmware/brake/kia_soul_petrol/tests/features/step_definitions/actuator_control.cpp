#pragma once

THEN("^control should be enabled$")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(1));

    assert_that(
        g_mock_arduino_analog_write_pins[0],
        is_equal_to(PIN_MASTER_CYLINDER_SOLENOID));

    assert_that(
        g_mock_arduino_analog_write_val[0],
        is_equal_to(SOLENOID_PWM_ON));

    assert_that(
        g_mock_arduino_analog_write_pins[1],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[1],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[2],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[2],
        is_equal_to(SOLENOID_PWM_OFF));
}


THEN("^control should be disabled$")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(0));

    // turn actuator solenoids off
    assert_that(
        g_mock_arduino_analog_write_pins[0],
        is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[0],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[1],
        is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[1],
        is_equal_to(SOLENOID_PWM_OFF));

    // turn release solenoids on
    assert_that(
        g_mock_arduino_analog_write_pins[2],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[2],
        is_equal_to(SOLENOID_PWM_ON));

    assert_that(
        g_mock_arduino_analog_write_pins[3],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[3],
        is_equal_to(SOLENOID_PWM_ON));

    // turn brake lights off
    assert_that(
        g_mock_arduino_digital_write_pins[0],
        is_equal_to(PIN_BRAKE_LIGHT));

    assert_that(
        g_mock_arduino_digital_write_val[0],
        is_equal_to(LOW));

    // open master cylinder
    assert_that(
        g_mock_arduino_analog_write_pins[4],
        is_equal_to(PIN_MASTER_CYLINDER_SOLENOID));

    assert_that(
        g_mock_arduino_analog_write_val[4],
        is_equal_to(SOLENOID_PWM_OFF));

    // turn release solenoids off
    assert_that(
        g_mock_arduino_analog_write_pins[5],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[5],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[6],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[6],
        is_equal_to(SOLENOID_PWM_OFF));
}
