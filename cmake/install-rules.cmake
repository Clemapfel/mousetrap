set(package mousetrap)
include(GNUInstallDirs)

install(
    DIRECTORY include
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mousetrap"
)

install(
    FILES mousetrap.hpp
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(
    TARGETS mousetrap
    EXPORT mousetrap
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${package}-config-version.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)