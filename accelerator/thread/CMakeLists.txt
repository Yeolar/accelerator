# Copyright 2017 Yeolar

set(ACCELERATOR_THREAD_SRCS
    CacheLocality.cpp
    Futex.cpp
    SharedMutex.cpp
    ThreadUtil.cpp
)
add_library(accelerator_thread OBJECT ${ACCELERATOR_THREAD_SRCS})

install(FILES *.h DESTINATION include/accelerator/thread)
