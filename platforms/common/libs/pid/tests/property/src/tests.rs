#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![allow(unused_imports)]
include!(concat!(env!("OUT_DIR"), "/pid.rs"));

extern crate quickcheck;

use quickcheck::{QuickCheck, Arbitrary, Gen, TestResult};

impl Arbitrary for pid_s {
    fn arbitrary<G: Gen>(g: &mut G) -> pid_s {
        pid_s {
            windup_guard: f32::arbitrary(g),
            proportional_gain: f32::arbitrary(g),
            integral_gain: f32::arbitrary(g),
            derivative_gain: f32::arbitrary(g),
            prev_input: f32::arbitrary(g),
            int_error: f32::arbitrary(g),
            control: f32::arbitrary(g),
            prev_steering_angle: f32::arbitrary(g),
        }
    }
}

/// zeroize should always reset these values
fn prop_zeroize(mut pid: pid_s, integral_windup_guard: f32) -> bool {
    unsafe { pid_zeroize(&mut pid, integral_windup_guard) }
    pid.prev_input == 0.0 && pid.int_error == 0.0 && pid.prev_steering_angle == 0.0 &&
    pid.windup_guard == integral_windup_guard
}

#[test]
fn check_zeroize() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_zeroize as fn(pid_s, f32) -> bool)
}

/// sending the inputs in reversed order should result in the same, only negated, control
fn prop_reverse_inputs(mut pid: pid_s, setpoint: f32, input: f32, dt: f32) -> TestResult {
    if dt <= 0.0 || setpoint == input {
        return TestResult::discard();
    }

    // these depend on order, so zero them out.
    pid.integral_gain = 0.0;
    pid.derivative_gain = 0.0;
    pid.prev_input = 0.0;
    pid.int_error = 0.0;

    let mut original = pid.clone();
    unsafe { pid_update(&mut original, setpoint, input, dt) };
    let first_control_size = original.control;
    unsafe { pid_update(&mut pid, input, setpoint, dt) };
    TestResult::from_bool(first_control_size == -pid.control)
}

#[test]
fn check_reversed_inputs() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_reverse_inputs as fn(pid_s, f32, f32, f32) -> TestResult)
}

/// passing in the same inputs for two separate calls should result in the same answer
fn prop_same_control_for_same_inputs(mut pid: pid_s, setpoint: f32, input: f32, dt: f32) -> bool {
    let mut original = pid.clone();
    unsafe { pid_update(&mut original, setpoint, input, dt) };
    let first_control_size = original.control;
    unsafe { pid_update(&mut pid, setpoint, input, dt) };
    first_control_size == pid.control
}

#[test]
fn check_same_control_for_same_inputs() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_same_control_for_same_inputs as fn(pid_s, f32, f32, f32) -> bool)
}

/// check proportional term validity with standard formula
fn prop_proportional_term(mut pid: pid_s, setpoint: f32, input: f32, dt: f32) -> TestResult {
    if dt <= 0.0 {
        return TestResult::discard();
    }
    pid.integral_gain = 0.0;
    pid.derivative_gain = 0.0;
    unsafe { pid_update(&mut pid, setpoint, input, dt) };
    let p_only_control = pid.proportional_gain * (setpoint - input);
    TestResult::from_bool(p_only_control == pid.control)
}

#[test]
fn check_proportional_term() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_proportional_term as fn(pid_s, f32, f32, f32) -> TestResult)
}

/// check integral term validity with standard formula
fn prop_integral_term(mut pid: pid_s, setpoint: f32, input: f32, dt: f32) -> TestResult {
    if dt <= 0.0 {
        return TestResult::discard();
    }
    pid.proportional_gain = 0.0;
    pid.derivative_gain = 0.0;
    let mut pid_int_error = pid.int_error;
    unsafe { pid_update(&mut pid, setpoint, input, dt) };
    pid_int_error += (setpoint - input) * dt;
    if pid_int_error < -pid.windup_guard {
        pid_int_error = -pid.windup_guard;
    } else if pid_int_error > pid.windup_guard {
        pid_int_error = pid.windup_guard;
    }
    let i_term = pid.integral_gain * pid_int_error;
    TestResult::from_bool(i_term == pid.control)
}

#[test]
fn check_integral_term() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_integral_term as fn(pid_s, f32, f32, f32) -> TestResult)
}

/// check derivative term validity with standard formula
fn prop_derivative_term(mut pid: pid_s, setpoint: f32, input: f32, dt: f32) -> TestResult {
    if dt <= 0.0 {
        return TestResult::discard();
    }
    pid.proportional_gain = 0.0;
    pid.integral_gain = 0.0;
    let pid_prev_input = pid.prev_input;
    unsafe { pid_update(&mut pid, setpoint, input, dt) };
    let d_term = pid.derivative_gain * ((input - pid_prev_input) / dt);
    // currently, we subtract the d_term from our control calculation
    // to mitigate derivative kick, hence our control term = -d_term if
    // i_tem and p_term are 0.
    // this could probably use some cleanup to make the functionality
    // more clear to developers.
    TestResult::from_bool(d_term == -pid.control)
}

#[test]
fn check_derivative_term() {
    QuickCheck::new()
        .tests(1000)
        .quickcheck(prop_derivative_term as fn(pid_s, f32, f32, f32) -> TestResult)
}
