#=============================================================================#
# make_arduino_example
# [PRIVATE/INTERNAL]
#
# make_arduino_example(TARGET_NAME EXAMPLE_NAME OUTPUT_VAR [CATEGORY_NAME])
#
#      TARGET_NAME  - Target name
#      EXAMPLE_NAME - Example name
#      OUTPUT_VAR   - Variable name to save sketch path.
#      [CATEGORY_NAME] - Optional name of the example's parent category, such as 'Basics' is for 'Blink'.
#
# Creates an Arduino example from the built-in categories.
#=============================================================================#
function(make_arduino_example TARGET_NAME EXAMPLE_NAME OUTPUT_VAR)

    set(OPTIONAL_ARGUMENTS ${ARGN})
    list(LENGTH OPTIONAL_ARGUMENTS ARGC)
    if (${ARGC} GREATER 0)
        list(GET OPTIONAL_ARGUMENTS 0 CATEGORY_NAME)
    endif ()

    # Case-insensitive support
    string(TOLOWER ${EXAMPLE_NAME} EXAMPLE_NAME)

    if (CATEGORY_NAME)
        string(TOLOWER ${CATEGORY_NAME} LOWER_CATEGORY_NAME)
        list(FIND ARDUINO_EXAMPLE_CATEGORIES ${LOWER_CATEGORY_NAME} CATEGORY_INDEX)
        if (${CATEGORY_INDEX} LESS 0)
            message(SEND_ERROR "${CATEGORY_NAME} example category doesn't exist, please check your spelling")
            return()
        endif ()
        INCREMENT_EXAMPLE_CATEGORY_INDEX(CATEGORY_INDEX)
        set(CATEGORY_NAME ${CATEGORY_INDEX}.${CATEGORY_NAME})
        file(GLOB EXAMPLES RELATIVE ${ARDUINO_EXAMPLES_PATH}/${CATEGORY_NAME}
                ${ARDUINO_EXAMPLES_PATH}/${CATEGORY_NAME}/*)
        foreach (EXAMPLE_PATH ${EXAMPLES})
            string(TOLOWER ${EXAMPLE_PATH} LOWER_EXAMPLE_PATH)
            if (${LOWER_EXAMPLE_PATH} STREQUAL ${EXAMPLE_NAME})
                set(EXAMPLE_SKETCH_PATH
                        "${ARDUINO_EXAMPLES_PATH}/${CATEGORY_NAME}/${EXAMPLE_PATH}")
                break()
            endif ()
        endforeach ()

    else ()

        file(GLOB CATEGORIES RELATIVE ${ARDUINO_EXAMPLES_PATH} ${ARDUINO_EXAMPLES_PATH}/*)
        foreach (CATEGORY_PATH ${CATEGORIES})
            file(GLOB EXAMPLES RELATIVE ${ARDUINO_EXAMPLES_PATH}/${CATEGORY_PATH}
                    ${ARDUINO_EXAMPLES_PATH}/${CATEGORY_PATH}/*)
            foreach (EXAMPLE_PATH ${EXAMPLES})
                string(TOLOWER ${EXAMPLE_PATH} LOWER_EXAMPLE_PATH)
                if (${LOWER_EXAMPLE_PATH} STREQUAL ${EXAMPLE_NAME})
                    set(EXAMPLE_SKETCH_PATH
                            "${ARDUINO_EXAMPLES_PATH}/${CATEGORY_PATH}/${EXAMPLE_PATH}")
                    break()
                endif ()
            endforeach ()
        endforeach ()

    endif ()

    if (EXAMPLE_SKETCH_PATH)
        make_arduino_sketch(${TARGET_NAME} ${EXAMPLE_SKETCH_PATH} SKETCH_CPP)
        set("${OUTPUT_VAR}" ${${OUTPUT_VAR}} ${SKETCH_CPP} PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Could not find example ${EXAMPLE_NAME}")
    endif ()

endfunction()
