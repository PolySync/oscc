#=============================================================================#
# load_generator_settings
# [PRIVATE/INTERNAL]
#
# load_generator_settings(TARGET_NAME PREFIX [SUFFIX_1 SUFFIX_2 .. SUFFIX_N])
#
#         TARGET_NAME - The base name of the user settings
#         PREFIX      - The prefix name used for generator settings
#         SUFFIX_XX   - List of suffixes to load
#
#  Loads a list of user settings into the generators scope. User settings have
#  the following syntax:
#
#      ${BASE_NAME}${SUFFIX}
#
#  The BASE_NAME is the target name and the suffix is a specific generator settings.
#
#  For every user setting found a generator setting is created of the follwoing fromat:
#
#      ${PREFIX}${SUFFIX}
#
#  The purpose of loading the settings into the generator is to not modify user settings
#  and to have a generic naming of the settings within the generator.
#
#=============================================================================#
function(LOAD_GENERATOR_SETTINGS TARGET_NAME PREFIX)
    foreach (GEN_SUFFIX ${ARGN})
        if (${TARGET_NAME}_${GEN_SUFFIX} AND NOT ${PREFIX}_${GEN_SUFFIX})
            set(${PREFIX}_${GEN_SUFFIX} ${${TARGET_NAME}_${GEN_SUFFIX}} PARENT_SCOPE)
        endif ()
    endforeach ()
endfunction()
