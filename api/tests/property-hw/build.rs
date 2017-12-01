extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn gcc_builder(target_vehicle: &str) {
    gcc::Build::new()
        .flag("-w")
        .define(target_vehicle, Some("ON"))
        .include("include")
        .include("../../include")
        .include("../../src")
        .file("../../src/oscc.c")
        .compile("liboscc_test.a");
}

fn bindgen_builder(target_vehicle: &str) {
    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::Builder::default()
        .header("include/wrapper.h")
        .generate_comments(false)
        .layout_tests(false)
        .trust_clang_mangling(false)
        .clang_arg(target_vehicle)
        .clang_arg("-I../../include")
        .clang_arg("-I../../src")
        .whitelisted_type("oscc_[a-z]+_[a-z]+_s")
        .whitelisted_var("OSCC_[A-Z]+_[A-Z]+_CAN_ID")
        .whitelisted_var("[A-Z]+_[A-Z]+_COMMAND")
        .whitelisted_var("TORQUE_SPOOF_[A-Z]+_SIGNAL_CALIBRATION_CURVE_[A-Z]+")
        .whitelisted_var("[A-Z]+_SPOOF_[A-Z]+_SIGNAL_[A-Z]+_[A-Z]+")
        .whitelisted_var("STEPS_PER_VOLT")
        .whitelisted_var("OSCC_MAGIC_BYTE_[0|1]")
        .whitelisted_function("oscc_publish_[a-z]+_[a-z]+")
        .whitelisted_function("oscc_subscribe_to_[a-z]+_[a-z]+")
        .whitelisted_function("oscc_[a-z]+")
        .whitelisted_function("oscc_enable_[a-z]+")
        .whitelisted_function("oscc_disable_[a-z]+")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("oscc_test.rs"))
        .expect("Unable to generate bindings");
}

fn main() {
    gcc_builder("KIA_SOUL_EV");
    bindgen_builder("-DKIA_SOUL_EV=ON")
}