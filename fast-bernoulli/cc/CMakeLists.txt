set(CMAKE_CXX_STANDARD 17)

add_subdirectory("benchmark")
add_subdirectory("unittest")
add_library("fast-bernoulli-cc" SHARED)

configure_file("config.h.in" "config.h")

target_compile_options("fast-bernoulli-cc" PRIVATE "-mavx" "-mavx2")

target_sources("fast-bernoulli-cc" PRIVATE
    "cpuid.cc"
    "executor.cc"
    "sampler.cc"
)

target_include_directories("fast-bernoulli-cc" INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (USE_LLVM)
    target_sources("fast-bernoulli-cc" PRIVATE "executor_jit.cc")
    target_link_libraries("fast-bernoulli-cc" INTERFACE "LLVM")
    target_compile_definitions("fast-bernoulli-cc" PRIVATE
        "-DUSE_JIT_EXECUTOR"
    )
endif()

install(
    FILES "common.h" "cpuid.h" "sampler.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}/cc"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}/cc"
)

install(
    TARGETS "fast-bernoulli-cc" EXPORT "${CMAKE_PROJECT_NAME}"
    LIBRARY
)

install(
    EXPORT "${CMAKE_PROJECT_NAME}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)
