# Copyright 2017-present Yeolar

file(GLOB ACCELERATOR_BASE_SRCS *.cpp)
file(GLOB ACCELERATOR_BASE_HDRS *.h)

file(GLOB ACCELERATOR_BASE_DETAIL_SRCS detail/*.cpp)
file(GLOB ACCELERATOR_BASE_DETAIL_HDRS detail/*.h)

add_library(accelerator_base OBJECT
  ${ACCELERATOR_BASE_SRCS}
  ${ACCELERATOR_BASE_DETAIL_SRCS}
)

install(FILES ${ACCELERATOR_BASE_HDRS} DESTINATION include/accelerator)
install(FILES ${ACCELERATOR_BASE_DETAIL_HDRS} DESTINATION include/accelerator/detail)
