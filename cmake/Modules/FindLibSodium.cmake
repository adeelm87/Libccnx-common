########################################
#
# Find the LibSodium libraries and includes
# This module sets:
#  LIBSODIUM_FOUND: True if LibSodium was found
#  LIBSODIUM_LIBRARY:  The LibSodium library
#  LIBSODIUM_LIBRARIES:  The LibSodium library and dependencies
#  LIBSODIUM_INCLUDE_DIR:  The LibSodium include dir
#
# This module will look for the libraries in various locations
# See the LIBSODIUM_SEARCH_PATH_LIST for a full list.
#
# The caller can hint at locations using the following variables:
#
# LIBSODIUM_HOME (passed as -D to cmake)
# CCNX_DEPENDENCIES (in environment)
# LIBSODIUM_HOME (in environment)
# CCNX_HOME (in environment)
#

set(LIBSODIUM_SEARCH_PATH_LIST
  ${LIBSODIUM_HOME}
  $ENV{CCNX_DEPENDENCIES}
  $ENV{LIBSODIUM_HOME}
  $ENV{CCNX_HOME}
  /usr/local/ccnx/dependencies/build
  /usr/local/ccnx/dependencies
  /usr/local/ccnx
  /usr/local/ccn
  /usr/local
  /opt
  /usr
  )

find_path(LIBSODIUM_INCLUDE_DIR sodium.h
  HINTS ${LIBSODIUM_SEARCH_PATH_LIST}
  PATH_SUFFIXES include
  DOC "Find the LibSodium includes" )

find_library(LIBSODIUM_LIBRARY NAMES sodium
  HINTS ${LIBSODIUM_SEARCH_PATH_LIST}
  PATH_SUFFIXES lib
  DOC "Find the LibSodium libraries" )

set(LIBSODIUM_LIBRARIES ${LIBSODIUM_LIBRARY})
set(LIBSODIUM_INCLUDE_DIRS ${LIBSODIUM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibSodium  DEFAULT_MSG LIBSODIUM_LIBRARY LIBSODIUM_INCLUDE_DIR)
