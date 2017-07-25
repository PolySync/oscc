extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    println!("cargo:rerun-if-changed=../../include");
    println!("cargo:rerun-if-changed=../../include/*");
    println!("cargo:rerun-if-changed=../src");
    println!("cargo:rerun-if-changed=../../src/*");
    println!("cargo:rerun-if-changed=../../../../../common/testing/mocks");
    println!("cargo:rerun-if-changed=../../../../../common/testing/mocks/*");

    gcc::Config::new()
        .flag("-w")
        .include("include")
        .include("../../include")
        .include("../../../../common/include")
        .include("/usr/lib/avr/include")
        .include("../../../../common/testing/mocks/")
        .include("../../../../common/libs/can")
        .include("../../../../common/libs/dac")
        .include("../../../../../api/include")
        .file("../../../../common/testing/mocks/Arduino_mock.cpp")
        .file("../../../../common/testing/mocks/mcp_can_mock.cpp")
        .file("../../../../common/testing/mocks/DAC_MCP49xx_mock.cpp")
        .file("../../../../common/libs/can/oscc_can.cpp")
        .file("../../../../common/libs/dac/oscc_dac.cpp")
        .file("../../src/communications.cpp")
        .file("../../src/steering_control.cpp")
        .file("../../src/globals.cpp")
        .compile("libsteering_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        .generate_comments(false)
        .clang_arg("-I../../include")
        .clang_arg("-I/usr/lib/avr/include")
        .clang_arg("-I../../../../common/include")
        .clang_arg("-I../../../../common/libs/can")
        .clang_arg("-I../../../../common/testing/mocks")
        .clang_arg("-I../../../../../api/include")
        .whitelisted_function("publish_steering_report")
        .whitelisted_function("check_for_incoming_message")
        .whitelisted_function("check_for_operator_override")
        .whitelisted_var("g_steering_control_state")
        .whitelisted_var("OSCC_STEERING_REPORT_CAN_ID")
        .whitelisted_var("OSCC_STEERING_REPORT_CAN_DLC")
        .whitelisted_var("OSCC_STEERING_COMMAND_CAN_ID")
        .whitelisted_var("OSCC_STEERING_COMMAND_CAN_DLC")
        .whitelisted_var("OSCC_FAULT_REPORT_CAN_ID")
        .whitelisted_var("OSCC_STEERING_REPORT_PUBLISH_INTERVAL_IN_MSEC")
        .whitelisted_var("OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC")
        .whitelisted_var("STEERING_SPOOF_SIGNAL_RANGE_MIN")
        .whitelisted_var("STEERING_SPOOF_SIGNAL_RANGE_MAX")
        .whitelisted_var("CAN_STANDARD")
        .whitelisted_var("CAN_MSGAVAIL")
        .whitelisted_var("g_control_can")
        .whitelisted_type("oscc_steering_report_s")
        .whitelisted_type("oscc_steering_command_s")
        .whitelisted_type("can_frame_s")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("steering_test.rs"))
        .expect("Unable to generate bindings");
}
