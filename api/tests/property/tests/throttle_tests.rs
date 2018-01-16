#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]

include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;
extern crate socketcan;

extern crate oscc_tests;

use socketcan::CANFrame;
use quickcheck::{QuickCheck, TestResult, StdGen};
use std::{thread, time};

fn calculate_throttle_spoofs( throttle_command: f64 ) -> ( u16, u16 ) {
    let clamped_position = oscc_tests::constrain(throttle_command,
    MINIMUM_THROTTLE_COMMAND, MAXIMUM_THROTTLE_COMMAND);
    let high_spoof = clamped_position * (THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN) + THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN;
    let low_spoof = clamped_position * (THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN) + THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN;
    ((high_spoof  * STEPS_PER_VOLT) as u16, (low_spoof * STEPS_PER_VOLT) as u16)
}

fn get_throttle_command_msg_from_buf( buffer: &[u8]) -> oscc_throttle_command_s {
    let data_ptr: *const u8 = buffer.as_ptr();

    let throttle_command_ptr: *const oscc_throttle_command_s = data_ptr as *const _;

    unsafe { *throttle_command_ptr as oscc_throttle_command_s }
}

mod callbacks {
    use super::*;

    static mut THROTTLE_REPORT_RECIEVED: bool = false;

    pub unsafe extern "C" fn throttle_report_callback(report: *mut oscc_throttle_report_s) {
            THROTTLE_REPORT_RECIEVED = true;
    }

    pub fn recieved_throttle_report() -> bool {
        let ret = unsafe { THROTTLE_REPORT_RECIEVED };
        // reset value
        unsafe { THROTTLE_REPORT_RECIEVED = false; }
        ret
    }
}

/// The API should correctly register valid callback functions
fn prop_throttle_report_callback() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_throttle_reports(Some(callbacks::throttle_report_callback)) };

    TestResult::from_bool(ret == oscc_result_t::OSCC_OK)
}

#[test]
fn check_throttle_report_callback() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_throttle_report_callback as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should correctly register valid callback functions
fn prop_throttle_report_callback_triggered() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_throttle_reports(Some(callbacks::throttle_report_callback)) };

    let report: [u8; 2] = [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8];

    socket.write_frame_insist(&CANFrame::new(OSCC_THROTTLE_REPORT_CAN_ID, &report, false, false).unwrap());
 
    thread::sleep(time::Duration::from_millis(10));

    TestResult::from_bool(callbacks::recieved_throttle_report() == true)
}

#[test]
fn check_throttle_report_callback_triggered() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(10)
        .quickcheck(prop_throttle_report_callback_triggered as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should properly calculate throttle spoofs for valid range
fn prop_valid_throttle_spoofs(throttle_position: f64) -> TestResult {
    let socket = oscc_tests::init_socket();

    unsafe { oscc_enable() };

    oscc_tests::skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_throttle_position(throttle_position); }

    // read from can frame
    let frame_result = socket.read_frame();
    match frame_result {
        Err(why) => TestResult::discard(),
        Ok(frame) => {
            let throttle_command_msg = get_throttle_command_msg_from_buf( frame.data() );

            let actual_spoofs = (throttle_command_msg.spoof_value_high, throttle_command_msg.spoof_value_low);

            let expected_spoofs = calculate_throttle_spoofs(throttle_position);

            TestResult::from_bool(actual_spoofs == expected_spoofs)   
        }
    }
}

#[test]
fn check_valid_throttle_spoofs() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), 1 as usize))
        .quickcheck(prop_valid_throttle_spoofs as fn(f64) -> TestResult);

    oscc_tests::close_oscc();

    ret
}

/// For any valid throttle input, the API should never send a spoof value 
/// outside of the valid range
fn prop_constrain_throttle_spoofs(throttle_position: f64) -> TestResult {
    let socket = oscc_tests::init_socket();

    unsafe { oscc_enable() };

    oscc_tests::skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_throttle_position(throttle_position); }

    // read from can frame
    let frame_result = socket.read_frame();
    match frame_result {
        Err(why) => TestResult::discard(),
        Ok(frame) => {
            let throttle_command_msg = get_throttle_command_msg_from_buf( frame.data() );

            let spoof_high = throttle_command_msg.spoof_value_high as u32;
            let spoof_low = throttle_command_msg.spoof_value_low as u32;

            TestResult::from_bool( (spoof_high <= THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX ) && ( spoof_high >= THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN ) && ( spoof_low <= THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX ) && ( spoof_low >= THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN ) )
        }
    }
}

#[test]
fn check_constrain_throttle_spoofs() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), std::f64::MAX as usize))
        .quickcheck(prop_constrain_throttle_spoofs as fn(f64) -> TestResult);

    oscc_tests::close_oscc();

    ret
}