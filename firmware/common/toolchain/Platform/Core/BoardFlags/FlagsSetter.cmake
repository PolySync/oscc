include(CompilerFlagsSetter)
include(LinkerFlagsSetter)

#=============================================================================#
# set_board_flags
# [PRIVATE/INTERNAL]
#
# set_board_flags(COMPILER_FLAGS LINKER_FLAGS BOARD_ID IS_MANUAL)
#
#       COMPILER_FLAGS - Variable holding compiler flags
#       LINKER_FLAGS - Variable holding linker flags
#       BOARD_ID - The board id name
#       IS_MANUAL - (Advanced) Only use AVR Libc/Includes
#
# Configures the build settings for the specified Arduino Board.
#
#=============================================================================#
function(set_board_flags COMPILER_FLAGS LINKER_FLAGS BOARD_ID IS_MANUAL)

    _get_board_property(${BOARD_ID} build.core BOARD_CORE)
    if (BOARD_CORE)
        _get_normalized_sdk_version(NORMALIZED_SDK_VERSION)

        set_board_compiler_flags(COMPILE_FLAGS ${NORMALIZED_SDK_VERSION} ${BOARD_ID} ${IS_MANUAL})
        set_board_linker_flags(LINK_FLAGS ${BOARD_ID} ${IS_MANUAL})

        # output
        set(${COMPILER_FLAGS} "${COMPILE_FLAGS}" PARENT_SCOPE)
        set(${LINKER_FLAGS} "${LINK_FLAGS}" PARENT_SCOPE)

    else ()
        message(FATAL_ERROR "Invalid Arduino board ID (${BOARD_ID}), aborting.")
    endif ()

endfunction()

#=============================================================================#
# _get_normalized_sdk_version
# [PRIVATE/INTERNAL]
#
# _get_normalized_sdk_version(OUTPUT_VAR)
#
#       OUTPUT_VAR - Returned variable storing the normalized version
#
# Normalizes SDK's version for a proper use of the '-DARDUINO' compile flag.
# Note that there are differences between normalized versions in specific SDK versions:
#       SDK Version 1.5.8 and above - Appends zeros to version parts.
#                                     e.g Version 1.6.5 will be normalized as 10605
#       SDK Versions between 1.0.0 and 1.5.8 - Joins all version parts together.
#                                              e.g Version 1.5.3 will be normalized as 153
#       SDK Version 1.0.0 and below - Uses only the 'Minor' version part.
#                                     e.g Version 0.20.0 will be normalized as 20
#
#=============================================================================#
function(_get_normalized_sdk_version OUTPUT_VAR)

    if (${ARDUINO_SDK_VERSION} VERSION_GREATER 1.5.8)
        # -DARDUINO format has changed since 1.6.0 by appending zeros when required,
        # e.g for 1.6.5 version -DARDUINO=10605
        _append_suffix_zero_to_version_if_required(${ARDUINO_SDK_VERSION_MAJOR} 10 MAJOR_VERSION)
        _append_suffix_zero_to_version_if_required(${ARDUINO_SDK_VERSION_MINOR} 10 MINOR_VERSION)
        set(NORMALIZED_VERSION
                "${MAJOR_VERSION}${MINOR_VERSION}${ARDUINO_SDK_VERSION_PATCH}")
    else ()
        # -DARDUINO format before 1.0.0 uses only minor version,
        # e.g. for 0020 version -DARDUINO=20
        if (${ARDUINO_SDK_VERSION} VERSION_LESS 1.0.0)
            set(NORMALIZED_VERSION "${ARDUINO_SDK_VERSION_MINOR}")
        else ()
            # -DARDUINO format after 1.0.0 combines all 3 version parts together,
            # e.g. for 1.5.3 version -DARDUINO=153
            set(NORMALIZED_VERSION
                    "${ARDUINO_SDK_VERSION_MAJOR}${ARDUINO_SDK_VERSION_MINOR}${ARDUINO_SDK_VERSION_PATCH}")
        endif ()
    endif ()

    set(${OUTPUT_VAR} ${NORMALIZED_VERSION} PARENT_SCOPE)

endfunction()

#=============================================================================#
# _append_suffix_zero_to_version_if_required
# [PRIVATE/INTERNAL]
#
# _append_suffix_zero_to_version_if_required(VERSION_PART VERSION_LIMIT OUTPUT_VAR)
#
#       VERSION_PART - Version to check and possibly append to.
#                 Must be a version part - Major, Minor or Patch.
#       VERSION_LIMIT - Append limit. For a version greater than this number
#                       a zero will NOT be appended.
#       OUTPUT_VAR - Returned variable storing the normalized version.
#
# Appends a suffic zero to the given version part if it's below than the given limit.
# Otherwise, the version part is returned as it is.
#
#=============================================================================#
macro(_append_suffix_zero_to_version_if_required VERSION_PART VERSION_LIMIT OUTPUT_VAR)
    if (${VERSION_PART} LESS ${VERSION_LIMIT})
        set(${OUTPUT_VAR} "${VERSION_PART}0")
    else ()
        set(${OUTPUT_VAR} "${VERSION_PART}")
    endif ()
endmacro()
