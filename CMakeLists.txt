cmake_minimum_required(VERSION 3.15)

project(dpipe VERSION 0.0.0)
set(DPIPE_VERSION "${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wvla -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
endif()

option(DPIPE_BUILD_DOXYGEN "Build Doxygen documentation" YES)
option(DPIPE_BUILD_TESTS "Build tests" YES)

add_subdirectory(src)

if(DPIPE_BUILD_DOXYGEN)
    add_subdirectory(docs)
endif()

if(DPIPE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
