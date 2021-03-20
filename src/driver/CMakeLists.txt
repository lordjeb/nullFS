cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/submodules/FindWDK/cmake")
find_package(WDK REQUIRED)

wdk_add_driver(nullfs
    nullfs.c
    cleanup.c              
    close.c                
    create.c               
    debug.h                
    devctrl.c              
    dispatchRoutines.h     
    flowControl.h          
    fsctrl.c               
    ../include/nullFS/ioctl.h                
    ../include/nullFS/names.h                
    nullfs.c               
    struct.h               
    support.c              
    support.h
    )
target_precompile_headers(
    nullfs
    PRIVATE
        <ntifs.h>
        "debug.h"
        "support.h"
        "flowControl.h"
    )
target_include_directories(nullfs
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/submodules/wil/include
    )
