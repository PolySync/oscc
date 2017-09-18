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

use quickcheck::{QuickCheck, TestResult, StdGen};

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

// test subscribe commands
