#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/communications.rs"));

extern crate quickcheck;
extern crate rand;

#[macro_use]
extern crate lazy_static;

use std::mem;

#[derive(Debug)]
struct mock_can_frame {
    id: u32,
    ext: u8,
    dlc: u8,
    data: oscc_report_steering_data_s
}

// have a static can message that our callback function can populate and check values against
static mut can_msg: mock_can_frame = mock_can_frame {
    id: 0,
    ext: 0,
    dlc: 0,
    data: oscc_report_steering_data_s{
        current_steering_wheel_angle: 0,
        commanded_steering_wheel_angle: 0,
        vehicle_speed: 0,
        spoofed_torque_output: 0,
        _bitfield_1: 0
    }
};

#[allow(dead_code)]
extern fn retrieve_sent_can_msg( id: u32, ext: u8, dlc: u8, buf: *mut u8) {
    unsafe { 
        can_msg.id = id; 
        can_msg.ext = ext;
        can_msg.dlc = dlc;
        // endianness is important here
        can_msg.data.current_steering_wheel_angle = mem::transmute([*buf, *buf.offset(1)]);
        can_msg.data.commanded_steering_wheel_angle = mem::transmute([*buf.offset(2), *buf.offset(3)]);
        can_msg.data.vehicle_speed = mem::transmute([*buf.offset(4), *buf.offset(5)]);
        can_msg.data.spoofed_torque_output = mem::transmute(*buf.offset(6));
        can_msg.data._bitfield_1 = mem::transmute(*buf.offset(7));
    }
}

static mut spoof_low_signal: u16 = 0;
static mut spoof_high_signal: u16 = 0;

#[allow(dead_code)]
extern fn spoof_analog_read_low() -> u16 {
    unsafe { spoof_low_signal }
}

#[allow(dead_code)]
extern fn spoof_analog_read_high() -> u16 {
    unsafe { spoof_high_signal }
}

