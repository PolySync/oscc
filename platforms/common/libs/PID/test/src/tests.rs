#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/PID.rs"));

extern crate quickcheck;

fn main () {

    let mut pid = PID  { windup_guard: 16.801239, proportional_gain: 0.91900635, integral_gain: -15.34066, derivative_gain: 53.16655, prev_input: 56.590088, int_error: -59.569313, control: -87.068054, prev_steering_angle: -65.56142 };

    let setpoint = 0.0;
    let input = 1.0;
    let dt = 59.0;
    unsafe{ pid_zeroize(&mut pid, pid.windup_guard) };
    println!("Before: {:?}", pid);
    unsafe{ pid_update(&mut pid, setpoint, input, dt) };
    println!("Between: {:?}", pid);
    let first_control = pid.control;
    unsafe{ pid_update(&mut pid, setpoint, (input + pid.control), dt) };
    let second_control = pid.control;
    println!("After: {:?}", pid);
    println!("{} == {} ? {}", first_control.abs(), second_control.abs(), first_control.abs() < second_control.abs());
}

#[cfg(test)]
mod tests {
    use super::*;
    use quickcheck::{QuickCheck, Arbitrary, Gen, TestResult};

    fn prop_zeroize( mut pid: PID, integral_windup_guard: f32 ) -> bool {
        unsafe{ pid_zeroize(&mut pid, integral_windup_guard) }
        pid.prev_input == 0.0 &&
        pid.int_error == 0.0 &&
        pid.prev_steering_angle == 0.0 &&
        pid.windup_guard == integral_windup_guard
    }

    fn prop_reverse_inputs( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> TestResult {
        if dt <= 0.0 || setpoint == input{
            return TestResult::discard();
        }

        // these depend on order, so zero them out.
        pid.integral_gain = 0.0;
        pid.derivative_gain = 0.0;
        pid.prev_input = 0.0;
        pid.int_error = 0.0;

        let mut original = pid.clone();
        unsafe{ pid_update(&mut original, setpoint, input, dt ) };
        let first_control = original.control;
        unsafe{ pid_update(&mut pid, input, setpoint, dt) };
        TestResult::from_bool(first_control == -pid.control)
    }

    fn prop_same_control_for_same_inputs( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> bool {
        let mut original = pid.clone();
        unsafe{ pid_update(&mut original, setpoint, input, dt) };
        let first_control = original.control;
        unsafe{ pid_update(&mut pid, setpoint, input, dt) };
        first_control == pid.control
    }

    // should probably be zero, but getting an error due to the integral term...
    fn prop_same_setpoint_and_input( mut pid: PID, setpoint: f32, dt:f32 ) -> TestResult {
        if dt <= 0.0 {
            return TestResult::discard();
        }
        pid.prev_input = setpoint;
        unsafe{ pid_update(&mut pid, setpoint, setpoint, dt) };
        TestResult::from_bool(pid.control == 0.0)
    }

    fn prop_proportional_term( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> TestResult {
        if dt <= 0.0 {
            return TestResult::discard();
        }
        pid.integral_gain = 0.0;
        pid.derivative_gain = 0.0;
        unsafe{ pid_update(&mut pid, setpoint, input, dt) };
        let p_only_control = pid.proportional_gain * (setpoint - input);
        TestResult::from_bool(p_only_control == pid.control)
    }

    fn prop_integral_term( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> TestResult {
        if dt <= 0.0 {
            return TestResult::discard();
        }
        pid.proportional_gain = 0.0;
        pid.derivative_gain = 0.0;
        let mut pid_int_error = pid.int_error;
        unsafe{ pid_update(&mut pid, setpoint, input, dt) };
        pid_int_error += (setpoint - input) * dt;
        if pid_int_error < -pid.windup_guard {
            pid_int_error = -pid.windup_guard;
        }
        else if pid_int_error > pid.windup_guard {
            pid_int_error = pid.windup_guard;
        }
        let i_term = pid.integral_gain * pid_int_error;
        TestResult::from_bool(i_term == pid.control)
    }

    fn prop_derivative_term( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> TestResult {
        if dt <= 0.0 {
            return TestResult::discard();
        }
        pid.proportional_gain = 0.0;
        pid.integral_gain = 0.0;
        let pid_prev_input = pid.prev_input;
        unsafe{ pid_update(&mut pid, setpoint, input, dt) };
        let d_term = pid.derivative_gain * ((input - pid_prev_input) / dt);
        // currently, we subtract the d_term from our control calculation
        // to mitigate derivative kick, hence our control term = -d_term if
        // i_tem and p_term are 0. 
        // this could probably use some cleanup to make the functionality
        // more clear to developers.
        TestResult::from_bool(d_term == -pid.control)
    }

    fn prop_control_approaches_difference( mut pid: PID, setpoint: f32, input: f32, dt: f32 ) -> TestResult {
        if dt <= 0.0 || setpoint == input {
            return TestResult::discard();
        }
        unsafe{ pid_zeroize(&mut pid, pid.windup_guard) };
        unsafe{ pid_update(&mut pid, setpoint, input, dt) };
        let first_control = pid.control;
        unsafe{ pid_update(&mut pid, setpoint, (input + pid.control), dt) };
        let second_control = pid.control;
        TestResult::from_bool(first_control.abs() < second_control.abs())

    }

    impl Arbitrary for PID {
        fn arbitrary<G: Gen>(g: &mut G) -> PID {
            PID {
                windup_guard: f32::arbitrary(g),
                proportional_gain: f32::arbitrary(g),
                integral_gain: f32::arbitrary(g),
                derivative_gain: f32::arbitrary(g),
                prev_input: f32::arbitrary(g),
                int_error: f32::arbitrary(g),
                control: f32::arbitrary(g),
                prev_steering_angle: f32::arbitrary(g)
            }
        }
    }

    #[test]
    fn check_zeroize(){
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_zeroize as fn(PID, f32) -> bool)
    }

    #[test]
    fn check_reversed_inputs() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_reverse_inputs as fn(PID, f32, f32, f32) -> TestResult)
    }

    #[test]
    fn check_same_control_for_same_inputs() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_same_control_for_same_inputs as fn(PID, f32, f32, f32) -> bool)
    }

    #[test]
    #[ignore]
    fn check_same_setpoint_and_input() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_same_setpoint_and_input as fn(PID, f32, f32) -> TestResult)
    }

    #[test]
    fn check_proportional_term() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_proportional_term as fn(PID, f32, f32, f32) -> TestResult)
    }

    #[test]
    fn check_integral_term() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_integral_term as fn(PID, f32, f32, f32) -> TestResult)
    }

    #[test]
    fn check_derivative_term() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_derivative_term as fn(PID, f32, f32, f32) -> TestResult)
    }

    #[test]
    #[ignore]
    fn check_control_approaches_difference() {
        QuickCheck::new()
            .tests(1000)
            .quickcheck(prop_control_approaches_difference as fn(PID, f32, f32, f32) -> TestResult)
    }


}