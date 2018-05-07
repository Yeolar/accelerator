# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_IO_SRCS *.cpp)
file(GLOB ACCELERATOR_IO_HDRS *.h)

add_library(accelerator_io OBJECT ${ACCELERATOR_IO_SRCS})

install(FILES ${ACCELERATOR_IO_HDRS} DESTINATION include/accelerator/io)
