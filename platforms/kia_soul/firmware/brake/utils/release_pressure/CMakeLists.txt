project(kia-soul-brake-utils-release-pressure)

generate_arduino_firmware(
    kia-soul-brake-utils-release-pressure
    SKETCH src/release_pressure.ino)

target_compile_options(
    kia-soul-brake-utils-release-pressure
    PRIVATE
    "-std=gnu++11"
    "-Os")
