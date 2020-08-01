# Copyright 2017-present Yeolar

set(ACCELERATOR_THREAD_TEST_SRCS
  ThreadLocalTest.cpp
)

foreach(test_src ${ACCELERATOR_THREAD_TEST_SRCS})
  get_filename_component(test_name ${test_src} NAME_WE)
  set(test accelerator_thread_${test_name})
  add_executable(${test} ${test_src})
  target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} accelerator)
  add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()
