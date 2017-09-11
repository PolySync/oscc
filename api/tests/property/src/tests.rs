#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;
extern crate socketcan;

use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};
use rand::Rng;
use socketcan::{CANSocket, CANFrame, ShouldRetry};
use std::os::unix::io::AsRawFd;
use std::time::Duration;

extern "C" {
    pub static mut can_socket: i32;
    pub fn oscc_enable() -> i32;
    pub fn oscc_open(channel: u16) -> i32;
    pub fn oscc_publish_steering_torque(val: f32) -> i32;
}

#[test]
fn test() {
    let socket = CANSocket::open("vcan0").unwrap();
    socket.set_nonblocking(true);
    socket.set_read_timeout(Duration::new(1, 0));

    unsafe { oscc_open(0); }

    // unsafe{ can_socket = socket.as_raw_fd(); }
    unsafe { oscc_enable(); }

    // let socket = CANSocket::

    // let mut buff: [u8; 8] = [0; 8];

    // buff[1] = 187;

    // let frame = CANFrame::new(15, &buff, false, false);

    // let ret = socket.write_frame(&frame.unwrap());

    let mut ret2 = socket.read_frame();
    while ret2.should_retry()
    {
        ret2 = socket.read_frame();
        unsafe{ oscc_publish_steering_torque(0.5); }
        println!("{:?}", ret2);
    }
    println!("Id: {}", ret2.unwrap().id());
    
    ret2 = socket.read_frame();

    while ret2.should_retry()
    {
        ret2 = socket.read_frame();
        unsafe{ oscc_publish_steering_torque(0.5); }
        println!("{:?}", ret2);
    }
    println!("Id: {}", ret2.unwrap().id());

        ret2 = socket.read_frame();

    while ret2.should_retry()
    {
        ret2 = socket.read_frame();
        unsafe{ oscc_publish_steering_torque(0.5); }
        println!("{:?}", ret2);
    }
    println!("Id: {}", ret2.unwrap().id());

        ret2 = socket.read_frame();

    while ret2.should_retry()
    {
        ret2 = socket.read_frame();
        unsafe{ oscc_publish_steering_torque(0.5); }
        println!("{:?}", ret2);
    }
    println!("Id: {}", ret2.unwrap().id());
}

// ///The API should never output spoof voltages outside of the valid range
// fn prop_only_spoof_within_valid_steering_range() -> TestResult {
//     unsafe {
//         TestResult::from_bool(true == true)
//     }
// }

// #[test]
// fn check_message_type_validity() {
//     QuickCheck::new()
//         .tests(1)
//         // .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
//         .quickcheck(prop_only_spoof_within_valid_steering_range as fn() -> TestResult)
// }
