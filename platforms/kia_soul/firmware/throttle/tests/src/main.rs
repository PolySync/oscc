#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// extern crate core;

include!(concat!(env!("OUT_DIR"), "/communications.rs"));

fn main() {
    unsafe{ say_hello() };
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn print_thing() {
        // println!("erg source: {}", _POSIX_C_SOURCE);
        // say_hello();
    }
}