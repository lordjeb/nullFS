cmake_minimum_required(VERSION 3.19)

project(nullFS)

# Remove /RTC1 from the compile flags because it is not valid for kernel mode
# Any other project that requires it can include it individually
foreach (flag_var CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG)
    string(REPLACE "/RTC1" "" ${flag_var} "${${flag_var}}")
endforeach()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(src/driver)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    set(INSTALL_GTEST OFF)
    add_subdirectory(submodules/googletest)
    add_subdirectory(test/integrationTests)
endif()
