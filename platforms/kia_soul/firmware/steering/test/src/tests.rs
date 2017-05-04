#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/communications.rs"));

extern crate quickcheck;

#[macro_use]
extern crate lazy_static;

use std::mem;

struct mock_can_frame {
    id: u32,
    dlc: u8,
    data: oscc_command_msg_steering
}

// static mut can_msg: mock_can_frame = mock_can_frame {
//     id: 0,
//     dlc: 0,
//     data: oscc_report_msg_steering{
//         accelerator_input: 0,
//         accelerator_command: 0,
//         accelerator_output: 0,
//         _bitfield_1: 0
//     }
// };

// #[allow(dead_code)]
// extern fn retrieve_sent_can_msg( id: u32, dlc: u8, buf: *mut u8) {
//     unsafe { 
//         can_msg.id = id; 
//         can_msg.dlc = dlc;
//         // endianness is important here
//         can_msg.data.accelerator_input = mem::transmute([*buf, *buf.offset(1)]);
//         can_msg.data.accelerator_command = mem::transmute([*buf.offset(2), *buf.offset(3)]);
//         can_msg.data.accelerator_output = mem::transmute([*buf.offset(4), *buf.offset(5)]);
//         can_msg.data._bitfield_1 = mem::transmute([*buf.offset(6), *buf.offset(7)]);
//     }
// }

fn main() {
    println!("hi");
//             // ************************************** process enable command
//         let mut command_msg = oscc_command_msg_throttle {
//             // halp how can we get numbers bigger than 100 from this freaking dude?
//             accelerator_command: 83,
//             reserved_0: 72,
//             _bitfield_1: 38,
//             reserved_2: 85,
//             reserved_3: 18,
//             reserved_4: 67,
//             count: 56
//         };

//         command_msg.set_enabled(1); // we're going to recieve an enable command
//         let pointer = &command_msg as *const _;
//         unsafe {
//             process_throttle_command( pointer as *const u8);
//             println!("control state: {}", control_state.enabled);
//         }

//     // ********************************** check valid can frame
//         // unsafe {
//         //     override_flags.accelerator_pressed = true;
//         //     override_flags.voltage = 28;
//         //     control_state.enabled = true;
//         //     throttle_state.accel_pos_sensor_low = 79;
//         //     throttle_state.accel_pos_sensor_high = 38;
//         //     throttle_state.accel_pos_target = 65.157394;

//         //     let mut override_flag = 1;

//         //     if !override_flags.accelerator_pressed && override_flags.voltage == 0 {
//         //         override_flag = 0;
//         //     }

//         //     MCP_CAN_register_callback(&mut can, Some(retrieve_sent_can_msg));
//         //     publish_throttle_report();

//         //     println!("{} == {} ? {}", can_msg.id, OSCC_CAN_ID_THROTTLE_REPORT, can_msg.id == OSCC_CAN_ID_THROTTLE_REPORT);
//         //     println!("{} == {} ? {}", can_msg.dlc, 8, can_msg.dlc == 8);
//         //     println!("{} == {} ? {}", can_msg.data.accelerator_input, (throttle_state.accel_pos_sensor_low + throttle_state.accel_pos_sensor_high), can_msg.data.accelerator_input == (throttle_state.accel_pos_sensor_low + throttle_state.accel_pos_sensor_high));
//         //     println!("{} == {} ? {}", can_msg.data.accelerator_command, throttle_state.accel_pos_target as u16 , can_msg.data.accelerator_command == throttle_state.accel_pos_target as u16);
//         //     println!("{} == {} ? {}", can_msg.data.enabled(), control_state.enabled as u8, can_msg.data.enabled() == control_state.enabled as u8);
//         //     println!("{} == {} ? {}", can_msg.data.override_(), override_flag, can_msg.data.override_() == override_flag);
//         // }
}

