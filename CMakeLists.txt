cmake_minimum_required(VERSION 3.15)
project(fast-bernoulli VERSION 0.2.0)
enable_testing()

option(USE_ASAN "Turn on Address Sanitizer (ASan)" OFF)
option(USE_LLVM "Turn on JIT compilation in run-time" OFF)
option(BUILD_WHEEL "Build Python distribution" OFF)
option(BUILD_BENCHMARKS "Build benchmarks " OFF)
option(BUILD_TESTS "Build tests" OFF)

include(GNUInstallDirs)

find_package(LLVM)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (USE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if (USE_LLVM AND NOT LLVM_FOUND)
    message(FATAL_ERROR "LLVM is not found.")
endif()

if (USE_LLVM AND LLVM_FOUND)
    if (NOT ${LLVM_VERION_MAJOR} MATCHES 8 AND NOT ${LLVM_VERSION_MAJOR} MATCHES 9)
        message(FATAL_ERROR "LLVM of version either 8 or 9 is supported.")
    endif()
endif()

add_subdirectory(fast-bernoulli)
