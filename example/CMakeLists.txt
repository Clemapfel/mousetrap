cmake_minimum_required(VERSION 3.12)
project(mousetrap_example VERSION 0.0.0 LANGUAGES CXX)

find_package(mousetrap REQUIRED)
add_executable(mousetrap_example main.cpp)
target_link_libraries(mousetrap_example PRIVATE mousetrap)

#get_target_property(MOUSETRAP_INCLUDE_DIRS mousetrap::mousetrap-targets INTERFACE_INCLUDE_DIRECTORIES)
#get_target_property(MOUSETRAP_LIBRARIES mousetrap::mousetrap-targets INTERFACE_LINK_LIBRARIES)

#target_include_directories(mousetrap_example PRIVATE
 #   "${MOUSETRAP_INCLUDE_DIRS}"
#)

#target_link_libraries(mousetrap_example PRIVATE
 #   "${MOUSETRAP_LIBRARIES}"
#)
