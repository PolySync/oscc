#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_must_use)]
include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

static mut STEERING_ENABLED: bool = false;
static mut THROTTLE_ENABLED: bool = false;
static mut BRAKE_ENABLED: bool = false;

// static last_steering_report: oscc_steering_report_s

pub fn constrain(value: f64, min: f64, max: f64) -> f64 {
    if value < min {
        return min;
    }
    else if value > max {
        return max;
    }
    value
}

pub mod callbacks {
    use super::*;

    pub unsafe extern "C" fn steering_report_callback(report: *mut oscc_steering_report_s) {
        // actually we should store the report

            last_steering_report = (*report).enabled > 0;
    }

    // what we really need is a function to return a copy of the report struct
    pub fn steering_enabled() -> bool {
        let ret = unsafe { STEERING_ENABLED };
        // reset value
        unsafe { STEERING_ENABLED = false; }
        ret
    }
}

pub fn register_steering_callback() {
    unsafe { oscc_subscribe_to_steering_reports(Some(callbacks::steering_report_callback)) };
}