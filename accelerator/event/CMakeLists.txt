# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_EVENT_SRCS *.cpp)
file(GLOB ACCELERATOR_EVENT_HDRS *.h)

add_library(accelerator_event OBJECT ${ACCELERATOR_EVENT_SRCS})

install(FILES ${ACCELERATOR_EVENT_HDRS} DESTINATION include/accelerator/event)
