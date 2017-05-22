#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
include!(concat!(env!("OUT_DIR"), "/brake_test.rs"));

extern crate quickcheck;
extern crate rand;

#[allow(unused_imports)]
#[macro_use]
extern crate lazy_static;

extern "C" {
    #[link_name = "g_mock_mcp_can_check_receive_return"]
    pub static mut g_mock_mcp_can_check_receive_return: u8;
    #[link_name = "g_mock_mcp_can_read_msg_buf_id"]
    pub static mut g_mock_mcp_can_read_msg_buf_id: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_mcp_can_read_msg_buf_buf"]
    pub static mut g_mock_mcp_can_read_msg_buf_buf: [u8; 8usize];
    #[link_name = "g_mock_mcp_can_send_msg_buf_id"]
    pub static mut g_mock_mcp_can_send_msg_buf_id: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_mcp_can_send_msg_buf_ext"]
    pub static mut g_mock_mcp_can_send_msg_buf_ext: u8;
    #[link_name = "g_mock_mcp_can_send_msg_buf_len"]
    pub static mut g_mock_mcp_can_send_msg_buf_len: u8;
    #[link_name = "g_mock_mcp_can_send_msg_buf_buf"]
    pub static mut g_mock_mcp_can_send_msg_buf_buf: *mut u8;
    #[link_name = "g_mock_arduino_millis_return"]
    pub static mut g_mock_arduino_millis_return: ::std::os::raw::c_ulong;
    #[link_name = "g_mock_arduino_analog_read_return"]
    pub static mut g_mock_arduino_analog_read_return: u16;
}

