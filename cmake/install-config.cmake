include("${CMAKE_CURRENT_LIST_DIR}/mousetrap-targets.cmake")

get_target_property(MOUSETRAP_INCLUDE_DIRECTORIES mousetrap::mousetrap INTERFACE_INCLUDE_DIRECTORIES)

get_target_property(MOUSETRAP_LIBRARIES mousetrap::mousetrap INTERFACE_LINK_LIBRARIES)
set(MOUSETRAP_LIBRARIES "mousetrap::mousetrap;${MOUSETRAP_LIBRARIES}")