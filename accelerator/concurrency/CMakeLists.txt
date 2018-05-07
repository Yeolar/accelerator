# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_CONCURRENCY_SRCS *.cpp)
file(GLOB ACCELERATOR_CONCURRENCY_HDRS *.h)

add_library(accelerator_concurrency OBJECT ${ACCELERATOR_CONCURRENCY_SRCS})

install(FILES ${ACCELERATOR_CONCURRENCY_HDRS} DESTINATION include/accelerator/concurrency)
