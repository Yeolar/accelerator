# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_COMPRESSION_SRCS *.cpp)
file(GLOB ACCELERATOR_COMPRESSION_HDRS *.h)

add_library(accelerator_compression OBJECT ${ACCELERATOR_COMPRESSION_SRCS})

install(FILES ${ACCELERATOR_COMPRESSION_HDRS} DESTINATION include/accelerator/compression)
