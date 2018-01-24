#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/brake_test.rs"));

extern crate quickcheck;
extern crate rand;

use quickcheck::{QuickCheck, TestResult, Arbitrary, StdGen, Gen};
use rand::Rng;

extern "C" {
    pub static mut g_mock_mcp_can_check_receive_return: u8;
    pub static mut g_mock_mcp_can_read_msg_buf_id: u32;
    pub static mut g_mock_mcp_can_read_msg_buf_buf: [u8; 8usize];
    pub static mut g_mock_mcp_can_send_msg_buf_id: u32;
    pub static mut g_mock_mcp_can_send_msg_buf_ext: u8;
    pub static mut g_mock_mcp_can_send_msg_buf_len: u8;
    pub static mut g_mock_mcp_can_send_msg_buf_buf: *mut u8;
    pub static mut g_mock_arduino_analog_read_return: [isize; 8usize];
    pub static mut g_mock_dac_output_a: u16;
    pub static mut g_mock_dac_output_b: u16;
    pub static mut g_brake_control_state: brake_control_state_s;
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
            reserved: [u8::arbitrary(g); 3]
        }
    }
}

impl Arbitrary for oscc_brake_command_s {
    fn arbitrary<G: Gen>(g: &mut G) -> oscc_brake_command_s {
        oscc_brake_command_s {
            magic: [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8],
            pedal_command: f32::arbitrary(g),
            reserved: [u8::arbitrary(g); 2]
        }
    }
}

impl Arbitrary for can_frame_s {
    fn arbitrary<G: Gen>(g: &mut G) -> can_frame_s {
        can_frame_s {
            id: u32::arbitrary(g),
            dlc: u8::arbitrary(g),
            timestamp: u32::arbitrary(g),
            data: [u8::arbitrary(g); 8]
        }
    }
}

/// the brake firmware should not attempt processing any messages
/// that are not brake or fault commands
/// this means that none of the brake control state would
/// change, nor would it output any values onto the DAC.
fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, enabled: bool, operator_override: bool, dtcs: u8) -> TestResult {
    if rx_can_msg.id == OSCC_BRAKE_COMMAND_CAN_ID ||
       rx_can_msg.id == OSCC_FAULT_REPORT_CAN_ID
    {
        return TestResult::discard();
    }
    unsafe {
        let dac_a = g_mock_dac_output_a;
        let dac_b = g_mock_dac_output_b;
        g_brake_control_state.enabled = enabled;
        g_brake_control_state.operator_override = operator_override;
        g_brake_control_state.dtcs = dtcs;

        g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id;
        g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.enabled == enabled &&
                              g_brake_control_state.operator_override == operator_override &&
                              g_brake_control_state.dtcs == dtcs &&
                              g_mock_dac_output_a == dac_a &&
                              g_mock_dac_output_b == dac_b)
    }
}

#[test]
fn check_message_type_validity() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
        .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, bool, bool, u8) -> TestResult)
}

/// the brake firmware should set the control state as enabled
/// upon receipt of a valid enable brake message telling it to enable
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
        .tests(1000)
        .quickcheck(prop_process_enable_command as fn(oscc_brake_enable_s) -> TestResult)
}

/// the brake firmware should set the control state as disabled
/// upon receipt of a valid disable brake message telling it to disable
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
        .tests(1000)
        .quickcheck(prop_process_disable_command as fn(oscc_brake_disable_s) -> TestResult)
}

/// the brake firmware should send requested spoof values
/// upon receiving a brake command message
fn prop_output_accurate_spoofs(mut brake_command_msg: oscc_brake_command_s) -> TestResult {
    brake_command_msg.pedal_command = rand::thread_rng().gen_range(0f32, 1f32);

    unsafe {
        g_brake_control_state.enabled = true;

        g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(brake_command_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_mock_dac_output_b >= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN as u16 &&
                              g_mock_dac_output_b <= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX as u16 &&
                              g_mock_dac_output_a >= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN as u16 &&
                              g_mock_dac_output_a <= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX as u16)
    }
}

