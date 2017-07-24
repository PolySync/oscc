cmake_minimum_required( VERSION 2.8 )

project( joystick-commander )

string(COMPARE EQUAL "${OSCC_INTF}" linuxcan result)

if( result )
  set(OSCC_INTF_SRC "src/oscc_linuxcan.c")
  set(OSCC_INTF_LIB "-lcanlib")
else()
  set(OSCC_INTF_SRC "src/oscc_socketcan.c")
  set(OSCC_INTF_LIB "-lsocketcan")
endif()

include_directories(
	../../platforms/common/include include /usr/include/SDL2
)

add_executable( ${PROJECT_NAME}
    src/main.c src/commander.c src/joystick.c ${OSCC_INTF_SRC}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT")

target_link_libraries( ${PROJECT_NAME}
    ${LIBS} -lSDL2 ${OSCC_INTF_LIB} -lm
)

