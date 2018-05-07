# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_SSL_SRCS *.cpp)
file(GLOB ACCELERATOR_SSL_HDRS *.h)

add_library(accelerator_ssl OBJECT ${ACCELERATOR_SSL_SRCS})

install(FILES ${ACCELERATOR_SSL_HDRS} DESTINATION include/accelerator/ssl)
