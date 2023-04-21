set(package mousetrap)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    FILES mousetrap.hpp
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    DIRECTORY include src
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    TARGETS mousetrap
    EXPORT mousetrap-targets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

write_basic_package_version_file(
    "${package}-config-version.cmake"
    COMPATIBILITY AnyNewerVersion
)

set(MOUSETRAP_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}")

install(
    FILES "${PROJECT_BINARY_DIR}/${package}-config-version.cmake"
    DESTINATION "${MOUSETRAP_INSTALL_CMAKEDIR}"
)

install(
    EXPORT mousetrap-targets
    NAMESPACE mousetrap::
    DESTINATION "${MOUSETRAP_INSTALL_CMAKEDIR}"
)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${MOUSETRAP_INSTALL_CMAKEDIR}"
    RENAME "${package}-config.cmake"
)


include(CPack)