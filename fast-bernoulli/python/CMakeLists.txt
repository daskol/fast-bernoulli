if (NOT BUILD_WHEEL)
    return()
endif()

find_package(Python3 COMPONENTS Development)

if (BUILD_WHEEL AND NOT Python3_Development_FOUND)
    message(FATAL_ERROR "Python3 is not found.")
endif()

set_target_properties("fast-bernoulli-cc" PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

configure_file("setup.py.in" "setup.py")

add_custom_target("fast-bernoulli-python-wheel" DEPENDS "fast-bernoulli-cc")
add_custom_command(
    TARGET "fast-bernoulli-python-wheel"
    COMMAND "python" "${CMAKE_CURRENT_BINARY_DIR}/setup.py" "-q"
        "build_ext"
            "--cython-c-in-temp"
            "-b${CMAKE_CURRENT_BINARY_DIR}/build"
            "-t${CMAKE_CURRENT_BINARY_DIR}"
            "-I${CMAKE_SOURCE_DIR}:${CMAKE_BINARY_DIR}"
            "-O${CMAKE_BINARY_DIR}/fast-bernoulli/cc/${CMAKE_STATIC_LIBRARY_PREFIX}fast-bernoulli-cc${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "bdist_wheel"
            "-b${CMAKE_CURRENT_BINARY_DIR}/build"
            "-d${CMAKE_CURRENT_BINARY_DIR}"
        "install_lib"
            "-b${CMAKE_CURRENT_BINARY_DIR}/build"
        "egg_info"
            "-e${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Built python extension and make wheel distribution"
    DEPENDS "fast-bernoulli-cc" "_fast_bernoulli.pyx"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
