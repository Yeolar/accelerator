# Copyright 2017 Yeolar

set(ACCELERATOR_IO_SRCS
    Cursor.cpp
    File.cpp
    FileUtil.cpp
    FSUtil.cpp
    HugePages.cpp
    IOBuf.cpp
    IOBufQueue.cpp
    Path.cpp
    RecordIO.cpp
    Waker.cpp
)
add_library(accelerator_io OBJECT ${ACCELERATOR_IO_SRCS})

install(FILES *.h DESTINATION include/accelerator/io)
