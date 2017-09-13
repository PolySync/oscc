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
        .include("../../src")
        .file("../../src/oscc.c")
        .compile("liboscc_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::Builder::default()
        .header("include/wrapper.h")
        .rust_target(bindgen::RustTarget::Stable_1_0)
        .generate_comments(false)
        .layout_tests(false)
        .trust_clang_mangling(false)
        .clang_arg("-DKIA_SOUL=ON")
        .clang_arg("-I../../include")
        .clang_arg("-I../../src")
        .whitelisted_type("oscc_[a-z]+_[a-z]+_s")
        .whitelisted_var("OSCC_[A-Z]+_[A-Z]+_CAN_ID")
        .whitelisted_function("oscc_publish_[a-z]+_[a-z]+")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("oscc_test.rs"))
        .expect("Unable to generate bindings");
}
