cmake_minimum_required(VERSION 3.12)

# project name
project(mousetrap_example VERSION 0.1.0 LANGUAGES CXX)

# find mousetrap
find_package(mousetrap REQUIRED)

# target name
set(TARGET_NAME example)

# target declaration
add_executable(${TARGET_NAME} main.cpp)

# link mousetrap libraries, including mousetrap itself
target_link_libraries(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_LIBRARIES}
)

# add mousetrap include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_INCLUDE_DIRECTORIES}
)

# add std:: for C++17
target_compile_features(${TARGET_NAME} PUBLIC
    cxx_std_17
)
