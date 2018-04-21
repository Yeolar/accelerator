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

install(FILES
    Cursor.h
    Cursor-inl.h
    File.h
    FileUtil.h
    FSUtil.h
    HugePages.h
    IOBuf.h
    IOBufQueue.h
    Path.h
    RecordIO.h
    TypedIOBuf.h
    Waker.h
    DESTINATION include/accelerator/io)
