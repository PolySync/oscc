project(pid-library-tests)

add_custom_target(
    run-pid-library-property-tests
    COMMAND
    cargo test -- --test-threads=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)