#[test]
fn check_output_accurate_spoofs() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_output_accurate_spoofs as fn(oscc_brake_command_s) -> TestResult)
}

/// the brake firmware should constrain requested spoof values
/// upon receiving a brake command message
fn prop_output_constrained_spoofs(brake_command_msg: oscc_brake_command_s) -> TestResult {
    unsafe {
        if brake_command_msg.pedal_command >= MINIMUM_BRAKE_COMMAND as f32 &&
            brake_command_msg.pedal_command <= MAXIMUM_BRAKE_COMMAND as f32 {
            return TestResult::discard();
        }

        g_brake_control_state.enabled = true;

        g_mock_mcp_can_read_msg_buf_id = OSCC_BRAKE_COMMAND_CAN_ID;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;
        g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(brake_command_msg);

        check_for_incoming_message();

        TestResult::from_bool(g_mock_dac_output_a >= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN as u16 &&
                              g_mock_dac_output_a <= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX as u16 &&
                              g_mock_dac_output_b >= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN as u16 &&
                              g_mock_dac_output_b <= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX as u16)
    }
}

#[test]
fn check_output_constrained_spoofs() {
    QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
        .quickcheck(prop_output_constrained_spoofs as fn(oscc_brake_command_s) -> TestResult)
}

/// the brake firmware should create only valid CAN frames
fn prop_send_valid_can_fields(enabled: bool, operator_override: bool, dtcs: u8) -> TestResult {
    unsafe {
        g_brake_control_state.operator_override = operator_override;
        g_brake_control_state.enabled = enabled;
        g_brake_control_state.dtcs = dtcs;

        publish_brake_report();

        let brake_report_msg = g_mock_mcp_can_send_msg_buf_buf as *mut oscc_brake_report_s;

        TestResult::from_bool((*brake_report_msg).enabled == enabled as u8 &&
                              (*brake_report_msg).operator_override == operator_override as u8 &&
                              (*brake_report_msg).dtcs == dtcs)
    }
}

#[test]
fn check_valid_can_frame() {
    QuickCheck::new()
        .tests(10)
        .gen(StdGen::new(rand::thread_rng(), u8::max_value() as usize))
        .quickcheck(prop_send_valid_can_fields as fn(bool, bool, u8) -> TestResult)
}

// the brake firmware should be able to correctly and consistently
// detect operator overrides, disable on receipt, and send a fault report
fn prop_check_operator_override(analog_read_spoof: u16) -> TestResult {
    unsafe {
        g_brake_control_state.enabled = true;
        g_brake_control_state.operator_override = false;
        g_mock_arduino_analog_read_return[0] = analog_read_spoof as isize;
        g_mock_arduino_analog_read_return[1] = analog_read_spoof as isize;

        check_for_faults();

        if analog_read_spoof >= (BRAKE_PEDAL_OVERRIDE_THRESHOLD as u16) {
            TestResult::from_bool(g_brake_control_state.operator_override == true &&
                                  g_brake_control_state.enabled == false &&
                                  g_mock_mcp_can_send_msg_buf_id == OSCC_FAULT_REPORT_CAN_ID)
        } else {
            TestResult::from_bool(g_brake_control_state.operator_override == false)
        }
    }
}

#[test]
fn check_operator_override() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_check_operator_override as fn(u16) -> TestResult)
}

/// the brake firmware should set disable itself when it receives a
/// fault report from any other module
fn prop_process_fault_command(enabled: bool, operator_override: bool) -> TestResult {
    unsafe {
        g_brake_control_state.enabled = enabled;
        g_brake_control_state.operator_override = operator_override;

        g_mock_mcp_can_read_msg_buf_id = OSCC_FAULT_REPORT_CAN_ID;
        g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

        check_for_incoming_message();

        TestResult::from_bool(g_brake_control_state.enabled == false)
    }
}

#[test]
fn check_process_fault_command() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_process_fault_command as fn(bool, bool) -> TestResult)
}
