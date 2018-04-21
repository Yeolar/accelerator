# Copyright 2017 Yeolar

set(ACCELERATOR_BASE_SRCS
    Arena.cpp
    Benchmark.cpp
    Checksum.cpp
    Conv.cpp
    Demangle.cpp
    dynamic.cpp
    EscapeTables.cpp
    json.cpp
    LogBase.cpp
    MemoryMapping.cpp
    ProcessUtil.cpp
    Random.cpp
    Range.cpp
    RendezvousHash.cpp
    SpookyHashV2.cpp
    String.cpp
    SysUtil.cpp
    TestUtil.cpp
    Time.cpp
    Unicode.cpp
    Uri.cpp
    Utf8StringPiece.cpp
    Uuid.cpp
)
add_library(accelerator_base OBJECT ${ACCELERATOR_BASE_SRCS})

install(FILES *.h DESTINATION include/accelerator)
