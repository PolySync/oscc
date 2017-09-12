cmake_minimum_required(VERSION 2.8)

add_custom_target(
    run-api-property-tests
    COMMAND
    cargo test -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)