#=============================================================================#
# make_arduino_sketch
# [PRIVATE/INTERNAL]
#
# make_arduino_sketch(TARGET_NAME SKETCH_PATH OUTPUT_VAR)
#
#      TARGET_NAME - Target name
#      SKETCH_PATH - Path to sketch directory
#      OUTPUT_VAR  - Variable name where to save generated sketch source
#
# Generates C++ sources from Arduino Sketch.
#=============================================================================#
function(make_arduino_sketch TARGET_NAME SKETCH_PATH OUTPUT_VAR)
    get_filename_component(SKETCH_NAME "${SKETCH_PATH}" NAME)
    get_filename_component(SKETCH_PATH "${SKETCH_PATH}" ABSOLUTE)

    if (EXISTS "${SKETCH_PATH}")
        set(SKETCH_CPP ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_${SKETCH_NAME}.cpp)

        # Always set sketch path to the parent directory -
        # Sketch files will be found later
        string(REGEX REPLACE "[^\\/]+(.\\.((pde)|(ino)))" ""
                SKETCH_PATH ${SKETCH_PATH})

        # Find all sketch files
        file(GLOB SKETCH_SOURCES ${SKETCH_PATH}/*.pde ${SKETCH_PATH}/*.ino)
        list(LENGTH SKETCH_SOURCES NUMBER_OF_SOURCES)
        if (NUMBER_OF_SOURCES LESS 0) # Sketch sources not found
            message(FATAL_ERROR "Could not find sketch
            (${SKETCH_NAME}.pde or ${SKETCH_NAME}.ino) at ${SKETCH_PATH}!")
        endif ()
        list(SORT SKETCH_SOURCES)

        convert_sketch_to_cpp(${SKETCH_SOURCES} ${SKETCH_CPP})

        # Regenerate build system if sketch changes
        add_custom_command(OUTPUT ${SKETCH_CPP}
                COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS ${MAIN_SKETCH} ${SKETCH_SOURCES}
                COMMENT "Regnerating ${SKETCH_NAME} Sketch")
        set_source_files_properties(${SKETCH_CPP} PROPERTIES GENERATED TRUE)
        # Mark file that it exists for find_file
        set_source_files_properties(${SKETCH_CPP} PROPERTIES GENERATED_SKETCH TRUE)

        set(${OUTPUT_VAR} ${${OUTPUT_VAR}} ${SKETCH_CPP} PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Sketch does not exist: ${SKETCH_PATH}")
    endif ()
endfunction()
