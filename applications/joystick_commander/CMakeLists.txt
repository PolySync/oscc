cmake_minimum_required(VERSION 2.8)

project(joystick-commander)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

if(KIA_SOUL)
    add_definitions(-DKIA_SOUL)
endif()

add_executable(
    joystick-commander
    src/commander.c
    src/joystick.c
    src/main.c
    ../../api/src/oscc.c)

target_include_directories(
    joystick-commander
    PRIVATE
    include
    ../../api/include
    ../../firmware/common/include
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(
    joystick-commander
    PRIVATE
    ${SDL2_LIBRARIES})

