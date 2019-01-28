project(brake-utils-release-pressure)

generate_arduino_firmware(
    brake-utils-release-pressure
    SKETCH src/release_pressure.ino
    BOARD_CPU atmega2560
    BOARD mega)

target_compile_options(
    brake-utils-release-pressure
    PRIVATE
    "-std=gnu++11"
    "-Os")
