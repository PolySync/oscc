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

fn get_brake_command_msg_from_buf( buffer: &[u8 ]) -> oscc_brake_command_s {
    let data_ptr: *const u8 = buffer.as_ptr();

    let brake_command_ptr: *const oscc_brake_command_s = data_ptr as *const _;

    unsafe { *brake_command_ptr as oscc_brake_command_s }
}

unsafe extern "C" fn brake_report_callback(report: *mut oscc_brake_report_s) {
    println!("recieved a brake report!");
}

/// The API should correctly register valid callback functions
fn prop_brake_report_callback() -> TestResult {
    let socket = oscc_tests::init_socket();

    let ret = unsafe { oscc_subscribe_to_brake_reports(Some(brake_report_callback)) };

    TestResult::from_bool(ret == oscc_result_t::OSCC_OK)
}

#[test]
fn check_brake_report_callback() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_brake_report_callback as fn() -> TestResult);
    
    oscc_tests::close_oscc();
    
    ret
}

#[cfg(feature = "kia-soul")]
mod brake_tests {
    use super::*;

    /// The API should properly calculate brake command within valid range
    pub fn prop_valid_brake_spoofs(pedal_command: f64) -> TestResult {
        let socket = oscc_tests::init_socket();

        unsafe { oscc_enable() };

        oscc_tests::skip_enable_frames(&socket);

        unsafe { oscc_publish_brake_position(pedal_command.abs()); }

        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => TestResult::discard(),
            Ok(frame) => {
                let brake_command_msg = get_brake_command_msg_from_buf( frame.data() );

                let scaled_pedal_command = oscc_tests::constrain(pedal_command.abs() * (MAXIMUM_BRAKE_COMMAND as f64), MINIMUM_BRAKE_COMMAND as f64, MAXIMUM_BRAKE_COMMAND as f64);

                TestResult::from_bool(brake_command_msg.pedal_command == scaled_pedal_command as u16)
            }
        }
    }

    /// For any valid steering input, the API should never send a command value 
    /// outside of the valid range
    pub fn prop_constrain_brake_spoofs(pedal_command: f64) -> TestResult {
        let socket = oscc_tests::init_socket();

        unsafe { oscc_enable() };

        oscc_tests::skip_enable_frames(&socket);

        unsafe { oscc_publish_brake_position(pedal_command.abs()); }

        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => TestResult::discard(),
            Ok(frame) => {
                let brake_command_msg = get_brake_command_msg_from_buf( frame.data() );

                TestResult::from_bool( 
                    (brake_command_msg.pedal_command >= MINIMUM_BRAKE_COMMAND as u16) && 
                    (brake_command_msg.pedal_command <= MAXIMUM_BRAKE_COMMAND as u16))
            }
        }
    }
}

#[cfg(feature = "kia-soul-ev")]
mod brake_tests {
    use super::*;

    fn calculate_ev_brake_spoofs( brake_command: f64 ) -> ( u16, u16 ) {
        let high_spoof = brake_command * (BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX - BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN) + BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN;
        let low_spoof = brake_command * (BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX - BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN) + BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN;
        ((high_spoof  * STEPS_PER_VOLT) as u16, (low_spoof * STEPS_PER_VOLT) as u16)
    }

    /// The API should properly calculate torque spoofs for valid range
    pub fn prop_valid_brake_spoofs(pedal_command: f64) -> TestResult {
        let socket = oscc_tests::init_socket();

        unsafe { oscc_enable() };

        oscc_tests::skip_enable_frames(&socket);

        unsafe { oscc_publish_brake_position(pedal_command.abs()); }

        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => TestResult::discard(),
            Ok(frame) => {
                let brake_command_msg = get_brake_command_msg_from_buf( frame.data() );

                let actual_spoofs = (brake_command_msg.spoof_value_high, brake_command_msg.spoof_value_low);

                let expected_spoofs = calculate_ev_brake_spoofs(pedal_command.abs());

                TestResult::from_bool(actual_spoofs == expected_spoofs)
            }
        }
    }

    /// For any valid steering input, the API should never send a spoof value 
    /// outside of the valid range
    pub fn prop_constrain_brake_spoofs(pedal_command: f64) -> TestResult {
        let socket = oscc_tests::init_socket();

        unsafe { oscc_enable() };

        oscc_tests::skip_enable_frames(&socket);

        unsafe { oscc_publish_brake_position(pedal_command.abs()); }

        let frame_result = socket.read_frame();
        match frame_result {
            Err(why) => TestResult::discard(),
            Ok(frame) => {
                let brake_command_msg = get_brake_command_msg_from_buf( frame.data() );

                let spoof_high = brake_command_msg.spoof_value_high as u32;
                let spoof_low = brake_command_msg.spoof_value_low as u32;

                TestResult::from_bool( 
                    (spoof_high <= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX) && 
                    (spoof_high >= BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN) && (spoof_low <= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX) && (spoof_low >= BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN))
            }
        }
    }
}

#[test]
fn check_valid_brake_spoofs() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), 1 as usize))
        .quickcheck(brake_tests::prop_valid_brake_spoofs as fn(f64) -> TestResult);

    oscc_tests::close_oscc();

    ret
}

#[test]
fn check_constrain_brake_spoofs() {
    oscc_tests::open_oscc();

    let ret = QuickCheck::new()
        .tests(1000)
        .gen(StdGen::new(rand::thread_rng(), std::f64::MAX as usize))
        .quickcheck(brake_tests::prop_constrain_brake_spoofs as fn(f64) -> TestResult);

    oscc_tests::close_oscc();

    ret
}



