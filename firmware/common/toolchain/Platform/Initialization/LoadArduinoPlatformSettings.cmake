# SETTINGS_LIST - Variable name of settings list
# SETTINGS_PATH - File path of settings file to load.
#
# Loads an Arduino settings file into the cache.
#
# Examples of this type of settings file is the boards.txt and
# programmers.txt files located in ${ARDUINO_SDK}/hardware/arduino.
#
# Settings have to following format:
#
#      entry.setting[.subsetting] = value
#
# where [.subsetting] is optional
#
# For example, the following settings:
#
#      uno.name=Arduino Uno
#      uno.upload.protocol=stk500
#      uno.upload.maximum_size=32256
#      uno.build.mcu=atmega328p
#      uno.build.core=arduino
#
# will generate the follwoing equivalent CMake variables:
#
#      set(uno.name "Arduino Uno")
#      set(uno.upload.protocol     "stk500")
#      set(uno.upload.maximum_size "32256")
#      set(uno.build.mcu  "atmega328p")
#      set(uno.build.core "arduino")
#
#      set(uno.SETTINGS  name upload build)              # List of settings for uno
#      set(uno.upload.SUBSETTINGS protocol maximum_size) # List of sub-settings for uno.upload
#      set(uno.build.SUBSETTINGS mcu core)               # List of sub-settings for uno.build
#
# Note that Arduino 1.6 SDK or greater treats most of the settings differently for some boards,
# by grouping them into menus which differentiate on their cpu architecture.
#
# For example, the settings declared earlier for "regular" SDKs
# would be declared as such in 1.6:
#
#      mega.name=Arduino Uno
#      mega.menu.cpu.atmega2560.upload.protocol=wiring
#      mega.menu.cpu.atmega2560.upload.maximum_size=253952
#      mega.menu.cpu.atmega2560.build.mcu=atmega2560
#      mega.build.core=arduino
#
# The ${ENTRY_NAME}.SETTINGS variable lists all settings for the entry, while
# ${ENTRY_NAME}.SUBSETTINGS variables lists all settings for a sub-setting of
# a entry setting pair.
#
# These variables are generated in order to be able to  programatically traverse
# all settings (for a example see print_board_settings() function).
#
#=============================================================================#
if (NOT ${SETTINGS_LIST} AND EXISTS ${SETTINGS_PATH})
    file(STRINGS ${SETTINGS_PATH} FILE_ENTRIES)  # Settings file split into lines

    foreach (FILE_ENTRY ${FILE_ENTRIES})
        if ("${FILE_ENTRY}" MATCHES "^[^#]+=.*")
            string(REGEX MATCH "^[^=]+" SETTING_NAME ${FILE_ENTRY})
            string(REGEX MATCH "[^=]+$" SETTING_VALUE ${FILE_ENTRY})
            string(REPLACE "." ";" ENTRY_NAME_TOKENS ${SETTING_NAME})
            string(STRIP "${SETTING_VALUE}" SETTING_VALUE)

            list(LENGTH ENTRY_NAME_TOKENS ENTRY_NAME_TOKENS_LEN)

            # Add entry to settings list if it does not exist
            list(GET ENTRY_NAME_TOKENS 0 ENTRY_NAME)
            list(FIND ${SETTINGS_LIST} ${ENTRY_NAME} ENTRY_NAME_INDEX)
            if (ENTRY_NAME_INDEX LESS 0)
                # Add entry to main list
                list(APPEND ${SETTINGS_LIST} ${ENTRY_NAME})
            endif ()

            # Add entry setting to entry settings list if it does not exist
            set(ENTRY_SETTING_LIST ${ENTRY_NAME}.SETTINGS)

            # menu.cpu.architecture settings
            if (ENTRY_NAME_TOKENS_LEN GREATER 5)
                list(GET ENTRY_NAME_TOKENS 3 CPU_ARCH)
                list(GET ENTRY_NAME_TOKENS 4 ENTRY_SETTING)
                set(ENTRY_SETTING menu.cpu.${CPU_ARCH}.${ENTRY_SETTING})
            else ()
                list(GET ENTRY_NAME_TOKENS 1 ENTRY_SETTING)
            endif ()

            list(FIND ${ENTRY_SETTING_LIST} ${ENTRY_SETTING} ENTRY_SETTING_INDEX)
            if (ENTRY_SETTING_INDEX LESS 0)
                # Add setting to entry
                list(APPEND ${ENTRY_SETTING_LIST} ${ENTRY_SETTING})
                set(${ENTRY_SETTING_LIST} ${${ENTRY_SETTING_LIST}}
                        CACHE INTERNAL "Arduino ${ENTRY_NAME} Board settings list")
            endif ()

            set(FULL_SETTING_NAME ${ENTRY_NAME}.${ENTRY_SETTING})

            # Add entry sub-setting to entry sub-settings list if it does not exists
            if (ENTRY_NAME_TOKENS_LEN GREATER 2)

                set(ENTRY_SUBSETTING_LIST ${ENTRY_NAME}.${ENTRY_SETTING}.SUBSETTINGS)
                if (ENTRY_NAME_TOKENS_LEN GREATER 5)
                    list(GET ENTRY_NAME_TOKENS 5 ENTRY_SUBSETTING)
                elseif (ENTRY_NAME_TOKENS_LEN GREATER 3)
                    # Search for special cpu sub-settings
                    list(GET ENTRY_NAME_TOKENS 2 ENTRY_SUBSETTING)
                    string(TOLOWER ${ENTRY_SUBSETTING} ENTRY_SUBSETTING)
                    if ("${ENTRY_SUBSETTING}" STREQUAL "cpu")
                        # cpu setting found, determine architecture
                        list(GET ENTRY_NAME_TOKENS 3 ENTRY_SUBSETTING)
                        set(ENTRY_SUBSETTING_LIST ${ENTRY_NAME}.${ENTRY_SETTING}.CPUS)
                    endif ()
                else ()
                    list(GET ENTRY_NAME_TOKENS 2 ENTRY_SUBSETTING)
                endif ()
                list(FIND ${ENTRY_SUBSETTING_LIST} ${ENTRY_SUBSETTING} ENTRY_SUBSETTING_INDEX)
                if (ENTRY_SUBSETTING_INDEX LESS 0)
                    list(APPEND ${ENTRY_SUBSETTING_LIST} ${ENTRY_SUBSETTING})
                    set(${ENTRY_SUBSETTING_LIST} ${${ENTRY_SUBSETTING_LIST}}
                            CACHE INTERNAL "Arduino ${ENTRY_NAME} Board sub-settings list")
                endif ()
                set(FULL_SETTING_NAME ${FULL_SETTING_NAME}.${ENTRY_SUBSETTING})

            endif ()

            # Save setting value
            set(${FULL_SETTING_NAME} ${SETTING_VALUE}
                    CACHE INTERNAL "Arduino ${ENTRY_NAME} Board setting")

        endif ()
    endforeach ()
    set(${SETTINGS_LIST} ${${SETTINGS_LIST}}
            CACHE STRING "List of detected Arduino Board configurations")
    mark_as_advanced(${SETTINGS_LIST})
endif ()
