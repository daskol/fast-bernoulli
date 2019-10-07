cmake_minimum_required(VERSION 3.15)

project(fast-bernoulli VERSION 0.0.0 LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CXX_SOURCES "executor.cc" "fast-bernoulli.cc")
set(TEST_CXX_SOURCES "executor_test.cc" "fast-bernoulli_test.cc")
set(BENCH_CXX_SOURCES "executor_bench.cc" "fast-bernoulli_bench.cc")

add_library("${CMAKE_PROJECT_NAME}-obj" OBJECT "${CXX_SOURCES}")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx")
target_compile_options("${CMAKE_PROJECT_NAME}-obj" PRIVATE "-mavx2")

add_executable("${CMAKE_PROJECT_NAME}" "main.cc"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE
                           "${CMAKE_CURRENT_BINARY_DIR}")

add_executable("${CMAKE_PROJECT_NAME}-test" "${TEST_CXX_SOURCES}"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_link_libraries("${CMAKE_PROJECT_NAME}-test" PRIVATE GTest::Main)

add_executable("${CMAKE_PROJECT_NAME}-bench" "${BENCH_CXX_SOURCES}"
               "$<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}-obj>")
target_link_libraries("${CMAKE_PROJECT_NAME}-bench"
                      PRIVATE benchmark::benchmark_main)
