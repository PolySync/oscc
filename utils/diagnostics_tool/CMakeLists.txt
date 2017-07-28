cmake_minimum_required(VERSION 2.8)

project(diagnostics-tool)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(
    diagnostics-tool
    src/brake_module_state.c
    src/can_monitor.c
    src/diagnostics.c
    src/steering_module_state.c
    src/system_state.c
    src/terminal_print.c
    src/throttle_module_state.c)

target_include_directories(
    diagnostics-tool
    PRIVATE
    include
    ../../api/include
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(
    diagnostics-tool
    PRIVATE
    ${SDL2_LIBRARIES}
    canlib)
