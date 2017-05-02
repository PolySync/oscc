#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/communications.rs"));

extern crate quickcheck;

#[allow(dead_code)]
extern fn callback(a: i32) {
    println!("I'm called from C with value {0}", a);
}

#[cfg(test)]
mod tests {
    use super::*;
    use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen};

    fn prop_only_process_valid_messages( rx_can_msg: can_frame_s, current_target: f32 ) -> TestResult {
        // if we generate a throttle can message, ignore the result
        if rx_can_msg.id == OSCC_CAN_ID_THROTTLE_COMMAND {
            return TestResult::discard()
        }
        unsafe {
            throttle_state.accel_pos_target = current_target;
            handle_ready_rx_frame( &rx_can_msg );
            TestResult::from_bool(current_target == throttle_state.accel_pos_target)
        }
    }

    fn prop_no_invalid_targets( command_msg: oscc_command_msg_throttle ) -> bool { 
        let pointer = &command_msg as *const _;
        unsafe {
            process_throttle_command( pointer as *const u8);
            throttle_state.accel_pos_target >= 0.0 && throttle_state.accel_pos_target <= (u16::max_value() / 4) as f32
        }
    }

    fn prop_process_enable_command( mut command_msg: oscc_command_msg_throttle ) -> bool {
        command_msg.set_enabled(1); // we're going to recieve an enable command
        let pointer = &command_msg as *const _;
        unsafe {
            process_throttle_command( pointer as *const u8);
            control_state.enabled == true
        }
    }

    fn prop_process_disable_command( mut command_msg: oscc_command_msg_throttle ) -> bool { 
        command_msg.set_enabled(0);
        let pointer = &command_msg as *const _;
        unsafe {
            process_throttle_command( pointer as *const u8 );
            control_state.enabled == false
        }
    }

    fn prop_send_valid_can_fields() -> bool {
        unsafe { publish_throttle_report() };
        return true;
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

    impl Arbitrary for oscc_command_msg_throttle {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_msg_throttle {
            oscc_command_msg_throttle {
                // halp how can we get numbers bigger than 100 from this freaking dude?
                accelerator_command: u16::arbitrary(g),
                reserved_0: u8::arbitrary(g),
                _bitfield_1: u8::arbitrary(g),
                reserved_2: u8::arbitrary(g),
                reserved_3: u8::arbitrary(g),
                reserved_4: u8::arbitrary(g),
                count: u8::arbitrary(g)
            }
        }
    }

    #[test]
    #[ignore]
    fn check_message_type_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, f32) -> TestResult)
    }

    #[test]
    #[ignore]
    fn check_accel_pos_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_no_invalid_targets as fn(oscc_command_msg_throttle) -> bool)
    }

    #[test]
    #[ignore]
    fn check_process_enable_command() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_process_enable_command as fn(oscc_command_msg_throttle) -> bool)
    }

    #[test]
    #[ignore]
    fn check_process_disable_command() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_process_disable_command as fn(oscc_command_msg_throttle) -> bool)
    }

    #[test]
    fn check_valid_can_frame() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_send_valid_can_fields as fn() -> bool)
    }
}