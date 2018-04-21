# Copyright 2017 Yeolar

set(ACCELERATOR_COMPRESSION_SRCS
    Compression.cpp
    ZlibStreamCompressor.cpp
    ZlibStreamDecompressor.cpp
)
add_library(accelerator_compression OBJECT ${ACCELERATOR_COMPRESSION_SRCS})

install(FILES *.h DESTINATION include/accelerator/compression)
