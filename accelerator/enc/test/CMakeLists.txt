# Copyright 2017 Yeolar

set(ACCELERATOR_ENC_TEST_SRCS
    CharPropertyTest.cpp
)

foreach(test_src ${ACCELERATOR_ENC_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test accelerator_enc_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} accelerator_static)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()

if(NOT PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    execute_process(
        COMMAND ln -fs "${PROJECT_SOURCE_DIR}/accelerator/enc/test/char.pecker.def"
                       "${PROJECT_BINARY_DIR}/accelerator/enc/test/char.pecker.def"
    )
endif()
