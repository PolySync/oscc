extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .flag("-w")
        .include("include")
        .include("../include")
        .include("../../../../common/include")
        .include("../../../../common/libs/arduino_init")
        .include("../../../../common/libs/serial")
        .include("../../../../common/libs/can")
        .include("../../../../common/libs/time")
        .include("../../../../common/libs/PID")
        .include("/usr/lib/avr/include")
        .file("src/mocks/SPI.cpp")
        .file("src/mocks/Arduino.cpp")
        .file("src/mocks/mcp_can.cpp")
        .file("src/mocks/DAC_MCP49xx.cpp")
        .file("../src/communications.cpp")
        .file("../src/steering_control.cpp")
        .file("../src/globals.cpp")
        .cpp(true)
        .compiler("/usr/bin/g++")
        .compile("libcomm_test.a");
    
    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        .generate_comments(false)
        .clang_arg("-Iinclude")
        .clang_arg("-I/usr/lib/avr/include")
        .clang_arg("-I../../../../common/libs/can")
        .clang_arg("-I../../../../common/libs/PID")
        .whitelisted_function("publish_steering_report")
        .whitelisted_function("handle_ready_rx_frame")
        .whitelisted_function("process_steering_command")
        .whitelisted_function("*register_callback*")
        .whitelisted_var("tx_frame_steering_report")
        .whitelisted_var("rx_frame_steering_command")
        .whitelisted_var("control_state")
        .whitelisted_var("steering_state")
        .whitelisted_var("override_flags")
        .whitelisted_var("OSCC_CAN_ID_STEERING_COMMAND")
        .whitelisted_var("OSCC_CAN_ID_STEERING_REPORT")
        .whitelisted_var("messsage")
        .whitelisted_var("can")
        .whitelisted_type("oscc_report_msg_steering")
        .whitelisted_type("oscc_command_msg_steering")
        .generate().unwrap()
        .write_to_file(Path::new(&out_dir).join("communications.rs"))
        .expect("Unable to generate bindings");
}