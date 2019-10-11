cmake_minimum_required(VERSION 3.15)

project(fast-bernoulli VERSION 0.0.0 LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(LLVM 8.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CXX_SOURCES "cpuid.cc" "executor.cc" "sampler.cc")
set(TEST_CXX_SOURCES "executor_test.cc" "sampler_test.cc")
set(BENCH_CXX_SOURCES "executor_bench.cc" "sampler_bench.cc")

if (USE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if (USE_LLVM AND LLVM_FOUND)
    set(CXX_SOURCES "${CXX_SOURCES}" "executor_jit.cc")
    set(TEST_CXX_SOURCES "${TEST_CXX_SOURCES}" "executor_jit_test.cc")
    set(LLVM_LIBS "LLVM")
endif()

add_library("${CMAKE_PROJECT_NAME}-obj" OBJECT "${CXX_SOURCES}")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx2")

add_executable("${CMAKE_PROJECT_NAME}" "main.cc"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE
                           "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries("${CMAKE_PROJECT_NAME}" LLVM)

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
