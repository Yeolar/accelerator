# Copyright 2018 Yeolar

file(GLOB DOUBLE_CONVERSION_SRCS *.cc)
file(GLOB DOUBLE_CONVERSION_HDRS *.h)

add_library(double-conversion OBJECT ${DOUBLE_CONVERSION_SRCS})

install(FILES ${DOUBLE_CONVERSION_HDRS} DESTINATION include/accelerator/double-conversion)
