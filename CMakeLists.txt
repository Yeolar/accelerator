# Copyright (C) 2018, Yeolar

# Setup compiler
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)

# Define project
project(accelerator)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-scripts)

# Set project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 3)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS context regex)
find_package(GFlags REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Optional packages
find_package(GTest)

# Setup environment
set(CMAKE_BUILD_TYPE Release)   # Debug: -g; Release: -O3 -DNDEBUG
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -mpclmul")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    # memcheck
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Link libraries
link_libraries(
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    ${Boost_LIBRARIES}
    ${GFLAGS_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

# Check and setup configurations
include(CheckCXXSourceCompiles REQUIRED)

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
add_subdirectory(accelerator/concurrency)
add_subdirectory(accelerator/double-conversion)
add_subdirectory(accelerator/event)
add_subdirectory(accelerator/gen)
add_subdirectory(accelerator/io)
add_subdirectory(accelerator/scheduler)
add_subdirectory(accelerator/ssl)
add_subdirectory(accelerator/stats)
add_subdirectory(accelerator/thread)

add_library(accelerator_static STATIC
    $<TARGET_OBJECTS:double-conversion>
    $<TARGET_OBJECTS:accelerator_base>
    $<TARGET_OBJECTS:accelerator_compression>
    $<TARGET_OBJECTS:accelerator_concurrency>
    $<TARGET_OBJECTS:accelerator_event>
    $<TARGET_OBJECTS:accelerator_io>
    $<TARGET_OBJECTS:accelerator_scheduler>
    $<TARGET_OBJECTS:accelerator_ssl>
    $<TARGET_OBJECTS:accelerator_stats>
    $<TARGET_OBJECTS:accelerator_thread>
)
set_target_properties(accelerator_static PROPERTIES OUTPUT_NAME accelerator)

add_library(accelerator_shared SHARED
    $<TARGET_OBJECTS:double-conversion>
    $<TARGET_OBJECTS:accelerator_base>
    $<TARGET_OBJECTS:accelerator_compression>
    $<TARGET_OBJECTS:accelerator_concurrency>
    $<TARGET_OBJECTS:accelerator_event>
    $<TARGET_OBJECTS:accelerator_io>
    $<TARGET_OBJECTS:accelerator_scheduler>
    $<TARGET_OBJECTS:accelerator_ssl>
    $<TARGET_OBJECTS:accelerator_stats>
    $<TARGET_OBJECTS:accelerator_thread>
)
set_target_properties(accelerator_shared PROPERTIES OUTPUT_NAME accelerator)

install(TARGETS accelerator_static
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE
                GROUP_READ
                WORLD_READ
)
install(TARGETS accelerator_shared
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

# Binary

# Test
if(GTEST_FOUND)
    enable_testing()
    add_subdirectory(accelerator/test)
    add_subdirectory(accelerator/compression/test)
    add_subdirectory(accelerator/concurrency/test)
    add_subdirectory(accelerator/gen/test)
    add_subdirectory(accelerator/io/test)
    add_subdirectory(accelerator/scheduler/test)
    add_subdirectory(accelerator/ssl/test)
    add_subdirectory(accelerator/thread/test)
endif()

# Pack
set(CPACK_SYSTEM_NAME amd64)
find_program(DPKG_CMD dpkg)
if(DPKG_CMD)
    execute_process(
        COMMAND "${DPKG_CMD}" --print-architecture
        OUTPUT_VARIABLE CPACK_SYSTEM_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB;TXZ")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "rdd@rddoc.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "foundation framework of rddoc.")
set(CPACK_SYSTEM_NAME "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev,libgflags-dev,libssl-dev,zlib1g-dev")
include(CPack)

