# Copyright (C) 2017, Yeolar

set(ACCELERATOR_THREAD_TEST_SRCS
    ThreadUtilTest.cpp
)

foreach(test_src ${ACCELERATOR_THREAD_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test accelerator_thread_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} accelerator_static)
    add_test(${test} ${test} CONFIGURATIONS Debug)
endforeach()

set(ACCELERATOR_THREAD_BENCHMARK_SRCS
    AtomicPtrBenchmark.cpp
)

foreach(bench_src ${ACCELERATOR_THREAD_BENCHMARK_SRCS})
    get_filename_component(bench_name ${bench_src} NAME_WE)
    set(bench accelerator_thread_${bench_name})
    add_executable(${bench} ${bench_src})
    target_link_libraries(${bench} accelerator_static)
endforeach()

