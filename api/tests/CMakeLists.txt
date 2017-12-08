cmake_minimum_required(VERSION 2.8)

project(api-tests)

if(KIA_SOUL)
add_custom_target(
    run-api-property-tests
    COMMAND
    ../initialize_vcan.sh && rustup update && rustup default stable && cargo test --features "kia-soul" --no-fail-fast -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
elseif(KIA_SOUL_EV)
add_custom_target(
    run-api-property-tests
    COMMAND
    ../initialize_vcan.sh && rustup update && rustup default stable && cargo test --features "kia-soul-ev" --no-fail-fast -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)
endif()