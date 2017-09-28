#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]

include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate socketcan;

extern crate oscc_tests;

use socketcan::CANFrame;
use quickcheck::{QuickCheck, TestResult, StdGen};
use std::{thread, time};


mod callbacks {
    use super::*;

    static mut FAULT_REPORT_RECIEVED: bool = false;
    static mut OBD_MESSAGE_RECIEVED: bool = false;

    pub unsafe extern "C" fn fault_report_callback(report: *mut oscc_fault_report_s) {
            FAULT_REPORT_RECIEVED = true;
    }

    pub fn recieved_fault_report() -> bool {
        let ret = unsafe { FAULT_REPORT_RECIEVED };
        // reset value
        unsafe { FAULT_REPORT_RECIEVED = false; }
        ret
    }

    pub unsafe extern "C" fn obd_message_callback(frame: *mut can_frame) {
        OBD_MESSAGE_RECIEVED = true;
    }

    pub fn recieved_obd_message() -> bool {
        let ret = unsafe { OBD_MESSAGE_RECIEVED };
        // reset value
        unsafe { OBD_MESSAGE_RECIEVED = false; }
        ret
    }
}

/// The API should correctly register valid callback functions
fn prop_fault_report_callback() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_fault_reports(Some(callbacks::fault_report_callback)) };

    TestResult::from_bool(ret == oscc_result_t::OSCC_OK)
}

#[test]
fn check_fault_report_callback() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_fault_report_callback as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should correctly register valid callback functions
fn prop_fault_report_callback_triggered() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_fault_reports(Some(callbacks::fault_report_callback)) };

    let report: [u8; 2] = [OSCC_MAGIC_BYTE_0 as u8, OSCC_MAGIC_BYTE_1 as u8];

    socket.write_frame_insist(&CANFrame::new(OSCC_FAULT_REPORT_CAN_ID, &report, false, false).unwrap());
 
    thread::sleep(time::Duration::from_millis(10));

    TestResult::from_bool(callbacks::recieved_fault_report() == true)
}

#[test]
fn check_fault_report_callback_triggered() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(10)
        .quickcheck(prop_fault_report_callback_triggered as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should correctly register valid callback functions
fn prop_obd_message_callback() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_obd_messages(Some(callbacks::obd_message_callback)) };

    TestResult::from_bool(ret == oscc_result_t::OSCC_OK)
}

#[test]
fn check_obd_message_callback() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_obd_message_callback as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should correctly register valid callback functions
fn prop_obd_message_callback_triggered(frame_id: u8) -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_obd_messages(Some(callbacks::obd_message_callback)) };

    socket.write_frame_insist(&CANFrame::new(frame_id as u32, &[0], false, false).unwrap());
 
    thread::sleep(time::Duration::from_millis(10));

    TestResult::from_bool(callbacks::recieved_obd_message() == true)
}

#[test]
fn check_obd_message_callback_triggered() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(10)
        .quickcheck(prop_obd_message_callback_triggered as fn(u8) -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}


/// The API should send enable CAN frames to all modules when enable is called
fn prop_enable_all_modules() -> TestResult {
    let socket = oscc_tests::init_socket();

    unsafe { oscc_enable() };

    let mut actual = Vec::new();

    for _ in 0..3 {
        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => panic!("{:?}", why),
            Ok(frame) => actual.push(frame.id())
        }
    }

    actual.sort();

    let mut expected = [OSCC_THROTTLE_ENABLE_CAN_ID, OSCC_STEERING_ENABLE_CAN_ID, OSCC_BRAKE_ENABLE_CAN_ID];

    expected.sort();

    TestResult::from_bool(actual == expected)
}

#[test]
fn check_enable_all_modules() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_enable_all_modules as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

/// The API should send disable CAN frames to all modules when disable is called
fn prop_disable_all_modules() -> TestResult {
    let socket = oscc_tests::init_socket();

    unsafe { oscc_disable() };

    let mut actual = Vec::new();

    for _ in 0..3 {
        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => panic!("{:?}", why),
            Ok(frame) => actual.push(frame.id())
        }
    }

    actual.sort();

    let mut expected = [OSCC_THROTTLE_DISABLE_CAN_ID, OSCC_STEERING_DISABLE_CAN_ID, OSCC_BRAKE_DISABLE_CAN_ID];

    expected.sort();

    TestResult::from_bool(actual == expected)
}

#[test]
fn check_disable_all_modules() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_disable_all_modules as fn() -> TestResult);

    oscc_tests::close_oscc();

    ret
} 
