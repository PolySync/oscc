#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/oscc_test.rs"));

// extern crate quickcheck;
// extern crate rand;

// use quickcheck::{QuickCheck, TestResult, Arbitrary, Gen, StdGen};
// use rand::Rng;

// /// the steering firmware should not attempt processing any messages
// /// that are not steering or fault commands
// /// this means that none of the steering control state would
// /// change, nor would it output any values onto the DAC.
// fn prop_only_process_valid_messages(rx_can_msg: can_frame_s, enabled: bool, operator_override: bool, dtcs: u8) -> TestResult {
//     if rx_can_msg.id == OSCC_STEERING_COMMAND_CAN_ID || 
//        rx_can_msg.id == OSCC_FAULT_REPORT_CAN_ID
//     {
//         return TestResult::discard();
//     }
//     unsafe {
//         let dac_a = g_mock_dac_output_a;
//         let dac_b = g_mock_dac_output_b;
//         g_steering_control_state.enabled = enabled;
//         g_steering_control_state.operator_override = operator_override;
//         g_steering_control_state.dtcs = dtcs;

//         g_mock_mcp_can_read_msg_buf_id = rx_can_msg.id;
//         g_mock_mcp_can_read_msg_buf_buf = rx_can_msg.data;
//         g_mock_mcp_can_check_receive_return = CAN_MSGAVAIL as u8;

//         check_for_incoming_message();

//         TestResult::from_bool(g_steering_control_state.enabled ==
//                               enabled &&
//                               g_steering_control_state.operator_override == operator_override &&
//                               g_steering_control_state.dtcs == dtcs &&
//                               g_mock_dac_output_a == dac_a &&
//                               g_mock_dac_output_b == dac_b)
//     }
// }

// #[test]
// fn check_message_type_validity() {
//     QuickCheck::new()
//         .tests(1000)
//         .gen(StdGen::new(rand::thread_rng(), u32::max_value() as usize))
//         .quickcheck(prop_only_process_valid_messages as fn(can_frame_s, bool, bool, u8) -> TestResult)
// }
