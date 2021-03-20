cmake_minimum_required(VERSION 3.19)

add_compile_definitions(UNICODE)
add_compile_definitions(_UNICODE)
remove_definitions(_MBCS)

add_executable(nullFsIntegrationTests
    controlDeviceObjectTests.cpp 
    InstallTestDriver.cpp        
    InstallTestDriver.h          
    integrationTests.cpp         
    nullFsDriverEnvironment.cpp  
    nullFsDriverEnvironment.h    
    StartTestDriver.cpp          
    StartTestDriver.h            
)
target_compile_options(nullFsIntegrationTests PRIVATE /W4 /WX)
target_precompile_headers(
    nullFsIntegrationTests
    PRIVATE
       <string>
       <gtest/gtest.h>
       <gmock/gmock.h>
       <wil/resource.h>
       <Windows.h>
    )
# TODO: Should be a better way to get this include directory, if we are including the cmake for wil?
target_include_directories(nullFsIntegrationTests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/submodules/wil/include
    )
target_link_libraries(nullFsIntegrationTests 
    PRIVATE 
    gtest 
    gmock
    setupapi
    )

gtest_discover_tests(nullFsIntegrationTests)
