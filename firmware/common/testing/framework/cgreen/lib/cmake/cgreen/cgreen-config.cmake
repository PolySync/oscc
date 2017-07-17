# - Config file for the cgreen package
# It defines the following variables
#  CGREEN_CMAKE_DIR - include directories for cgreen
#  CGREEN_INCLUDE_DIRS - include directories for cgreen
#  CGREEN_LIBRARIES    - libraries to link against
#  CGREEN_EXECUTABLE   - the cgreen executable

get_filename_component( CGREEN_CMAKE_DIRS "${CMAKE_CURRENT_LIST_FILE}" PATH )

#  leave this up to cmake
find_path(CGREEN_INCLUDE_DIRS NAMES cgreen/cgreen.h)

set( CGREEN_LIBRARIES cgreen )
set( CGREEN_EXECUTABLE cgreen-runner )

