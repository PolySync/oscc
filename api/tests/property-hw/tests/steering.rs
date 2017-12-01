
#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]

include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

extern crate quickcheck;
extern crate rand;

extern crate oscc_tests;

use quickcheck::{QuickCheck, TestResult, StdGen};

use std::{time, thread};

mod callbacks {
    use super::*;

    static mut STEERING_ENABLED: bool = false;

    pub unsafe extern "C" fn steering_report_callback(report: *mut oscc_steering_report_s) {
            STEERING_ENABLED = (*report).enabled > 0;
    }

    pub fn steering_enabled() -> bool {
        let ret = unsafe { STEERING_ENABLED };
        // reset value
        unsafe { STEERING_ENABLED = false; }
        ret
    }
}

#[test]
fn prop_steering_doesnt_disable() {
    fn steering_doesnt_disable( torque_command: f64 ) -> TestResult {
        unsafe { oscc_subscribe_to_steering_reports(Some(callbacks::steering_report_callback)) };

        thread::sleep(time::Duration::new(1, 0));

        assert_eq!(callbacks::steering_enabled(), true);

        unsafe { oscc_publish_steering_torque( torque_command ); }

        thread::sleep(time::Duration::new(1, 0));

        TestResult::from_bool(callbacks::steering_enabled() == true)
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