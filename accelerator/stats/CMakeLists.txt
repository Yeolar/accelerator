# Copyright 2017 Yeolar

set(ACCELERATOR_STATS_SRCS
    Monitor.cpp
)
add_library(accelerator_stats OBJECT ${ACCELERATOR_STATS_SRCS})

install(FILES *.h DESTINATION include/accelerator/stats)
