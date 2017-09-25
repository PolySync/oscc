#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]

include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;
extern crate socketcan;

use socketcan::CANSocket;
use std::ffi::CString;

pub fn constrain(value: f64, min: f64, max: f64) -> f64 {
    if value < min {
        return min;
    }
    else if value > max {
        return max;
    }
    value
}

pub fn open_oscc() {
    let channel_result = CString::new("vcan0");
    match channel_result {
        Ok(channel) => {
            // initialize oscc socket on vcan0
            unsafe { oscc_init_can(channel.as_ptr()); }
        },
        Err(why) => panic!("{:?}", why)
    }
}

pub fn close_oscc() {
    unsafe { oscc_close(0); }
}

pub fn skip_enable_frames(socket: &CANSocket) {
    socket.read_frame();
    socket.read_frame();
    socket.read_frame();
}

pub fn init_socket() -> CANSocket {
    // initialize our PBT socket on the vcan0
    let socket_result = CANSocket::open("vcan0");
    match socket_result {
        Err(why) => panic!("{:?}", why),
        Ok(socket) => {
            socket.set_nonblocking(true);

            // get rid of initial empty frame
            socket.read_frame();

            socket
        }
    }
}