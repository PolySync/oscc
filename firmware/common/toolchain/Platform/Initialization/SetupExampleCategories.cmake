# Setups all of Arduino's built-in examples categories, listing it by their names
# without the index prefix ('01.Basics' becomes 'Basics').
# This list is saved in a cached variable named 'ARDUINO_EXAMPLE_CATEGORIES'.

file(GLOB EXAMPLE_CATEGORIES RELATIVE ${ARDUINO_EXAMPLES_PATH} ${ARDUINO_EXAMPLES_PATH}/*)
list(SORT EXAMPLE_CATEGORIES)

foreach (CATEGORY ${EXAMPLE_CATEGORIES})
    if (NOT EXAMPLE_CATEGORY_INDEX_LENGTH)
        string(REGEX MATCH "^[0-9]+" CATEGORY_INDEX ${CATEGORY})
        string(LENGTH ${CATEGORY_INDEX} INDEX_LENGTH)
        set(EXAMPLE_CATEGORY_INDEX_LENGTH ${INDEX_LENGTH} CACHE INTERNAL
                "Number of digits preceeding an example's category path")
    endif ()
    string(REGEX MATCH "[^0-9.]+$" PARSED_CATEGORY ${CATEGORY})
    string(TOLOWER ${PARSED_CATEGORY} PARSED_CATEGORY)
    list(APPEND CATEGORIES "${PARSED_CATEGORY}")
endforeach ()

set(ARDUINO_EXAMPLE_CATEGORIES ${CATEGORIES} CACHE INTERNAL
        "List of categories containing the built-in Arduino examples")
