cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/submodules/FindWDK/cmake")
find_package(WDK REQUIRED)

wdk_add_driver(nullfs
    cleanup.cpp             
    close.cpp              
    create.cpp               
    debug.h                
    devctrl.cpp              
    dispatchRoutines.h     
    flowControl.h          
    fsctrl.cpp               
    ../include/nullFS/ioctl.h                
    ../include/nullFS/names.h                
    nullfs.cpp              
    struct.h               
    support.cpp              
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
