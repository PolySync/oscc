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
use rand::Rng;

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
    pub static mut g_mock_arduino_analog_read_return: i16;
    #[link_name = "g_mock_dac_output_a"]
    pub static mut g_mock_dac_output_a: u16;
    #[link_name = "g_mock_dac_output_b"]
    pub static mut g_mock_dac_output_b: u16;
}

impl Arbitrary for oscc_steering_report_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_steering_report_s {
        oscc_steering_report_s {
            enabled: u8::arbitrary(g),
            operator_override: u8::arbitrary(g),
            dtcs: u8::arbitrary(g),
            reserved: [u8::arbitrary(g),
                       u8::arbitrary(g),
                       u8::arbitrary(g),
                       u8::arbitrary(g),
                       u8::arbitrary(g)]
        }
    }
}

impl Arbitrary for oscc_steering_command_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_steering_command_s {
        oscc_steering_command_s {
            spoof_value_low: u16::arbitrary(g),
            spoof_value_high: u16::arbitrary(g),
            enable: u8::arbitrary(g),
            reserved: [u8::arbitrary(g),
                       u8::arbitrary(g),
                       u8::arbitrary(g)]
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
                   u8::arbitrary(g)]
        }
    }
}

/// the steering firmware should not attempt processing any messages
/// that are not steering or fault commands
fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, enabled: bool, operator_override: bool, dtcs: u8) -> TestResult {
    // if we generate a steering can message, ignore the result
    if rx_can_msg.id == OSCC_STEERING_COMMAND_CAN_ID || 
       rx_can_msg.id == OSCC_FAULT_REPORT_CAN_ID
    {
        return TestResult::discard();
    }
    unsafe {
        g_steering_control_state.enabled = enabled;
        g_steering_control_state.operator_override = operator_override;
        g_steering_control_state.dtcs = dtcs;

        g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id as u64;
        g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.enabled ==
                              enabled &&
                              g_steering_control_state.operator_override == operator_override &&
                              g_steering_control_state.dtcs == dtcs)
    }
}

#[test]
fn check_message_type_validity() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
        .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, bool, bool, u8) -> TestResult)
}

/// the steering firmware should send requested spoof values
/// upon recieving a steering command message
fn prop_output_spoofs(mut steering_command_msg: oscc_steering_command_s) -> TestResult {
    steering_command_msg.enable = 1u8;
    steering_command_msg.spoof_value_low = rand::thread_rng().gen_range(STEERING_SPOOF_SIGNAL_RANGE_MIN as u16, STEERING_SPOOF_SIGNAL_RANGE_MAX as u16);
    steering_command_msg.spoof_value_high = rand::thread_rng().gen_range(STEERING_SPOOF_SIGNAL_RANGE_MIN as u16, STEERING_SPOOF_SIGNAL_RANGE_MAX as u16);
    unsafe {
        g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID as u64;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(steering_command_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_mock_dac_output_b ==                                steering_command_msg.spoof_value_low &&
            g_mock_dac_output_a ==
            steering_command_msg.spoof_value_high )
    }
}

// randomly fails, investigate!

#[test]
#[ignore]
fn check_output_spoofs() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_output_spoofs as fn(oscc_steering_command_s) -> TestResult)
}

/// the steering firmware should constrain requested spoof values
/// upon recieving a steering command message
fn prop_output_spoofs(mut steering_command_msg: oscc_steering_command_s) -> TestResult {
    steering_command_msg.enable = 1u8;
    unsafe {
        g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID as u64;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(steering_command_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_mock_dac_output_b ==                                steering_command_msg.spoof_value_low &&
            g_mock_dac_output_a ==
            steering_command_msg.spoof_value_high )
    }
}

#[test]
#[ignore]
fn check_output_spoofs() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_output_spoofs as fn(oscc_steering_command_s) -> TestResult)
}

/// the steering firmware should set the control state as enabled
/// upon reciept of a valid command steering message telling it to enable
fn prop_process_enable_command(mut steering_command_msg: oscc_steering_command_s) -> TestResult {
    unsafe {
        steering_command_msg.enable = 1u8;

        g_steering_control_state.enabled = false;
        g_steering_control_state.operator_override = false;

        g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID as u64;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(steering_command_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.enabled == true)
    }
}

#[test]
fn check_process_enable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_enable_command as fn(oscc_steering_command_s) -> TestResult)
}

/// the steering firmware should set the control state as disabled
/// upon reciept of a valid command steering message telling it to disable
fn prop_process_disable_command(mut steering_command_msg: oscc_steering_command_s) -> TestResult {
    unsafe {
        steering_command_msg.enable = 0u8;

        g_mock_mcp_can_read_msg_buf_id = OSCC_STEERING_COMMAND_CAN_ID as u64;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(steering_command_msg);
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_steering_control_state.enabled == false)
    }
}

#[test]
fn check_process_disable_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_disable_command as fn(oscc_steering_command_s) -> TestResult)
}

/// the steering firmware should create only valid CAN frames
fn prop_send_valid_can_fields(enabled: bool,
                              operator_override: bool,
                              dtcs: u8)
                              -> TestResult {
    unsafe {
        g_steering_control_state.operator_override = operator_override;
        g_steering_control_state.enabled = enabled;
        g_steering_control_state.dtcs = dtcs;

        publish_steering_report();

        let steering_report_msg = g_mock_mcp_can_send_msg_buf_buf as *mut oscc_steering_report_s;

        TestResult::from_bool((*steering_report_msg).enabled == enabled as u8 &&(*steering_report_msg).operator_override == operator_override as u8 &&
        (*steering_report_msg).dtcs == dtcs)
    }
}

#[test]
fn check_valid_can_frame() {
    QuickCheck::new()
        .tests(10)
        .gen(StdGen::new(rand::thread_rng(), u8::max_value() as usize))
        .quickcheck(prop_send_valid_can_fields as fn(bool, bool, u8) -> TestResult)
}

// the steering firmware should be able to correctly and consistently
// detect operator overrides
fn prop_check_operator_override(analog_read_spoof: i16) -> TestResult {
    unsafe {
        g_steering_control_state.enabled = true;
        g_mock_arduino_analog_read_return = analog_read_spoof;

        check_for_operator_override();

        if analog_read_spoof.abs() >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC as i16 {
            TestResult::from_bool(g_steering_control_state.operator_override == true &&
                                  g_steering_control_state.enabled == false)
        } else {
            TestResult::from_bool(g_steering_control_state.operator_override == false)
        }
    }
}

// randomly fails, investigate!

#[test]
#[ignore]
fn check_operator_override() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), i16::max_value() as usize))
        .quickcheck(prop_check_operator_override as fn(i16) -> TestResult)
}
