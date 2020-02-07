# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_BASE_SRCS *.cpp)
file(GLOB ACCELERATOR_BASE_HDRS *.h)

add_library(accelerator_base OBJECT ${ACCELERATOR_BASE_SRCS})

install(FILES ${ACCELERATOR_BASE_HDRS} DESTINATION include/accelerator)
