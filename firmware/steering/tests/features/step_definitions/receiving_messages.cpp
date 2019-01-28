WHEN("^an enable steering command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_ENABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_steering_enable_s * steering_enable =
        (oscc_steering_enable_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_enable->magic[0] = OSCC_MAGIC_BYTE_0;
    steering_enable->magic[1] = OSCC_MAGIC_BYTE_1;

    check_for_incoming_message();
}


WHEN("^a disable steering command is received$")
{
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_DISABLE_CAN_ID;
    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;

    oscc_steering_disable_s * steering_disable =
        (oscc_steering_disable_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_disable->magic[0] = OSCC_MAGIC_BYTE_0;
    steering_disable->magic[1] = OSCC_MAGIC_BYTE_1;

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


WHEN("^a command is received with request value (.*)$")
{
    REGEX_PARAM(float, request_value);

    g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL;
    g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID;


    oscc_steering_command_s * steering_command =
        (oscc_steering_command_s *) g_mock_mcp_can_read_msg_buf_buf;

    steering_command->magic[0] = OSCC_MAGIC_BYTE_0;
    steering_command->magic[1] = OSCC_MAGIC_BYTE_1;
    steering_command->torque_command = request_value;

    check_for_incoming_message();
}
