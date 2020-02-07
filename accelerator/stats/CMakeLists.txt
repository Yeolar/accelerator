# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_STATS_SRCS *.cpp)
file(GLOB ACCELERATOR_STATS_HDRS *.h)

add_library(accelerator_stats OBJECT ${ACCELERATOR_STATS_SRCS})

install(FILES ${ACCELERATOR_STATS_HDRS} DESTINATION include/accelerator/stats)
