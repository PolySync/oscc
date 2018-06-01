#=============================================================================#
# make_arduino_library
# [PRIVATE/INTERNAL]
#
# make_arduino_library(VAR_NAME BOARD_ID LIB_PATH COMPILE_FLAGS LINK_FLAGS)
#
#        VAR_NAME    - Vairable wich will hold the generated library names
#        BOARD_ID    - Board ID
#        LIB_PATH    - Path of the library
#        COMPILE_FLAGS - Compile flags
#        LINK_FLAGS    - Link flags
#
# Creates an Arduino library, with all it's library dependencies.
#
#      ${LIB_NAME}_RECURSE controls if the library will recurse
#      when looking for source files.
#
#=============================================================================#
function(make_arduino_library VAR_NAME BOARD_ID LIB_PATH COMPILE_FLAGS LINK_FLAGS)

    string(REGEX REPLACE "/src/?$" "" LIB_PATH_STRIPPED ${LIB_PATH})
    get_filename_component(LIB_NAME ${LIB_PATH_STRIPPED} NAME)
    set(TARGET_LIB_NAME ${BOARD_ID}_${LIB_NAME})

    if (NOT TARGET ${TARGET_LIB_NAME})
        string(REGEX REPLACE ".*/" "" LIB_SHORT_NAME ${LIB_NAME})

        # Detect if recursion is needed
        if (NOT DEFINED ${LIB_SHORT_NAME}_RECURSE)
            set(${LIB_SHORT_NAME}_RECURSE ${ARDUINO_CMAKE_RECURSION_DEFAULT})
        endif ()

        find_sources(LIB_SRCS ${LIB_PATH} ${${LIB_SHORT_NAME}_RECURSE})
        if (LIB_SRCS)

            arduino_debug_msg("Generating Arduino ${LIB_NAME} library")
            add_library(${TARGET_LIB_NAME} STATIC ${LIB_SRCS})

            set_board_flags(ARDUINO_COMPILE_FLAGS ARDUINO_LINK_FLAGS ${BOARD_ID} FALSE)

            find_arduino_libraries(LIB_DEPS "${LIB_SRCS}" "")

            foreach (LIB_DEP ${LIB_DEPS})
                make_arduino_library(DEP_LIB_SRCS ${BOARD_ID} ${LIB_DEP}
                        "${COMPILE_FLAGS}" "${LINK_FLAGS}")
                list(APPEND LIB_TARGETS ${DEP_LIB_SRCS})
                list(APPEND LIB_INCLUDES ${DEP_LIB_SRCS_INCLUDES})
            endforeach ()

            if (LIB_INCLUDES)
                string(REPLACE ";" " " LIB_INCLUDES_SPACE_SEPARATED "${LIB_INCLUDES}")
            endif ()

            set_target_properties(${TARGET_LIB_NAME} PROPERTIES
                    COMPILE_FLAGS "${ARDUINO_COMPILE_FLAGS} ${LIB_INCLUDES_SPACE_SEPARATED} -I\"${LIB_PATH}\" -I\"${LIB_PATH}/utility\" ${COMPILE_FLAGS}"
                    LINK_FLAGS "${ARDUINO_LINK_FLAGS} ${LINK_FLAGS}")
            list(APPEND LIB_INCLUDES "-I\"${LIB_PATH}\";-I\"${LIB_PATH}/utility\"")

            if (LIB_TARGETS)
                list(REMOVE_ITEM LIB_TARGETS ${TARGET_LIB_NAME})
            endif ()

            target_link_libraries(${TARGET_LIB_NAME} ${BOARD_ID}_CORE ${LIB_TARGETS})
            list(APPEND LIB_TARGETS ${TARGET_LIB_NAME})

        endif ()

    else ()
        # Target already exists, skiping creating
        list(APPEND LIB_TARGETS ${TARGET_LIB_NAME})
        list(APPEND LIB_INCLUDES "-I\"${LIB_PATH}\"")
    endif ()

    if (LIB_TARGETS)
        list(REMOVE_DUPLICATES LIB_TARGETS)
    endif ()
    if (LIB_INCLUDES)
        list(REMOVE_DUPLICATES LIB_INCLUDES)
    endif ()

    set(${VAR_NAME} ${LIB_TARGETS} PARENT_SCOPE)
    set(${VAR_NAME}_INCLUDES ${LIB_INCLUDES} PARENT_SCOPE)

endfunction()

#=============================================================================#
# make_arduino_libraries
# [PRIVATE/INTERNAL]
#
# make_arduino_libraries(VAR_NAME BOARD_ID SRCS COMPILE_FLAGS LINK_FLAGS)
#
#        VAR_NAME    - Vairable wich will hold the generated library names
#        BOARD_ID    - Board ID
#        SRCS        - source files
#        COMPILE_FLAGS - Compile flags
#        LINK_FLAGS    - Linker flags
#
# Finds and creates all dependency libraries based on sources.
#
#=============================================================================#
function(make_arduino_libraries VAR_NAME BOARD_ID SRCS ARDLIBS COMPILE_FLAGS LINK_FLAGS)
    foreach (TARGET_LIB ${ARDLIBS})
        # Create static library instead of returning sources
        make_arduino_library(LIB_DEPS ${BOARD_ID} ${TARGET_LIB}
                "${COMPILE_FLAGS}" "${LINK_FLAGS}")
        list(APPEND LIB_TARGETS ${LIB_DEPS})
        list(APPEND LIB_INCLUDES ${LIB_DEPS_INCLUDES})
    endforeach ()

    set(${VAR_NAME} ${LIB_TARGETS} PARENT_SCOPE)
    set(${VAR_NAME}_INCLUDES ${LIB_INCLUDES} PARENT_SCOPE)
endfunction()
