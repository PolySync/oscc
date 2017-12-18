#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]
include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;

extern crate oscc_tests;

use std::{time, thread};
use quickcheck::{QuickCheck, TestResult, StdGen};

#[test]
fn prop_steering_doesnt_disable() {
    fn steering_doesnt_disable( torque_command: f64 ) -> TestResult {
        oscc_tests::register_steering_callback();

        thread::sleep(time::Duration::new(1, 0));

        assert_eq!(oscc_tests::callbacks::steering_enabled(), true);

        unsafe { oscc_publish_steering_torque( torque_command ); }

        thread::sleep(time::Duration::new(1, 0));

        TestResult::from_bool(oscc_tests::callbacks::steering_enabled() == true)
    }
    
    unsafe { oscc_open(0); }

    unsafe { oscc_enable() };

    let ret = QuickCheck::new()
        .tests(100)
        .gen(StdGen::new(rand::thread_rng(), 1 as usize))
        .quickcheck(steering_doesnt_disable as fn(f64) -> TestResult);
    
    unsafe { oscc_disable() };

    unsafe { oscc_close(0); }

    ret
}