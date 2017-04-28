#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/communications.rs"));

struct oscc_command_msg_throttle {
    accelerator_command: u16,
    enabled: u8,
    count: u8
}

fn main() {
    println!("blah");
    unsafe { 
        let mut command_msg = oscc_command_msg_throttle {
            accelerator_command: 90,
            enabled: 1,
            count: 1
        };
        println!("id: {}, dlc: {}, timestamp: {}, data: {}, {}", tx_frame_throttle_report.id, tx_frame_throttle_report.dlc, tx_frame_throttle_report.timestamp, tx_frame_throttle_report.data[0], tx_frame_throttle_report.data[1] );
        publish_throttle_report();
        println!("id: {}, dlc: {}, timestamp: {}, data: {}, {}", tx_frame_throttle_report.id, tx_frame_throttle_report.dlc, tx_frame_throttle_report.timestamp, tx_frame_throttle_report.data[0], tx_frame_throttle_report.data[1] );
        // process_throttle_command(&command_msg as *const u8);
        println!("id: {}, dlc: {}, timestamp: {}, data: {}, {}", tx_frame_throttle_report.id, tx_frame_throttle_report.dlc, tx_frame_throttle_report.timestamp, tx_frame_throttle_report.data[0], tx_frame_throttle_report.data[1] );
        tx_frame_throttle_report.id = 666; 
        println!("id: {}, dlc: {}, timestamp: {}, data: {}, {}", tx_frame_throttle_report.id, tx_frame_throttle_report.dlc, tx_frame_throttle_report.timestamp, tx_frame_throttle_report.data[0], tx_frame_throttle_report.data[1] );
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn print_thing() {
    }
}