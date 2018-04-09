# Copyright (C) 2017, Yeolar

set(ACCELERATOR_CONCURRENCY_SRCS
    CPUThreadPoolExecutor.cpp
    ThreadedExecutor.cpp
    ThreadPoolExecutor.cpp
)
add_library(accelerator_concurrency OBJECT ${ACCELERATOR_CONCURRENCY_SRCS})

install(FILES
    CPUThreadPoolExecutor.h
    Executor.h
    ThreadedExecutor.h
    ThreadFactory.h
    ThreadPoolExecutor.h
    DESTINATION include/accelerator/concurrency)
