#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]
include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;
extern crate socketcan;

use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};
use rand::Rng;
use socketcan::{CANSocket, CANFrame, ShouldRetry};
use std::time::Duration;
use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    pub static mut can_socket: i32;
    pub fn oscc_enable() -> i32;
    pub fn oscc_disable() -> i32;
    pub fn oscc_init_can(channel: *const c_char) -> i32;
    pub fn oscc_close(channel: i32) -> i32;
}

fn open_oscc() {
    let channel = CString::new("vcan0").unwrap();

    // initialize oscc socket on vcan0
    unsafe { oscc_init_can(channel.as_ptr()); }
}

fn close_oscc() {
    unsafe { oscc_close(0); }
}

fn skip_enable_frames(socket: &CANSocket) {
    socket.read_frame();
    socket.read_frame();
    socket.read_frame();
}

fn init_socket() -> CANSocket {
    // initialize our PBT socket on the vcan0
    let socket = CANSocket::open("vcan0").unwrap();
    socket.set_nonblocking(true);
    socket.set_read_timeout(Duration::new(1, 0));

    // get rid of initial empty frame
    socket.read_frame();

    socket
}

fn get_throttle_command_msg_from_buf( buffer: &[u8 ]) -> oscc_throttle_command_s {
    let data_ptr: *const u8 = buffer.as_ptr();

    let throttle_command_ptr: *const oscc_throttle_command_s = data_ptr as *const _;

    unsafe { *throttle_command_ptr as oscc_throttle_command_s }
}

fn get_steering_command_msg_from_buf( buffer: &[u8 ]) -> oscc_steering_command_s {
    let data_ptr: *const u8 = buffer.as_ptr();

    let steering_command_ptr: *const oscc_steering_command_s = data_ptr as *const _;

    unsafe { *steering_command_ptr as oscc_steering_command_s }
}


/// The API should send enable CAN frames to all modules when enable is called
fn prop_enable_all_modules() -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    let mut actual = Vec::new();

    for i in 0..3 {
        actual.push(socket.read_frame().unwrap().id());
    }

    actual.sort();

    let mut expected = [OSCC_THROTTLE_ENABLE_CAN_ID, OSCC_STEERING_ENABLE_CAN_ID, OSCC_BRAKE_ENABLE_CAN_ID];

    expected.sort();

    TestResult::from_bool(actual == expected)
}

#[test]
fn check_enable_all_modules() {
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_enable_all_modules as fn() -> TestResult);
    
    close_oscc();
    
    ret
}

/// The API should send disable CAN frames to all modules when disable is called
fn prop_disable_all_modules() -> TestResult {
    let socket = init_socket();

    unsafe { oscc_disable() };

    let mut actual = Vec::new();

    for i in 0..3 {
        actual.push(socket.read_frame().unwrap().id());
    }

    actual.sort();

    let mut expected = [OSCC_THROTTLE_DISABLE_CAN_ID, OSCC_STEERING_DISABLE_CAN_ID, OSCC_BRAKE_DISABLE_CAN_ID];

    expected.sort();

    TestResult::from_bool(actual == expected)
}

#[test]
fn check_disable_all_modules() {
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_disable_all_modules as fn() -> TestResult);

    close_oscc();

    ret
}

/// The API should properly calculate throttle spoofs
fn prop_valid_throttle_spoofs(throttle_position: f64) -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_throttle_position(throttle_position.abs()); }

    // read from can frame
    // need to add error checking and discard result if socket is busy
    let frame = socket.read_frame().unwrap();

    let throttle_command_msg = get_throttle_command_msg_from_buf( frame.data() );

    TestResult::from_bool(frame.id() == OSCC_THROTTLE_COMMAND_CAN_ID)
}

#[test]
fn check_valid_throttle_spoofs() {
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), 1 as usize))
        .quickcheck(prop_valid_throttle_spoofs as fn(f64) -> TestResult);

    close_oscc();

    ret
}

// test subscribe commands
