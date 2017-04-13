project(kia-soul)

set(CMAKE_CFLAGS "-std=gnu11 -Os")
set(CMAKE_CXX_FLAGS "-std=gnu++11 -Os")

add_subdirectory(brake)
add_subdirectory(can_gateway)
add_subdirectory(steering)
add_subdirectory(throttle)
