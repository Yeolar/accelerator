# Copyright 2017 Yeolar

set(ACCELERATOR_SSL_SRCS
    OpenSSL.cpp
)
add_library(accelerator_ssl OBJECT ${ACCELERATOR_SSL_SRCS})

install(FILES
    OpenSSL.h
    OpenSSLHash.h
    OpenSSLPtrTypes.h
    DESTINATION include/accelerator/ssl)
