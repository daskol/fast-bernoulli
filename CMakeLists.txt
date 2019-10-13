cmake_minimum_required(VERSION 3.15)
project(fast-bernoulli VERSION 0.1.0)
enable_testing()

option(USE_ASAN "Turn on Address Sanitizer (ASan)" OFF)
option(USE_LLVM "Turn on JIT compilation in run-time" OFF)

include(GNUInstallDirs)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(LLVM)

set(CMAKE_CXX_STANDARD 17)

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

add_subdirectory(src)
