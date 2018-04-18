# Copyright 2017 Yeolar

set(ACCELERATOR_THREAD_SRCS
    CacheLocality.cpp
    Futex.cpp
    SharedMutex.cpp
    ThreadUtil.cpp
)
add_library(accelerator_thread OBJECT ${ACCELERATOR_THREAD_SRCS})

install(FILES
    AtomicBitSet.h
    AtomicLinkedList.h
    AtomicPtr.h
    AtomicUnorderedMap.h
    Baton.h
    BlockingQueue.h
    CacheLocality.h
    ConcurrentSkipList.h
    ConcurrentSkipList-inl.h
    Futex.h
    LockedMap.h
    LockedQueue.h
    LockTraits.h
    MPMCQueue.h
    ProducerConsumerQueue.h
    RWSpinLock.h
    Semaphore.h
    SharedMutex.h
    SpinLock.h
    Synchronized.h
    ThreadUtil.h
    UnionBasedStatic.h
    Waiter.h
    DESTINATION include/accelerator/thread)
