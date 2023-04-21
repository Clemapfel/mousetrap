cmake_minimum_required(VERSION 3.12)
project(mousetrap_example VERSION 0.0.0 LANGUAGES CXX)

find_package(mousetrap REQUIRED)

add_executable(mousetrap_example main.cpp)
set_target_properties(mousetrap_example PROPERTIES
    LINKER_LANGUAGE CXX
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(mousetrap_example PRIVATE
    ${MOUSETRAP_LIBRARIES}
)
target_include_directories(mousetrap_example PRIVATE
    ${MOUSETRAP_INCLUDE_DIRECTORIES}
)
