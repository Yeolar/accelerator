# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_THREAD_SRCS *.cpp)
file(GLOB ACCELERATOR_THREAD_HDRS *.h)

add_library(accelerator_thread OBJECT ${ACCELERATOR_THREAD_SRCS})

install(FILES ${ACCELERATOR_THREAD_HDRS} DESTINATION include/accelerator/thread)
