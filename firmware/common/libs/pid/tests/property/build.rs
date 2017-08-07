extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .flag("-w")
        .file("../../oscc_pid.cpp")
        .cpp(true)
        .compile("libpid_test.a");

    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::Builder::default()
        .header("include/wrapper.hpp")
        .clang_arg("-I../../")
        .whitelisted_function("pid_zeroize")
        .whitelisted_function("pid_update")
        .generate()
        .unwrap()
        .write_to_file(Path::new(&out_dir).join("pid.rs"))
        .expect("Unable to generate bindings");
}
