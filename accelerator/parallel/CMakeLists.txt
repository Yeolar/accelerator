# Copyright (C) 2017, Yeolar

set(ACCELERATOR_PARALLEL_SRCS
    DAG.cpp
    ParallelScheduler.cpp
)
add_library(accelerator_parallel OBJECT ${ACCELERATOR_PARALLEL_SRCS})

install(FILES
    DAG.h
    Graph.h
    ParallelScheduler.h
    DESTINATION include/accelerator/parallel)
