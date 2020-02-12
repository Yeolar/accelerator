# Copyright 2017-present Yeolar

set(ACCELERATOR_BASE_TEST_SRCS
    Base64Test.cpp
    FixedStreamTest.cpp
    LoggingTest.cpp
    PathTest.cpp
    ReflectObjectTest.cpp
    SingletonTest.cpp
)

foreach(test_src ${ACCELERATOR_BASE_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test accelerator_base_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} accelerator_static)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()

set(ACCELERATOR_BASE_BENCHMARK_SRCS
    TimeBenchmark.cpp
)

foreach(bench_src ${ACCELERATOR_BASE_BENCHMARK_SRCS})
    get_filename_component(bench_name ${bench_src} NAME_WE)
    set(bench accelerator_base_${bench_name})
    add_executable(${bench} ${bench_src})
    target_link_libraries(${bench} accelerator_static)
endforeach()

