add_library(dpipe INTERFACE)
target_include_directories(dpipe INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_include_directories(dpipe INTERFACE $<INSTALL_INTERFACE:include>)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/dpipe"
    DESTINATION "include"
)

install(TARGETS dpipe
    EXPORT dpipe-targets
)

install(EXPORT dpipe-targets
    NAMESPACE dpipe::
    FILE dpipe-config.cmake
    DESTINATION lib/cmake/dpipe
)

add_library(dpipe::dpipe ALIAS dpipe)
