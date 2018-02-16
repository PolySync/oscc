#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/brake_test.rs"));

extern crate quickcheck;
extern crate rand;

use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};

extern "C" {
    pub static mut g_mock_mcp_can_check_receive_return: u8;
    pub static mut g_mock_mcp_can_read_msg_buf_id: u32;
    pub static mut g_mock_mcp_can_read_msg_buf_buf: [u8; 8usize];
    pub static mut g_mock_mcp_can_send_msg_buf_id: u32;
    pub static mut g_mock_mcp_can_send_msg_buf_ext: u8;
    pub static mut g_mock_mcp_can_send_msg_buf_len: u8;
    pub static mut g_mock_mcp_can_send_msg_buf_buf: *mut u8;
    pub static mut g_mock_arduino_analog_read_return: isize;
}

impl Arbitrary for oscc_brake_enable_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_brake_enable_s {
        oscc_brake_enable_s {
            magic: [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8],
            reserved: [u8::arbitrary(g); 6],
        }
    }
}

impl Arbitrary for oscc_brake_disable_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_brake_disable_s {
        oscc_brake_disable_s {
            magic: [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8],
            reserved: [u8::arbitrary(g); 6],
        }
    }
}

impl Arbitrary for oscc_brake_report_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_brake_report_s {
        oscc_brake_report_s {
            magic: [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8],
            enabled: u8::arbitrary(g),
            operator_override: u8::arbitrary(g),
            dtcs: u8::arbitrary(g),
            reserved: [u8::arbitrary(g); 3],
        }
    }
}

impl Arbitrary for oscc_brake_command_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_brake_command_s {
        oscc_brake_command_s {
            magic: [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8],
            pedal_command: f32::arbitrary(g),
            reserved: [u8::arbitrary(g); 2usize],
        }
    }
}


impl Arbitrary for can_frame_s {
    fn arbitrary<G: Gen>(g: &mut G) -> can_frame_s {
        can_frame_s {
            id: u32::arbitrary(g),
            dlc: u8::arbitrary(g),
            timestamp: u32::arbitrary(g),
            data: [u8::arbitrary(g); 8],
        }
    }
}

/// the throttle firmware should not attempt processing any messages
/// that are not throttle commands
fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, current_target: f32) -> TestResult {
    // if we generate a throttle can message, ignore the result
    if rx_can_msg.id == OSCC_BRAKE_COMMAND_CAN_ID {
        return TestResult::discard();
    }
    unsafe {
        g_brake_control_state.commanded_pedal_position = current_target;

        g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id;
        g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.commanded_pedal_position == current_target)
    }
}

#[test]
fn check_message_type_validity() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, f32) -> TestResult)
}

/// the throttle firmware should set the commanded pedal position
/// upon reciept of a valid command throttle message
fn prop_no_invalid_targets(brake_command_msg: oscc_brake_command_s) -> TestResult {
    unsafe {
        g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(brake_command_msg);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.commanded_pedal_position ==
                              brake_command_msg.pedal_command as f32)
    }
}

#[test]
fn check_accel_pos_validity() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_no_invalid_targets as fn(oscc_brake_command_s) -> TestResult)
}

/// the brake firmware should set the control state as enabled
/// upon reciept of a valid enable brake message telling it to enable
fn prop_process_enable_command(brake_enable_msg: oscc_brake_enable_s) -> TestResult {
    unsafe {
        g_brake_control_state.enabled = false;
        g_brake_control_state.operator_override = false;

        g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_ENABLE_CAN_ID;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(brake_enable_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.enabled == true)
    }
}

#[test]
fn check_process_enable_command() {
    QuickCheck::new()
        .tests(25)
        .quickcheck(prop_process_enable_command as fn(oscc_brake_enable_s) -> TestResult)
}

/// the brake firmware should set the control state as disabled
/// upon reciept of a valid disable brake message telling it to disable
fn prop_process_disable_command(brake_disable_msg: oscc_brake_disable_s) -> TestResult {
    unsafe {
        g_brake_control_state.enabled = true;
        g_brake_control_state.operator_override = false;

        g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_DISABLE_CAN_ID;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(brake_disable_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.enabled == false)
    }
}

#[test]
fn check_process_disable_command() {
    QuickCheck::new()
        .tests(25)
        .quickcheck(prop_process_disable_command as fn(oscc_brake_disable_s) -> TestResult)
}
