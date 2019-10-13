cmake_minimum_required(VERSION 3.15)

project(fast-bernoulli VERSION 0.0.0 LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(LLVM)

set(CMAKE_CXX_STANDARD 17)
set(CXX_SOURCES "cpuid.cc" "executor.cc" "sampler.cc")
set(TEST_CXX_SOURCES "executor_test.cc" "sampler_test.cc")
set(BENCH_CXX_SOURCES "executor_bench.cc" "sampler_bench.cc")

if (USE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if (USE_LLVM AND LLVM_FOUND)
    if (NOT ${LLVM_VERION_MAJOR} MATCHES 8 AND NOT ${LLVM_VERSION_MAJOR} MATCHES 9)
        message(FATAL_ERROR "LLVM of version either 8 or 9 is supported.")
    endif()

    set(CXX_SOURCES "${CXX_SOURCES}" "executor_jit.cc")
    set(TEST_CXX_SOURCES "${TEST_CXX_SOURCES}" "executor_jit_test.cc")
    set(LLVM_LIBS "LLVM")
    add_definitions(-DUSE_JIT_EXECUTOR)
endif()

configure_file("config.h.in" "config.h")

add_library("${CMAKE_PROJECT_NAME}-obj" OBJECT "${CXX_SOURCES}")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx2")

add_library("${CMAKE_PROJECT_NAME}-lib" STATIC "main.cc"
            "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_include_directories("${CMAKE_PROJECT_NAME}-lib" PRIVATE
                           "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries("${CMAKE_PROJECT_NAME}-lib" ${LLVM_LIBS})

add_executable("${CMAKE_PROJECT_NAME}" "main.cc"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE
                           "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries("${CMAKE_PROJECT_NAME}" ${LLVM_LIBS})

add_executable("${CMAKE_PROJECT_NAME}-test" "${TEST_CXX_SOURCES}"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_link_libraries("${CMAKE_PROJECT_NAME}-test" PRIVATE
                      GTest::Main
                      ${LLVM_LIBS})

add_executable("${CMAKE_PROJECT_NAME}-bench" "${BENCH_CXX_SOURCES}"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_link_libraries("${CMAKE_PROJECT_NAME}-bench" PRIVATE
                      benchmark::benchmark_main
                      ${LLVM_LIBS})

add_test(NAME all COMMAND "${CMAKE_PROJECT_NAME}-test")

include(GNUInstallDirs)
install(FILES "common.h" "cpuid.h" "fast-bernoulli.h" "sampler.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}")
install(TARGETS "${CMAKE_PROJECT_NAME}-lib" EXPORT "${CMAKE_PROJECT_NAME}"
        LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/libfast-bernoulli.a")
install(EXPORT "${CMAKE_PROJECT_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}")
