# Find accelerator
#
#  ACCELERATOR_INCLUDE_DIR - where to find accelerator/*.
#  ACCELERATOR_LIBRARY     - List of libraries when using accelerator.
#  ACCELERATOR_FOUND       - True if accelerator found.

find_package(PkgConfig)
pkg_check_modules(ACCELERATOR QUIET accelerator)

IF (ACCELERATOR_INCLUDE_DIR)
  # Already in cache, be silent
  SET(ACCELERATOR_FIND_QUIETLY TRUE)
ENDIF ()

FIND_PATH(ACCELERATOR_INCLUDE_DIR accelerator/accelerator-config.h)

FIND_LIBRARY(ACCELERATOR_LIBRARY accelerator)

# handle the QUIETLY and REQUIRED arguments and set ACCELERATOR_FOUND to TRUE 
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ACCELERATOR DEFAULT_MSG ACCELERATOR_LIBRARY ACCELERATOR_INCLUDE_DIR)

MARK_AS_ADVANCED(ACCELERATOR_LIBRARY ACCELERATOR_INCLUDE_DIR)
