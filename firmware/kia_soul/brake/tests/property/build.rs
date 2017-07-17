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
        .define("__STDC_LIMIT_MACROS", None)
        .include("include")
        .include("../../include")
        .include("../../../../../common/testing/mocks")
        .include("../../../../../common/include")
        .include("../../../../../common/libs/can")
        .include("../../../../../common/libs/time")
        .include("../../../../../common/libs/pid")
        .include("../../../../../common/libs/signal_smoothing")
        .include("/usr/lib/avr/include")
        .file("../../../../../common/testing/mocks/Arduino_mock.cpp")
        .file("../../../../../common/testing/mocks/mcp_can_mock.cpp")
        .file("../../src/communications.cpp")
        .file("../../src/brake_control.cpp")
        .file("../../src/globals.cpp")
        .file("../../src/master_cylinder.cpp")
        .file("../../src/helper.cpp")
        .file("../../../../../common/libs/time/oscc_time.cpp")
        .file("../../../../../common/libs/can/oscc_can.cpp")
        .file("../../../../../common/libs/signal_smoothing/oscc_signal_smoothing.cpp")
        .cpp(true)
        .compiler("/usr/bin/g++")
        .compile("libbrake_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        .generate_comments(false)
        .clang_arg("-I../../include")
        .clang_arg("-I../../../../../common/testing/mocks")
        .clang_arg("-I../../../../../common/include")
        .clang_arg("-I../../../../../common/libs/can")
        .clang_arg("-I../../../../../common/libs/pid")
        .clang_arg("-I/usr/lib/avr/include")
        .whitelisted_function("publish_reports")
        .whitelisted_function("check_for_incoming_message")
        .whitelisted_function("raw_adc_to_pressure")
        .whitelisted_function("check_for_operator_override")
        .whitelisted_var("OSCC_REPORT_BRAKE_CAN_ID")
        .whitelisted_var("OSCC_REPORT_BRAKE_CAN_DLC")
        .whitelisted_var("OSCC_COMMAND_BRAKE_CAN_ID")
        .whitelisted_var("OSCC_COMMAND_BRAKE_CAN_DLC")
        .whitelisted_var("DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS")
        .whitelisted_var("OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC")
        .whitelisted_var("CAN_STANDARD")
        .whitelisted_var("CAN_MSGAVAIL")
        .whitelisted_var("g_control_can")
        .whitelisted_var("g_brake_control_state")
        .whitelisted_type("oscc_report_brake_data_s")
        .whitelisted_type("oscc_report_brake_s")
        .whitelisted_type("oscc_command_brake_data_s")
        .whitelisted_type("oscc_command_brake_s")
        .whitelisted_type("can_frame_s")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("brake_test.rs"))
        .expect("Unable to generate bindings");
}