#[cfg(test)]
mod tests {
    use super::*;
    use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};
    use std::sync::Mutex;

    lazy_static! {
        static ref LOCK: Mutex<bool> = Mutex::new(true);
    }

    fn prop_only_process_valid_messages( mut rx_can_msg: can_frame_s, current_target: i16 ) -> TestResult {
        // if we generate a steering can message, ignore the result
        if rx_can_msg.id == OSCC_COMMAND_STEERING_CAN_ID {
            return TestResult::discard()
        }

        let buf_addr = &mut rx_can_msg.data as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                g_steering_control_state.commanded_steering_wheel_angle = current_target;

                // get MCP can to store our mocked message, so our comms module can retrieve it
                MCP_CAN_register_can_frame(&mut g_control_can, rx_can_msg.id as u64, CAN_STANDARD as u8, rx_can_msg.dlc, buf_addr as *mut u8);

                check_for_incoming_message();

                TestResult::from_bool(g_steering_control_state.commanded_steering_wheel_angle == current_target)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_no_invalid_targets( mut command_msg: oscc_command_steering_s ) -> TestResult { 
        let buf_addr = &mut command_msg.data as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                // get MCP can to store our mocked message, so our comms module can retrieve it
                MCP_CAN_register_can_frame(&mut g_control_can, OSCC_COMMAND_STEERING_CAN_ID as u64, CAN_STANDARD as u8, OSCC_REPORT_STEERING_CAN_DLC as u8, buf_addr as *mut u8);

                check_for_incoming_message();
                
                TestResult::from_bool(g_steering_control_state.commanded_steering_wheel_angle == command_msg.data.commanded_steering_wheel_angle / 9.0 as i16)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_process_enable_command( mut command_msg: oscc_command_steering_s ) -> TestResult {
        command_msg.data.set_enabled(1); // we're going to recieve an enable command
        let buf_addr = &mut command_msg.data as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                // get MCP can to store our mocked message, so our comms module can retrieve it
                MCP_CAN_register_can_frame(&mut g_control_can, OSCC_COMMAND_STEERING_CAN_ID as u64, CAN_STANDARD as u8, OSCC_REPORT_STEERING_CAN_DLC as u8, buf_addr as *mut u8);

                check_for_incoming_message();
                
                TestResult::from_bool(g_steering_control_state.enabled)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_process_disable_command( mut command_msg: oscc_command_steering_s ) -> TestResult {
        command_msg.data.set_enabled(0);
        let buf_addr = &mut command_msg.data as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                // get MCP can to store our mocked message, so our comms module can retrieve it
                MCP_CAN_register_can_frame(&mut g_control_can, OSCC_COMMAND_STEERING_CAN_ID as u64, CAN_STANDARD as u8, OSCC_REPORT_STEERING_CAN_DLC as u8, buf_addr as *mut u8);

                check_for_incoming_message();
                
                TestResult::from_bool(!g_steering_control_state.enabled)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_send_valid_can_fields(control_enabled: bool, operator_override: bool, current_steering_wheel_angle: i16, commanded_steering_wheel_angle: i16, spoof_low: u16, spoof_high: u16) -> TestResult 
    {
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                // set global state
                g_steering_control_state.enabled = control_enabled;
                g_steering_control_state.operator_override = operator_override;
                g_steering_control_state.commanded_steering_wheel_angle = commanded_steering_wheel_angle;
                g_steering_control_state.current_steering_wheel_angle = current_steering_wheel_angle;

                spoof_high_signal = spoof_high;
                spoof_low_signal = spoof_low;

                // register analog read callbacks
                register_signal_callbacks(Some(spoof_analog_read_low), Some(spoof_analog_read_high));

                MCP_CAN_register_callback(&mut g_control_can, Some(retrieve_sent_can_msg));
                publish_reports();

                TestResult::from_bool(
                    (can_msg.id == OSCC_REPORT_STEERING_CAN_ID) &&
                    (can_msg.ext == (CAN_STANDARD as u8)) &&
                    (can_msg.dlc == (OSCC_REPORT_STEERING_CAN_DLC as u8)) &&
                    (can_msg.data.current_steering_wheel_angle == current_steering_wheel_angle) &&
                    (can_msg.data.commanded_steering_wheel_angle == commanded_steering_wheel_angle) &&
                    (can_msg.data.enabled() == (control_enabled as u8)) &&
                    (can_msg.data.override_() == (operator_override as u8))
                )
            }
        } else {
            return TestResult::discard();
        }
    }


    fn prop_check_operator_override(spoof_low: u16, spoof_high: u16) -> TestResult{
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            static mut filtered_torque_a: f32 = 0.0;
            static mut filtered_torque_b: f32 = 0.0;
            const torque_filter_alpha: f32 = 0.5;
            unsafe {
                g_steering_control_state.enabled = true;

                spoof_high_signal = spoof_high;
                spoof_low_signal = spoof_low;

                // register analog read callbacks
                register_signal_callbacks(Some(spoof_analog_read_low), Some(spoof_analog_read_high));

                check_for_operator_override();

                filtered_torque_a = (torque_filter_alpha * (spoof_high << 2) as f32) + 
                                    ((1.0 - torque_filter_alpha) * filtered_torque_a);

                filtered_torque_b = (torque_filter_alpha * (spoof_low << 2) as f32) + 
                                    (1.0 - torque_filter_alpha * filtered_torque_b);

                if filtered_torque_a.abs() >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC as f32
                    || filtered_torque_b.abs() >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC as f32
                 {
                    TestResult::from_bool(
                        g_steering_control_state.operator_override == true && 
                        g_steering_control_state.enabled == false
                    )
                }
                else {
                    TestResult::from_bool(g_steering_control_state.operator_override == false)
                }
            }
        } else {
            TestResult::discard()
        }
    }

    impl Arbitrary for oscc_report_steering_data_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_steering_data_s {
            oscc_report_steering_data_s {
                current_steering_wheel_angle: i16::arbitrary(g),
                commanded_steering_wheel_angle: i16::arbitrary(g),
                vehicle_speed: u16::arbitrary(g),
                spoofed_torque_output: i8::arbitrary(g),
                _bitfield_1: u8::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_report_steering_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_steering_s {
            oscc_report_steering_s {
                id: u32::arbitrary(g),
                dlc: u8::arbitrary(g),
                timestamp: u32::arbitrary(g),
                data: oscc_report_steering_data_s::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_command_steering_data_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_steering_data_s {
            oscc_command_steering_data_s {
                commanded_steering_wheel_angle: i16::arbitrary(g),
                commanded_steering_wheel_angle_rate: u8::arbitrary(g),
                _bitfield_1: u8::arbitrary(g),
                reserved_1: u8::arbitrary(g),
                reserved_2: u8::arbitrary(g),
                reserved_3: u8::arbitrary(g),
                count: u8::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_command_steering_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_steering_s {
            oscc_command_steering_s {
                timestamp: u32::arbitrary(g),
                data: oscc_command_steering_data_s::arbitrary(g)
            }
        }
    }


    impl Arbitrary for can_frame_s {
        fn arbitrary<G: Gen>(g: &mut G) -> can_frame_s {
            can_frame_s {
                id: u32::arbitrary(g),
                dlc: u8::arbitrary(g),
                timestamp: u32::arbitrary(g),
                data: [
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g),
                    u8::arbitrary(g)
                ]
            }
        }
    }

    /// the steering firmware should not attempt processing any messages that are not steering commands
    #[test]
    // #[ignore]
    fn check_message_type_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, i16) -> TestResult)
    }

    /// the steering firmware should set the commanded accelerator position
    /// upon reciept of a valid command steering message
    #[test]
    // #[ignore]
    fn check_accel_pos_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_no_invalid_targets as fn(oscc_command_steering_s) -> TestResult)
    }

    /// the steering firmware should set the control state as enabled
    /// upon reciept of a valid command steering message telling it to enable
    #[test]
    // #[ignore]
    fn check_process_enable_command() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_process_enable_command as fn(oscc_command_steering_s) -> TestResult)
    }

    /// the steering firmware should set the control state as disabled
    /// upon reciept of a valid command steering message telling it to disable
    #[test]
    // #[ignore]
    fn check_process_disable_command() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_process_disable_command as fn(oscc_command_steering_s) -> TestResult)
    }

    /// the steering firmware should create only valid CAN frames
    #[test]
    // #[ignore]
    fn check_valid_can_frame() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_send_valid_can_fields as fn(bool, bool, i16, i16, u16, u16) -> TestResult)
    }

    // the steering firmware should be able to correctly and consistently
    // detect operator overrides
    #[test]
    fn check_operator_override() {
        QuickCheck::new()
            .tests(1000)
            .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
            .quickcheck(prop_check_operator_override as fn(u16, u16) -> TestResult)
    }
}