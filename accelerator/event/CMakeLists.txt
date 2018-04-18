# Copyright 2017 Yeolar

set(ACCELERATOR_EVENT_SRCS
    EPoll.cpp
    EventBase.cpp
    EventLoop.cpp
    EventLoopManager.cpp
    EventUtil.cpp
)
add_library(accelerator_event OBJECT ${ACCELERATOR_EVENT_SRCS})

install(FILES
    EPoll.h
    EventBase.h
    EventHandlerBase.h
    EventLoop.h
    EventLoopManager.h
    EventUtil.h
    DESTINATION include/accelerator/event)
