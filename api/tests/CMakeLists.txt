cmake_minimum_required(VERSION 2.8)

project(api-tests)

if(KIA_SOUL)
add_custom_target(
    run-api-property-tests
    COMMAND
    RUST_TEST_THREADS=1 $ENV{HOME}/.cargo/bin/rustup run 1.17.0 cargo test --features "kia-soul" ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
elseif(KIA_SOUL_EV)
add_custom_target(
    run-api-property-tests
    COMMAND
    RUST_TEST_THREADS=1 $ENV{HOME}/.cargo/bin/rustup run 1.17.0 cargo test --features "kia-soul-ev" ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
endif()