#[cfg(test)]
mod tests {
    use super::*;
    use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};
    use std::sync::Mutex;

    lazy_static! {
        static ref LOCK: Mutex<bool> = Mutex::new(true);
    }

    fn prop_only_process_valid_messages( rx_can_msg: can_frame_s, current_target: u16 ) -> TestResult {
        // if we generate a throttle can message, ignore the result
        if rx_can_msg.id == OSCC_COMMAND_BRAKE_CAN_ID {
            return TestResult::discard()
        }
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                g_brake_control_state.commanded_pedal_position = current_target;

                g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id as u64;
                g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
                g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

                check_for_incoming_message();

                TestResult::from_bool(g_brake_control_state.commanded_pedal_position == current_target)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_no_invalid_targets( command_brake_msg: oscc_command_brake_s ) -> TestResult { 
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID as u64;
                g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_brake_msg.data);
                g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

                check_for_incoming_message();

                TestResult::from_bool(g_brake_control_state.commanded_pedal_position == command_brake_msg.data.pedal_command)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_process_enable_command( command_brake_msg: oscc_command_brake_s ) -> TestResult {
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired && command_brake_msg.data.enabled() == 1 {
            unsafe {
                g_brake_control_state.enabled = false;
                
                g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID as u64;
                g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_brake_msg.data);
                g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

                check_for_incoming_message();
                
                TestResult::from_bool(g_brake_control_state.enabled == true)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_process_disable_command( command_brake_msg: oscc_command_brake_s ) -> TestResult {
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired && command_brake_msg.data.enabled() == 0 {
            unsafe {
                g_mock_mcp_can_read_msg_buf_id = OSCC_COMMAND_BRAKE_CAN_ID as u64;
                g_mock_mcp_can_read_msg_buf_buf = std::mem::transmute(command_brake_msg.data);
                g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

                check_for_incoming_message();
                
                TestResult::from_bool(g_brake_control_state.enabled == false)
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_send_valid_can_fields(control_enabled: bool, operator_override: bool, commanded_pedal_position: u16 ) -> TestResult {
        static mut time: u64 = 0;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                g_brake_control_state.operator_override = operator_override;
                g_brake_control_state.commanded_pedal_position = commanded_pedal_position;

                time = time + OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC as u64;

                g_mock_arduino_millis_return = time;

                publish_reports();

                let brake_data = oscc_report_brake_data_s {
                    pedal_input: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf, *g_mock_mcp_can_send_msg_buf_buf.offset(1)]),
                    pedal_command: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf.offset(2), *g_mock_mcp_can_send_msg_buf_buf.offset(3)]),
                    pedal_output: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf.offset(4), *g_mock_mcp_can_send_msg_buf_buf.offset(5)]),
                    _bitfield_1: std::mem::transmute([*g_mock_mcp_can_send_msg_buf_buf.offset(6), *g_mock_mcp_can_send_msg_buf_buf.offset(7)])
                };

                TestResult::from_bool(
                    (g_mock_mcp_can_send_msg_buf_id == OSCC_REPORT_BRAKE_CAN_ID as u64) &&
                    (g_mock_mcp_can_send_msg_buf_ext == (CAN_STANDARD as u8)) &&
                    (g_mock_mcp_can_send_msg_buf_len == (OSCC_REPORT_BRAKE_CAN_DLC as u8)) &&
                    (brake_data.pedal_command == commanded_pedal_position) && 
                    (brake_data.enabled() == g_brake_control_state.enabled as u8) &&
                    (brake_data.override_() == operator_override as u8)
                )
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_check_operator_override(analog_read_spoof: u16) -> TestResult {
        let lock_acquired = LOCK.lock().unwrap();
        unsafe {
            if *lock_acquired && g_brake_control_state.enabled == true {
                static mut filtered_torque_a: f32 = 0.0;
                static mut filtered_torque_b: f32 = 0.0;
                const torque_filter_alpha: f32 = 0.5;

                // g_brake_control_state.enabled = 1;

                g_mock_arduino_analog_read_return = analog_read_spoof;

                filtered_torque_a = (torque_filter_alpha * raw_adc_to_pressure((analog_read_spoof << 2) as u16)) + 
                                    ((1.0 - torque_filter_alpha) * filtered_torque_a);

                filtered_torque_b = (torque_filter_alpha * raw_adc_to_pressure((analog_read_spoof << 2) as u16)) + 
                                    (1.0 - torque_filter_alpha * filtered_torque_b);

                check_for_operator_override();

                if filtered_torque_a.abs() >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS as f32
                    || filtered_torque_b.abs() >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS as f32
                 {
                    TestResult::from_bool(
                        g_brake_control_state.operator_override == true && 
                        g_brake_control_state.enabled == false
                    )
                }
                else {
                    TestResult::from_bool(g_brake_control_state.operator_override == false)
                }
            } else {
                TestResult::discard()
            }
        }
    }

    impl Arbitrary for oscc_report_brake_data_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_brake_data_s {
            oscc_report_brake_data_s {
                pedal_input: i16::arbitrary(g),
                pedal_command: u16::arbitrary(g),
                pedal_output: u16::arbitrary(g),
                _bitfield_1: u16::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_report_brake_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_report_brake_s {
            oscc_report_brake_s {
                id: u32::arbitrary(g),
                dlc: u8::arbitrary(g),
                timestamp: u32::arbitrary(g),
                data: oscc_report_brake_data_s::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_command_brake_data_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_brake_data_s {
            oscc_command_brake_data_s {
                pedal_command: u16::arbitrary(g),
                _bitfield_1: u16::arbitrary(g),
                reserved_2: u8::arbitrary(g),
                reserved_3: u8::arbitrary(g),
                reserved_4: u8::arbitrary(g),
                count: u8::arbitrary(g)
            }
        }
    }

    impl Arbitrary for oscc_command_brake_s {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_brake_s {
            oscc_command_brake_s {
                timestamp: u32::arbitrary(g),
                data: oscc_command_brake_data_s::arbitrary(g)
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

    /// the throttle firmware should not attempt processing any messages that are not throttle commands
    #[test]
    // #[ignore]
    fn check_message_type_validity() {
        QuickCheck::new()
            .tests(1000)
            .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
            .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, u16) -> TestResult)
    }

    /// the throttle firmware should set the commanded pedal position
    /// upon reciept of a valid command throttle message
    #[test]
    // #[ignore]
    fn check_accel_pos_validity() {
        QuickCheck::new()
            .tests(1000)
            // .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
            .quickcheck(prop_no_invalid_targets as fn(oscc_command_brake_s) -> TestResult)
    }

    /// the throttle firmware should set the control state as enabled
    /// upon reciept of a valid command throttle message telling it to enable
    #[test]
    // #[ignore]
    fn check_process_enable_command() {
        QuickCheck::new()
            .tests(1000)
            // .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
            .quickcheck(prop_process_enable_command as fn(oscc_command_brake_s) -> TestResult)
    }

    /// the throttle firmware should set the control state as disabled
    /// upon reciept of a valid command throttle message telling it to disable
    #[test]
    // #[ignore]
    fn check_process_disable_command() {
        QuickCheck::new()
            .tests(1000)
            .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
            .quickcheck(prop_process_disable_command as fn(oscc_command_brake_s) -> TestResult)
    }

    /// the throttle firmware should create only valid CAN frames
    #[test]
    // #[ignore]
    fn check_valid_can_frame() {
        QuickCheck::new()
            .tests(1000)
            .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
            .quickcheck(prop_send_valid_can_fields as fn(bool, bool, u16) -> TestResult)
    }

    // the brake firmware should be able to correctly and consistently
    // detect operator overrides
    #[test]
    // #[ignore]
    fn check_operator_override() {
        QuickCheck::new()
            .tests(1000)
            .gen(StdGen::new(rand::thread_rng(), u16::max_value() as usize))
            .quickcheck(prop_check_operator_override as fn(u16) -> TestResult)
    }
}