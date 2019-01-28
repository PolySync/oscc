#pragma once


THEN("^control should be enabled$")
{
    assert_that(
        g_vcm_control_state.enabled,
        is_equal_to(1));

    assert_that(
        g_mock_arduino_digital_write_pins[0],
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val[0],
        is_equal_to(HIGH));
}


THEN("^control should be disabled$")
{
    assert_that(
        g_vcm_control_state.enabled,
        is_equal_to(0));

    assert_that(
        g_mock_arduino_digital_write_pins[0],
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val[0],
        is_equal_to(LOW));
}

THEN("^(.*) should be sent to DAC A$")
{
    REGEX_PARAM(std::string, dac_output_a);

    assert_that(
        g_mock_dac_output_a,
        is_equal_to(vehicle_defines[dac_output_a]));
}


THEN("^(.*) should be sent to DAC B$")
{
    REGEX_PARAM(std::string, dac_output_b);

    assert_that(
        g_mock_dac_output_b,
        is_equal_to(vehicle_defines[dac_output_b]));
}
