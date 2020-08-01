# Copyright (C) 2018-present, Yeolar

cmake_minimum_required(VERSION 3.2)

# Define project
project(accelerator VERSION 2.0.0)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-scripts)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(Boost_USE_MULTITHREADED TRUE)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS random regex thread)
find_package(DoubleConversion REQUIRED)
find_package(GFlags REQUIRED)

# Optional packages
find_package(GTest)
find_package(BZip2)
find_package(LZ4)
find_package(LibLZMA)
find_package(Snappy)
find_package(ZLIB)
find_package(Zstd)

set(ACC_HAVE_LIBGTEST ${GTEST_FOUND})
set(ACC_HAVE_LIBBZ2 ${BZIP2_FOUND})
set(ACC_HAVE_LIBLZ4 ${LZ4_FOUND})
set(ACC_HAVE_LIBLZMA ${LIBLZMA_FOUND})
set(ACC_HAVE_LIBSNAPPY ${SNAPPY_FOUND})
set(ACC_HAVE_LIBZ ${ZLIB_FOUND})
set(ACC_HAVE_LIBZSTD ${ZSTD_FOUND})

# Setup environment
option(BUILD_SHARED_LIBS "Build libraries as shared ones" OFF)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    # memcheck
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(ACC_USE_WITH_FOLLY ON)

# Link libraries
link_libraries(
  ${Boost_LIBRARIES}
  ${DOUBLE_CONVERSION_LIBRARY}
  ${GFLAGS_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
)
if(BZIP2_FOUND)
  link_libraries(${BZIP2_LIBRARIES})
endif()
if(LZ4_FOUND)
  link_libraries(${LZ4_LIBRARY})
endif()
if(LIBLZMA_FOUND)
  link_libraries(${LIBLZMA_LIBRARIES})
endif()
if(SNAPPY_FOUND)
  link_libraries(${SNAPPY_LIBRARIES})
endif()
if(ZLIB_FOUND)
  link_libraries(${ZLIB_LIBRARIES})
endif()
if(ZSTD_FOUND)
  link_libraries(${ZSTD_LIBRARIES})
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
)

# Check and setup configurations
include(CheckCXXSourceCompiles REQUIRED)
include(CheckFunctionExists REQUIRED)

check_cxx_source_compiles("
  extern \"C\"
  void configure_link_extern_weak_test() __attribute__((__weak__));
  int main() {
    return configure_link_extern_weak_test == nullptr;
  }"
  ACC_HAVE_WEAK_SYMBOLS)

check_cxx_source_compiles("
  #include <string.h>
  #include <errno.h>
  int main() {
    char buf[1024];
    buf[0] = 0;
    int ret = strerror_r(ENOMEM, buf, sizeof(buf));
    return ret;
  }"
  ACC_HAVE_XSI_STRERROR_R)

check_function_exists(memrchr ACC_HAVE_MEMRCHR)
check_function_exists(pipe2 ACC_HAVE_PIPE2)
check_function_exists(preadv ACC_HAVE_PREADV)
check_function_exists(pwritev ACC_HAVE_PWRITEV)

if(GFLAGS_FOUND)
  set(CMAKE_REQUIRED_LIBRARIES "gflags")

  check_cxx_source_compiles("
    #include <gflags/gflags.h>
    void foo() { google::GetArgv(); }
    int main() { return 0; }"
    ACC_UNUSUAL_GFLAGS_NAMESPACE)

  if(ACC_UNUSUAL_GFLAGS_NAMESPACE)
    set(ACC_GFLAGS_NAMESPACE "google")
  else()
    set(ACC_GFLAGS_NAMESPACE "gflags")
  endif()
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/accelerator/accelerator-config.h.in
  ${PROJECT_SOURCE_DIR}/accelerator/accelerator-config.h
)

# Work on all sub directories
add_subdirectory(accelerator)
add_subdirectory(accelerator/compression)
add_subdirectory(accelerator/io)
add_subdirectory(accelerator/thread)

add_library(accelerator
  $<TARGET_OBJECTS:accelerator_base>
  $<TARGET_OBJECTS:accelerator_compression>
  $<TARGET_OBJECTS:accelerator_io>
  $<TARGET_OBJECTS:accelerator_thread>
)

install(TARGETS accelerator
  EXPORT accelerator-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Binary

# Test
if(GTEST_FOUND)
  enable_testing()
  add_subdirectory(accelerator/test)
  add_subdirectory(accelerator/io/test)
  add_subdirectory(accelerator/thread/test)
endif()

# CmakeConfig
include(CMakePackageConfigHelpers)

install(
  EXPORT accelerator-targets
  NAMESPACE accelerator::
  FILE accelerator-targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/accelerator
)

configure_package_config_file(
  ${CMAKE_MODULE_PATH}/accelerator-config.cmake.in
  accelerator-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/accelerator
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/accelerator-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/accelerator
)

# Pack
set(CPACK_SYSTEM_NAME "amd64")
find_program(DPKG_CMD dpkg)
if(DPKG_CMD)
  execute_process(
    COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE CPACK_SYSTEM_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(InstallRequiredSystemLibraries)
if(DPKG_CMD)
  set(CPACK_GENERATOR "DEB;TXZ")
else()
  set(CPACK_GENERATOR "TXZ")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "yeolar@qq.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A C++ base library forked from facebook folly.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev,libdouble-conversion-dev,libgflags-dev")
include(CPack)

