add_executable("fast-bernoulli")
target_sources("fast-bernoulli" PRIVATE "main.cc")
target_link_libraries("fast-bernoulli" PRIVATE "fast-bernoulli-lib")