#[cfg(test)]
mod tests {
    use super::*;
    use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen};
    use std::sync::Mutex;

    lazy_static! {
        static ref LOCK: Mutex<bool> = Mutex::new(true);
    }

    fn prop_only_process_valid_messages( rx_can_msg: can_frame_s, current_steering_target: f32, current_max_angle_rate: f32 ) -> TestResult {
        // if we generate a throttle can message, ignore the result
        if rx_can_msg.id == OSCC_CAN_ID_STEERING_COMMAND {
            return TestResult::discard()
        }
        unsafe {
            let lock_acquired = LOCK.lock().unwrap();
            if *lock_acquired {
                steering_state.steering_angle_target = current_steering_target;
                steering_state.steering_angle_rate_max = current_max_angle_rate;
                handle_ready_rx_frame( &rx_can_msg );
                TestResult::from_bool(  steering_state.steering_angle_target == current_steering_target &&
                                        steering_state.steering_angle_rate_max == current_max_angle_rate)
            } else {
                return TestResult::discard();
            }
        }
    }

    fn prop_no_invalid_targets( command_msg: oscc_command_msg_steering ) -> TestResult { 
        let pointer = &command_msg as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                process_steering_command( pointer as *const u8);
                TestResult::from_bool(
                    steering_state.steering_angle_target == 
                        command_msg.steering_wheel_angle_command / 9.0 && 
                    steering_state.steering_angle_rate_max ==
                        command_msg.steering_wheel_max_velocity * 9.0
                )
            }
        } else {
            return TestResult::discard();
        }
    }

    fn prop_process_enable_command( mut command_msg: oscc_command_msg_steering ) -> TestResult {
        command_msg.set_enabled(1); // we're going to recieve an enable command
        let pointer = &command_msg as *const _;
        let lock_acquired = LOCK.lock().unwrap();
        if *lock_acquired {
            unsafe {
                control_state.emergency_stop = false;
                process_steering_command( pointer as *const u8);
                TestResult::from_bool(control_state.enabled)
            }
        } else {
            return TestResult::discard();
        }
    }

    // fn prop_process_disable_command( mut command_msg: oscc_command_msg_throttle ) -> TestResult {
    //     command_msg.set_enabled(0);
    //     let pointer = &command_msg as *const _;
    //     let lock_acquired = LOCK.lock().unwrap();
    //     if *lock_acquired {
    //         unsafe {
    //             process_throttle_command( pointer as *const u8 );
    //             TestResult::from_bool(control_state.enabled == false)
    //         }
    //     } else {
    //         return TestResult::discard();
    //     }
    // }

    // fn prop_send_valid_can_fields(
    //         (accelerator_pressed, voltage, control_enabled): (bool, u16, bool), 
    //         (accel_pos_sensor_low, accel_pos_sensor_high, accel_pos_target): (u16, u16, f32)
    //             ) -> TestResult 
    // {
    //     // MUTEX.lock().unwrap();
    //     let lock_acquired = LOCK.lock().unwrap();
    //     if *lock_acquired {
    //         unsafe {
    //             override_flags.accelerator_pressed = accelerator_pressed;
    //             override_flags.voltage = voltage;
    //             control_state.enabled = control_enabled;
    //             throttle_state.accel_pos_sensor_low = accel_pos_sensor_low;
    //             throttle_state.accel_pos_sensor_high = accel_pos_sensor_high;
    //             throttle_state.accel_pos_target = accel_pos_target;

    //             let mut override_flag = 1;

    //             if !accelerator_pressed && voltage == 0 {
    //                 override_flag = 0;
    //             }

    //             MCP_CAN_register_callback(&mut can, Some(retrieve_sent_can_msg));
    //             publish_throttle_report();

    //             TestResult::from_bool(
    //                 (can_msg.id == OSCC_CAN_ID_THROTTLE_REPORT) &&
    //                 (can_msg.dlc == 8) &&
    //                 (can_msg.data.accelerator_input == (accel_pos_sensor_low + accel_pos_sensor_high)) &&
    //                 (can_msg.data.accelerator_command == accel_pos_target as u16) &&
    //                 (can_msg.data.enabled() == control_enabled as u8) &&
    //                 (can_msg.data.override_() == override_flag)
    //             )
    //         }
    //     } else {
    //         return TestResult::discard();
    //     }
    // }

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

    impl Arbitrary for oscc_command_msg_steering {
        fn arbitrary<G: Gen>(g: &mut G) -> oscc_command_msg_steering {
            oscc_command_msg_steering {
                steering_wheel_angle_command: i16::arbitrary(g),
                _bitfield_1: u8::arbitrary(g),
                steering_wheel_max_velocity: u8::arbitrary(g),
                torque: u16::arbitrary(g),
                reserved_3: u8::arbitrary(g),
                count: u8::arbitrary(g)
            }
        }
    }

    #[test]
    // #[ignore]
    fn check_message_type_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, f32, f32) -> TestResult)
    }

    #[test]
    // #[ignore]
    fn check_accel_pos_validity() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_no_invalid_targets as fn(oscc_command_msg_steering) -> TestResult)
    }

    // #[test]
    // #[ignore]
    // fn check_process_enable_command() {
    //     QuickCheck::new()
    //         .tests(1000)
    //         .quickcheck(prop_process_enable_command as fn(oscc_command_msg_throttle) -> TestResult)
    // }

    // #[test]
    // #[ignore]
    // fn check_process_disable_command() {
    //     QuickCheck::new()
    //         .tests(1000)
    //         .quickcheck(prop_process_disable_command as fn(oscc_command_msg_throttle) -> TestResult)
    // }

    // #[test]
    // #[ignore]
    // fn check_valid_can_frame() {
    //     QuickCheck::new()
    //         .tests(1000)
    //         .quickcheck(prop_send_valid_can_fields as fn((bool, u16, bool), (u16, u16, f32)) -> TestResult)
    // }
}