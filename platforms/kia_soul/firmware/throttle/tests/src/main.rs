#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/communications.rs"));

// recreate some structs to pass into our cpp lib
#[no_mangle]
struct oscc_command_msg_throttle {
    accelerator_command: u16,
    reserved_0: u8,
    enabled: u8,
    clear: u8,
    ignore: u8,
    reserved_1: u8,
    reserved_2: u8,
    reserved_3: u8,
    reserved_4: u8,
    count: u8
}

fn main() {
    unsafe { 
        let mut command_msg = oscc_command_msg_throttle {
            accelerator_command: 90,
            reserved_0: 0,
            enabled: 1,
            clear: 1, 
            ignore: 1,
            reserved_1: 5,
            reserved_2: 0,
            reserved_3: 0,
            reserved_4: 0,
            count: 0
        };

        let pointer= &command_msg as *const _;

        println!("Before: {}, {}", throttle_state.accel_pos_target, control_state.enabled);
        process_throttle_command(pointer as *const u8);
        println!("After: {}, {}", throttle_state.accel_pos_target, control_state.enabled);
        command_msg.enabled = 0;
        process_throttle_command(pointer as *const u8);
        println!("After disable: {}, {}", throttle_state.accel_pos_target, control_state.enabled);
        publish_throttle_report();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn print_thing() {
    }
}