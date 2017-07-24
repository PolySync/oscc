cmake_minimum_required(VERSION 2.8)

project(joystick-commander)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

string(COMPARE EQUAL "${OSCC_INTF}" linuxcan result)

if( result )
  set(OSCC_INTF_SRC "src/oscc_linuxcan.c")
  set(OSCC_INTF_LIB "-lcanlib")
else()
  set(OSCC_INTF_SRC "src/oscc_socketcan.c")
endif()

add_executable(
    joystick-commander
    src/commander.c
    src/joystick.c
    src/main.c
    ${OSCC_INTF_SRC})

target_include_directories(
    joystick-commander
    PRIVATE
    include
    ../../platforms/common/include
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(
    joystick-commander
    PRIVATE
    ${SDL2_LIBRARIES}
    ${OSCC_INTF_LIB})

