#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/throttle_test.rs"));

extern crate quickcheck;
extern crate rand;

use quickcheck::{QuickCheck, TestResult, Arbitrary, StdGen, Gen};

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

impl Arbitrary for oscc_report_throttle_data_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_throttle_data_s {
        oscc_report_throttle_data_s {
            current_accelerator_position: u16::arbitrary(g),
            commanded_accelerator_position: u16::arbitrary(g),
            spoofed_accelerator_output: u16::arbitrary(g),
            _bitfield_1: u16::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_report_throttle_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_throttle_s {
        oscc_report_throttle_s {
            id: u32::arbitrary(g),
            dlc: u8::arbitrary(g),
            timestamp: u32::arbitrary(g),
            data: oscc_report_throttle_data_s::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_command_throttle_data_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_throttle_data_s {
        oscc_command_throttle_data_s {
            commanded_accelerator_position: u16::arbitrary(g),
            reserved_0: u8::arbitrary(g),
            _bitfield_1: u8::arbitrary(g),
            reserved_2: u8::arbitrary(g),
            reserved_3: u8::arbitrary(g),
            reserved_4: u8::arbitrary(g),
            count: u8::arbitrary(g),
        }
    }
}

impl Arbitrary for oscc_command_throttle_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_throttle_s {
        oscc_command_throttle_s {
            timestamp: u32::arbitrary(g),
            data: oscc_command_throttle_data_s::arbitrary(g),
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

/// the throttle firmware should not attempt processing any messages
/// that are not throttle commands
fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, current_target: u16) -> TestResult {
    // if we generate a throttle can message, ignore the result
    if rx_can_msg.id == OSCC_COMMAND_THROTTLE_CAN_ID {
        return TestResult::discard();
    }
    unsafe {
        g_throttle_control_state.commanded_accelerator_position = current_target;

        g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id as u64;
        g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_throttle_control_state.commanded_accelerator_position ==
                              current_target)
    }
}

#[test]
fn check_message_type_validity() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, u16) -> TestResult)
}

/// the throttle firmware should set the commanded accelerator position
/// upon reciept of a valid command throttle message
fn prop_no_invalid_targets(command_throttle_msg: oscc_command_throttle_s) -> TestResult {
    unsafe {
        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_THROTTLE_CAN_ID as u64;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_throttle_msg.data);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_throttle_control_state.commanded_accelerator_position ==
                              command_throttle_msg.data.commanded_accelerator_position)
    }
}

#[test]
fn check_accel_pos_validity() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_no_invalid_targets as fn(oscc_command_throttle_s) -> TestResult)
}

/// the throttle firmware should set the control state as enabled
/// upon reciept of a valid command throttle message telling it to enable
fn prop_process_enable_command(mut command_throttle_msg: oscc_command_throttle_s) -> TestResult {
    unsafe {
        command_throttle_msg.data.set_enabled(1);

        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_THROTTLE_CAN_ID as u64;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_throttle_msg.data);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_throttle_control_state.enabled == true)
    }
}

#[test]
fn check_process_enable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_enable_command as fn(oscc_command_throttle_s) -> TestResult)
}

/// the throttle firmware should set the control state as disabled
/// upon reciept of a valid command throttle message telling it to disable
fn prop_process_disable_command(mut command_throttle_msg: oscc_command_throttle_s) -> TestResult {
    unsafe {
        command_throttle_msg.data.set_enabled(0);

        g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_THROTTLE_CAN_ID as u64;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_throttle_msg.data);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_throttle_control_state.enabled == false)
    }
}

#[test]
fn check_process_disable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_disable_command as fn(oscc_command_throttle_s) -> TestResult)
}

/// the throttle firmware should create only valid CAN frames
fn prop_send_valid_can_fields(operator_override: bool,
                              commanded_accelerator_position: u16)
                              -> TestResult {
    static mut time: u64 = 0;
    unsafe {
        g_throttle_control_state.operator_override = operator_override;
        g_throttle_control_state.commanded_accelerator_position = commanded_accelerator_position;

        time = time + OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC as u64;

        g_mock_arduino_millis_return = time;

        publish_reports();

        let throttle_data = oscc_report_throttle_data_s {
            current_accelerator_position: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf,
                                                               *g_mock_mcp_can_send_msg_buf_buf
                                                                    .offset(1)]),
            commanded_accelerator_position: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf
                                                                      .offset(2),
                                                                 *g_mock_mcp_can_send_msg_buf_buf
                                                                      .offset(3)]),
            spoofed_accelerator_output: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf
                                                                  .offset(4),
                                                             *g_mock_mcp_can_send_msg_buf_buf
                                                                  .offset(5)]),
            _bitfield_1: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf.offset(6),
                                              *g_mock_mcp_can_send_msg_buf_buf.offset(7)]),
        };

        TestResult::from_bool((g_mock_mcp_can_send_msg_buf_id ==
                               OSCC_REPORT_THROTTLE_CAN_ID as u64) &&
                              (g_mock_mcp_can_send_msg_buf_ext == (CAN_STANDARD as u8)) &&
                              (g_mock_mcp_can_send_msg_buf_len ==
                               (OSCC_REPORT_THROTTLE_CAN_DLC as u8)) &&
                              (throttle_data.commanded_accelerator_position ==
                               commanded_accelerator_position) &&
                              (throttle_data.enabled() ==
                               (g_throttle_control_state.enabled as u8)) &&
                              (throttle_data.override_() == (operator_override as u8)))
    }
}

#[test]
fn check_valid_can_frame() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_send_valid_can_fields as fn(bool, u16) -> TestResult)
}

// the throttle firmware should be able to correctly and consistently
// detect operator overrides
fn prop_check_operator_override(analog_read_spoof: u16) -> TestResult {
    unsafe {
        g_throttle_control_state.enabled = true;
        g_mock_arduino_analog_read_return = analog_read_spoof;

        check_for_operator_override();

        let accel_pos_normalized: u32 =
            ((analog_read_spoof << 2) as u32 + (analog_read_spoof << 2) as u32) / 2;

        if accel_pos_normalized >= ACCELERATOR_OVERRIDE_THRESHOLD as u32 {
            TestResult::from_bool(g_throttle_control_state.operator_override == true &&
                                  g_throttle_control_state.enabled == false)
        } else {
            TestResult::from_bool(g_throttle_control_state.operator_override == false)
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
