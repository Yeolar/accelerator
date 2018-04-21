# Copyright 2017 Yeolar

set(ACCELERATOR_ENC_SRCS
    CharProperty.cpp
    uconv.cpp
)
add_library(accelerator_enc OBJECT ${ACCELERATOR_ENC_SRCS})

install(FILES *.h DESTINATION include/accelerator/enc)

add_executable(CharPropertyBuilder
    CharPropertyBuilder.cpp
)
target_link_libraries(CharPropertyBuilder accelerator_static)
