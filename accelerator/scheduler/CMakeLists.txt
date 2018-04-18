# Copyright 2017 Yeolar

set(ACCELERATOR_SCHEDULER_SRCS
    DAG.cpp
    ParallelScheduler.cpp
    PeriodicScheduler.cpp
)
add_library(accelerator_scheduler OBJECT ${ACCELERATOR_SCHEDULER_SRCS})

install(FILES
    DAG.h
    Graph.h
    ParallelScheduler.h
    PeriodicScheduler.h
    DESTINATION include/accelerator/scheduler)
