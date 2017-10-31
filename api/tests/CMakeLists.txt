cmake_minimum_required(VERSION 2.8)

project(api-tests)

if(KIA_SOUL)
add_custom_target(
    run-api-property-tests
    COMMAND
    cargo +1.17.0 test --features "kia-soul" --no-fail-fast -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
elseif(KIA_SOUL_EV)
add_custom_target(
    run-api-property-tests
    COMMAND
    cargo +1.17.0 test --features "kia-soul-ev" --no-fail-fast -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
endif()