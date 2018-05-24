#=============================================================================#
# parse_generator_arguments
# [PRIVATE/INTERNAL]
#
# parse_generator_arguments(TARGET_NAME PREFIX OPTIONS ARGS MULTI_ARGS [ARG1 ARG2 .. ARGN])
#
#         PREFIX     - Parsed options prefix
#         OPTIONS    - List of options
#         ARGS       - List of one value keyword arguments
#         MULTI_ARGS - List of multi value keyword arguments
#         [ARG1 ARG2 .. ARGN] - command arguments [optional]
#
# Parses generator options from either variables or command arguments
#
#=============================================================================#
macro(PARSE_GENERATOR_ARGUMENTS TARGET_NAME PREFIX OPTIONS ARGS MULTI_ARGS)
    cmake_parse_arguments(${PREFIX} "${OPTIONS}" "${ARGS}" "${MULTI_ARGS}" ${ARGN})
    error_for_unparsed(${PREFIX})
    load_generator_settings(${TARGET_NAME} ${PREFIX} ${OPTIONS} ${ARGS} ${MULTI_ARGS})
endmacro()

#=============================================================================#
# get_mcu
# [PRIVATE/INTERNAL]
#
# get_mcu(FULL_MCU_NAME, OUTPUT_VAR)
#
#         FULL_MCU_NAME - Board's full mcu name, including a trailing 'p' if present
#         OUTPUT_VAR - String value in which a regex match will be stored
#
# Matches the board's mcu without leading or trailing characters that would rather mess
# further processing that requires the board's mcu.
#
#=============================================================================#
macro(GET_MCU FULL_MCU_NAME OUTPUT_VAR)
    string(REGEX MATCH "^.+[^p]" ${OUTPUT_VAR} "FULL_MCU_NAME" PARENT_SCOPE)
endmacro()

#=============================================================================#
# increment_example_category_index
# [PRIVATE/INTERNAL]
#
# increment_example_category_index(OUTPUT_VAR)
#
#         OUTPUT_VAR - A number representing an example's category prefix
#
# Increments the given number by one, taking into consideration the number notation
# which is defined (Some SDK's or OSs use a leading '0' in single-digit numbers.
#
#=============================================================================#
macro(INCREMENT_EXAMPLE_CATEGORY_INDEX OUTPUT_VAR)
    math(EXPR INC_INDEX "${${OUTPUT_VAR}}+1")
    if (EXAMPLE_CATEGORY_INDEX_LENGTH GREATER 1 AND INC_INDEX LESS 10)
        set(${OUTPUT_VAR} "0${INC_INDEX}")
    else ()
        set(${OUTPUT_VAR} ${INC_INDEX})
    endif ()
endmacro()
