extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .flag("-w")
        .define("__STDC_LIMIT_MACROS", None)
        .include("include")
        .include("../../include")
        .include("../../../../../common/testing/mocks")
        .include("../../../../../common/include")
        .include("../../../../../common/libs/arduino_init")
        .include("../../../../../common/libs/serial")
        .include("../../../../../common/libs/can")
        .include("../../../../../common/libs/time")
        .include("../../../../../common/libs/dac")
        .include("/usr/lib/avr/include")
        .file("../../../../../common/testing/mocks/Arduino_mock.cpp")
        .file("../../../../../common/testing/mocks/mcp_can_mock.cpp")
        .file("../../../../../common/testing/mocks/DAC_MCP49xx_mock.cpp")
        .file("../../../../../common/libs/time/oscc_time.cpp")
        .file("../../../../../common/libs/can/oscc_can.cpp")
        .file("../../../../../common/libs/dac/oscc_dac.cpp")
        .file("../../src/communications.cpp")
        .file("../../src/throttle_control.cpp")
        .file("../../src/globals.cpp")
        .cpp(true)
        .compiler("/usr/bin/g++")
        .compile("libthrottle_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        .generate_comments(false)
        .clang_arg("-I/usr/lib/avr/include")
        .clang_arg("-I../../include")
        .clang_arg("-I../../../../../common/include")
        .clang_arg("-I../../../../../common/libs/can")
        .clang_arg("-I../../../../../common/libs/dac")
        .clang_arg("-I../../../../../common/libs/time")
        .clang_arg("-I../../../../../common/testing/mocks")
        .whitelisted_function("publish_reports")
        .whitelisted_function("check_for_incoming_message")
        .whitelisted_function("check_for_operator_override")
        .whitelisted_var("g_throttle_control_state")
        .whitelisted_var("OSCC_REPORT_THROTTLE_CAN_ID")
        .whitelisted_var("OSCC_REPORT_THROTTLE_CAN_DLC")
        .whitelisted_var("OSCC_COMMAND_THROTTLE_CAN_ID")
        .whitelisted_var("OSCC_COMMAND_THROTTLE_CAN_DLC")
        .whitelisted_var("ACCELERATOR_OVERRIDE_THRESHOLD")
        .whitelisted_var("OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC")
        .whitelisted_var("CAN_MSGAVAIL")
        .whitelisted_var("CAN_STANDARD")
        .whitelisted_var("g_control_can")
        .whitelisted_type("oscc_report_throttle_data_s")
        .whitelisted_type("oscc_report_throttle_s")
        .whitelisted_type("oscc_command_throttle_data_s")
        .whitelisted_type("oscc_command_throttle_s")
        .whitelisted_type("can_frame_s")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("throttle_test.rs"))
        .expect("Unable to generate bindings");
}
