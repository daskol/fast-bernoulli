add_executable("fast-bernoulli-cc-test")
add_test(NAME "fast-bernoulli-cc-test" COMMAND "fast-bernoulli-cc-test")

target_sources("fast-bernoulli-cc-test" PRIVATE
    "executor_test.cc"
    "sampler_test.cc"
)

target_link_libraries("fast-bernoulli-cc-test" PRIVATE
    "fast-bernoulli-cc"
    GTest::Main
)

if (USE_LLVM)
    target_sources("fast-bernoulli-cc-test" PRIVATE "executor_jit_test.cc")
endif()
