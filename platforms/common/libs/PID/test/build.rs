extern crate gcc;
extern crate bindgen;

use std::env;
use std::path::Path;

fn main() {
    gcc::Config::new()
        .flag("-w")
        .file("../PID.c")
        .compile("libpid_test.a");
    
    let out_dir = env::var("OUT_DIR").unwrap();

    let _ = bindgen::builder()
        .header("../PID.h")
        .generate().unwrap()
        .write_to_file(Path::new(&out_dir).join("PID.rs"))
        .expect("Unable to generate bindings");
}