cmake_minimum_required(VERSION 2.8)

add_custom_target(
    run-api-property-tests
    COMMAND
    chmod +x ../initialize_vcan.sh && 
    ./../initialize_vcan.sh && 
    $ENV{HOME}/.cargo/bin/rustup override set 1.17.0 &&
    cargo test -- --test-threads=1 &&
    $ENV{HOME}/.cargo/bin/rustup override unset &&
    sudo ip link set vcan0 down &&
    sudo rmmod vcan
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)