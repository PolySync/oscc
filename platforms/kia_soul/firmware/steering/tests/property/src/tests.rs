#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/steering_test.rs"));

extern crate quickcheck;
extern crate rand;

use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};

extern "C" {
    #[link_name = "g_mock_mcp_can_check_receive_return"]
    pub static mut g_mock_mcp_can_check_receive_return: u8;
    #[link_name = "g_mock_mcp_can_read_msg_buf_id"]
    pub static mut g_mock_mcp_can_read_msg_buf_id: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_mcp_can_read_msg_buf_buf"]
    pub static mut g_mock_mcp_can_read_msg_buf_buf: [u8; 8usize];
    #[link_name = "g_mock_mcp_can_send_msg_buf_id"]
    pub static mut g_mock_mcp_can_send_msg_buf_id: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_mcp_can_send_msg_buf_ext"]
    pub static mut g_mock_mcp_can_send_msg_buf_ext: u8;
    #[link_name = "g_mock_mcp_can_send_msg_buf_len"]
    pub static mut g_mock_mcp_can_send_msg_buf_len: u8;
    #[link_name = "g_mock_mcp_can_send_msg_buf_buf"]
    pub static mut g_mock_mcp_can_send_msg_buf_buf: *mut u8;
    #[link_name = "g_mock_arduino_millis_return"]
    pub static mut g_mock_arduino_millis_return: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_arduino_analog_read_return"]
    pub static mut g_mock_arduino_analog_read_return: u16;
}

impl Arbitrary for oscc_report_steering_data_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_steering_data_s {
        oscc_report_steering_data_s {
            current_steering_wheel_angle: i16::arbitrary(g),
            commanded_steering_wheel_angle: i16::arbitrary(g),
            reserved_0: u16::arbitrary(g),
            spoofed_torque_output: i8::arbitrary(g),
            _bitfield_1: u8::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_report_steering_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_steering_s {
        oscc_report_steering_s {
            id: u32::arbitrary(g),
            dlc: u8::arbitrary(g),
            timestamp: u32::arbitrary(g),
            data: oscc_report_steering_data_s::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_command_steering_data_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_steering_data_s {
        oscc_command_steering_data_s {
            commanded_steering_wheel_angle: i16::arbitrary(g),
            commanded_steering_wheel_angle_rate: u8::arbitrary(g),
            _bitfield_1: u8::arbitrary(g),
            reserved_3: u8::arbitrary(g),
            reserved_4: u8::arbitrary(g),
            reserved_5: u8::arbitrary(g),
            reserved_6: u8::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_command_steering_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_steering_s {
        oscc_command_steering_s {
            timestamp: u32::arbitrary(g),
            data: oscc_command_steering_data_s::arbitrary(g),
        }
    }
}


impl Arbitrary for can_frame_s {
    fn arbitrary<G: Gen>(g: &mut G) -> can_frame_s {
        can_frame_s {
            id: u32::arbitrary(g),
            dlc: u8::arbitrary(g),
            timestamp: u32::arbitrary(g),
            data: [u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g),
                   u8::arbitrary(g)],
        }
    }
}

/// the steering firmware should not attempt processing any messages
/// that are not steering commands
fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, current_target: f32) -> TestResult {
    // if we generate a steering can message, ignore the result
    if rx_can_msg.id == OSCC_COMMAND_STEERING_CAN_ID {
        return TestResult::discard();
    }
    unsafe {
        g_steering_control_state.commanded_steering_wheel_angle = current_target;

        g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id as u64;
        g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.commanded_steering_wheel_angle ==
                              current_target)
    }
}

#[test]
fn check_message_type_validity() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), i16::max_value() as usize))
        .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, f32) -> TestResult)
}

/// the steering firmware should set the commanded accelerator position
/// upon reciept of a valid command steering message
fn prop_no_invalid_targets(mut command_steering_msg: oscc_command_steering_s) -> TestResult {
    command_steering_msg.data.set_enabled(1);
    unsafe {
        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID as u64;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_steering_msg.data);

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.commanded_steering_wheel_angle ==
                              (command_steering_msg.data.commanded_steering_wheel_angle as
                               f32 / 9.0) &&
                              g_steering_control_state.commanded_steering_wheel_angle_rate ==
                              (command_steering_msg
                                   .data
                                   .commanded_steering_wheel_angle_rate as
                               f32 * 9.0))
    }
}

#[test]
fn check_wheel_angle_validity() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_no_invalid_targets as fn(oscc_command_steering_s) -> TestResult)
}

