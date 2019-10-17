add_subdirectory("cc")

install(
    FILES "fast-bernoulli.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}"
)
