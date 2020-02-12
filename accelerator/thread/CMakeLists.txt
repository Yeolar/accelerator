# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_THREAD_SRCS *.cpp)
file(GLOB ACCELERATOR_THREAD_HDRS *.h)

file(GLOB ACCELERATOR_THREAD_DETAIL_SRCS detail/*.cpp)
file(GLOB ACCELERATOR_THREAD_DETAIL_HDRS detail/*.h)

add_library(accelerator_thread OBJECT
  ${ACCELERATOR_THREAD_SRCS}
  ${ACCELERATOR_THREAD_DETAIL_SRCS}
)

install(FILES ${ACCELERATOR_THREAD_HDRS} DESTINATION include/accelerator/thread)
install(FILES ${ACCELERATOR_THREAD_DETAIL_HDRS} DESTINATION include/accelerator/thread/detail)
