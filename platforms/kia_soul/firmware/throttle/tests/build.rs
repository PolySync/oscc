extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .file("../src/communications.cpp")
        .include("include")
        .include("../include")
        .include("../../../../common/include")
        .include("../../../../common/libs/arduino_init")
        .include("../../../../common/libs/DAC_MCP49xx")
        .include("../../../../common/libs/mcp_can")
        .include("../../../../common/libs/serial")
        .include("../../../../common/libs/can")
        .include("../../../../common/libs/time")
        .include("/usr/share/arduino/hardware/arduino/cores/arduino/")
        .include("/usr/lib/avr/include/")
        .include("/usr/lib/avr/include/avr")
        .include("/usr/share/arduino/hardware/arduino/variants/standard/")
        .compile("libbarf.a");
    
    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("include/wrapper.hpp")
        // // .use_core()
        .whitelisted_function("say_hello")
        .generate().unwrap()
        .write_to_file(Path::new(&out_dir).join("communications.rs"))
        .expect("Unable to generate bindings");
}