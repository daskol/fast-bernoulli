if (NOT BUILD_BENCHMARKS)
    return()
endif()

find_package(benchmark REQUIRED)

add_executable("fast-bernoulli-cc-bench")
add_test(NAME "fast-bernoulli-cc-bench" COMMAND "fast-bernoulli-cc-bench")

target_sources("fast-bernoulli-cc-bench" PRIVATE
    "executor_bench.cc"
    "sampler_bench.cc"
)

target_link_libraries("fast-bernoulli-cc-bench" PRIVATE
    "fast-bernoulli-cc"
    benchmark::benchmark_main
)

if (USE_LLVM)
    target_compile_definitions("fast-bernoulli-cc-bench" PRIVATE
        "-DUSE_JIT_EXECUTOR"
    )
endif()
