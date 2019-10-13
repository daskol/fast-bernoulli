add_subdirectory("benchmark")
add_subdirectory("unittest")
add_library("fast-bernoulli-lib" STATIC)

configure_file("config.h.in" "config.h")

target_compile_options("fast-bernoulli-lib" PRIVATE "-mavx" "-mavx2")

target_sources("fast-bernoulli-lib" PRIVATE
    "cpuid.cc"
    "executor.cc"
    "sampler.cc"
)

target_include_directories("fast-bernoulli-lib" INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (USE_LLVM)
    target_sources("fast-bernoulli-lib" PRIVATE "executor_jit.cc")
    target_link_libraries("fast-bernoulli-lib" INTERFACE "LLVM")
    target_compile_definitions("fast-bernoulli-lib" PRIVATE
        "-DUSE_JIT_EXECUTOR"
    )
endif()

install(
    FILES "common.h" "cpuid.h" "fast-bernoulli.h" "sampler.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}"
)

install(
    TARGETS "fast-bernoulli-lib" EXPORT "${CMAKE_PROJECT_NAME}"
    LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/lib${CMAKE_PROJECT_NAME}.a"
)

install(
    EXPORT "${CMAKE_PROJECT_NAME}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)
