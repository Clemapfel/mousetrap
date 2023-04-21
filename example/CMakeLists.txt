cmake_minimum_required(VERSION 3.12)
project(mousetrap_example VERSION 0.0.0 LANGUAGES CXX)

find_library(mousetrap REQUIRED NAMES mousetrap PATHS ../cmake-build-debug/)
get_target_property(mousetrap mousetrap::)
add_executable(mousetrap_example main.cpp)
target_link_libraries(mousetrap_example PRIVATE mousetrap)

target_include_directories(mousetrap_example PRIVATE
    mousetrap::
)
