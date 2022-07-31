cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 20)

### DEPENDENCIES ###

# OpenGL
find_library(OpenGL REQUIRED NAMES GL)
find_library(GLEW REQUIRED NAMES GLEW)

# GTK
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

# GLM
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("glm/glm.hpp" GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "Missing Dependency: glm")
endif()

### CONFIGURE ###

set(RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources")
configure_file(
    "${CMAKE_SOURCE_DIR}/src/get_resource_path.hpp.in"
    "${CMAKE_SOURCE_DIR}/include/get_resource_path.hpp"
    IMMEDIATE @ONLY
)

### LIBRARY ###

add_library(mousetrap SHARED
    include/get_resource_path.hpp
    include/gl_common.hpp
    src/gl_common.inl
    include/shape.hpp
    include/colors.hpp
    include/shader.hpp
        include/gl_area.hpp
    include/transform.hpp
    include/angle.hpp
    include/gtk_common.hpp
    include/geometric_shapes.hpp
    include/vector.hpp
    include/render_task.hpp
        include/scale.hpp include/spin_button.hpp include/range.hpp include/entry.hpp include/button.hpp include/box.hpp include/container.hpp include/overlay.hpp include/concepts.hpp)

set_target_properties(mousetrap PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
)

target_include_directories(mousetrap PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${GTKMM_INCLUDE_DIRS}
)

target_link_libraries(mousetrap PUBLIC
    ${OpenGL}
    ${GLEW}
    ${GLU}
    ${GTKMM_LIBRARIES}
)

### EXECUTABLE ###

add_executable(debug main.cpp)
target_link_libraries(debug mousetrap)

