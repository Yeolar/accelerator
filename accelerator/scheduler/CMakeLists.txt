# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_SCHEDULER_SRCS *.cpp)
file(GLOB ACCELERATOR_SCHEDULER_HDRS *.h)

add_library(accelerator_scheduler OBJECT ${ACCELERATOR_SCHEDULER_SRCS})

install(FILES ${ACCELERATOR_SCHEDULER_HDRS} DESTINATION include/accelerator/scheduler)
