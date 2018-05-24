#=============================================================================#
# Detects the Arduino SDK Version based on the revisions.txt file.
# The following variables will be generated:
#
#    ${OUTPUT_VAR_NAME}         -> the full version (major.minor.patch)
#    ${OUTPUT_VAR_NAME}_MAJOR   -> the major version
#    ${OUTPUT_VAR_NAME}_MINOR   -> the minor version
#    ${OUTPUT_VAR_NAME}_PATCH   -> the patch version
#
#=============================================================================#
find_file(ARDUINO_VERSION_PATH
        NAMES lib/version.txt
        PATHS ${ARDUINO_SDK_PATH}
        DOC "Path to Arduino version file.")

find_file(ARDUINO_REVISION_PATH
        NAMES revisions.txt
        PATHS ${ARDUINO_SDK_PATH}
        DOC "Path to Arduino's revision-tracking file.")

if (NOT ARDUINO_VERSION_PATH)
    message(FATAL_ERROR "Couldn't find SDK's version file, aborting.")
endif ()

file(READ ${ARDUINO_VERSION_PATH} RAW_VERSION)
if ("${RAW_VERSION}" MATCHES " *[0]+([0-9]+)")
    set(PARSED_VERSION 0.${CMAKE_MATCH_1}.0)
elseif ("${RAW_VERSION}" MATCHES "[ ]*([0-9]+[.][0-9]+[.][0-9]+)")
    set(PARSED_VERSION ${CMAKE_MATCH_1})
elseif ("${RAW_VERSION}" MATCHES "[ ]*([0-9]+[.][0-9]+)")
    set(PARSED_VERSION ${CMAKE_MATCH_1}.0)
endif ()

if (NOT ARDUINO_REVISION_PATH)
    message(WARNING "Couldn't find SDK's revisions file, defaulting to 0.")
else ()
    file(READ ${ARDUINO_REVISION_PATH} RAW_REVISION 0 30)
    if ("${RAW_REVISION}" MATCHES ".*${PARSED_VERSION}.*[-].*")
        string(REGEX MATCH "[-][ ]?([0-9]+[.][0-9]+[.][0-9]+)"
                TMP_REV ${RAW_REVISION})
        set(PARSED_REVISION ${CMAKE_MATCH_1})
    else ()
        set(PARSED_REVISION 0)
    endif ()
endif ()

if (NOT PARSED_VERSION STREQUAL "")
    string(REPLACE "." ";" SPLIT_VERSION ${PARSED_VERSION})
    list(GET SPLIT_VERSION 0 SPLIT_VERSION_MAJOR)
    list(GET SPLIT_VERSION 1 SPLIT_VERSION_MINOR)
    list(GET SPLIT_VERSION 2 SPLIT_VERSION_PATCH)

    string(CONCAT FULL_SDK_VERSION "${PARSED_VERSION}" "-" "${PARSED_REVISION}")

    set(ARDUINO_SDK_VERSION "${PARSED_VERSION}" CACHE STRING "Arduino SDK Version")
    set(ARUDINO_SDK_FULL_VERSION "${FULL_SDK_VERSION}" CACHE STRING "Full Arduino SDK version")
    set(ARDUINO_SDK_VERSION_MAJOR ${SPLIT_VERSION_MAJOR} CACHE STRING "Arduino SDK Major Version")
    set(ARDUINO_SDK_VERSION_MINOR ${SPLIT_VERSION_MINOR} CACHE STRING "Arduino SDK Minor Version")
    set(ARDUINO_SDK_VERSION_PATCH ${SPLIT_VERSION_PATCH} CACHE STRING "Arduino SDK Patch Version")
    set(ARDUINO_SDK_VERSION_REVISION ${PARSED_REVISION} CACHE STRING "Arduino SDK Revision")
endif ()

if (ARDUINO_SDK_VERSION VERSION_LESS 0.19)
    message(FATAL_ERROR "Unsupported Arduino SDK (requires version 0.19 or higher)")
endif ()

message(STATUS "Arduino SDK version ${ARUDINO_SDK_FULL_VERSION}: ${ARDUINO_SDK_PATH}")
