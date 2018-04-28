# Copyright 2017 Yeolar

set(ACCELERATOR_GEN_TEST_SRCS
    BaseTest.cpp
    CombineTest.cpp
    FileTest.cpp
    StringTest.cpp
)

foreach(test_src ${ACCELERATOR_GEN_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test accelerator_gen_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} accelerator_static)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()
