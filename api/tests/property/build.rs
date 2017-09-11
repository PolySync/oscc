extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Build::new()
        .flag("-w")
        .define("KIA_SOUL", Some("ON"))
        .include("include")
        .include("../../include")
        .file("../../src/oscc.c")
        .compile("liboscc_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::Builder::default()
        .header("include/wrapper.hpp")
        .rust_target(bindgen::RustTarget::Stable_1_0)
        .generate_comments(false)
        .layout_tests(false)
        .clang_arg("-DKIA_SOUL=ON")
        .clang_arg("-I../../include")
        .whitelisted_type("oscc_steering_command_s")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("oscc_test.rs"))
        .expect("Unable to generate bindings");
}
