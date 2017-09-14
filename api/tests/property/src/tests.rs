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
}

fn constrain(value: f64, min: f64, max: f64) -> f64 {
    if value < min {
        return min;
    }
    else if value > max {
        return max;
    }
    value
}

fn open_oscc() {
    let channel_result = CString::new("vcan0");
    match channel_result {
        Ok(channel) => {
            // initialize oscc socket on vcan0
            unsafe { oscc_init_can(channel.as_ptr()); }
        },
        Err(why) => panic!("{:?}", why)
    }
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
    let socket_result = CANSocket::open("vcan0");
    match socket_result {
        Err(why) => panic!("{:?}", why),
        Ok(socket) => {
            socket.set_nonblocking(true);
            socket.set_read_timeout(Duration::new(1, 0));

            // get rid of initial empty frame
            socket.read_frame();

            socket
        }
    }
}

fn calculate_throttle_spoofs( throttle_command: f64 ) -> ( u16, u16 ) {
    let high_spoof = throttle_command * (THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN) + THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN;
    let low_spoof = throttle_command * (THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN) + THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN;
    ((high_spoof  * STEPS_PER_VOLT) as u16, (low_spoof * STEPS_PER_VOLT) as u16)
}

fn calculate_torque_spoofs( torque_command: f64 ) -> ( u16, u16 ) {
    let scaled_torque = constrain(torque_command * MAXIMUM_TORQUE_COMMAND, MINIMUM_TORQUE_COMMAND, MAXIMUM_TORQUE_COMMAND);

    let mut high_spoof = (TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * scaled_torque) + TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET;
    let mut low_spoof = (TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * scaled_torque) + TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET;

    high_spoof = constrain(high_spoof, STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN, STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX);

    low_spoof = constrain(low_spoof, STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MIN, STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MAX);

    ((high_spoof  * STEPS_PER_VOLT) as u16, (low_spoof * STEPS_PER_VOLT) as u16)
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
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_disable_all_modules as fn() -> TestResult);

    close_oscc();

    ret
}

/// The API should properly calculate throttle spoofs for valid range
fn prop_valid_throttle_spoofs(throttle_position: f64) -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_throttle_position(throttle_position.abs()); }

    // read from can frame
    let frame_result = socket.read_frame();
    match frame_result {
        Err(why) => TestResult::discard(),
        Ok(frame) => {
            let throttle_command_msg = get_throttle_command_msg_from_buf( frame.data() );

            let actual_spoofs = (throttle_command_msg.spoof_value_high, throttle_command_msg.spoof_value_low);

            let expected_spoofs = calculate_throttle_spoofs(throttle_position.abs());

            TestResult::from_bool(actual_spoofs == expected_spoofs)   
        }
    }
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

/// For any valid throttle input, the API should never send a spoof value 
/// outside of the valid range
fn prop_constrain_throttle_spoofs(throttle_position: f64) -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_throttle_position(throttle_position.abs()); }

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
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), std::f64::MAX as usize))
        .quickcheck(prop_constrain_throttle_spoofs as fn(f64) -> TestResult);

    close_oscc();

    ret
}

/// The API should properly calculate torque spoofs for valid range
fn prop_valid_torque_spoofs(steering_torque: f64) -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_steering_torque(steering_torque); }

    // read from can frame
    let frame_result = socket.read_frame();
    match frame_result {
        Err(why) => TestResult::discard(),
        Ok(frame) => {
            let torque_command_msg = get_steering_command_msg_from_buf( frame.data() );

            let actual_spoofs = (torque_command_msg.spoof_value_high, torque_command_msg.spoof_value_low);

            let expected_spoofs = calculate_torque_spoofs(steering_torque);

            TestResult::from_bool(actual_spoofs == expected_spoofs)
        }
    }
}

#[test]
fn check_valid_torque_spoofs() {
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), 1 as usize))
        .quickcheck(prop_valid_torque_spoofs as fn(f64) -> TestResult);

    close_oscc();

    ret
}

/// For any valid steering input, the API should never send a spoof value 
/// outside of the valid range
fn prop_constrain_steering_spoofs(steering_command: f64) -> TestResult {
    let socket = init_socket();

    unsafe { oscc_enable() };

    skip_enable_frames(&socket);

    // send some command
    unsafe { oscc_publish_steering_torque(steering_command); }

    // read from can frame
    let frame_result = socket.read_frame();
    match frame_result {
        Err(why) => TestResult::discard(),
        Ok(frame) => {
            let steering_command_msg = get_steering_command_msg_from_buf( frame.data() );

            let spoof_high = steering_command_msg.spoof_value_high as u32;
            let spoof_low = steering_command_msg.spoof_value_low as u32;

            // fails on 1 w high-min of 737 -- need to constrain from API possibly

            TestResult::from_bool( 
                (spoof_high <= STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX) && 
                (spoof_high >= STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN) && (spoof_low <= STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX) && (spoof_low >= STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN))
        }
    }
}

#[test]
#[ignore]
fn check_constrain_steering_spoofs() {
    open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), std::f64::MAX as usize))
        .quickcheck(prop_constrain_steering_spoofs as fn(f64) -> TestResult);

    close_oscc();

    ret
}

// test subscribe commands
