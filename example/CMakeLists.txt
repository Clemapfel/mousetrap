cmake_minimum_required(VERSION 3.12)
project(mousetrap_example VERSION 0.0.0 LANGUAGES CXX)

find_package(mousetrap REQUIRED)

message(WARNING ${MOUSETRAP_LIBRARIES})

add_executable(mousetrap_example main.cpp)
target_link_libraries(mousetrap_example PRIVATE
    ${MOUSETRAP_LIBRARIES}
)
target_include_directories(mousetrap_example PRIVATE
    ${MOUSETRAP_INCLUDE_DIRECTORIES}
)