/// the steering firmware should set the control state as enabled
/// upon reciept of a valid command steering message telling it to enable
fn prop_process_enable_command(mut command_steering_msg: oscc_command_steering_s) -> TestResult {
    unsafe {
        command_steering_msg.data.set_enabled(1);

        g_steering_control_state.enabled = false;
        g_steering_control_state.operator_override = false;

        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID as u64;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_steering_msg.data);

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.enabled == true)
    }
}

#[test]
fn check_process_enable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_enable_command as fn(oscc_command_steering_s) -> TestResult)
}

/// the steering firmware should set the control state as disabled
/// upon reciept of a valid command steering message telling it to disable
fn prop_process_disable_command(mut command_steering_msg: oscc_command_steering_s) -> TestResult {
    unsafe {
        command_steering_msg.data.set_enabled(0);

        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_STEERING_CAN_ID as u64;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_steering_msg.data);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.enabled == false)
    }
}

#[test]
fn check_process_disable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_disable_command as fn(oscc_command_steering_s) -> TestResult)
}

/// the steering firmware should create only valid CAN frames
fn prop_send_valid_can_fields(operator_override: bool,
                              current_steering_wheel_angle: f32,
                              commanded_steering_wheel_angle: f32)
                              -> TestResult {
    static mut time: u64 = 0;
    unsafe {
        g_steering_control_state.operator_override = operator_override;
        g_steering_control_state.commanded_steering_wheel_angle = commanded_steering_wheel_angle;
        g_steering_control_state.current_steering_wheel_angle = current_steering_wheel_angle;

        time = time + OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC as u64;

        g_mock_arduino_millis_return = time;

        publish_reports();

        let steering_data = oscc_report_steering_data_s {
            current_steering_wheel_angle: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf,
                                                               *g_mock_mcp_can_send_msg_buf_buf
                                                                    .offset(1)]),
            commanded_steering_wheel_angle: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf
                                                                      .offset(2),
                                                                 *g_mock_mcp_can_send_msg_buf_buf
                                                                      .offset(3)]),
            reserved_0: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf.offset(4),
                                             *g_mock_mcp_can_send_msg_buf_buf.offset(5)]),
            spoofed_torque_output: std::mem::transmute(*g_mock_mcp_can_send_msg_buf_buf.offset(6)),
            _bitfield_1: std::mem::transmute(*g_mock_mcp_can_send_msg_buf_buf.offset(7)),
        };

        TestResult::from_bool((g_mock_mcp_can_send_msg_buf_id ==
                               OSCC_REPORT_STEERING_CAN_ID as u64) &&
                              (g_mock_mcp_can_send_msg_buf_ext == (CAN_STANDARD as u8)) &&
                              (g_mock_mcp_can_send_msg_buf_len ==
                               (OSCC_REPORT_STEERING_CAN_DLC as u8)) &&
                              (steering_data.current_steering_wheel_angle ==
                               current_steering_wheel_angle as i16) &&
                              (steering_data.commanded_steering_wheel_angle ==
                               commanded_steering_wheel_angle as i16) &&
                              (steering_data.enabled() ==
                               (g_steering_control_state.enabled as u8)) &&
                              (steering_data.override_() == (operator_override as u8)))
    }
}

#[test]
fn check_valid_can_frame() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), i16::max_value() as usize))
        .quickcheck(prop_send_valid_can_fields as fn(bool, f32, f32) -> TestResult)
}

// the steering firmware should be able to correctly and consistently
// detect operator overrides
fn prop_check_operator_override(analog_read_spoof: u16) -> TestResult {
    unsafe {
        static mut filtered_torque_a: f32 = 0.0;
        static mut filtered_torque_b: f32 = 0.0;
        const torque_filter_alpha: f32 = 0.5;

        g_steering_control_state.enabled = true;
        g_mock_arduino_analog_read_return = analog_read_spoof;

        check_for_operator_override();

        filtered_torque_a = (torque_filter_alpha * (analog_read_spoof << 2) as f32) +
                            ((1.0 - torque_filter_alpha) * filtered_torque_a);

        filtered_torque_b = (torque_filter_alpha * (analog_read_spoof << 2) as f32) +
                            (1.0 - torque_filter_alpha * filtered_torque_b);

        if filtered_torque_a.abs() >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC as f32 ||
           filtered_torque_b.abs() >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC as f32 {
            TestResult::from_bool(g_steering_control_state.operator_override == true &&
                                  g_steering_control_state.enabled == false)
        } else {
            TestResult::from_bool(g_steering_control_state.operator_override == false)
        }
    }
}

#[test]
fn check_operator_override() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_check_operator_override as fn(u16) -> TestResult)
}
