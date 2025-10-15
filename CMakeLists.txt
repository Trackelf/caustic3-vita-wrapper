cmake_minimum_required(VERSION 3.15)
project(caustic3_vita C)

set(CMAKE_C_STANDARD 11)

set(VITA_APP_NAME "Caustic3Vita")
set(VITA_TITLEID  "CSTC00001")
set(VITA_VERSION  "01.00")

# Vita-Makros laden

include($ENV{VITASDK}/share/vita.cmake)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(caustic3.elf
    src/main.c
    src/libloader.c
    src/log.c
    src/soloader.c
    src/so_util.c
)

target_include_directories(caustic3.elf PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(caustic3.elf
    SceLibKernel_stub
    SceProcessmgr_stub
    SceIofilemgr_stub
    SceLibc_stub
    kubridge_stub
)

vita_create_self(eboot.bin caustic3.elf)
vita_create_vpk(Caustic3Vita.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE ${CMAKE_SOURCE_DIR}/assets assets
    FILE ${CMAKE_SOURCE_DIR}/lib/libcaustic.so lib/libcaustic.so
)


