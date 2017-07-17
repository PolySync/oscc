project(kia-soul)

set(CMAKE_CFLAGS "-std=gnu11 -Os")
set(CMAKE_CXX_FLAGS "-std=gnu++11 -Os")

add_subdirectory(brake)
add_subdirectory(can_gateway)
add_subdirectory(steering)
add_subdirectory(throttle)

add_custom_target(
    kia-soul-all-upload
    DEPENDS
    kia-soul-brake-upload
    kia-soul-can-gateway-upload
    kia-soul-steering-upload
    kia-soul-throttle-upload)
