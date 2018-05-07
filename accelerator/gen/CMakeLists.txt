# Copyright 2017 Yeolar

file(GLOB ACCELERATOR_GEN_HDRS *.h)

install(FILES ${ACCELERATOR_GEN_HDRS} DESTINATION include/accelerator/gen)
