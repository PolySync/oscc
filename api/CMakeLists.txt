cmake_minimum_required(VERSION 2.8)

project(osccapi)

include(${CMAKE_SOURCE_DIR}/OsccConfig.cmake)

set(INCLUDES ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
set(SOURCES ${CMAKE_SOURCE_DIR}/src/oscc.c)
set_source_files_properties(SOURCES PROPERTIES LANGUAGE C)

set(OBJECTS ${PROJECT_NAME}_objects)
set(SHARED_LIB ${PROJECT_NAME}_shared_lib)
set(STATIC_LIB ${PROJECT_NAME}_static_lib)

# Reuse object files for both shared and static libraries
# rather than recompiling for both
add_library(${OBJECTS} OBJECT ${SOURCES})
target_include_directories(${OBJECTS} PUBLIC ${INCLUDES})
set_target_properties(${OBJECTS} PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(${SHARED_LIB} SHARED $<TARGET_OBJECTS:${OBJECTS}>)
set_target_properties(${SHARED_LIB} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_include_directories(${SHARED_LIB} PUBLIC ${INCLUDES})

add_library(${STATIC_LIB} STATIC $<TARGET_OBJECTS:${OBJECTS}>)
target_include_directories(${STATIC_LIB} PUBLIC ${INCLUDES})
set_target_properties(${STATIC_LIB} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})