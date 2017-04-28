extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .include("include")
        .include("../include")
        .include("../../../../common/include")
        .include("../../../../common/libs/arduino_init")
        .include("../../../../common/libs/serial")
        .include("../../../../common/libs/can")
        .include("../../../../common/libs/time")
        .include("/usr/lib/avr/include")
        .file("src/SPI.cpp") // include Arduino lib mockups so that we can test on s/w only
        .file("src/Arduino.cpp")
        .file("src/mcp_can.cpp")
        .file("src/DAC_MCP49xx.cpp")
        .file("../src/communications.cpp")
        .file("../src/throttle_control.cpp")
        .file("../src/globals.cpp")
        .compile("libcomm_test.a");
    
    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        .generate_comments(false)
        .clang_arg("-Iinclude")
        .clang_arg("-I/usr/lib/avr/include")
        .clang_arg("-I../../../../common/libs/can")
        .whitelisted_function("publish_throttle_report")
        .whitelisted_function("process_throttle_command")
        .whitelisted_var("tx_frame_throttle_report")
        .whitelisted_var("rx_frame_throttle_command")
        .whitelisted_var("control_state")
        .whitelisted_var("throttle_state")
        .whitelisted_var("override_flags")
        // .whitelisted_type("oscc_report_msg_throttle") // works!
        // .whitelisted_type("oscc_command_msg_throttle")
        .generate().unwrap()
        .write_to_file(Path::new(&out_dir).join("communications.rs"))
        .expect("Unable to generate bindings");
}