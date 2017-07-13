WHEN("^the time since the last heartbeat report publishing exceeds the interval$")
{
    // set the timestamps of the other reports to their respective publish intervals
    // so that they aren't seen as being in need of a publishing themselves
    g_tx_chassis_state_1.timestamp = OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC;
    g_tx_chassis_state_2.timestamp = OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC;

    g_tx_heartbeat.timestamp = 0;

    g_mock_arduino_millis_return = OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a heartbeat report should be published to the control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(OSCC_REPORT_HEARTBEAT_CAN_ID + OSCC_MODULE_CAN_GATEWAY_NODE_ID));

    assert_that(
        g_mock_mcp_can_send_msg_buf_ext,
        is_equal_to(CAN_STANDARD));

    assert_that(
        g_mock_mcp_can_send_msg_buf_len,
        is_equal_to(OSCC_REPORT_HEARTBEAT_CAN_DLC));

    assert_that(
        g_tx_heartbeat.data.hardware_version,
        is_equal_to(OSCC_MODULE_CAN_GATEWAY_VERSION_HARDWARE));

    assert_that(
        g_tx_heartbeat.data.firmware_version,
        is_equal_to(OSCC_MODULE_CAN_GATEWAY_VERSION_FIRMWARE));

    assert_that(
        g_tx_heartbeat.timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}


WHEN("^the time since the last Chassis State 1 report publishing exceeds the interval$")
{
    // set the timestamps of the other reports to their respective publish intervals
    // so that they aren't seen as being in need of a publishing themselves
    g_tx_heartbeat.timestamp = OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC;
    g_tx_chassis_state_2.timestamp = OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC;

    g_tx_chassis_state_1.timestamp = 0;

    g_mock_arduino_millis_return = OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a Chassis State 1 report should be published to the control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_CAN_ID));

    assert_that(
        g_mock_mcp_can_send_msg_buf_ext,
        is_equal_to(CAN_STANDARD));

    assert_that(
        g_mock_mcp_can_send_msg_buf_len,
        is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_CAN_DLC));

    assert_that(
        g_tx_chassis_state_1.timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}


WHEN("^the time since the last Chassis State 2 report publishing exceeds the interval$")
{
    // set the timestamps of the other reports to their respective publish intervals
    // so that they aren't seen as being in need of a publishing themselves
    g_tx_heartbeat.timestamp = OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC;
    g_tx_chassis_state_1.timestamp = OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC;

    g_tx_chassis_state_2.timestamp = 0;

    g_mock_arduino_millis_return = OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC;

    publish_reports();
}


THEN("^a Chassis State 2 report should be published to the control CAN bus$")
{
    assert_that(
        g_mock_mcp_can_send_msg_buf_id,
        is_equal_to(OSCC_REPORT_CHASSIS_STATE_2_CAN_ID));

    assert_that(
        g_mock_mcp_can_send_msg_buf_ext,
        is_equal_to(CAN_STANDARD));

    assert_that(
        g_mock_mcp_can_send_msg_buf_len,
        is_equal_to(OSCC_REPORT_CHASSIS_STATE_2_CAN_DLC));

    assert_that(
        g_tx_chassis_state_2.timestamp,
        is_equal_to(g_mock_arduino_millis_return));
}


