add_subdirectory("cc")
add_subdirectory("python")

install(
    FILES "fast-bernoulli.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}"
)
